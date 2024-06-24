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
 * @brief
 *   This file includes the platform-specific initializers.
 */

#include <stdbool.h>

#include <openthread-core-config.h>
#include <openthread-system.h>

#include "common/logging.hpp"

#include "sl_system_init.h"

//==============================================================================
// Component Catalog includes
//==============================================================================
#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_system_kernel.h"
#else
#include "sl_system_process_action.h"
#endif

#if defined(SL_CATALOG_MEMORY_MANAGER_PRESENT)
#include "sl_memory_manager.h"
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

//==============================================================================
// PAL includes
//==============================================================================
#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
#include "sl_gp_interface.h"
#endif

#include "alarm.h"
#include "platform-efr32.h"

//==============================================================================
// Preprocessor macro definitions
//==============================================================================
#define USE_EFR32_LOG (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED)

#if defined(SL_CATALOG_OPENTHREAD_CLI_PRESENT) && defined(SL_CATALOG_KERNEL_PRESENT)
#define CLI_TASK_ENABLED (SL_OPENTHREAD_ENABLE_CLI_TASK)
#else
#define CLI_TASK_ENABLED (0)
#endif

//==============================================================================
// Forward declarations
//==============================================================================
static void efr32SerialProcess(void);

#if (OPENTHREAD_RADIO)
static void efr32NcpProcess(void);
#elif (CLI_TASK_ENABLED == 0)
static void efr32CliProcess(void);
#endif

//==============================================================================
// Global variables
//==============================================================================
otInstance *sInstance;

//==============================================================================
// Serial process helper functions
//==============================================================================
static void efr32SerialProcess(void)
{
#if (OPENTHREAD_RADIO)
    efr32NcpProcess();
#elif (CLI_TASK_ENABLED == 0)
    efr32CliProcess();
#endif // OPENTHREAD_RADIO0
}

#if (OPENTHREAD_RADIO)
static void efr32NcpProcess(void)
{
#if OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
    efr32UartProcess();
#elif OPENTHREAD_CONFIG_NCP_CPC_ENABLE
    efr32CpcProcess();
#elif OPENTHREAD_CONFIG_NCP_SPI_ENABLE
    efr32SpiProcess();
#endif
}
#elif (CLI_TASK_ENABLED == 0)
static void efr32CliProcess(void)
{
    efr32UartProcess();
}
#endif

//==============================================================================
// Weakly defined function definitions
//==============================================================================
OT_TOOL_WEAK void sl_openthread_init(void)
{
    // Placeholder for enabling Silabs specific features available only through Simplicity Studio
}

/**
 * @brief Application initialization
 */
OT_TOOL_WEAK void app_init(void)
{
    // Placeholder for any application specific initialization
}

OT_TOOL_WEAK void otSysEventSignalPending(void)
{
    // Intentionally empty
}

//==============================================================================
// Other function definitions
//==============================================================================
void otSysInit(int argc, char *argv[])
{
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
    // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
    // Note that if the kernel is present, processing task(s) will be created by
    // this call.
    sl_system_init();

    // Initialize the application. For example, create periodic timer(s) or
    // task(s) if the kernel is present.
    app_init();

    sl_ot_sys_init();
}

void sl_ot_sys_init(void)
{
    sl_openthread_init();

#if USE_EFR32_LOG
    efr32LogInit();
#endif
    efr32RadioInit();
    efr32AlarmInit();
    efr32MiscInit();
}

bool otSysPseudoResetWasRequested(void)
{
    return false;
}

void otSysDeinit(void)
{
    efr32RadioDeinit();

#if USE_EFR32_LOG
    efr32LogDeinit();
#endif
}

void otSysProcessDrivers(otInstance *aInstance)
{
    sInstance = aInstance;

    // should sleep and wait for interrupts here

#if !defined(SL_CATALOG_KERNEL_PRESENT)
    // Do not remove this call: Silicon Labs components process action routine
    // must be called from the super loop.
    sl_system_process_action();
#endif

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    efr32GpProcess();
#endif

    efr32SerialProcess();

    efr32RadioProcess(aInstance);

    // See alarm.c: Wrapped in a critical section
    efr32AlarmProcess(aInstance);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT)
    // Let the CPU go to sleep if the system allows it.
    sl_power_manager_sleep();
#endif
}

//==============================================================================
