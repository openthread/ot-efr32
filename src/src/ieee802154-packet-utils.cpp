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
 *   This file implements utility functions needed for 802.15.4 frame processing
 *
 */

#include "ieee802154-packet-utils.hpp"
#include "em_device.h"
#include "sl_core.h"
#include "sl_packet_utils.h"
#if defined(RADIOAES_PRESENT)
#include "sli_protocol_crypto.h"
#else
#include "sli_crypto.h"
#endif

#include <assert.h>
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "crypto/aes_ccm.hpp"
#include "mac/mac_frame.hpp"

using namespace ot;
using namespace Crypto;

#if defined(RADIOAES_PRESENT)
void TxSecurityProcessing::Init(uint32_t    aHeaderLength,
                                uint32_t    aPlainTextLength,
                                uint8_t     aTagLength,
                                const void *aNonce,
                                uint8_t     aNonceLength)
{
    const uint8_t *nonceBytes  = reinterpret_cast<const uint8_t *>(aNonce);
    uint8_t        blockLength = 0;
    uint32_t       len;
    uint8_t        L;
    uint8_t        i;

    // Tag length must be even and within [kMinTagLength, kMaxTagLength]
    OT_ASSERT(((aTagLength & 0x1) == 0) && (Crypto::AesCcm::kMinTagLength <= aTagLength)
              && (aTagLength <= Crypto::AesCcm::kMaxTagLength));

    L = 0;

    for (len = aPlainTextLength; len; len >>= 8)
    {
        L++;
    }

    if (L <= 1)
    {
        L = 2;
    }

    if (aNonceLength > 13)
    {
        aNonceLength = 13;
    }

    // increase L to match nonce len
    if (L < (15 - aNonceLength))
    {
        L = 15 - aNonceLength;
    }

    // decrease nonceLength to match L
    if (aNonceLength > (15 - L))
    {
        aNonceLength = 15 - L;
    }

    // setup initial block

    // write flags
    mBlock[0] = (static_cast<uint8_t>((aHeaderLength != 0) << 6) | static_cast<uint8_t>(((aTagLength - 2) >> 1) << 3)
                 | static_cast<uint8_t>(L - 1));

    // write nonce
    memcpy(&mBlock[1], nonceBytes, aNonceLength);

    // write len
    len = aPlainTextLength;

    for (i = sizeof(mBlock) - 1; i > aNonceLength; i--)
    {
        mBlock[i] = len & 0xff;
        len >>= 8;
    }

    // encrypt initial block
    sli_aes_crypt_ecb_radio(true, mKey, kKeyBits, mBlock, mBlock);

    // process header
    if (aHeaderLength > 0)
    {
        // process length
        if (aHeaderLength < (65536U - 256U))
        {
            mBlock[blockLength++] ^= aHeaderLength >> 8;
            mBlock[blockLength++] ^= aHeaderLength >> 0;
        }
        else
        {
            mBlock[blockLength++] ^= 0xff;
            mBlock[blockLength++] ^= 0xfe;
            mBlock[blockLength++] ^= aHeaderLength >> 24;
            mBlock[blockLength++] ^= aHeaderLength >> 16;
            mBlock[blockLength++] ^= aHeaderLength >> 8;
            mBlock[blockLength++] ^= aHeaderLength >> 0;
        }
    }

    // init counter
    mCtr[0] = L - 1;
    memcpy(&mCtr[1], nonceBytes, aNonceLength);
    memset(&mCtr[aNonceLength + 1], 0, sizeof(mCtr) - aNonceLength - 1);

    mNonceLength     = aNonceLength;
    mHeaderLength    = aHeaderLength;
    mHeaderCur       = 0;
    mPlainTextLength = aPlainTextLength;
    mPlainTextCur    = 0;
    mBlockLength     = blockLength;
    mCtrLength       = sizeof(mCtrPad);
    mTagLength       = aTagLength;
}

