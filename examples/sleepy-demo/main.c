/*******************************************************************************
 * @file
 * @brief main() function.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "app.h"
#include "sl_component_catalog.h"
#include "sl_main_init.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif // SL_CATALOG_POWER_MANAGER_PRESENT
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_main_kernel.h"
#else // !SL_CATALOG_KERNEL_PRESENT
#include "sl_main_process_action.h"
#endif // SL_CATALOG_KERNEL_PRESENT

int main(void)
{
    // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
    // Note that if the kernel is present, the start task will be started and
    // software component configuration will take place there.
    sl_main_init();

    // Initialize the application. For example, create periodic timer(s) or
    // task(s) if the kernel is present.
    app_init();

#if defined(SL_CATALOG_KERNEL_PRESENT)
    // Start the kernel. The start task will be executed (Highest priority) to
    // complete the Simplicity SDK components initialization and the user
    // app_init() hook function will be called.
    sl_main_kernel_start();
#else // SL_CATALOG_KERNEL_PRESENT
    while (1)
    {
        // Silicon Labs components process action routine
        // must be called from the super loop.
        sl_main_process_action();

        // Application process.
        app_process_action();

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
        // Let the CPU go to sleep if the system allows it.
        sl_power_manager_sleep();
#endif
    }
    // Clean-up when exiting the application.
    app_exit();
#endif // SL_CATALOG_KERNEL_PRESENT
}
