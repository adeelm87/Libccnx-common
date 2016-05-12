/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Buffer.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_FixedHeader.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_Types.h>
#include <ccnx/common/codec/ccnxCodec_TlvUtilities.h>

static const size_t _fixedHeaderBytes = 8;

static const int _fixedHeader_VersionOffset = 0;
static const int _fixedHeader_PacketTypeOffset = 1;
static const int _fixedHeader_PacketLengthOffset = 2;
static const int _fixedHeader_HopLimitOffset = 4;
static const int _fixedHeader_ReturnCodeOffset = 5;
static const int _fixedHeader_FlagsOffset = 6;
static const int _fixedHeader_HeaderLengthOffset = 7;

bool
ccnxCodecSchemaV1FixedHeaderDecoder_Decode(CCNxCodecTlvDecoder *decoder, CCNxTlvDictionary *packetDictionary)
{
    if (ccnxCodecTlvDecoder_EnsureRemaining(decoder, _fixedHeaderBytes)) {
        PARCBuffer *buffer = ccnxCodecTlvDecoder_GetValue(decoder, _fixedHeaderBytes);
        bool success = ccnxTlvDictionary_PutBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader, buffer);

        // validation
        parcBuffer_SetPosition(buffer, _fixedHeader_VersionOffset);
        uint8_t version = parcBuffer_GetUint8(buffer);

        parcBuffer_SetPosition(buffer, _fixedHeader_PacketLengthOffset);
        uint16_t packetLength = parcBuffer_GetUint16(buffer);

        parcBuffer_SetPosition(buffer, _fixedHeader_ReturnCodeOffset);
        uint8_t interestReturnCode = parcBuffer_GetUint8(buffer);

        parcBuffer_SetPosition(buffer, _fixedHeader_HopLimitOffset);
        uint8_t hopLimit = parcBuffer_GetUint8(buffer);

        parcBuffer_SetPosition(buffer, _fixedHeader_HeaderLengthOffset);
        uint8_t headerLength = parcBuffer_GetUint8(buffer);

        if (version != 1) {
            CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_VERSION, __func__, __LINE__, _fixedHeader_VersionOffset);
            ccnxCodecTlvDecoder_SetError(decoder, error);
            ccnxCodecError_Release(&error);
            success = false;
        } else if (packetLength < _fixedHeaderBytes) {
            CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_PACKETLENGTH_TOO_SHORT, __func__, __LINE__, _fixedHeader_PacketTypeOffset);
            ccnxCodecTlvDecoder_SetError(decoder, error);
            ccnxCodecError_Release(&error);
            success = false;
        } else if (headerLength < _fixedHeaderBytes) {
            CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_HEADERLENGTH_TOO_SHORT, __func__, __LINE__, _fixedHeader_HeaderLengthOffset);
            ccnxCodecTlvDecoder_SetError(decoder, error);
            ccnxCodecError_Release(&error);
            success = false;
        } else if (packetLength < headerLength) {
            CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_PACKETLENGTHSHORTER, __func__, __LINE__, _fixedHeader_PacketTypeOffset);
            ccnxCodecTlvDecoder_SetError(decoder, error);
            ccnxCodecError_Release(&error);
            success = false;
        }

        // decoder now points to just past the fixed header
        parcBuffer_Release(&buffer);

        // Set the hoplimit in the dictionary.
        ccnxTlvDictionary_PutInteger(packetDictionary,
                                     CCNxCodecSchemaV1TlvDictionary_MessageFastArray_HOPLIMIT, hopLimit);

        // Set the InterestReturn code in the dictionary.
        ccnxTlvDictionary_PutInteger(packetDictionary,
                                     CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_InterestReturnCode,
                                     interestReturnCode);

        return success;
    } else {
        CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_DECODE, __func__, __LINE__, ccnxCodecTlvDecoder_Position(decoder));
        ccnxCodecTlvDecoder_SetError(decoder, error);
        ccnxCodecError_Release(&error);
        return false;
    }
}

int
ccnxCodecSchemaV1FixedHeaderDecoder_GetVersion(CCNxTlvDictionary *packetDictionary)
{
    PARCBuffer *fixedHeader = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader);
    if (fixedHeader != NULL) {
        parcBuffer_SetPosition(fixedHeader, _fixedHeader_VersionOffset);
        uint8_t version = parcBuffer_GetUint8(fixedHeader);
        return version;
    }

    return -1;
}

int
ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketType(CCNxTlvDictionary *packetDictionary)
{
    PARCBuffer *fixedHeader = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader);
    if (fixedHeader != NULL) {
        parcBuffer_SetPosition(fixedHeader, _fixedHeader_PacketTypeOffset);
        uint8_t packetType = parcBuffer_GetUint8(fixedHeader);
        return packetType;
    }

    return -1;
}

int
ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketLength(CCNxTlvDictionary *packetDictionary)
{
    PARCBuffer *fixedHeader = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader);
    if (fixedHeader != NULL) {
        parcBuffer_SetPosition(fixedHeader, _fixedHeader_PacketLengthOffset);
        uint16_t payloadLength = parcBuffer_GetUint16(fixedHeader);
        return payloadLength;
    }

    return -1;
}

int
ccnxCodecSchemaV1FixedHeaderDecoder_GetHeaderLength(CCNxTlvDictionary *packetDictionary)
{
    int length = -1;
    PARCBuffer *fixedHeader = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader);
    if (fixedHeader != NULL) {
        parcBuffer_SetPosition(fixedHeader, _fixedHeader_HeaderLengthOffset);
        uint8_t headerLength = parcBuffer_GetUint8(fixedHeader);

        // 8 is the minimum size of headerLength
        if (headerLength >= _fixedHeaderBytes) {
            length = headerLength;
        }
    }

    return length;
}

int
ccnxCodecSchemaV1FixedHeaderDecoder_GetOptionalHeaderLength(CCNxTlvDictionary *packetDictionary)
{
    int headerLength = ccnxCodecSchemaV1FixedHeaderDecoder_GetHeaderLength(packetDictionary);
    return headerLength - _fixedHeaderBytes;
}

int
ccnxCodecSchemaV1FixedHeaderDecoder_GetHopLimit(CCNxTlvDictionary *packetDictionary)
{
    PARCBuffer *fixedHeader = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader);
    if (fixedHeader != NULL) {
        parcBuffer_SetPosition(fixedHeader, _fixedHeader_HopLimitOffset);
        uint8_t hopLimit = parcBuffer_GetUint8(fixedHeader);
        return hopLimit;
    }

    return -1;
}

int
ccnxCodecSchemaV1FixedHeaderDecoder_GetReturnCode(CCNxTlvDictionary *packetDictionary)
{
    PARCBuffer *fixedHeader = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader);
    if (fixedHeader != NULL) {
        parcBuffer_SetPosition(fixedHeader, _fixedHeader_ReturnCodeOffset);
        uint8_t returnCode = parcBuffer_GetUint8(fixedHeader);
        return returnCode;
    }

    return -1;
}

int
ccnxCodecSchemaV1FixedHeaderDecoder_GetFlags(CCNxTlvDictionary *packetDictionary)
{
    PARCBuffer *fixedHeader = ccnxTlvDictionary_GetBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader);
    if (fixedHeader != NULL) {
        parcBuffer_SetPosition(fixedHeader, _fixedHeader_FlagsOffset);
        uint8_t flags = parcBuffer_GetUint8(fixedHeader);
        return flags;
    }

    return -1;
}
