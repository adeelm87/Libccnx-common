/*
 * Copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>

#include <stdlib.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Buffer.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_Types.h>
#include <ccnx/common/codec/ccnxCodec_TlvUtilities.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_HashCodec.h>

/**
 * These are the accepted sizes for the pre-defined hash types.
 * Hash TLVs with lengths that do not match one of these values will be deemed
 * invalid and not parse correctly.
 */
static const size_t _PARCCryptoHashType_SHA256_Sizes[] = { 32 };
static const size_t _PARCCryptoHashType_SHA512_Sizes[] = { 32, 64 };

static bool
_ccnxCodecSchemaV1HashCodec_ValidHashSize(size_t size, size_t numSizes, size_t sizes[numSizes])
{
    for (size_t i = 0; i < numSizes; i++) {
        if (sizes[i] == size) {
            return true;
        }
    }
    return false;
}

ssize_t
ccnxCodecSchemaV1HashCodec_Encode(CCNxCodecTlvEncoder *encoder, const PARCCryptoHash *hash)
{
    PARCBuffer *digest = parcCryptoHash_GetDigest(hash);
    PARCCryptoHashType hashType = parcCryptoHash_GetDigestType(hash);
    size_t digestLength = parcBuffer_Remaining(digest);

    uint16_t tlvHashType = CCNxCodecSchemaV1Types_HashType_App;
    bool validHash = true;
    switch (hashType) {
        case PARCCryptoHashType_SHA256:
            tlvHashType = CCNxCodecSchemaV1Types_HashType_SHA256;
            validHash = _ccnxCodecSchemaV1HashCodec_ValidHashSize(digestLength,
                sizeof(_PARCCryptoHashType_SHA256_Sizes) / sizeof(size_t), (size_t *) _PARCCryptoHashType_SHA256_Sizes);
            break;
        case PARCCryptoHashType_SHA512:
            tlvHashType = CCNxCodecSchemaV1Types_HashType_SHA512;
            validHash = _ccnxCodecSchemaV1HashCodec_ValidHashSize(digestLength,
                sizeof(_PARCCryptoHashType_SHA512_Sizes) / sizeof(size_t), (size_t *) _PARCCryptoHashType_SHA512_Sizes);
            break;
        default:
            break;
    }

    if (validHash) {
        return ccnxCodecTlvEncoder_AppendBuffer(encoder, tlvHashType, digest);
    } else {
        CCNxCodecError *error = ccnxCodecError_Create(TLV_MISSING_MANDATORY, __func__, __LINE__, ccnxCodecTlvEncoder_Position(encoder));
        ccnxCodecTlvEncoder_SetError(encoder, error);
        ccnxCodecError_Release(&error);

        return -1;
    }
}

static bool
_ccnxCodecSchemaV1HashCodec_ValidHash(uint16_t hashType, uint16_t hashSize)
{
    bool validHash = true;

    switch (hashType) {
        case CCNxCodecSchemaV1Types_HashType_SHA256:
            validHash = _ccnxCodecSchemaV1HashCodec_ValidHashSize(hashSize,
                sizeof(_PARCCryptoHashType_SHA256_Sizes) / sizeof(size_t), (size_t *) _PARCCryptoHashType_SHA256_Sizes);
            break;
        case CCNxCodecSchemaV1Types_HashType_SHA512:
            validHash = _ccnxCodecSchemaV1HashCodec_ValidHashSize(hashSize,
                sizeof(_PARCCryptoHashType_SHA512_Sizes) / sizeof(size_t), (size_t *) _PARCCryptoHashType_SHA512_Sizes);
            break;
        default:
            break;
    }

    return validHash;
}

PARCCryptoHash *
ccnxCodecSchemaV1HashCodec_DecodeValue(CCNxCodecTlvDecoder *decoder, size_t limit)
{
    PARCCryptoHash *hash = NULL;
    uint16_t hashType = 0;
    uint16_t length = 0;

    if (ccnxCodecTlvDecoder_EnsureRemaining(decoder, 4)) {
        hashType = ccnxCodecTlvDecoder_GetType(decoder);
        length = ccnxCodecTlvDecoder_GetLength(decoder);

        if (length > limit) {
            CCNxCodecError *error = ccnxCodecError_Create(TLV_MISSING_MANDATORY, __func__, __LINE__, ccnxCodecTlvDecoder_Position(decoder));
            ccnxCodecTlvDecoder_SetError(decoder, error);
            ccnxCodecError_Release(&error);
            return NULL;
        }

        if (ccnxCodecTlvDecoder_EnsureRemaining(decoder, length)) {
            PARCBuffer *value = ccnxCodecTlvDecoder_GetValue(decoder, length);
            switch (hashType) {
                case CCNxCodecSchemaV1Types_HashType_SHA256:
                    hash = parcCryptoHash_Create(PARCCryptoHashType_SHA256, value);
                    break;
                case CCNxCodecSchemaV1Types_HashType_SHA512:
                    hash = parcCryptoHash_Create(PARCCryptoHashType_SHA512, value);
                    break;
                case CCNxCodecSchemaV1Types_HashType_App:
                    hash = parcCryptoHash_Create(PARCCryptoHashType_NULL, value);
                    break;
            }
            parcBuffer_Release(&value);
        }
    }

    // Verify the hash size, if one was parsed correctly.
    if (hash != NULL && !_ccnxCodecSchemaV1HashCodec_ValidHash(hashType, length)) {
        parcCryptoHash_Release(&hash);
    }

    return hash;
}
