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

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_Types.h>
#include <ccnx/common/codec/ccnxCodec_TlvUtilities.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_NameCodec.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_LinkCodec.h>

ssize_t
ccnxCodecSchemaV1LinkCodec_Encode(CCNxCodecTlvEncoder *encoder, const CCNxLink *link)
{
    ssize_t length = 0;

    const CCNxName *name = ccnxLink_GetName(link);
    if (name) {
        length += ccnxCodecSchemaV1NameCodec_Encode(encoder, CCNxCodecSchemaV1Types_Link_Name, name);

        PARCBuffer *keyid = ccnxLink_GetKeyID(link);
        if (keyid) {
            length += ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_Link_KeyIdRestriction, keyid);
        }

        PARCBuffer *hash = ccnxLink_GetContentObjectHash(link);
        if (hash) {
            length += ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_Link_ContentObjectHashRestriction, hash);
        }
    } else {
        length = -1;
        CCNxCodecError *error = ccnxCodecError_Create(TLV_MISSING_MANDATORY, __func__, __LINE__, ccnxCodecTlvEncoder_Position(encoder));
        ccnxCodecTlvEncoder_SetError(encoder, error);
        ccnxCodecError_Release(&error);
    }

    return length;
}

typedef struct decoded_link {
    CCNxName *linkName;
    PARCBuffer *linkKeyId;
    PARCBuffer *linkHash;
} _DecodedLink;

static int
_decodeField(CCNxCodecTlvDecoder *decoder, _DecodedLink *decodedLink)
{
    int errorCode = TLV_ERR_NO_ERROR;

    uint16_t type = ccnxCodecTlvDecoder_GetType(decoder);
    uint16_t length = ccnxCodecTlvDecoder_GetLength(decoder);

    if (ccnxCodecTlvDecoder_EnsureRemaining(decoder, length)) {
        switch (type) {
            case CCNxCodecSchemaV1Types_Link_Name:
                if (decodedLink->linkName == NULL) {
                    decodedLink->linkName = ccnxCodecSchemaV1NameCodec_DecodeValue(decoder, length);
                } else {
                    errorCode = TLV_ERR_DUPLICATE_FIELD;
                }
                break;

            case CCNxCodecSchemaV1Types_Link_KeyIdRestriction:
                if (decodedLink->linkKeyId == NULL) {
                    decodedLink->linkKeyId = ccnxCodecTlvDecoder_GetValue(decoder, length);
                } else {
                    errorCode = TLV_ERR_DUPLICATE_FIELD;
                }
                break;

            case CCNxCodecSchemaV1Types_Link_ContentObjectHashRestriction:
                if (decodedLink->linkHash == NULL) {
                    decodedLink->linkHash = ccnxCodecTlvDecoder_GetValue(decoder, length);
                } else {
                    errorCode = TLV_ERR_DUPLICATE_FIELD;
                }
                break;

            default:
                // we do not support unknown TLVs
                errorCode = TLV_ERR_DECODE;
                break;
        }
    } else {
        errorCode = TLV_ERR_TOO_LONG;
    }

    return errorCode;
}

static void
_decodecLinkCleanup(_DecodedLink *decodedLink)
{
    if (decodedLink->linkName) {
        ccnxName_Release(&decodedLink->linkName);
    }

    if (decodedLink->linkKeyId) {
        parcBuffer_Release(&decodedLink->linkKeyId);
    }

    if (decodedLink->linkHash) {
        parcBuffer_Release(&decodedLink->linkHash);
    }
}

CCNxLink *
ccnxCodecSchemaV1LinkCodec_DecodeValue(CCNxCodecTlvDecoder *decoder, uint16_t linkLength)
{
    int errorCode = TLV_ERR_NO_ERROR;

    CCNxLink *link = NULL;

    _DecodedLink decodedLink;
    memset(&decodedLink, 0, sizeof(_DecodedLink));

    if (ccnxCodecTlvDecoder_EnsureRemaining(decoder, linkLength)) {
        while (errorCode == TLV_ERR_NO_ERROR && ccnxCodecTlvDecoder_EnsureRemaining(decoder, 4)) {
            errorCode = _decodeField(decoder, &decodedLink);
        }
    } else {
        errorCode = TLV_ERR_TOO_LONG;
    }

    if (errorCode == TLV_ERR_NO_ERROR && decodedLink.linkName == NULL) {
        errorCode = TLV_ERR_DECODE;
    }

    if (errorCode != TLV_ERR_NO_ERROR) {
        CCNxCodecError *error = ccnxCodecError_Create(errorCode, __func__, __LINE__, ccnxCodecTlvDecoder_Position(decoder));
        ccnxCodecTlvDecoder_SetError(decoder, error);
        ccnxCodecError_Release(&error);
    } else {
        link = ccnxLink_Create(decodedLink.linkName, decodedLink.linkKeyId, decodedLink.linkHash);
    }

    // cleanup any partial memory allocations
    _decodecLinkCleanup(&decodedLink);

    return link;
}
