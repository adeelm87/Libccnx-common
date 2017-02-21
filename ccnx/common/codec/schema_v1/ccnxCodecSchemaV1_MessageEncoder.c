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

#include <stdio.h>
#include <stdlib.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Buffer.h>

#include <ccnx/common/ccnx_PayloadType.h>
#include <ccnx/common/ccnx_InterestReturn.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_ManifestEncoder.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_MessageEncoder.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_NameCodec.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_HashCodec.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_Types.h>

#include <ccnx/common/codec/ccnxCodec_TlvUtilities.h>

#include <ccnx/common/ccnx_Manifest.h>
#include <ccnx/common/ccnx_ManifestHashGroup.h>

static ssize_t
_encodeName(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = -1;
    CCNxName *name = ccnxTlvDictionary_GetName(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_NAME);
    if (name != NULL) {
        length = ccnxCodecSchemaV1NameCodec_Encode(encoder, CCNxCodecSchemaV1Types_CCNxMessage_Name, name);
    }

    // required field for everything except CCNxContentObjects
    if (!ccnxTlvDictionary_IsContentObject(packetDictionary) && length < 0) {
        CCNxCodecError *error = ccnxCodecError_Create(TLV_MISSING_MANDATORY, __func__, __LINE__, ccnxCodecTlvEncoder_Position(encoder));
        ccnxCodecTlvEncoder_SetError(encoder, error);
        ccnxCodecError_Release(&error);
    } else if (ccnxTlvDictionary_IsContentObject(packetDictionary) && name == NULL) {
        length = 0;
    }

    return length;
}

static ssize_t
_encodeJsonPayload(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    PARCJSON *json = ccnxTlvDictionary_GetJson(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOAD);
    if (json != NULL) {
        char *jsonString = parcJSON_ToCompactString(json);
        size_t len = strlen(jsonString);
        length = ccnxCodecTlvEncoder_AppendArray(encoder, CCNxCodecSchemaV1Types_CCNxMessage_Payload, len, (uint8_t *) jsonString);
    }
    return length;
}

static ssize_t
_encodePayload(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    PARCBuffer *buffer = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOAD);
    if (buffer != NULL) {
        length = ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_CCNxMessage_Payload, buffer);
    }
    return length;
}

static ssize_t
_encodePayloadType(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    if (ccnxTlvDictionary_IsValueInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOADTYPE)) {
        CCNxPayloadType payloadType = (CCNxPayloadType) ccnxTlvDictionary_GetInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOADTYPE);

        CCNxCodecSchemaV1Types_PayloadType wireFormatType = CCNxCodecSchemaV1Types_PayloadType_Data;

        switch (payloadType) {
            case CCNxPayloadType_KEY:
                wireFormatType = CCNxCodecSchemaV1Types_PayloadType_Key;
                break;

            case CCNxPayloadType_LINK:
                wireFormatType = CCNxCodecSchemaV1Types_PayloadType_Link;
                break;

            default:
                // anything else is encoded as DATA
                break;
        }

        length = ccnxCodecTlvEncoder_AppendUint8(encoder, CCNxCodecSchemaV1Types_CCNxMessage_PayloadType, wireFormatType);
    } else if (ccnxTlvDictionary_IsValueBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOADTYPE)) {
        PARCBuffer *buffer = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOADTYPE);
        length = ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_CCNxMessage_PayloadType, buffer);
    }

    return length;
}

static ssize_t
_encodeExpiryTime(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    if (ccnxTlvDictionary_IsValueInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_EXPIRY_TIME)) {
        uint64_t millis = ccnxTlvDictionary_GetInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_EXPIRY_TIME);
        length = ccnxCodecTlvEncoder_AppendUint64(encoder, CCNxCodecSchemaV1Types_CCNxMessage_ExpiryTime, millis);
    } else if (ccnxTlvDictionary_IsValueBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_EXPIRY_TIME)) {
        PARCBuffer *buffer = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_EXPIRY_TIME);
        length = ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_CCNxMessage_ExpiryTime, buffer);
    }

    return length;
}

static ssize_t
_encodeEndChunkNumber(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    if (ccnxTlvDictionary_IsValueInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_ENDSEGMENT)) {
        uint64_t endChunkId = ccnxTlvDictionary_GetInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_ENDSEGMENT);
        length = ccnxCodecTlvEncoder_AppendVarInt(encoder, CCNxCodecSchemaV1Types_CCNxMessage_EndChunkNumber, endChunkId);
    } else {
        PARCBuffer *buffer = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_ENDSEGMENT);
        if (buffer != NULL) {
            length = ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_CCNxMessage_EndChunkNumber, buffer);
        }
    }
    return length;
}

