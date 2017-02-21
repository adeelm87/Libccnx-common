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
 * @author Glenn Scott, Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <LongBow/runtime.h>

#include <parc/security/parc_SigningAlgorithm.h>

#include <ccnx/common/internal/ccnx_ContentObjectFacadeV1.h>
#include <ccnx/common/internal/ccnx_ValidationFacadeV1.h>
#include <ccnx/common/internal/ccnx_ChunkingFacadeV1.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.h>

#include <ccnx/common/codec/ccnxCodec_TlvEncoder.h>

static void
_assertInvariants(const CCNxTlvDictionary *contentObjectDictionary)
{
    assertNotNull(contentObjectDictionary, "Dictionary is null");
    assertTrue(ccnxTlvDictionary_IsContentObject(contentObjectDictionary), "Dictionary is not a content object");
    assertTrue(ccnxTlvDictionary_GetSchemaVersion(contentObjectDictionary) == CCNxTlvDictionary_SchemaVersion_V1,
               "Dictionary is wrong schema version, got %d expected %d",
               ccnxTlvDictionary_GetSchemaVersion(contentObjectDictionary), CCNxTlvDictionary_SchemaVersion_V1);
}

// =========================
// Creation

static CCNxTlvDictionary *
_ccnxContentObjectFacadeV1_CreateWithNameAndPayload(const CCNxName *name,              // required
                                                    const CCNxPayloadType payloadType, // required
                                                    const PARCBuffer *payload)         // may be null
{
    assertNotNull(name, "Parameter name must be non-null");

    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();

    if (dictionary) {
        ccnxTlvDictionary_PutName(dictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_NAME, name);

        if (payloadType != CCNxPayloadType_DATA) {
            ccnxTlvDictionary_PutInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOADTYPE, payloadType);
        }

        if (payload) {
            ccnxTlvDictionary_PutBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOAD, payload);
        }
    } else {
        trapOutOfMemory("Could not allocate ContentObject");
    }

    return dictionary;
}

static CCNxTlvDictionary *
_ccnxContentObjectFacadeV1_CreateWithPayload(const CCNxPayloadType payloadType, // required
                                             const PARCBuffer *payload)         // may be null
{
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();

    if (dictionary) {
        if (payloadType != CCNxPayloadType_DATA) {
            ccnxTlvDictionary_PutInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOADTYPE, payloadType);
        }

        if (payload) {
            ccnxTlvDictionary_PutBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOAD, payload);
        }
    } else {
        trapOutOfMemory("Could not allocate ContentObject");
    }

    return dictionary;
}

// =========================
// Getters

static CCNxName *
_ccnxContentObjectFacadeV1_GetName(const CCNxTlvDictionary *contentObjectDictionary)
{
    _assertInvariants(contentObjectDictionary);
    return ccnxTlvDictionary_GetName(contentObjectDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_NAME);
}

static bool
_ccnxContentObjectFacadeV1_HasExpiryTime(const CCNxTlvDictionary *packetDictionary)
{
    if (ccnxTlvDictionary_IsValueInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_EXPIRY_TIME)) {
        return true;
    }
    return false;
}

static uint64_t
_ccnxContentObjectFacadeV1_GetExpiryTime(const CCNxTlvDictionary *packetDictionary)
{
    if (ccnxTlvDictionary_IsValueInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_EXPIRY_TIME)) {
        return ccnxTlvDictionary_GetInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_EXPIRY_TIME);
    }
    trapUnexpectedState("The dictionary does not contain an Expiry Time");
}

static bool
_ccnxContentObjectFacadeV1_HasMessageId(const CCNxTlvDictionary *packetDictionary)
{
    if (ccnxTlvDictionary_IsValueInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_MESSAGEID)) {
        return true;
    }
    return false;
}

static uint32_t
_ccnxContentObjectFacadeV1_GetMessageId(const CCNxTlvDictionary *packetDictionary)
{
    if (ccnxTlvDictionary_IsValueInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_MESSAGEID)) {
        return ccnxTlvDictionary_GetInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_MESSAGEID);
    }
    trapUnexpectedState("The dictionary does not contain a Message ID");
}

static PARCBuffer *
_ccnxContentObjectFacadeV1_GetPayload(const CCNxTlvDictionary *contentObjectDictionary)
{
    _assertInvariants(contentObjectDictionary);
    return ccnxTlvDictionary_GetBuffer(contentObjectDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOAD);
}

static CCNxPayloadType
_ccnxContentObjectFacadeV1_GetPayloadType(const CCNxTlvDictionary *contentObjectDictionary)
{
    CCNxPayloadType result = CCNxPayloadType_DATA;

    _assertInvariants(contentObjectDictionary);
    if (ccnxTlvDictionary_IsValueInteger(contentObjectDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOADTYPE)) {
        result = (CCNxPayloadType) ccnxTlvDictionary_GetInteger(contentObjectDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOADTYPE);
    }

    return result;
}

// =========================
// Setters

