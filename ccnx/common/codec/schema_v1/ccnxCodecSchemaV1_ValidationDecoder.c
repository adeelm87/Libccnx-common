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

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_ValidationDecoder.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_Types.h>
#include <ccnx/common/codec/ccnxCodec_TlvUtilities.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_LinkCodec.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_CryptoSuite.h>

static bool
_decodeKeyName(CCNxCodecTlvDecoder *decoder, CCNxTlvDictionary *packetDictionary, uint16_t type, uint16_t length)
{
    // At this point, the decoder should point to the 1st byte of the "value" of the (type, length) continer.
    // This is defined as a CCNxLink

    bool success = false;

    // this will set the decoder error if it fails.
    CCNxLink *link = ccnxCodecSchemaV1LinkCodec_DecodeValue(decoder, length);
    if (link != NULL) {
        const CCNxName *name = ccnxLink_GetName(link);
        if (name != NULL) {
            success = ccnxTlvDictionary_PutName(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEYNAME_NAME, name);

            if (success) {
                PARCBuffer *keyid = ccnxLink_GetKeyID(link);
                if (keyid) {
                    ccnxTlvDictionary_PutBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEYNAME_KEYID, keyid);
                }

                PARCBuffer *hash = ccnxLink_GetContentObjectHash(link);
                if (hash) {
                    ccnxTlvDictionary_PutBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEYNAME_OBJHASH, hash);
                }
            }
        }

        if (!success) {
            CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_DECODE, __func__, __LINE__, ccnxCodecTlvDecoder_Position(decoder));
            ccnxCodecTlvDecoder_SetError(decoder, error);
            ccnxCodecError_Release(&error);
        }

        ccnxLink_Release(&link);
    }

    return success;
}

static bool
_decodeAlgParametersType(CCNxCodecTlvDecoder *decoder, CCNxTlvDictionary *packetDictionary, uint16_t type, uint16_t length)
{
    bool success = false;
    switch (type) {
        case CCNxCodecSchemaV1Types_ValidationAlg_Cert:
            success = ccnxCodecTlvUtilities_PutAsBuffer(decoder, packetDictionary, type, length, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_CERT);
            break;

        case CCNxCodecSchemaV1Types_ValidationAlg_KeyId:
            success = ccnxCodecTlvUtilities_PutAsBuffer(decoder, packetDictionary, type, length, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEYID);
            break;

        case CCNxCodecSchemaV1Types_ValidationAlg_KeyName:
            // The "value" is a link
            success = _decodeKeyName(decoder, packetDictionary, type, length);
            break;

        case CCNxCodecSchemaV1Types_ValidationAlg_SigTime:
            // This is a time, so put it as an Integer
            success = ccnxCodecTlvUtilities_PutAsInteger(decoder, packetDictionary, type, length, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_SIGNTIME);
            break;

        case CCNxCodecSchemaV1Types_ValidationAlg_PublicKey:
            success = ccnxCodecTlvUtilities_PutAsBuffer(decoder, packetDictionary, type, length, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEY);
            break;

        default: {
            // if we do not know the TLV type, put it in this container's unknown list
            success = ccnxCodecTlvUtilities_PutAsListBuffer(decoder, packetDictionary, type, length, CCNxCodecSchemaV1TlvDictionary_Lists_VALIDATION_ALG_LIST);
        }
        break;
    }

    if (!success) {
        CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_DECODE, __func__, __LINE__, ccnxCodecTlvDecoder_Position(decoder));
        ccnxCodecTlvDecoder_SetError(decoder, error);
        ccnxCodecError_Release(&error);
    }
    return success;
}

/**
 * Called by _decodeAlgType() via ccnxCodecTlvUtilities_DecodeSubcontainer() to decode the
 * algorithm specific parameters
 */
static bool
_decodeAlgParameters(CCNxCodecTlvDecoder *decoder, CCNxTlvDictionary *packetDictionary)
{
    return ccnxCodecTlvUtilities_DecodeContainer(decoder, packetDictionary, _decodeAlgParametersType);
}

static bool
_decodeAlgType(CCNxCodecTlvDecoder *decoder, CCNxTlvDictionary *packetDictionary, uint16_t type, uint16_t length)
{
    bool success = false;

    PARCCryptoSuite parcSuite;
    bool match = ccnxCodecSchemaV1CryptoSuite_TlvToParc((CCNxCodecSchemaV1TlvDictionary_CryptoSuite) type, &parcSuite);

    if (match) {
        success = ccnxTlvDictionary_PutInteger(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_CRYPTO_SUITE, parcSuite);

        if (success) {
            success = ccnxCodecTlvUtilities_DecodeSubcontainer(decoder, packetDictionary, type, length, _decodeAlgParameters);
        }
    } else {
        // if we do not know the TLV type, put it in this container's unknown list
        success = ccnxCodecTlvUtilities_PutAsListBuffer(decoder, packetDictionary, type, length, CCNxCodecSchemaV1TlvDictionary_Lists_VALIDATION_ALG_LIST);
    }

    if (!success) {
        CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_DECODE, __func__, __LINE__, ccnxCodecTlvDecoder_Position(decoder));
        ccnxCodecTlvDecoder_SetError(decoder, error);
        ccnxCodecError_Release(&error);
    }
    return success;
}

// ==================
// Public API

bool
ccnxCodecSchemaV1ValidationDecoder_DecodeAlg(CCNxCodecTlvDecoder *decoder, CCNxTlvDictionary *packetDictionary)
{
    return ccnxCodecTlvUtilities_DecodeContainer(decoder, packetDictionary, _decodeAlgType);
}


bool
ccnxCodecSchemaV1ValidationDecoder_DecodePayload(CCNxCodecTlvDecoder *decoder, CCNxTlvDictionary *packetDictionary)
{
    bool success = false;
    // A 0-length payload is treaded like an error
    size_t remaining = ccnxCodecTlvDecoder_Remaining(decoder);
    if (remaining > 0) {
        PARCBuffer *payload = ccnxCodecTlvDecoder_GetValue(decoder, remaining);
        success = ccnxTlvDictionary_PutBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_PAYLOAD, payload);
        parcBuffer_Release(&payload);
    }
    return success;
}
