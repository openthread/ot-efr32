/*
 *  Copyright (c) 2023, The OpenThread Authors.
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
 *   This file adds additional functions to the OpenThread diagnostics module
 *
 */

#include <stdio.h>
#include <string.h>

#include <common/code_utils.hpp>
#include <openthread-core-config.h>
#include <utils/code_utils.h>
#include <openthread/config.h>
#include <openthread/instance.h>
#include <openthread/platform/diag.h>

#include "rail_types.h"

#if OPENTHREAD_CONFIG_DIAG_ENABLE

struct PlatformDiagCommand
{
    const char *mName;
    otError (*mCommand)(otInstance *aInstance, uint8_t aArgsLength, char *aArgs[], char *aOutput, size_t aOutputMaxLen);
};

// *****************************************************************************
// Helper functions
// *****************************************************************************
static void appendErrorResult(otError aError, char *aOutput, size_t aOutputMaxLen)
{
    if (aError != OT_ERROR_NONE)
    {
        snprintf(aOutput, aOutputMaxLen, "failed\r\nstatus %#x\r\n", aError);
    }
}

// *****************************************************************************
// CLI functions
// *****************************************************************************

static otError processVendorVersion(otInstance *aInstance,
                                    uint8_t     aArgsLength,
                                    char       *aArgs[],
                                    char       *aOutput,
                                    size_t      aOutputMaxLen)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aArgs);

    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(otPlatDiagModeGet(), error = OT_ERROR_INVALID_STATE);

    otEXPECT_ACTION(aArgsLength == 0, error = OT_ERROR_INVALID_ARGS);

    snprintf(aOutput, aOutputMaxLen, "1.0.0.0\r\n");

exit:
    appendErrorResult(error, aOutput, aOutputMaxLen);
    return error;
}

// *****************************************************************************
const struct PlatformDiagCommand sCommands[] = {
    {"vendorversion", &processVendorVersion},
};

otError otPlatDiagProcess(otInstance *aInstance,
                          uint8_t     aArgsLength,
                          char       *aArgs[],
                          char       *aOutput,
                          size_t      aOutputMaxLen)
{
    otError error = OT_ERROR_INVALID_COMMAND;
    size_t  i;

    for (i = 0; i < otARRAY_LENGTH(sCommands); i++)
    {
        if (strcmp(aArgs[0], sCommands[i].mName) == 0)
        {
            error = sCommands[i].mCommand(
                aInstance, aArgsLength - 1, aArgsLength > 1 ? &aArgs[1] : NULL, aOutput, aOutputMaxLen);
            break;
        }
    }

    return error;
}

#endif // #if OPENTHREAD_CONFIG_DIAG_ENABLE
