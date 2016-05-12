/*
 * Copyright (c) 2013-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ################################################################################
 * #
 * # PATENT NOTICE
 * #
 * # This software is distributed under the BSD 2-clause License (see LICENSE
 * # file).  This BSD License does not make any patent claims and as such, does
 * # not act as a patent grant.  The purpose of this section is for each contributor
 * # to define their intentions with respect to intellectual property.
 * #
 * # Each contributor to this source code is encouraged to state their patent
 * # claims and licensing mechanisms for any contributions made. At the end of
 * # this section contributors may each make their own statements.  Contributor's
 * # claims and grants only apply to the pieces (source code, programs, text,
 * # media, etc) that they have contributed directly to this software.
 * #
 * # There is no guarantee that this section is complete, up to date or accurate. It
 * # is up to the contributors to maintain their portion of this section and up to
 * # the user of the software to verify any claims herein.
 * #
 * # Do not remove this header notification.  The contents of this section must be
 * # present in all distributions of the software.  You may only modify your own
 * # intellectual property statements.  Please provide contact information.
 * 
 * - Palo Alto Research Center, Inc
 * This software distribution does not grant any rights to patents owned by Palo
 * Alto Research Center, Inc (PARC). Rights to these patents are available via
 * various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
 * intellectual property used by its contributions to this software. You may
 * contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
 */
