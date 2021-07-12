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
 *   This file includes all compile-time configuration constants used by
 *   efr32 applications for OpenThread.
 */

#include "board_config.h"

// IMPORTANT: Do not remove this include. Apps will build without error, but
// they will not boot up properly
#include "em_msc.h"

#ifndef OPENTHREAD_CORE_EFR32_CONFIG_H_
#define OPENTHREAD_CORE_EFR32_CONFIG_H_

/**
 * @def OPENTHREAD_CONFIG_LOG_OUTPUT
 *
 * The efr32 platform provides an otPlatLog() function.
 */
#ifndef OPENTHREAD_CONFIG_LOG_OUTPUT /* allow command line override */
#define OPENTHREAD_CONFIG_LOG_OUTPUT OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED
#endif

/*
 * @def OPENTHREAD_CONFIG_RADIO_915MHZ_OQPSK_SUPPORT
 *
 * Define to 1 if you want to enable physical layer to support OQPSK modulation in 915MHz band.
 * (currently not supported).
 *
 */
#if RADIO_CONFIG_915MHZ_OQPSK_SUPPORT
#define OPENTHREAD_CONFIG_RADIO_915MHZ_OQPSK_SUPPORT 1
#else
#define OPENTHREAD_CONFIG_RADIO_915MHZ_OQPSK_SUPPORT 0
#endif

/*
 * @def OPENTHREAD_CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT
 *
 * Define to 1 if you want to enable physical layer to support OQPSK modulation in 2.4GHz band.
 *
 */
#if RADIO_CONFIG_2P4GHZ_OQPSK_SUPPORT
#define OPENTHREAD_CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT 1
#else
#define OPENTHREAD_CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT 0
#endif

/*
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_SUPPORT
 *
 * Define to 1 if you want to enable physical layer to support proprietary radio configurations.
 *
 * This configuration option is used by the Sub-GHz feature to specify proprietary radio parameters,
 * currently not defined by the Thread spec.
 */
#if RADIO_CONFIG_SUBGHZ_SUPPORT
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_SUPPORT 1
#else
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_SUPPORT 0
#endif

#if RADIO_CONFIG_SUBGHZ_SUPPORT
/**
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE
 *
 * Channel Page value for (proprietary) Sub-GHz PHY using 2GFSK modulation in 915MHz band.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE 23
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN
 *
 * Minimum Channel number supported with (proprietary) Sub-GHz PHY using 2GFSK modulation in 915MHz band.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN 0
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX
 *
 * Maximum Channel number supported with (proprietary) Sub-GHz PHY using 2GFSK modulation in 915MHz band.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX 24
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MASK
 *
 * Default Channel Mask for (proprietary) Sub-GHz PHY using 2GFSK modulation in 915MHz band.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MASK
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MASK 0x1ffffff
#endif

/**
 * @def OPENTHREAD_CONFIG_DEFAULT_CHANNEL
 *
 * Default channel to use when working with proprietary radio configurations.
 *
 */
#ifndef OPENTHREAD_CONFIG_DEFAULT_CHANNEL
#define OPENTHREAD_CONFIG_DEFAULT_CHANNEL OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN
#endif
#endif // RADIO_CONFIG_SUBGHZ_SUPPORT

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_INFO
 *
 * The platform-specific string to insert into the OpenThread version string.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_INFO
#define OPENTHREAD_CONFIG_PLATFORM_INFO "EFR32"
#endif

/*
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
 *
 * Define to 1 if you want to enable software retransmission logic.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE
 *
 * Define to 1 if you want to enable software CSMA-CA backoff logic.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE
 *
 * Define to 1 if you want to enable software transmission security logic.
 *
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE \
    (OPENTHREAD_RADIO && (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2))

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE
 *
 * Define to 1 to enable software transmission target time logic.
 *
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE \
    (OPENTHREAD_RADIO && (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2))

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_ENERGY_SCAN_ENABLE
 *
 * Define to 1 if you want to enable software energy scanning logic.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_ENERGY_SCAN_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_ENERGY_SCAN_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
 *
 * Define to 1 if you want to support microsecond timer in platform.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE \
    (OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE && (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2))

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
 *
 * Define to 1 to enable otPlatFlash* APIs to support non-volatile storage.
 *
 * When defined to 1, the platform MUST implement the otPlatFlash* APIs instead of the otPlatSettings* APIs.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
 *
 * Define to 1 to enable the NCP HDLC interface.
 *
 */
#ifndef OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
#define OPENTHREAD_CONFIG_NCP_HDLC_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MIN_SLEEP_DURATION_MS
 *
 * Minimum duration in ms below which the platform will not
 * enter a deep sleep (EM2) mode.
 *
 */
#define OPENTHREAD_CONFIG_MIN_SLEEP_DURATION_MS 5

/**
 * @def OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
 *
 * Enable the external heap.
 *
 */
#ifndef OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
#define OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_EFR32_UART_TX_FLUSH_TIMEOUT_MS
 *
 * Maximum time to wait for a flush to complete in otPlatUartFlush().
 *
 * Value is in milliseconds
 *
 */
#ifndef OPENTHREAD_CONFIG_EFR32_UART_TX_FLUSH_TIMEOUT_MS
#define OPENTHREAD_CONFIG_EFR32_UART_TX_FLUSH_TIMEOUT_MS 500
#endif

/**
 * @def OPENTHREAD_CONFIG_PSA_CRYPTO_ENABLE
 *
 * Define to 1 to enable PSA CRYPTO support.
 *
 */
#ifndef OPENTHREAD_CONFIG_PSA_CRYPTO_ENABLE
#define OPENTHREAD_CONFIG_PSA_CRYPTO_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_PSA_ITS_NVM_OFFSET
 *
 * This is the offset in ITS where the persistent keys are stored.
 * For Silabs OT applications, this needs to be in the range of
 * 0x20000 to 0x2ffff.
 *
 */
#define OPENTHREAD_CONFIG_PSA_ITS_NVM_OFFSET 0x20000

#endif // OPENTHREAD_CORE_EFR32_CONFIG_H_