static bool
_ccnxContentObjectFacadeV1_SetSignature(CCNxTlvDictionary *contentObject, const PARCBuffer *keyId,
                                        const PARCSignature *signature, const CCNxKeyLocator *keyLocator)
{
    bool result = false;

    CCNxTlvDictionary *contentObjectDictionary = (CCNxTlvDictionary *) contentObject;

    if (parcSignature_GetSigningAlgorithm(signature) == PARCSigningAlgorithm_RSA
        && parcSignature_GetHashType(signature) == PARCCryptoHashType_SHA256) {
        ccnxValidationRsaSha256_Set(contentObjectDictionary, keyId, keyLocator);
    } else if (parcSignature_GetSigningAlgorithm(signature) == PARCSigningAlgorithm_HMAC
               && parcSignature_GetHashType(signature) == PARCCryptoHashType_SHA256) {
        ccnxValidationHmacSha256_Set(contentObjectDictionary, keyId);
    } else {
        trapNotImplemented("Have not implemented the signature parameters");
    }

    PARCBuffer *sigbits = parcSignature_GetSignature(signature);

    result = ccnxValidationFacadeV1_SetPayload(contentObjectDictionary, sigbits);

    return result;
}

static PARCBuffer *
_ccnxContentObjectFacadeV1_GetKeyId(const CCNxTlvDictionary *contentObject)
{
    return ccnxValidationFacadeV1_GetKeyId(contentObject);
}

static bool
_ccnxContentObjectFacadeV1_SetExpiryTime(CCNxTlvDictionary *contentObjectDictionary, uint64_t expiryTime)
{
    bool success = ccnxTlvDictionary_PutInteger(contentObjectDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_EXPIRY_TIME, expiryTime);
    trapUnexpectedStateIf(!success, "Could not set integer in dictionary");
    return success;
}

static bool
_ccnxContentObjectFacadeV1_SetMessageId(CCNxTlvDictionary *contentObjectDictionary, uint32_t messageId)
{
    bool success = ccnxTlvDictionary_PutInteger(contentObjectDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_MESSAGEID, messageId);
    trapUnexpectedStateIf(!success, "Could not set integer in dictionary");
    return success;
}

static bool
_ccnxContentObjectFacadeV1_SetPayload(CCNxTlvDictionary *contentObjectDictionary, CCNxPayloadType payloadType, const PARCBuffer *payload)
{
    bool result = false;

    if (payload != NULL) {
        PARCBuffer *originalPayload = _ccnxContentObjectFacadeV1_GetPayload(contentObjectDictionary);

        result = ccnxTlvDictionary_PutBuffer(contentObjectDictionary,
                                             CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOAD, payload);

        if (result) {
            if (_ccnxContentObjectFacadeV1_GetPayloadType(contentObjectDictionary) != payloadType) {
                ccnxTlvDictionary_PutInteger(contentObjectDictionary,
                                             CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOADTYPE, payloadType);
            }

            if (originalPayload != NULL) {
                parcBuffer_Release(&originalPayload);
            }
        }
    }

    return result;
}

// =========================
// Miscellaneous functions

static bool
_ccnxContentObjectFacadeV1_Equals(const CCNxTlvDictionary *objectA, const CCNxTlvDictionary *objectB)
{
    return ccnxTlvDictionary_Equals(objectA, objectB);
}

static char *
_ccnxContentObjectFacadeV1_ToString(const CCNxTlvDictionary *contentObjectDictionary)
{
    trapNotImplemented("_ccnxContentObjectFacadeV1_ToString(): not yet implemented");
}

static void
_ccnxContentObjectFacadeV1_Display(const CCNxTlvDictionary *contentObjectDictionary, size_t indentation)
{
    _assertInvariants(contentObjectDictionary);
    ccnxTlvDictionary_Display(contentObjectDictionary, (unsigned) indentation);
}

/**
 * `CCNxContentObjectFacadeV1_Implementation` is the structure containing the pointers to the
 * V1 schema ContentObject implementation.
 */
CCNxContentObjectInterface CCNxContentObjectFacadeV1_Implementation = {
    .description         = "CCNxContentObjectFacadeV1_Implementation",

    .createWithNameAndPayload   = &_ccnxContentObjectFacadeV1_CreateWithNameAndPayload,
    .createWithPayload   = &_ccnxContentObjectFacadeV1_CreateWithPayload,

    .setSignature        = &_ccnxContentObjectFacadeV1_SetSignature,
    .getKeyId            = &_ccnxContentObjectFacadeV1_GetKeyId,

    .getName             = &_ccnxContentObjectFacadeV1_GetName,
    .getPayload          = &_ccnxContentObjectFacadeV1_GetPayload,
    .setPayload          = &_ccnxContentObjectFacadeV1_SetPayload,
    .getPayloadType      = &_ccnxContentObjectFacadeV1_GetPayloadType,

    .getFinalChunkNumber = &ccnxChunkingFacadeV1_GetEndChunkNumber,
    .setFinalChunkNumber = &ccnxChunkingFacadeV1_SetEndChunkNumber,
    .hasFinalChunkNumber = &ccnxChunkingFacadeV1_HasEndChunkNumber,

    .getExpiryTime       = &_ccnxContentObjectFacadeV1_GetExpiryTime,
    .setExpiryTime       = &_ccnxContentObjectFacadeV1_SetExpiryTime,
    .hasExpiryTime       = &_ccnxContentObjectFacadeV1_HasExpiryTime,

    .getMessageId        = &_ccnxContentObjectFacadeV1_GetMessageId,
    .setMessageId        = &_ccnxContentObjectFacadeV1_SetMessageId,
    .hasMessageId        = &_ccnxContentObjectFacadeV1_HasMessageId,
    
    .toString            = &_ccnxContentObjectFacadeV1_ToString,
    .display             = &_ccnxContentObjectFacadeV1_Display,
    .equals              = &_ccnxContentObjectFacadeV1_Equals,

    .assertValid         = &_assertInvariants,
};
