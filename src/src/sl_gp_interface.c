/*
 *  Copyright (c) 2024, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*******************************************************************************
 * @file
 * @brief This file implements Green Power interface.
 ******************************************************************************/

#include "sl_gp_interface.h"
#include "ieee802154mac.h"
#include "rail_ieee802154.h"
#include "sl_gp_interface_config.h"
#include "sl_packet_utils.h"
#include "sl_status.h"
#include <assert.h>
#include <string.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/time.h>
#include "common/debug.hpp"
#include "common/logging.hpp"
#include "utils/code_utils.h"
#include "utils/mac_frame.h"

// This implements mechanism to buffer outgoing Channel Configuration (0xF3) and
// Commissioning Reply (0xF0) GPDF commands on the RCP to sent out on request
// from GPD with bidirectional capability with in a certain time window, i.e.
// between 20 and 25 msec.
// The mechanism works following way -
// The zigbeed submits the outgoing GPDF command, this code on rcp intercepts the
// packet from transmit API and buffers the packet, does not send it out.
// The GPD sends request indicating its RX capability, this again intercept the
// rx message and based on the request, it sends out the above buffered message
// with in a time window of 20-25 msec from the time it received the message.

#define GP_MIN_MAINTENANCE_FRAME_LENGTH 10
#define GP_MIN_DATA_FRAME_LENGTH 14

#define GP_ADDRESSING_MODE_SRC_ID 0
#define GP_ADDRESSING_MODE_EUI64 2

// Check the GP Frame Type field to ensure it is either a maintenance frame (1) or a data frame (0).
#define GP_NWK_PROTOCOL_VERSION_CHECK(nwkFc) ((((nwkFc >> 2) & 0x0F) == 3) && ((nwkFc & 0x3) <= 1))
#define GP_NWK_FRAME_TYPE_MAINTENANCE_WITHOUT_EXTD_FC(nwkFc) ((nwkFc & 0xC3) == 0x01)
#define GP_NWK_FRAME_TYPE_DATA_WITH_EXTD_FC(nwkFc) ((nwkFc & 0xC3) == 0x80)

#define GP_NWK_UNSECURED_RX_DATA_FRAME(nwkExntdFc) ((nwkExntdFc & 0xF8) == 0x40)
#define GP_NWK_UNSECURED_TX_DATA_FRAME(nwkExntdFc) ((nwkExntdFc & 0xF8) == 0x80)
#define GP_NWK_ADDRESSING_APP_ID(nwkExntdFc) ((nwkExntdFc & 0x07))

#define GP_CHANNEL_REQUEST_CMD_ID 0xE3
#define GP_CHANNEL_CONFIGURATION_CMD_ID 0xF3
#define GP_COMMISSIONINGING_CMD_ID 0xE0
#define GP_COMMISSIONING_REPLY_CMD_ID 0xF0

#define GP_EXND_FC_INDEX 1
#define GP_COMMAND_INDEX_FOR_MAINT_FRAME 1
#define GP_SRC_ID_INDEX_WITH_APP_MODE_0 2
#define GP_APP_EP_INDEX_WITH_APP_MODE_1 2
#define GP_COMMAND_INDEX_WITH_APP_MODE_1 3
#define GP_COMMAND_INDEX_WITH_APP_MODE_0 6

#define BUFFERED_PSDU_GP_SRC_ID_INDEX_WITH_APP_MODE_0 9
#define BUFFERED_PSDU_GP_APP_EP_INDEX_WITH_APP_MODE_1 15

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
static volatile sl_gp_state_t gp_state = SL_GP_STATE_INIT;
static volatile uint64_t      gpStateTimeOut;

// Needed to retrieve buffered transmit frame present in global memory.
static otInstance *sBufferedTxInstance = NULL;

sl_gp_state_t sl_gp_intf_get_state(void)
{
    return gp_state;
}

