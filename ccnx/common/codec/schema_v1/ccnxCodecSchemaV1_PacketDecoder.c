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
#include <sys/time.h>

#include <LongBow/runtime.h>
#include <parc/algol/parc_Memory.h>
#include <ccnx/common/codec/ccnxCodec_TlvEncoder.h>
#include <ccnx/common/internal/ccnx_WireFormatFacadeV1.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_PacketDecoder.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_Types.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_FixedHeader.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_OptionalHeadersDecoder.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_MessageDecoder.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_ManifestDecoder.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_ValidationDecoder.h>

typedef struct rta_tlv_schema_v1_data {
    CCNxCodecTlvDecoder *decoder;
    CCNxTlvDictionary *packetDictionary;
} _CCNxCodecSchemaV1Data;

/**
 * Decodes the per-hop optional headers
 *
 * @param [in] data The packet decoder state
 *
 * @return true successful decode
 * @return false A decoding error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
static bool
_decodeOptionalHeaders(_CCNxCodecSchemaV1Data *data)
{
    size_t optionalHeaderLength = ccnxCodecSchemaV1FixedHeaderDecoder_GetOptionalHeaderLength(data->packetDictionary);
    CCNxCodecTlvDecoder *optionalHeaderDecoder = ccnxCodecTlvDecoder_GetContainer(data->decoder, optionalHeaderLength);

    bool success = ccnxCodecSchemaV1OptionalHeadersDecoder_Decode(optionalHeaderDecoder, data->packetDictionary);

    ccnxCodecTlvDecoder_Destroy(&optionalHeaderDecoder);
    return success;
}

/**
 * Decodes the "value" of the CPI "TLV"
 *
 * the CPI packet is encoded as a single TLV container of type 0xBEEF (detected in _decodeMessage).
 * At this point, the cpiDecoder wraps the CPI payload, which is the encapsulated JSON
 *
 * @param [in] cpiDecoder Decoder wrapping the value
 * @param [in] packetDictionary where to place the results
 *
 * @retval true Good decode
 * @retval false An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
static bool
_decodeCPI(CCNxCodecTlvDecoder *cpiDecoder, CCNxTlvDictionary *packetDictionary)
{
    // we just take the whole contents of the decoder and put in the the PAYLOAD dictionary entry.
    size_t length = ccnxCodecTlvDecoder_Remaining(cpiDecoder);
    PARCBuffer *payload = ccnxCodecTlvDecoder_GetValue(cpiDecoder, length);

    PARCJSON *json = parcJSON_ParseBuffer(payload);

    bool success = ccnxTlvDictionary_PutJson(packetDictionary,
                                             CCNxCodecSchemaV1TlvDictionary_MessageFastArray_PAYLOAD, json);
    parcJSON_Release(&json);
    parcBuffer_Release(&payload);
    return success;
}

/**
 * Decodes the CCNx message inside a TLV packet
 *
 * Creates an inner decoder that slices the decode buffer then passes that and our
 * message dictionary to the appropriate inner decoder.
 *
 * @param [in] data The packet decoder state
 *
 * @return true successful decode
 * @return false A decoding error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
static bool
_decodeMessage(_CCNxCodecSchemaV1Data *data)
{
    bool success = false;

    if (ccnxCodecTlvDecoder_EnsureRemaining(data->decoder, 4)) {
        // what kind of message are we looking at?
        // Note that this is based on the TLV container, not the fixed head PacketType
        uint16_t tlv_type = ccnxCodecTlvDecoder_GetType(data->decoder);
        uint16_t tlv_length = ccnxCodecTlvDecoder_GetLength(data->decoder);

        // ensure its a proper tlv type
        switch (tlv_type) {
            case CCNxCodecSchemaV1Types_MessageType_Interest: // fallthrough
            case CCNxCodecSchemaV1Types_MessageType_ContentObject: // fallthrough
            case CCNxCodecSchemaV1Types_MessageType_Control: // fallthrough
            case CCNxCodecSchemaV1Types_MessageType_Manifest: // fallthrough
                break;

            default:
                return false;
        }

        // cross check with the fixed header value
        // ccnxCodecSchemaV1FixedHeaderDecoder_Decode ensures that PacketLength is not less than HeaderLength
        size_t messageLength = ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketLength(data->packetDictionary) - ccnxCodecSchemaV1FixedHeaderDecoder_GetHeaderLength(data->packetDictionary);

        if (tlv_length <= messageLength && ccnxCodecTlvDecoder_EnsureRemaining(data->decoder, tlv_length)) {
            // This decode is for the "value" of the message, it does not include the wrapper
            CCNxCodecTlvDecoder *messageDecoder = ccnxCodecTlvDecoder_GetContainer(data->decoder, tlv_length);

            if (tlv_type == CCNxCodecSchemaV1Types_MessageType_Control) {
                // the CPI messages are not a proper "message" in that there's no inner TLV, its just data
                success = _decodeCPI(messageDecoder, data->packetDictionary);
            } else if (tlv_type == CCNxCodecSchemaV1Types_MessageType_Manifest) {
                ccnxTlvDictionary_SetMessageType_Manifest(data->packetDictionary, CCNxTlvDictionary_SchemaVersion_V1);
                success = ccnxCodecSchemaV1ManifestDecoder_Decode(messageDecoder, data->packetDictionary);
            } else {
                success = ccnxCodecSchemaV1MessageDecoder_Decode(messageDecoder, data->packetDictionary);
            }

            ccnxCodecTlvDecoder_Destroy(&messageDecoder);
        } else {
            // raise an error
            CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_TOO_LONG, __func__, __LINE__, ccnxCodecTlvDecoder_Position(data->decoder));
            ccnxCodecTlvDecoder_SetError(data->decoder, error);
            ccnxCodecError_Release(&error);
        }
    }

    return success;
}

static bool
_decodeValidationAlg(_CCNxCodecSchemaV1Data *data)
{
    bool success = false;

    if (ccnxCodecTlvDecoder_EnsureRemaining(data->decoder, 4)) {
        // what kind of message are we looking at?
        // Note that this is based on the TLV container, not the fixed head PacketType
        uint16_t tlv_type = ccnxCodecTlvDecoder_GetType(data->decoder);
        uint16_t tlv_length = ccnxCodecTlvDecoder_GetLength(data->decoder);

        if (tlv_type == CCNxCodecSchemaV1Types_MessageType_ValidationAlg &&
            ccnxCodecTlvDecoder_EnsureRemaining(data->decoder, tlv_length)) {
            CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_GetContainer(data->decoder, tlv_length);

            success = ccnxCodecSchemaV1ValidationDecoder_DecodeAlg(decoder, data->packetDictionary);

            ccnxCodecTlvDecoder_Destroy(&decoder);
        } else {
            // raise and error
            if (!ccnxCodecTlvDecoder_EnsureRemaining(data->decoder, tlv_length)) {
                // tlv_length goes beyond the decoder
                CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_TOO_LONG, __func__, __LINE__, ccnxCodecTlvDecoder_Position(data->decoder));
                ccnxCodecTlvDecoder_SetError(data->decoder, error);
                ccnxCodecError_Release(&error);
            } else {
                // not CCNxCodecSchemaV1Types_MessageType_ValidationAlg
                CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_DECODE, __func__, __LINE__, ccnxCodecTlvDecoder_Position(data->decoder));
                ccnxCodecTlvDecoder_SetError(data->decoder, error);
                ccnxCodecError_Release(&error);
            }
        }
    }

    return success;
}

static bool
_decodeValidationPayload(_CCNxCodecSchemaV1Data *data)
{
    bool success = false;

    if (ccnxCodecTlvDecoder_EnsureRemaining(data->decoder, 4)) {
        // what kind of message are we looking at?
        // Note that this is based on the TLV container, not the fixed head PacketType
        uint16_t tlv_type = ccnxCodecTlvDecoder_GetType(data->decoder);
        uint16_t tlv_length = ccnxCodecTlvDecoder_GetLength(data->decoder);

        if (tlv_type == CCNxCodecSchemaV1Types_MessageType_ValidationPayload &&
            ccnxCodecTlvDecoder_EnsureRemaining(data->decoder, tlv_length)) {
            CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_GetContainer(data->decoder, tlv_length);

            success = ccnxCodecSchemaV1ValidationDecoder_DecodePayload(decoder, data->packetDictionary);

            ccnxCodecTlvDecoder_Destroy(&decoder);
        }
    }

    return success;
}

bool
ccnxCodecSchemaV1PacketDecoder_Decode(CCNxCodecTlvDecoder *packetDecoder, CCNxTlvDictionary *packetDictionary)
{
    bool decodeSuccess = false;

    _CCNxCodecSchemaV1Data data;

    // we temporarily store this reference, but we do not destroy it.  This
    // is just to pass the reference down the decode chain, it is not
    // stored beyond the immediate scope.  Therefore, no reference acquired.
    data.packetDictionary = packetDictionary;
    data.decoder = packetDecoder;

    if (ccnxCodecSchemaV1FixedHeaderDecoder_Decode(data.decoder, data.packetDictionary)) {
        if (_decodeOptionalHeaders(&data)) {
            // Record the position we'd start the signature verification at
            size_t signatureStartPosition = ccnxCodecTlvDecoder_Position(data.decoder);


            // Mark the beginning of the ContentObject hash region.
            CCNxWireFormatFacadeV1_Implementation.setContentObjectHashRegionStart(data.packetDictionary, signatureStartPosition);

            if (_decodeMessage(&data)) {
                // If there's anything else left, it must be the validation alg and payload
                if (!ccnxCodecTlvDecoder_IsEmpty(data.decoder)) {

                    if (_decodeValidationAlg(&data)) {
                        // at this point, we've advanced to the end of the validation algorithm,
                        // that's where we would end signature verification
                        size_t signatureStopPosition = ccnxCodecTlvDecoder_Position(data.decoder);

                        CCNxWireFormatFacadeV1_Implementation.setProtectedRegionStart(data.packetDictionary, signatureStartPosition);
                        CCNxWireFormatFacadeV1_Implementation.setProtectedRegionLength(data.packetDictionary, signatureStopPosition - signatureStartPosition);

                        if (_decodeValidationPayload(&data)) {
                            decodeSuccess = true;
                        }
                    }
                } else {
                    // nothing after the message, so that's a successful decode
                    decodeSuccess = true;
                }

                // Mark the length of the ContentObject hash region (to the end of the packet).
                size_t contentObjectHashRegionLength = ccnxCodecTlvDecoder_Position(data.decoder) - signatureStartPosition;
                CCNxWireFormatFacadeV1_Implementation.setContentObjectHashRegionLength(data.packetDictionary, contentObjectHashRegionLength);
            }
        }
    }

    return decodeSuccess;
}

bool
ccnxCodecSchemaV1PacketDecoder_BufferDecode(PARCBuffer *packetBuffer, CCNxTlvDictionary *packetDictionary)
{
    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, packetDictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
    return success;
}