void TxSecurityProcessing::Header(const void *aHeader, uint32_t aHeaderLength)
{
    const uint8_t *headerBytes = reinterpret_cast<const uint8_t *>(aHeader);

    OT_ASSERT(mHeaderCur + aHeaderLength <= mHeaderLength);

    // process header
    for (unsigned i = 0; i < aHeaderLength; i++)
    {
        if (mBlockLength == sizeof(mBlock))
        {
            sli_aes_crypt_ecb_radio(true, mKey, kKeyBits, mBlock, mBlock);

            mBlockLength = 0;
        }

        mBlock[mBlockLength++] ^= headerBytes[i];
    }

    mHeaderCur += aHeaderLength;

    if (mHeaderCur == mHeaderLength)
    {
        // process remainder
        if (mBlockLength != 0)
        {
            sli_aes_crypt_ecb_radio(true, mKey, kKeyBits, mBlock, mBlock);
        }

        mBlockLength = 0;
    }
}

void TxSecurityProcessing::Payload(void *aPlainText, void *aCipherText, uint32_t aLength)
{
    uint8_t *plaintextBytes  = reinterpret_cast<uint8_t *>(aPlainText);
    uint8_t *ciphertextBytes = reinterpret_cast<uint8_t *>(aCipherText);
    uint8_t  byte;

    OT_ASSERT(mPlainTextCur + aLength <= mPlainTextLength);

    for (unsigned i = 0; i < aLength; i++)
    {
        if (mCtrLength == 16)
        {
            for (int j = sizeof(mCtr) - 1; j > mNonceLength; j--)
            {
                if (++mCtr[j])
                {
                    break;
                }
            }

            sli_aes_crypt_ecb_radio(true, mKey, kKeyBits, mCtr, mCtrPad);

            mCtrLength = 0;
        }

        byte               = plaintextBytes[i];
        ciphertextBytes[i] = byte ^ mCtrPad[mCtrLength++];

        if (mBlockLength == sizeof(mBlock))
        {
            sli_aes_crypt_ecb_radio(true, mKey, kKeyBits, mBlock, mBlock);

            mBlockLength = 0;
        }

        mBlock[mBlockLength++] ^= byte;
    }

    mPlainTextCur += aLength;

    if (mPlainTextCur >= mPlainTextLength)
    {
        if (mBlockLength != 0)
        {
            sli_aes_crypt_ecb_radio(true, mKey, kKeyBits, mBlock, mBlock);
        }

        // reset counter
        memset(&mCtr[mNonceLength + 1], 0, sizeof(mCtr) - mNonceLength - 1);
    }
}

void TxSecurityProcessing::Finalize(void *aTag)
{
    uint8_t *tagBytes = reinterpret_cast<uint8_t *>(aTag);

    OT_ASSERT(mPlainTextCur == mPlainTextLength);

    sli_aes_crypt_ecb_radio(true, mKey, kKeyBits, mCtr, mCtrPad);

    for (int i = 0; i < mTagLength; i++)
    {
        tagBytes[i] = mBlock[i] ^ mCtrPad[i];
    }
}
#endif

#if defined(LPWAES_PRESENT)
static inline void efr32CreateKeyDesc(const otMacKeyMaterial *key, sli_crypto_descriptor_t *key_desc)
{
    key_desc->location                         = SLI_CRYPTO_KEY_LOCATION_PLAINTEXT;
    key_desc->engine                           = SLI_CRYPTO_LPWAES;
    key_desc->key.plaintext_key.buffer.pointer = (uint8_t *)key->mKeyMaterial.mKey.m8;
    key_desc->key.plaintext_key.buffer.size    = OT_MAC_KEY_SIZE;
    key_desc->key.plaintext_key.key_size       = OT_MAC_KEY_SIZE;
}
#endif

