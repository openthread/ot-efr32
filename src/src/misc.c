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

/**
 * @file
 *   This file implements the OpenThread platform abstraction for miscellaneous behaviors.
 */

#include <openthread-core-config.h>
#include <openthread/platform/misc.h>

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT)
#include "btl_interface.h"
#endif

#include "em_rmu.h"
#include "platform-efr32.h"

static uint32_t sResetCause;

void efr32MiscInit(void)
{
    // Read the cause of last reset.
    sResetCause = RMU_ResetCauseGet();

    // Clear the register, as the causes cumulate over resets.
    RMU_ResetCauseClear();
}

void otPlatReset(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    NVIC_SystemReset();
}

OT_TOOL_WEAK void bootloader_rebootAndInstall(void)
{
    // Weak stub function
    // This should be discarded in favor of the function definition in bootloader_interface code, when that component is
    // used
}

otError otPlatResetToBootloader(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    bootloader_rebootAndInstall();

    // This should only be reached if the bootloader_interface component is not present.
    // When it is present, the stubbed bootloader_rebootAndInstall above is not used.
    // Instead, the non-weak definition of the function in the component is used, causing
    // the device to reset.
    return OT_ERROR_NOT_CAPABLE;
}

otPlatResetReason otPlatGetResetReason(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    otPlatResetReason reason = OT_PLAT_RESET_REASON_UNKNOWN;

#if defined(_EMU_RSTCAUSE_MASK)
    if (sResetCause & EMU_RSTCAUSE_POR)
    {
        reason = OT_PLAT_RESET_REASON_POWER_ON;
    }
    else if (sResetCause & EMU_RSTCAUSE_SYSREQ)
    {
        reason = OT_PLAT_RESET_REASON_SOFTWARE;
    }
    else if ((sResetCause & EMU_RSTCAUSE_WDOG0) || (sResetCause & EMU_RSTCAUSE_WDOG1))
    {
        reason = OT_PLAT_RESET_REASON_WATCHDOG;
    }
    else if (sResetCause & EMU_RSTCAUSE_PIN)
    {
        reason = OT_PLAT_RESET_REASON_EXTERNAL;
    }
    else if (sResetCause & EMU_RSTCAUSE_LOCKUP)
    {
        reason = OT_PLAT_RESET_REASON_FAULT;
    }
    /* clang-format off */
    else if ((sResetCause & EMU_RSTCAUSE_AVDDBOD)
             || (sResetCause & EMU_RSTCAUSE_DECBOD)
             || (sResetCause & EMU_RSTCAUSE_DVDDBOD)
             || (sResetCause & EMU_RSTCAUSE_DVDDLEBOD)
             || (sResetCause & EMU_RSTCAUSE_EM4))
    /* clang-format on */
    {
        reason = OT_PLAT_RESET_REASON_OTHER;
    }
#endif
#if defined(_RMU_RSTCAUSE_MASK)
    if (sResetCause & RMU_RSTCAUSE_PORST)
    {
        reason = OT_PLAT_RESET_REASON_POWER_ON;
    }
    else if (sResetCause & RMU_RSTCAUSE_SYSREQRST)
    {
        reason = OT_PLAT_RESET_REASON_SOFTWARE;
    }
    else if (sResetCause & RMU_RSTCAUSE_WDOGRST)
    {
        reason = OT_PLAT_RESET_REASON_WATCHDOG;
    }
    else if (sResetCause & RMU_RSTCAUSE_EXTRST)
    {
        reason = OT_PLAT_RESET_REASON_EXTERNAL;
    }
    else if (sResetCause & RMU_RSTCAUSE_LOCKUPRST)
    {
        reason = OT_PLAT_RESET_REASON_FAULT;
    }
    /* clang-format off */
    else if ((sResetCause & RMU_RSTCAUSE_AVDDBOD)
             || (sResetCause & RMU_RSTCAUSE_DECBOD)
             || (sResetCause & RMU_RSTCAUSE_DVDDBOD)
             || (sResetCause & RMU_RSTCAUSE_EM4RST))
    /* clang-format on */
    {
        reason = OT_PLAT_RESET_REASON_OTHER;
    }
#endif
    return reason;
}

void otPlatWakeHost(void)
{
    // TODO: implement an operation to wake the host from sleep state.
}

OT_TOOL_WEAK void otCliOutputFormat(const char *aFmt, ...)
{
    OT_UNUSED_VARIABLE(aFmt);

    // do nothing
}

OT_TOOL_WEAK void otCliPlatLogv(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, va_list aArgs)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);
    OT_UNUSED_VARIABLE(aFormat);
    OT_UNUSED_VARIABLE(aArgs);

    // do nothing
}

OT_TOOL_WEAK void efr32UartProcess(void)
{
    // do nothing
}

otError railStatusToOtError(RAIL_Status_t status)
{
    switch (status)
    {
    case RAIL_STATUS_NO_ERROR:
        return OT_ERROR_NONE;
    case RAIL_STATUS_INVALID_PARAMETER:
        return OT_ERROR_INVALID_ARGS;
    case RAIL_STATUS_INVALID_STATE:
        return OT_ERROR_INVALID_STATE;
    case RAIL_STATUS_INVALID_CALL:
    case RAIL_STATUS_SUSPENDED:
    case RAIL_STATUS_SCHED_ERROR:
    default:
        return OT_ERROR_FAILED;
    }
}

#if OPENTHREAD_CONFIG_PLATFORM_LOG_CRASH_DUMP_ENABLE
otError otPlatLogCrashDump(void)
{
    otError error = OT_ERROR_NONE;

#if defined(SL_CATALOG_OT_CRASH_HANDLER_PRESENT)
    efr32PrintResetInfo();
#else
    error = OT_ERROR_NOT_CAPABLE;
#endif

    return error;
}
#endif
