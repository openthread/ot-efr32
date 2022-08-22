/*
 *  Copyright (c) 2022, The OpenThread Authors.
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

/**
 * @file
 *   This file implements the OpenThread platform abstraction for the diagnostics.
 *
 */

#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE

#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>

#include "platform-efr32.h"
#include "rail_ieee802154.h"
#include <openthread/cli.h>
#include <openthread/config.h>
#include <openthread/logging.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>

#if OPENTHREAD_CONFIG_DIAG_ENABLE

/**
 * Diagnostics mode variables.
 *
 */
static bool sDiagMode = false;

void otPlatDiagModeSet(bool aMode)
{
    sDiagMode = aMode;
}

bool otPlatDiagModeGet()
{
    return sDiagMode;
}

void otPlatDiagChannelSet(uint8_t aChannel)
{
    OT_UNUSED_VARIABLE(aChannel);
}

void otPlatDiagTxPowerSet(int8_t aTxPower)
{
    OT_UNUSED_VARIABLE(aTxPower);
}

void otPlatDiagRadioReceived(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aFrame);
    OT_UNUSED_VARIABLE(aError);
}

void otPlatDiagAlarmCallback(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

otError otPlatDiagTxStreamRandom(void)
{
    RAIL_Status_t status;
    uint16_t      streamChannel;

    RAIL_GetChannel(gRailHandle, &streamChannel);

    otLogInfoPlat("Diag Stream PN9 Process", NULL);

    status = RAIL_StartTxStream(gRailHandle, streamChannel, RAIL_STREAM_PN9_STREAM);
    assert(status == RAIL_STATUS_NO_ERROR);

    return status;
}

otError otPlatDiagTxStreamTone(void)
{
    RAIL_Status_t status;
    uint16_t      streamChannel;

    RAIL_GetChannel(gRailHandle, &streamChannel);

    otLogInfoPlat("Diag Stream CARRIER-WAVE Process", NULL);

    status = RAIL_StartTxStream(gRailHandle, streamChannel, RAIL_STREAM_CARRIER_WAVE);
    assert(status == RAIL_STATUS_NO_ERROR);

    return status;
}

otError otPlatDiagTxStreamStop(void)
{
    RAIL_Status_t status;

    otLogInfoPlat("Diag Stream STOP Process", NULL);

    status = RAIL_StopTxStream(gRailHandle);
    assert(status == RAIL_STATUS_NO_ERROR);

    return status;
}

otError otPlatDiagTxStreamAddrMatch(uint8_t enable)
{
    RAIL_Status_t status;

    otLogInfoPlat("Diag Stream Disable addressMatch", NULL);

    status = RAIL_IEEE802154_SetPromiscuousMode(gRailHandle, !enable);
    assert(status == RAIL_STATUS_NO_ERROR);

    return status;
}

otError otPlatDiagTxStreamAutoAck(uint8_t autoAckEnabled)
{
    RAIL_Status_t status = RAIL_STATUS_NO_ERROR;

    otLogInfoPlat("Diag Stream Disable autoAck", NULL);

    RAIL_PauseRxAutoAck(gRailHandle, !autoAckEnabled);

    return status;
}

#endif // #if OPENTHREAD_CONFIG_DIAG_ENABLE