/**
 * See SCTP for a discussion of CRC32C http://tools.ietf.org/html/rfc4960#appendix-B
 * It is also used by iSCSI and other protocols.
 *
 * CRC-32C uses an initial value of 0xFFFFFFFF and a final XOR value of 0xFFFFFFFF.
 *
 * @author Marc Mosko, Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>
#include <stdio.h>
#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>
#include <parc/security/parc_CryptoHasher.h>

#include <ccnx/common/internal/ccnx_ValidationFacadeV1.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.h>

#include <fcntl.h>
#include <errno.h>

typedef struct crc32_signer {
    PARCCryptoHasher *hasher;
} _CRC32Signer;

typedef struct crc32_verifier {
    PARCCryptoHasher *hasher;
} _CRC32Verifier;

bool
ccnxValidationCRC32C_Set(CCNxTlvDictionary *message)
{
    bool success = true;
    switch (ccnxTlvDictionary_GetSchemaVersion(message)) {
        case CCNxTlvDictionary_SchemaVersion_V1: {
            success &= ccnxTlvDictionary_PutInteger(message, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_CRYPTO_SUITE, PARCCryptoSuite_NULL_CRC32C);

            break;
        }

        default:
        trapIllegalValue(message, "Unknown schema version: %d", ccnxTlvDictionary_GetSchemaVersion(message));
    }
    return success;
}

bool
ccnxValidationCRC32C_Test(const CCNxTlvDictionary *message)
{
    switch (ccnxTlvDictionary_GetSchemaVersion(message)) {
        case CCNxTlvDictionary_SchemaVersion_V1: {
            if (ccnxTlvDictionary_IsValueInteger(message, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_CRYPTO_SUITE)) {
                uint64_t cryptosuite = ccnxTlvDictionary_GetInteger(message, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_CRYPTO_SUITE);
                return (cryptosuite == PARCCryptoSuite_NULL_CRC32C);
            }
            return false;
        }

        default:
        trapIllegalValue(message, "Unknown schema version: %d", ccnxTlvDictionary_GetSchemaVersion(message));
    }
    return false;
}

static bool
_crc32cSigner_Destructor(_CRC32Signer **interfaceContextPtr)
{
    _CRC32Signer *signer = *interfaceContextPtr;
    parcCryptoHasher_Release(&signer->hasher);
    return true;
}

parcObject_ImplementAcquire(_crc32CSigner, _CRC32Signer);
parcObject_ImplementRelease(_crc32CSigner, _CRC32Signer);

parcObject_Override(_CRC32Signer, PARCObject,
    .destructor = (PARCObjectDestructor *) _crc32cSigner_Destructor);

static bool
_crc32Verifier_Destructor(_CRC32Verifier **verifierPtr)
{
    _CRC32Verifier *verifier = (_CRC32Verifier *) *verifierPtr;

    parcCryptoHasher_Release(&(verifier->hasher));
    return true;
}

parcObject_ImplementAcquire(_crc32Verifier, _CRC32Verifier);
parcObject_ImplementRelease(_crc32Verifier, _CRC32Verifier);

parcObject_Override(_CRC32Verifier, PARCObject,
    .destructor = (PARCObjectDestructor *) _crc32Verifier_Destructor);

static PARCSignature *
_crc32Signer_SignDigest(_CRC32Signer *interfaceContext, const PARCCryptoHash *cryptoHash)
{
    PARCSignature *signature =
            parcSignature_Create(PARCSigningAlgortihm_NULL, PARC_HASH_CRC32C, parcCryptoHash_GetDigest(cryptoHash));
    return signature;
}

static PARCSigningAlgorithm
_crc32Signer_GetSigningAlgorithm(_CRC32Signer *interfaceContext)
{
    return PARCSigningAlgortihm_NULL;
}

static PARCCryptoHashType
_crc32Signer_GetCryptoHashType(_CRC32Signer *interfaceContext)
{
    return PARC_HASH_CRC32C;
}

static PARCCryptoHasher *
_crc32Signer_GetCryptoHasher(_CRC32Signer *signer)
{
    return signer->hasher;
}

static PARCCryptoHasher *
_crc32Verifier_GetCryptoHasher(_CRC32Verifier *verifier, PARCKeyId *keyid, PARCCryptoHashType hashType)
{
    assertTrue(hashType == PARC_HASH_CRC32C, "Only supports PARC_HASH_CRC32C, got request for %s", parcCryptoHashType_ToString(hashType));

    return verifier->hasher;
}

static bool
_crc32Verifier_VerifyDigest(_CRC32Verifier *verifier, PARCKeyId *keyid, PARCCryptoHash *locallyComputedHash,
                                      PARCCryptoSuite suite, PARCSignature *signatureToVerify)
{
    assertTrue(suite == PARCCryptoSuite_NULL_CRC32C, "Only supports PARC_SUITE_NULL_CRC32C, got request for %d", suite);

    PARCBuffer *calculatedCrc = parcCryptoHash_GetDigest(locallyComputedHash);

    // the signature is the CRC, so we just need to compare to the to calculated CRC32C "hash"
    PARCBuffer *crcToVerify = parcSignature_GetSignature(signatureToVerify);

    return parcBuffer_Equals(calculatedCrc, crcToVerify);
}

static bool
_crc32Verifier_AllowedCryptoSuite(_CRC32Verifier *verifier, PARCKeyId *keyid, PARCCryptoSuite suite)
{
    return (suite == PARCCryptoSuite_NULL_CRC32C);
}

PARCSigningInterface *CRC32SignerAsPARCSigner = &(PARCSigningInterface) {
        .GetCryptoHasher          = (PARCCryptoHasher *(*)(void *)) _crc32Signer_GetCryptoHasher,
        .SignDigest               = (PARCSignature *(*)(void *, const PARCCryptoHash *)) _crc32Signer_SignDigest,
        .GetSigningAlgorithm      = (PARCSigningAlgorithm (*)(void *)) _crc32Signer_GetSigningAlgorithm,
        .GetCryptoHashType        = (PARCCryptoHashType (*)(void *)) _crc32Signer_GetCryptoHashType
};

PARCVerifierInterface *CRC32VerifierAsPARCVerifier = &(PARCVerifierInterface) {
        .GetCryptoHasher    = (PARCCryptoHasher *(*)(void *, PARCKeyId *, PARCCryptoHashType)) _crc32Verifier_GetCryptoHasher,
        .VerifyDigest       = (bool (*)(void *, PARCKeyId *, PARCCryptoHash *, PARCCryptoSuite, PARCSignature *)) _crc32Verifier_VerifyDigest,
        .AddKey             = NULL,
        .RemoveKeyId        = NULL,
        .AllowedCryptoSuite = (bool (*)(void *, PARCKeyId *, PARCCryptoSuite))_crc32Verifier_AllowedCryptoSuite,
};

static PARCSigner *
_crc32Signer_Create(void)
{
    _CRC32Signer *crc32Signer = parcObject_CreateInstance(_CRC32Signer);
    assertNotNull(crc32Signer, "parcObject_CreateInstance returned NULL");

    crc32Signer->hasher = parcCryptoHasher_Create(PARC_HASH_CRC32C);
    PARCSigner *signer = parcSigner_Create(crc32Signer, CRC32SignerAsPARCSigner);
    _crc32CSigner_Release(&crc32Signer);

    return signer;
}

PARCSigner *
ccnxValidationCRC32C_CreateSigner(void)
{
    return _crc32Signer_Create();
}

static PARCVerifier *
_crc32Verifier_Create(void)
{
    _CRC32Verifier *crcVerifier = parcObject_CreateInstance(_CRC32Verifier);
    assertNotNull(crcVerifier, "parcObject_CreateInstance returned NULL");

    crcVerifier->hasher = parcCryptoHasher_Create(PARC_HASH_CRC32C);

    PARCVerifier *verifier = parcVerifier_Create(crcVerifier, CRC32VerifierAsPARCVerifier);
    _crc32Verifier_Release(&crcVerifier);

    return verifier;
}

PARCVerifier *
ccnxValidationCRC32C_CreateVerifier(void)
{
    return _crc32Verifier_Create();
}