void efr32GpProcess(void)
{
    switch (gp_state)
    {
    case SL_GP_STATE_INIT:
    {
        gp_state = SL_GP_STATE_IDLE;
        otLogDebgPlat("GP RCP INTF: GP Frame init!!");
    }
    break;
    case SL_GP_STATE_SEND_RESPONSE:
    {
        if (otPlatTimeGet() >= gpStateTimeOut)
        {
            OT_ASSERT(sBufferedTxInstance != NULL);
            // Get the tx frame and send it without csma.
            otRadioFrame *aTxFrame                   = otPlatRadioGetTransmitBuffer(sBufferedTxInstance);
            aTxFrame->mInfo.mTxInfo.mCsmaCaEnabled   = false;
            aTxFrame->mInfo.mTxInfo.mMaxCsmaBackoffs = 0;
            // On successful transmit, this will call the transmit complete callback for the GP packet,
            // and go up to the CGP Send Handler and eventually the green power client.
            otPlatRadioTransmit(sBufferedTxInstance, aTxFrame);

            gp_state            = SL_GP_STATE_IDLE;
            sBufferedTxInstance = NULL;
            otLogDebgPlat("GP RCP INTF: Sending Response!!");
        }
    }
    break;
    case SL_GP_STATE_WAITING_FOR_PKT:
    {
        if (otPlatTimeGet() >= gpStateTimeOut)
        {
            OT_ASSERT(sBufferedTxInstance != NULL);
            // This is a timeout call for the case when the GPD did not poll the response with in 5 seconds.
            otPlatRadioTxDone(sBufferedTxInstance,
                              otPlatRadioGetTransmitBuffer(sBufferedTxInstance),
                              NULL,
                              OT_ERROR_ABORT);
            gp_state            = SL_GP_STATE_IDLE;
            sBufferedTxInstance = NULL;
        }
    }
    break;
    default:
    {
        // For all other states don't do anything
    }
    break;
    }
}

void sl_gp_intf_buffer_pkt(otInstance *aInstance)
{
    gpStateTimeOut = otPlatTimeGet() + GP_TX_MAX_TIMEOUT_IN_MICRO_SECONDS;
    gp_state       = SL_GP_STATE_WAITING_FOR_PKT;
    OT_ASSERT(aInstance != NULL);
    sBufferedTxInstance = aInstance;
    otLogDebgPlat("GP RCP INTF: buffered!!");
}

