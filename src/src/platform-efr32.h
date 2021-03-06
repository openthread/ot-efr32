/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file includes the platform-specific initializers.
 *
 */

#ifndef PLATFORM_EFR32_H_
#define PLATFORM_EFR32_H_

#include <openthread/instance.h>

#include "em_device.h"
#include "em_system.h"

#include "rail.h"

// Global OpenThread instance structure
extern otInstance *sInstance;

// Global reference to rail handle
extern RAIL_Handle_t emPhyRailHandle; // coex needs the emPhyRailHandle symbol.
#define gRailHandle emPhyRailHandle   // use gRailHandle in the OpenThread PAL.

/**
 * This function performs all platform-specific initialization of
 * OpenThread's drivers.
 *
 */
void sl_ot_sys_init(void);

/**
 * This function initializes the alarm service used by OpenThread.
 *
 */
void efr32AlarmInit(void);

/**
 * This function provides the remaining time (in milliseconds) on an alarm service.
 *
 */
uint32_t efr32AlarmPendingTime(void);

/**
 * This function checks if the alarm service is running.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
bool efr32AlarmIsRunning(otInstance *aInstance);

/**
 * This function performs alarm driver processing.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
void efr32AlarmProcess(otInstance *aInstance);

/**
 * This function initializes the radio service used by OpenThead.
 *
 */
void efr32RadioInit(void);

/**
 * This function deinitializes the radio service used by OpenThead.
 *
 */
void efr32RadioDeinit(void);

/**
 * This function performs radio driver processing.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
void efr32RadioProcess(otInstance *aInstance);

/**
 * This function performs UART driver processing.
 *
 */
void efr32UartProcess(void);

/**
 * This function performs CPC driver processing.
 *
 */
void efr32CpcProcess(void);

/**
 * Initialization of Misc module.
 *
 */
void efr32MiscInit(void);

/**
 * Initialization of Logger driver.
 *
 */
void efr32LogInit(void);

/**
 * Deinitialization of Logger driver.
 *
 */
void efr32LogDeinit(void);

/**
 * Registers the sleep callback handler.  The callback is used to check that
 * the application has no work pending and that it is safe to put the EFR32
 * into a low energy sleep mode.
 *
 * The callback should return true if it is ok to enter sleep mode. Note
 * that the callback itself is run with interrupts disabled and so should
 * be kept as short as possible.  Anny interrupt including those from timers
 * will wake the EFR32 out of sleep mode.
 *
 * @param[in]  aCallback  Callback function.
 *
 */
void efr32SetSleepCallback(bool (*aCallback)(void));

/**
 * Put the EFR32 into a low power mode.  Before sleeping it will call a callback
 * in the application registered with efr32SetSleepCallback to ensure that there
 * is no outstanding work in the application to do.
 */
void efr32Sleep(void);

#endif // PLATFORM_EFR32_H_