static ssize_t
_encodeKeyIdRestriction(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    PARCCryptoHash *hash = ccnxTlvDictionary_GetObject(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_KEYID_RESTRICTION);
    if (hash != NULL) {
        size_t startPosition = ccnxCodecTlvEncoder_Position(encoder);
        ccnxCodecTlvEncoder_AppendContainer(encoder, CCNxCodecSchemaV1Types_CCNxMessage_KeyIdRestriction, 0);
        length = ccnxCodecSchemaV1HashCodec_Encode(encoder, hash);
        if (length < 0) {
            return length;
        }

        ccnxCodecTlvEncoder_SetContainerLength(encoder, startPosition, length);
        length += 4; // this accounts for the TL fields
    }
    return length;
}

static ssize_t
_encodeContentObjectHashRestriction(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    PARCCryptoHash *hash = ccnxTlvDictionary_GetObject(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_OBJHASH_RESTRICTION);
    if (hash != NULL) {
        size_t startPosition = ccnxCodecTlvEncoder_Position(encoder);
        ccnxCodecTlvEncoder_AppendContainer(encoder, CCNxCodecSchemaV1Types_CCNxMessage_ContentObjectHashRestriction, 0);
        length = ccnxCodecSchemaV1HashCodec_Encode(encoder, hash);
        if (length < 0) {
            return length;
        }

        ccnxCodecTlvEncoder_SetContainerLength(encoder, startPosition, length);
        length += 4; // this accounts for the TL fields
    }
    return length;
}

static ssize_t
_encodeMessageId(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    if (ccnxTlvDictionary_IsValueInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_MESSAGEID)) {
        uint32_t messageId = ccnxTlvDictionary_GetInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_MESSAGEID);
        length = ccnxCodecTlvEncoder_AppendUint32(encoder, CCNxCodecSchemaV1Types_CCNxMessage_MessageId, messageId);
    }

    return length;
}

static ssize_t
_encodeContentObject(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    ssize_t result;

    result = _encodeName(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodePayloadType(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodeExpiryTime(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodeEndChunkNumber(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodeMessageId(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;    

    result = _encodePayload(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    return length;
}

static ssize_t
_encodeInterest(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    ssize_t result;

    result = _encodeName(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodeKeyIdRestriction(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodeContentObjectHashRestriction(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodeMessageId(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;    

    result = _encodePayload(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    return length;
}

static ssize_t
_encodeControl(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    ssize_t result;

    result = _encodeName(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = _encodeJsonPayload(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    return length;
}

static ssize_t
_encodeManifest(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;
    ssize_t result;

    result = _encodeName(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    result = ccnxCodecSchemaV1ManifestEncoder_Encode(encoder, packetDictionary);
    if (result < 0) {
        return result;
    }
    length += result;

    return length;
}

ssize_t
ccnxCodecSchemaV1MessageEncoder_Encode(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    assertNotNull(packetDictionary, "Parameter packetDictionary must be non-null");

    ssize_t length = -1;

    if (ccnxTlvDictionary_IsInterest(packetDictionary)) {
        length = _encodeInterest(encoder, packetDictionary);
    } else if (ccnxTlvDictionary_IsInterestReturn(packetDictionary)) {
        length = _encodeInterest(encoder, packetDictionary);
    } else if (ccnxTlvDictionary_IsContentObject(packetDictionary)) {
        length = _encodeContentObject(encoder, packetDictionary);
    } else if (ccnxTlvDictionary_IsControl(packetDictionary)) {
        length = _encodeControl(encoder, packetDictionary);
    } else if (ccnxTlvDictionary_IsManifest(packetDictionary)) {
        length = _encodeManifest(encoder, packetDictionary);
    } else {
        CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_PACKETTYPE, __func__, __LINE__, ccnxCodecTlvEncoder_Position(encoder));
        ccnxCodecTlvEncoder_SetError(encoder, error);
        ccnxCodecError_Release(&error);
        length = -1;
    }


    if (length >= 0) {
        // Put custom fields all last
        ssize_t customLength = ccnxCodecTlvUtilities_EncodeCustomList(encoder, packetDictionary, CCNxCodecSchemaV1TlvDictionary_Lists_MESSAGE_LIST);
        if (customLength < 0) {
            return customLength;
        }
        length += customLength;
    }

    return length;
}