void efr32PlatProcessTransmitAesCcm(otRadioFrame *aFrame, const otExtAddress *aExtAddress)
{
#if (OPENTHREAD_RADIO && (OPENTHREAD_CONFIG_THREAD_VERSION < OT_THREAD_VERSION_1_2))
    OT_UNUSED_VARIABLE(aFrame);
    OT_UNUSED_VARIABLE(aExtAddress);
#else

    uint32_t      frameCounter = 0;
    uint8_t       tagLength;
    uint8_t       securityLevel;
    uint8_t       nonce[Crypto::AesCcm::kNonceSize];
    Mac::TxFrame *aTxFrame = static_cast<Mac::TxFrame *>(aFrame);

    VerifyOrExit(aTxFrame->GetSecurityEnabled());

    SuccessOrExit(aTxFrame->GetSecurityLevel(securityLevel));
    SuccessOrExit(aTxFrame->GetFrameCounter(frameCounter));

    Crypto::AesCcm::GenerateNonce(*static_cast<const Mac::ExtAddress *>(aExtAddress),
                                  frameCounter,
                                  securityLevel,
                                  nonce);

    tagLength = aTxFrame->GetFooterLength() - aTxFrame->GetFcsSize();

#if defined(RADIOAES_PRESENT)
    TxSecurityProcessing packetSecurityHandler;

    packetSecurityHandler.SetKey(aFrame->mInfo.mTxInfo.mAesKey->mKeyMaterial.mKey.m8);
    packetSecurityHandler.Init(aTxFrame->GetHeaderLength(),
                               aTxFrame->GetPayloadLength(),
                               tagLength,
                               nonce,
                               sizeof(nonce));
    packetSecurityHandler.Header(aTxFrame->GetHeader(), aTxFrame->GetHeaderLength());
    packetSecurityHandler.Payload(aTxFrame->GetPayload(), aTxFrame->GetPayload(), aTxFrame->GetPayloadLength());
    packetSecurityHandler.Finalize(aTxFrame->GetFooter());

#elif defined(LPWAES_PRESENT)
    sli_crypto_descriptor_t key_desc;
    sl_status_t             ret;

    efr32CreateKeyDesc(aFrame->mInfo.mTxInfo.mAesKey, &key_desc);

    ret =
        sli_crypto_ccm(&key_desc,
                       true,
                       ((securityLevel >= Mac::Frame::SecurityLevel::kSecurityEnc) ? aTxFrame->GetPayload() : NULL),
                       ((securityLevel >= Mac::Frame::SecurityLevel::kSecurityEnc) ? aTxFrame->GetPayloadLength() : 0),
                       aTxFrame->GetPayload(),
                       nonce,
                       sizeof(nonce),
                       aTxFrame->GetHeader(),
                       aTxFrame->GetHeaderLength(),
                       aTxFrame->GetPayload() + aTxFrame->GetPayloadLength(),
                       tagLength);

    OT_ASSERT(ret == SL_STATUS_OK);
#endif

    aTxFrame->SetIsSecurityProcessed(true);

exit:
    return;
#endif // OPENTHREAD_RADIO && (OPENTHREAD_CONFIG_THREAD_VERSION < OT_THREAD_VERSION_1_2))
}

bool efr32IsFramePending(otRadioFrame *aFrame)
{
    return static_cast<Mac::RxFrame *>(aFrame)->GetFramePending();
}

otPanId efr32GetDstPanId(otRadioFrame *aFrame)
{
    otPanId aPanId = 0xFFFF;

    if (static_cast<Mac::RxFrame *>(aFrame)->IsDstPanIdPresent())
    {
        static_cast<Mac::RxFrame *>(aFrame)->GetDstPanId(aPanId);
    }

    return aPanId;
}

uint8_t *efr32GetPayload(otRadioFrame *aFrame)
{
    uint8_t *payload = static_cast<Mac::RxFrame *>(aFrame)->GetPayload();
    return payload;
}

bool efr32FrameIsPanIdCompressed(otRadioFrame *aFrame)
{
    return static_cast<Mac::RxFrame *>(aFrame)->IsPanIdCompressed();
}

uint16_t efr32GetFrameVersion(otRadioFrame *aFrame)
{
    return static_cast<Mac::RxFrame *>(aFrame)->GetVersion();
}
