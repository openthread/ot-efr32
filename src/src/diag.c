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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>

#include "platform-efr32.h"
#include "rail_ieee802154.h"
#include <openthread-core-config.h>
#include <openthread/cli.h>
#include <openthread/config.h>
#include <openthread/logging.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>
#include "common/code_utils.hpp"

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
#include "coexistence-802154.h"
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

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

    otLogInfoPlat("Diag Stream PN9 Process");

    status = RAIL_StartTxStream(gRailHandle, streamChannel, RAIL_STREAM_PN9_STREAM);
    assert(status == RAIL_STATUS_NO_ERROR);

    return railStatusToOtError(status);
}

otError otPlatDiagTxStreamTone(void)
{
    RAIL_Status_t status;
    uint16_t      streamChannel;

    RAIL_GetChannel(gRailHandle, &streamChannel);

    otLogInfoPlat("Diag Stream CARRIER-WAVE Process");

    status = RAIL_StartTxStream(gRailHandle, streamChannel, RAIL_STREAM_CARRIER_WAVE);
    assert(status == RAIL_STATUS_NO_ERROR);

    return railStatusToOtError(status);
}

otError otPlatDiagTxStreamStop(void)
{
    RAIL_Status_t status;

    otLogInfoPlat("Diag Stream STOP Process");

    status = RAIL_StopTxStream(gRailHandle);
    assert(status == RAIL_STATUS_NO_ERROR);

    return railStatusToOtError(status);
}

otError otPlatDiagTxStreamAddrMatch(uint8_t enable)
{
    RAIL_Status_t status;

    otLogInfoPlat("Diag Stream Disable addressMatch");

    status = RAIL_IEEE802154_SetPromiscuousMode(gRailHandle, !enable);
    assert(status == RAIL_STATUS_NO_ERROR);

    return railStatusToOtError(status);
}

otError otPlatDiagTxStreamAutoAck(uint8_t autoAckEnabled)
{
    otLogInfoPlat("Diag Stream Disable autoAck");

    RAIL_PauseRxAutoAck(gRailHandle, !autoAckEnabled);

    return OT_ERROR_NONE;
}

// coex
otError otPlatDiagCoexSetPriorityPulseWidth(uint8_t pulseWidthUs)
{
    OT_UNUSED_VARIABLE(pulseWidthUs);
    otError error = OT_ERROR_FAILED;

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    // Actual call on rcp side
    sl_status_t status = sl_rail_util_coex_set_directional_priority_pulse_width(pulseWidthUs);

    error = (status != SL_STATUS_OK) ? OT_ERROR_FAILED : OT_ERROR_NONE;
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    return error;
}

otError otPlatDiagCoexSetRadioHoldoff(bool enabled)
{
    OT_UNUSED_VARIABLE(enabled);
    otError error = OT_ERROR_FAILED;

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    // Actual call on rcp side
    sl_status_t status = sl_rail_util_coex_set_radio_holdoff(enabled);

    error = (status != SL_STATUS_OK) ? OT_ERROR_FAILED : OT_ERROR_NONE;
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    return error;
}

otError otPlatDiagCoexSetRequestPwm(uint8_t ptaReq, void *ptaCb, uint8_t dutyCycle, uint8_t periodHalfMs)
{
    OT_UNUSED_VARIABLE(ptaReq);
    OT_UNUSED_VARIABLE(ptaCb);
    OT_UNUSED_VARIABLE(dutyCycle);
    OT_UNUSED_VARIABLE(periodHalfMs);
    otError error = OT_ERROR_FAILED;

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    sl_status_t status = sl_rail_util_coex_set_request_pwm(ptaReq, NULL, dutyCycle, periodHalfMs);
    error              = (status != SL_STATUS_OK) ? OT_ERROR_FAILED : OT_ERROR_NONE;
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    return error;
}
otError otPlatDiagCoexSetPhySelectTimeout(uint8_t timeoutMs)
{
    OT_UNUSED_VARIABLE(timeoutMs);
    otError error = OT_ERROR_FAILED;

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    sl_status_t status = sl_rail_util_coex_set_phy_select_timeout(timeoutMs);
    error              = (status != SL_STATUS_OK) ? OT_ERROR_FAILED : OT_ERROR_NONE;
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    return error;
}

otError otPlatDiagCoexSetOptions(uint32_t options)
{
    OT_UNUSED_VARIABLE(options);
    otError error = OT_ERROR_FAILED;

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    sl_status_t status = sl_rail_util_coex_set_options(options);
    error              = (status != SL_STATUS_OK) ? OT_ERROR_FAILED : OT_ERROR_NONE;
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    return error;
}

otError otPlatDiagCoexGetPhySelectTimeout(uint8_t *timeoutMs)
{
    OT_UNUSED_VARIABLE(timeoutMs);
    otError error = OT_ERROR_FAILED;

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    if (timeoutMs == NULL)
    {
        return OT_ERROR_INVALID_ARGS;
    }

    *timeoutMs = sl_rail_util_coex_get_phy_select_timeout();
    error      = OT_ERROR_NONE;
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    return error;
}

otError otPlatDiagCoexGetOptions(uint32_t *options)
{
    OT_UNUSED_VARIABLE(options);
    otError error = OT_ERROR_FAILED;

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    if (options == NULL)
    {
        return OT_ERROR_INVALID_ARGS;
    }

    *options = sl_rail_util_coex_get_options();
    error    = OT_ERROR_NONE;
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    return error;
}

otError otPlatDiagCoexGetPriorityPulseWidth(uint8_t *pulseWidthUs)
{
    OT_UNUSED_VARIABLE(pulseWidthUs);
    otError error = OT_ERROR_FAILED;

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    if (pulseWidthUs == NULL)
    {
        return OT_ERROR_INVALID_ARGS;
    }

    *pulseWidthUs = sl_rail_util_coex_get_directional_priority_pulse_width();
    error         = OT_ERROR_NONE;
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    return error;
}

otError otPlatDiagCoexGetRequestPwmArgs(uint8_t *req, uint8_t *dutyCycle, uint8_t *periodHalfMs)
{
    OT_UNUSED_VARIABLE(req);
    OT_UNUSED_VARIABLE(dutyCycle);
    OT_UNUSED_VARIABLE(periodHalfMs);
    otError error = OT_ERROR_NONE;

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    if (req == NULL || dutyCycle == NULL || periodHalfMs == NULL)
    {
        return OT_ERROR_INVALID_ARGS;
    }

    const sl_rail_util_coex_pwm_args_t *p = sl_rail_util_coex_get_request_pwm_args();
    *req                                  = p->req;
    *dutyCycle                            = p->dutyCycle;
    *periodHalfMs                         = p->periodHalfMs;
    error                                 = OT_ERROR_NONE;
#else
    error = OT_ERROR_FAILED;
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    return error;
}
#endif // #if OPENTHREAD_CONFIG_DIAG_ENABLE