bool sl_gp_intf_should_buffer_pkt(otInstance *aInstance, otRadioFrame *aFrame, bool isRxFrame)
{
    bool shouldBufferPacket = false;

#if OPENTHREAD_CONFIG_DIAG_ENABLE
    // Exit immediately if diag mode is enabled.
    otEXPECT_ACTION(!otPlatDiagModeGet(), shouldBufferPacket = false);
#endif

    uint8_t *gpFrameStartIndex = efr32GetPayload(aFrame);
    otEXPECT_ACTION(gpFrameStartIndex != NULL, shouldBufferPacket = false);

    // A Typical MAC Frame with GP NWK Frame in it
    /* clang-format off */
    // MAC Frame  : [<---------------MAC Header------------->||<------------------------------------NWK Frame----------------------------------->]
    //               FC(2) | Seq(1) | DstPan(2) | DstAddr(2) || FC(1) | ExtFC(0/1) | SrcId(0/4) | SecFc(0/4) | MIC(0/4) | <------GPDF(1/n)------>
    // The Green Power NWK FC and Ext FC are described as :
    //              FC    : ExtFC Present(b7)=1| AC(b6)=0| Protocol Ver(b5-b2)=3 GP frames| Frame Type(b1-b0) = 0
    //              ExtFC :  rxAfteTX (b6) = 1 |  AppId(b2-b0) = 0
    /* clang-format on */

    uint8_t fc = *gpFrameStartIndex;

    otLogDebgPlat("GP RCP INTF : (%s) PL Index = %d Channel = %d Length = %d FC = %0X",
                  isRxFrame ? "Rx" : "Tx",
                  (gpFrameStartIndex - aFrame->mPsdu),
                  aFrame->mChannel,
                  aFrame->mLength,
                  fc);

    // Check if packet is a GP packet
    otEXPECT_ACTION(sl_gp_intf_is_gp_pkt(aFrame), shouldBufferPacket = false);

    otLogDebgPlat("GP RCP INTF : (%s) Length and Version Matched", isRxFrame ? "Rx" : "Tx");
    // For GP Maintenance Frame type without extended FC, the FC is exactly same for both RX and TX directions with
    // auto commissioning bit = 0, does not have a ExtFC field, only the command Id (which is the next byte in
    // frame) indicates the direction.
    if (GP_NWK_FRAME_TYPE_MAINTENANCE_WITHOUT_EXTD_FC(fc))
    {
        otLogDebgPlat("GP RCP INTF : (%s) Maintenance Frame match", isRxFrame ? "Rx" : "Tx");
        uint8_t cmdId = *(gpFrameStartIndex + GP_COMMAND_INDEX_FOR_MAINT_FRAME);
        if (cmdId == GP_CHANNEL_REQUEST_CMD_ID && isRxFrame && gp_state == SL_GP_STATE_WAITING_FOR_PKT)
        {
            // Send out the buffered frame
            gp_state       = SL_GP_STATE_SEND_RESPONSE;
            gpStateTimeOut = aFrame->mInfo.mRxInfo.mTimestamp + GP_RX_OFFSET_IN_MICRO_SECONDS;
            otLogDebgPlat("GP RCP INTF : (%s) Received GP_CHANNEL_REQUEST_CMD_ID - Send the Channel configuration",
                          isRxFrame ? "Rx" : "Tx");
        }
        else if (cmdId == GP_CHANNEL_CONFIGURATION_CMD_ID && !isRxFrame)
        {
            // Buffer the frame
            shouldBufferPacket = true;
            otLogDebgPlat("GP RCP INTF : (%s) Buffer GP_CHANNEL_CONFIGURATION_CMD_ID command", isRxFrame ? "Rx" : "Tx");
        }
    }
    else if (
        // Data frame with EXT FC present, extract the App Id, SrcId, direction and command Id
        GP_NWK_FRAME_TYPE_DATA_WITH_EXTD_FC(fc) &&
        // Minimum Data frame length with extended header and address
        aFrame->mLength >= GP_MIN_DATA_FRAME_LENGTH)
    {
        uint8_t extFc = *(gpFrameStartIndex + GP_EXND_FC_INDEX);

        // Process only unsecured commissioning frames for Tx/Rx with correct direction and RxAfterTx fields
        // A.3.9.1, step 12: the proxies (also in combos) receiving a Commissioning GPDF (0xE3), Application Description
        // GPDF (0xE4), any other GPD command from the GPD CommandID range 0xE5 – 0xEF, any GPD command from the GPD
        // CommandID range 0xB0 – 0xBF

        if ((!isRxFrame && GP_NWK_UNSECURED_TX_DATA_FRAME(extFc))
            || (isRxFrame && GP_NWK_UNSECURED_RX_DATA_FRAME(extFc)))
        {
            if (GP_NWK_ADDRESSING_APP_ID(extFc) == GP_ADDRESSING_MODE_SRC_ID)
            {
                uint8_t cmdId = *(gpFrameStartIndex + GP_COMMAND_INDEX_WITH_APP_MODE_0);
                if (cmdId == GP_COMMISSIONING_REPLY_CMD_ID && !isRxFrame)
                {
                    // Buffer the frame
                    shouldBufferPacket = true;
                }
                else if ((cmdId == GP_COMMISSIONINGING_CMD_ID || (0xE4 <= cmdId && cmdId <= 0xEF)
                          || (0xB0 <= cmdId && cmdId <= 0xBF))
                         && isRxFrame && gp_state == SL_GP_STATE_WAITING_FOR_PKT)
                {
                    otRadioFrame *aTxFrame = otPlatRadioGetTransmitBuffer(aInstance);
                    // Match the gpd src Id ?
                    if (!memcmp((const void *)(gpFrameStartIndex + GP_SRC_ID_INDEX_WITH_APP_MODE_0),
                                (const void *)((aTxFrame->mPsdu) + BUFFERED_PSDU_GP_SRC_ID_INDEX_WITH_APP_MODE_0),
                                sizeof(uint32_t)))
                    {
                        // Send out the buffered frame
                        gp_state       = SL_GP_STATE_SEND_RESPONSE;
                        gpStateTimeOut = aFrame->mInfo.mRxInfo.mTimestamp + GP_RX_OFFSET_IN_MICRO_SECONDS;
                    }
                }
            }
            else if (GP_NWK_ADDRESSING_APP_ID(extFc) == GP_ADDRESSING_MODE_EUI64)
            {
                uint8_t cmdId = *(gpFrameStartIndex + GP_COMMAND_INDEX_WITH_APP_MODE_1);
                if (cmdId == GP_COMMISSIONING_REPLY_CMD_ID && !isRxFrame)
                {
                    // Buffer the frame
                    shouldBufferPacket = true;
                }
                else if ((cmdId == GP_COMMISSIONINGING_CMD_ID || (0xE4 <= cmdId && cmdId <= 0xEF)
                          || (0xB0 <= cmdId && cmdId <= 0xBF))
                         && isRxFrame && gp_state == SL_GP_STATE_WAITING_FOR_PKT)
                {
                    otRadioFrame *aTxFrame = otPlatRadioGetTransmitBuffer(aInstance);
                    // Check the eui64 and app endpoint to send out the buffer packet.
                    otMacAddress aSrcAddress;
                    otMacAddress aDstAddress;
                    otMacFrameGetDstAddr(aTxFrame, &aDstAddress);
                    otMacFrameGetSrcAddr(aFrame, &aSrcAddress);
                    if (!memcmp(&(aDstAddress.mAddress.mExtAddress),
                                &(aSrcAddress.mAddress.mExtAddress),
                                sizeof(otExtAddress))
                        && (gpFrameStartIndex[GP_APP_EP_INDEX_WITH_APP_MODE_1]
                            == (aTxFrame->mPsdu)[BUFFERED_PSDU_GP_APP_EP_INDEX_WITH_APP_MODE_1]))
                    {
                        gp_state       = SL_GP_STATE_SEND_RESPONSE;
                        gpStateTimeOut = aFrame->mInfo.mRxInfo.mTimestamp + GP_RX_OFFSET_IN_MICRO_SECONDS;
                    }
                }
            }
        }
    }

    if (shouldBufferPacket)
    {
        otLogDebgPlat("GP RCP INTF: GP filter passed!!");
    }

exit:
    return shouldBufferPacket;
}
#endif // OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE

bool sl_gp_intf_is_gp_pkt(otRadioFrame *aFrame)
{
    /* clang-format off */

    // A Typical MAC Frame with GP NWK Frame in it
    // MAC Frame  : [<---------------MAC Header------------->||<------------------------------------NWK Frame----------------------------------->]
    //               FC(2) | Seq(1) | DstPan(2) | DstAddr(2) || FC(1) | ExtFC(0/1) | SrcId(0/4) | SecFc(0/4) | MIC(0/4) | <------GPDF(1/n)------>

    /* clang-format on */

    bool     isGpPkt           = false;
    uint8_t *gpFrameStartIndex = efr32GetPayload(aFrame);
    otEXPECT_ACTION(gpFrameStartIndex != NULL, isGpPkt = false);
    uint8_t fc = *gpFrameStartIndex;

    // Criteria:
    //  - The basic Identification of a GPDF Frame : The minimum GPDF length need to be 10 in this case for any
    //  direction
    //  - Network layer FC containing the Protocol Version field as 3.
    //  - The frame version should be 2003.

    bool lengthCheck         = (aFrame->mLength >= GP_MIN_MAINTENANCE_FRAME_LENGTH);
    bool networkVersionCheck = GP_NWK_PROTOCOL_VERSION_CHECK(fc);
    bool frameVersionCheck   = (efr32GetFrameVersion(aFrame) == IEEE802154_FRAME_VERSION_2003);

    isGpPkt = (lengthCheck && networkVersionCheck && frameVersionCheck);
#if 0 // Debugging
    if (!isGpPkt)
    {
        otLogCritPlat("GP RCP INTF checks: Length = %d, NWK Version = %d, PanId Compression = %d, Frame Version = %d",
                    lengthCheck,
                    networkVersionCheck,
                    panIdCompressionCheck,
                    frameVersionCheck);
    }
#endif
exit:
    return isGpPkt;
}
