/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <sys/time.h>
#include <ccnx/common/internal/ccnx_TlvDictionary.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.h>

#include <ccnx/common/codec/ccnxCodec_TlvUtilities.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_Types.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_ValidationEncoder.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_LinkCodec.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_CryptoSuite.h>

static ssize_t
_encodeKeyId(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    PARCBuffer *keyid = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEYID);
    if (keyid) {
        length = ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_ValidationAlg_KeyId, keyid);
    }
    return length;
}

static ssize_t
_encodePublicKey(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    PARCBuffer *key = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEY);
    if (key) {
        length = ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_ValidationAlg_PublicKey, key);
    }
    return length;
}

static ssize_t
_encodeCertificate(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    PARCBuffer *cert = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_CERT);
    if (cert) {
        length = ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_ValidationAlg_Cert, cert);
    }
    return length;
}

/**
 * If there is a CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEYNAME_NAME entry in the dictionary, look up the
 * optional keyid and hash restrictions, create a CCNxLink, then encode the Link.
 */
static ssize_t
_encodeKeyName(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    CCNxName *keyname = ccnxTlvDictionary_GetName(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEYNAME_NAME);
    if (keyname) {
        PARCBuffer *keyid = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEYNAME_KEYID);
        PARCBuffer *hash = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEYNAME_OBJHASH);
        CCNxLink *link = ccnxLink_Create(keyname, keyid, hash);

        size_t startPosition = ccnxCodecTlvEncoder_Position(encoder);
        ccnxCodecTlvEncoder_AppendContainer(encoder, CCNxCodecSchemaV1Types_ValidationAlg_KeyName, 0);
        ssize_t innerLength = ccnxCodecSchemaV1LinkCodec_Encode(encoder, link);

        if (innerLength == 0) {
            // backup and erase the container
            ccnxCodecTlvEncoder_SetPosition(encoder, startPosition);
        } else if (innerLength > 0) {
            ccnxCodecTlvEncoder_SetContainerLength(encoder, startPosition, innerLength);
            ssize_t endPosition = ccnxCodecTlvEncoder_Position(encoder);
            length = endPosition - startPosition;
        } else {
            // an error signal
            length = innerLength;
        }

        ccnxLink_Release(&link);
    }
    return length;
}

/**
 * If a time is not provided, use the current time
 */
static ssize_t
_encodeSignatureTime(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;

    bool haveSignTime = false;
    uint64_t signTime = 0;
    if (ccnxTlvDictionary_IsValueInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_SIGNTIME)) {
        haveSignTime = true;
        signTime = ccnxTlvDictionary_GetInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_SIGNTIME);
    } else {
        // if we have a signer and with a signature algorithm, create the signing time
        PARCSigner *signer = ccnxCodecTlvEncoder_GetSigner(encoder);
        if (signer) {
            PARCSigningAlgorithm alg = parcSigner_GetSigningAlgorithm(signer);
            if (alg != PARCSigningAlgortihm_NULL && alg != PARCSigningAlgorithm_UNKNOWN) {
                // We will generate a signature, so generate a signing time

                struct timeval tv;
                gettimeofday(&tv, NULL);

                // convert to milli-seconds
                signTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
                haveSignTime = true;
            }
        }
    }

    if (haveSignTime) {
        length = ccnxCodecTlvEncoder_AppendUint64(encoder, CCNxCodecSchemaV1Types_ValidationAlg_SigTime, signTime);
    }

    return length;
}

static ssize_t
_encodeAlgParameters(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    ssize_t result;

    result = _encodeKeyId(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodePublicKey(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodeCertificate(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodeKeyName(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodeSignatureTime(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    return length;
}

ssize_t
ccnxCodecSchemaV1ValidationEncoder_EncodeAlg(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    bool haveCryptoSuite = false;
    CCNxCodecSchemaV1TlvDictionary_CryptoSuite suite;

    if (ccnxTlvDictionary_IsValueInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_CRYPTO_SUITE)) {
        // try from dictionary

        PARCCryptoSuite parcSuite = (PARCCryptoSuite) ccnxTlvDictionary_GetInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_CRYPTO_SUITE);

        haveCryptoSuite = ccnxCodecSchemaV1CryptoSuite_ParcToTlv(parcSuite, &suite);
    } else if (ccnxTlvDictionary_IsContentObject(packetDictionary)) {
        // deduce from the signer

        PARCSigner *signer = ccnxCodecTlvEncoder_GetSigner(encoder);
        if (signer != NULL) {
            PARCCryptoHashType hashType = parcSigner_GetCryptoHashType(signer);
            PARCSigningAlgorithm signAlg = parcSigner_GetSigningAlgorithm(signer);

            if (ccnxCodecSchemaV1CryptoSuite_SignAndHashToTlv(signAlg, hashType, &suite)) {
                haveCryptoSuite = true;
            }
        }
    }

    if (haveCryptoSuite) {
        // write the TL container then encode any enclosed TLVs
        ssize_t startPosition = ccnxCodecTlvEncoder_Position(encoder);

        ccnxCodecTlvEncoder_AppendContainer(encoder, suite, 0);
        ssize_t innerLength = _encodeAlgParameters(encoder, packetDictionary);

        // 0 inner length is acceptable
        if (innerLength >= 0) {
            ccnxCodecTlvEncoder_SetContainerLength(encoder, startPosition, innerLength);
            ssize_t endPosition = ccnxCodecTlvEncoder_Position(encoder);
            length = endPosition - startPosition;
        } else {
            // an error signal
            length = innerLength;
        }
    }
    return length;
}

ssize_t
ccnxCodecSchemaV1ValidationEncoder_EncodePayload(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;

    if (!ccnxTlvDictionary_IsValueBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_PAYLOAD)) {
        // try to compute a signature

        // If signer is NULL, then no signature is genearted
        PARCSigner *signer = ccnxCodecTlvEncoder_GetSigner(encoder);
        if (signer != NULL) {
            // user did not give us one, so fill it in
            PARCSignature *signature = ccnxCodecTlvEncoder_ComputeSignature(encoder);
            PARCBuffer *sigbits = parcSignature_GetSignature(signature);

            // this creates its own reference to sigbits
            ccnxTlvDictionary_PutBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_PAYLOAD, sigbits);

            // this will release our hold on sigbitsElastic and sigbits.
            parcSignature_Release(&signature);
        }
    }

    PARCBuffer *sigbits = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_PAYLOAD);
    if (sigbits) {
        size_t remaining = parcBuffer_Remaining(sigbits);
        uint8_t *overlay = parcBuffer_Overlay(sigbits, 0);
        length = ccnxCodecTlvEncoder_AppendRawArray(encoder, remaining, overlay);
    }

    return length;
}
