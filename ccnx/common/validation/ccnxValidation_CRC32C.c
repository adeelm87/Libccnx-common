/*
 * Copyright (c) 2013-2016, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * See SCTP for a discussion of CRC32C http://tools.ietf.org/html/rfc4960#appendix-B
 * It is also used by iSCSI and other protocols.
 *
 * CRC-32C uses an initial value of 0xFFFFFFFF and a final XOR value of 0xFFFFFFFF.
 *
 * @author Marc Mosko, Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2016, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
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
} CRC32CSigner;

// ==================================================
// CRC32C Prototypes PARCSigner

static PARCSigner      *_crc32cSignerInterface_Create(void);
static void                  _crc32cSignerInterface_Destroy(void **interfaceContextPtr);
static PARCSignature        *_crc32cSignerInterface_SignDigest(void *interfaceContext, const PARCCryptoHash *cryptoHash);
static PARCSigningAlgorithm  _crc32cSignerInterface_GetSigningAlgorithm(void *interfaceContext);
static PARCCryptoHashType    _crc32cSignerInterface_GetCryptoHashType(void *interfaceContext);
static PARCCryptoHasher     *_crc32cSignerInterface_GetCryptoHasher(void *interfaceContext);

static PARCSigningInterface crc32c_signerinterface_template = {
        .Release                  = (void (*)(void **)) _crc32cSignerInterface_Destroy,
        .GetCryptoHasher          = _crc32cSignerInterface_GetCryptoHasher,
        .SignDigest               = _crc32cSignerInterface_SignDigest,
        .GetSigningAlgorithm      = _crc32cSignerInterface_GetSigningAlgorithm,
        .GetCryptoHashType        = _crc32cSignerInterface_GetCryptoHashType
};

// ==================================================
// CRC32C Prototypes PARCVerifier

static PARCVerifierInterface *_crc32cVerifierInterface_Create(void);
static PARCCryptoHasher *_crc32cVerifierInterface_GetCryptoHasher(void *interfaceContext, PARCKeyId *keyid,
                                                                  PARCCryptoHashType hashType);
static bool              _crc32cVerifierInterface_VerifyDigest(void *interfaceContext, PARCKeyId *keyid,
                                                               PARCCryptoHash *locallyComputedHash,
                                                               PARCCryptoSuite suite, PARCSignature *signatureToVerify);
static bool              _crc32cVerifierInterface_AllowedCryptoSuite(void *interfaceContext, PARCKeyId *keyid,
                                                                     PARCCryptoSuite suite);
static void              _crc32cVerifierInterface_Destroy(struct parc_verifier_interface **interfaceContextPtr);

static const PARCVerifierInterface crc32c_verifierinterface_template = {
        .interfaceContext   = NULL,
        .GetCryptoHasher    = _crc32cVerifierInterface_GetCryptoHasher,
        .VerifyDigest       = _crc32cVerifierInterface_VerifyDigest,
        .AddKey             = NULL,
        .RemoveKeyId        = NULL,
        .AllowedCryptoSuite = _crc32cVerifierInterface_AllowedCryptoSuite,
        .Destroy            = _crc32cVerifierInterface_Destroy,
};

// ========================================================================================

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

PARCSigner *
ccnxValidationCRC32C_CreateSigner(void)
{
    return _crc32cSignerInterface_Create();
}

PARCVerifier *
ccnxValidationCRC32C_CreateVerifier(void)
{
    PARCVerifier *verifier = parcVerifier_Create(_crc32cVerifierInterface_Create());
    return verifier;
}

// ==================================================
// CRC32C Implementation PARCSigner

static PARCSigner *
_crc32cSignerInterface_Create(void)
{
    CRC32CSigner *crc32Signer = parcMemory_AllocateAndClear(sizeof(CRC32CSigner));
    assertNotNull(crc32Signer, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(CRC32CSigner));
    crc32Signer->hasher = parcCryptoHasher_Create(PARC_HASH_CRC32C);

    PARCSigner *signer = parcSigner_Create(crc32Signer, &crc32c_signerinterface_template);

    return signer;
}

static void
_crc32cSignerInterface_Destroy(void **interfaceContextPtr)
{
    CRC32CSigner *signer = (CRC32CSigner *) *interfaceContextPtr;

    parcCryptoHasher_Release(&(signer->hasher));

    parcMemory_Deallocate((void **) &signer);
    *interfaceContextPtr = NULL;
}

static PARCSignature *
_crc32cSignerInterface_SignDigest(void *interfaceContext, const PARCCryptoHash *cryptoHash)
{
    PARCSignature *signature =
            parcSignature_Create(PARCSigningAlgortihm_NULL, PARC_HASH_CRC32C, parcCryptoHash_GetDigest(cryptoHash));
    return signature;
}

static PARCSigningAlgorithm
_crc32cSignerInterface_GetSigningAlgorithm(void *interfaceContext)
{
    return PARCSigningAlgortihm_NULL;
}

static PARCCryptoHashType
_crc32cSignerInterface_GetCryptoHashType(void *interfaceContext)
{
    return PARC_HASH_CRC32C;
}

static PARCCryptoHasher *
_crc32cSignerInterface_GetCryptoHasher(void *interfaceContext)
{
    CRC32CSigner *signer = interfaceContext;
    return signer->hasher;
}

// ==================================================
// CRC32C Implementation PARCVerifierInterface

static PARCVerifierInterface *
_crc32cVerifierInterface_Create(void)
{
    CRC32CSigner *crc32Signer = parcMemory_AllocateAndClear(sizeof(CRC32CSigner));
    assertNotNull(crc32Signer, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(CRC32CSigner));
    crc32Signer->hasher = parcCryptoHasher_Create(PARC_HASH_CRC32C);

    PARCVerifierInterface *interface = parcMemory_AllocateAndClear(sizeof(PARCVerifierInterface));
    assertNotNull(interface, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(PARCVerifierInterface));
    *interface = crc32c_verifierinterface_template;
    interface->interfaceContext = crc32Signer;
    return interface;
}

static PARCCryptoHasher *
_crc32cVerifierInterface_GetCryptoHasher(void *interfaceContext, PARCKeyId *keyid, PARCCryptoHashType hashType)
{
    assertTrue(hashType == PARC_HASH_CRC32C, "Only supports PARC_HASH_CRC32C, got request for %s", parcCryptoHashType_ToString(hashType));

    CRC32CSigner *signer = interfaceContext;
    return signer->hasher;
}

static bool
_crc32cVerifierInterface_VerifyDigest(void *interfaceContext, PARCKeyId *keyid, PARCCryptoHash *locallyComputedHash,
                                      PARCCryptoSuite suite, PARCSignature *signatureToVerify)
{
    assertTrue(suite == PARCCryptoSuite_NULL_CRC32C, "Only supports PARC_SUITE_NULL_CRC32C, got request for %d", suite);

    PARCBuffer *calculatedCrc = parcCryptoHash_GetDigest(locallyComputedHash);

    // the signature is the CRC, so we just need to compare to the to calculated CRC32C "hash"
    PARCBuffer *crcToVerify = parcSignature_GetSignature(signatureToVerify);

    return parcBuffer_Equals(calculatedCrc, crcToVerify);
}

static bool
_crc32cVerifierInterface_AllowedCryptoSuite(void *interfaceContext, PARCKeyId *keyid, PARCCryptoSuite suite)
{
    return (suite == PARCCryptoSuite_NULL_CRC32C);
}

static void
_crc32cVerifierInterface_Destroy(struct parc_verifier_interface **interfaceContextPtr)
{
    PARCVerifierInterface *interface = (PARCVerifierInterface *) *interfaceContextPtr;

    CRC32CSigner *signer = interface->interfaceContext;

    parcCryptoHasher_Release(&(signer->hasher));

    parcMemory_Deallocate((void **) &signer);
    parcMemory_Deallocate((void **) &interface);
    *interfaceContextPtr = NULL;
}
