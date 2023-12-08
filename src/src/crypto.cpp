

#include <string.h>

#include <mbedtls/cmac.h>
#include <mbedtls/version.h>

#include <openthread/platform/crypto.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/error.hpp"
#include "common/num_utils.hpp"
#include "crypto/mbedtls.hpp"

using namespace ot;
using namespace Crypto;

otError otPlatCryptoPbkdf2GenerateKey(const uint8_t *aPassword,
                                      uint16_t       aPasswordLen,
                                      const uint8_t *aSalt,
                                      uint16_t       aSaltLen,
                                      uint32_t       aIterationCounter,
                                      uint16_t       aKeyLen,
                                      uint8_t       *aKey)
{
#if (MBEDTLS_VERSION_NUMBER >= 0x03050000)
    const size_t kBlockSize = MBEDTLS_CMAC_MAX_BLOCK_SIZE;
#else
    const size_t kBlockSize = MBEDTLS_CIPHER_BLKSIZE_MAX;
#endif
    uint8_t  prfInput[OT_CRYPTO_PBDKF2_MAX_SALT_SIZE + 4]; // Salt || INT(), for U1 calculation
    long     prfOne[kBlockSize / sizeof(long)];
    long     prfTwo[kBlockSize / sizeof(long)];
    long     keyBlock[kBlockSize / sizeof(long)];
    uint32_t blockCounter = 0;
    uint8_t *key          = aKey;
    uint16_t keyLen       = aKeyLen;
    uint16_t useLen       = 0;
    Error    error        = kErrorNone;
    int      ret;

    OT_ASSERT(aSaltLen <= sizeof(prfInput));
    memcpy(prfInput, aSalt, aSaltLen);
    OT_ASSERT(aIterationCounter % 2 == 0);
    aIterationCounter /= 2;

#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    // limit iterations to avoid OSS-Fuzz timeouts
    aIterationCounter = 2;
#endif

    while (keyLen)
    {
        ++blockCounter;
        prfInput[aSaltLen + 0] = static_cast<uint8_t>(blockCounter >> 24);
        prfInput[aSaltLen + 1] = static_cast<uint8_t>(blockCounter >> 16);
        prfInput[aSaltLen + 2] = static_cast<uint8_t>(blockCounter >> 8);
        prfInput[aSaltLen + 3] = static_cast<uint8_t>(blockCounter);

        // Calculate U_1
        ret = mbedtls_aes_cmac_prf_128(aPassword,
                                       aPasswordLen,
                                       prfInput,
                                       aSaltLen + 4,
                                       reinterpret_cast<uint8_t *>(keyBlock));
        VerifyOrExit(ret == 0, error = MbedTls::MapError(ret));

        // Calculate U_2
        ret = mbedtls_aes_cmac_prf_128(aPassword,
                                       aPasswordLen,
                                       reinterpret_cast<const uint8_t *>(keyBlock),
                                       kBlockSize,
                                       reinterpret_cast<uint8_t *>(prfOne));
        VerifyOrExit(ret == 0, error = MbedTls::MapError(ret));

        for (uint32_t j = 0; j < kBlockSize / sizeof(long); ++j)
        {
            keyBlock[j] ^= prfOne[j];
        }

        for (uint32_t i = 1; i < aIterationCounter; ++i)
        {
            // Calculate U_{2 * i - 1}
            ret = mbedtls_aes_cmac_prf_128(aPassword,
                                           aPasswordLen,
                                           reinterpret_cast<const uint8_t *>(prfOne),
                                           kBlockSize,
                                           reinterpret_cast<uint8_t *>(prfTwo));
            VerifyOrExit(ret == 0, error = MbedTls::MapError(ret));
            // Calculate U_{2 * i}
            ret = mbedtls_aes_cmac_prf_128(aPassword,
                                           aPasswordLen,
                                           reinterpret_cast<const uint8_t *>(prfTwo),
                                           kBlockSize,
                                           reinterpret_cast<uint8_t *>(prfOne));
            VerifyOrExit(ret == 0, error = MbedTls::MapError(ret));

            for (uint32_t j = 0; j < kBlockSize / sizeof(long); ++j)
            {
                keyBlock[j] ^= prfOne[j] ^ prfTwo[j];
            }
        }

        useLen = Min(keyLen, static_cast<uint16_t>(kBlockSize));
        memcpy(key, keyBlock, useLen);
        key += useLen;
        keyLen -= useLen;
    }

exit:
    return error;
}
