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
#include <LongBow/runtime.h>
#include <arpa/inet.h>
#include <ccnx/common/codec/ccnxCodec_TlvPacket.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_Types.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_PacketDecoder.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_PacketEncoder.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_FixedHeader.h>

static CCNxTlvDictionary *
_decodeV1(PARCBuffer *packetBuffer)
{
    CCNxTlvDictionary *packetDictionary = NULL;

    CCNxCodecSchemaV1Types_PacketType packetType = (CCNxCodecSchemaV1Types_PacketType) parcBuffer_GetAtIndex(packetBuffer, 1);

    switch (packetType) {
        case CCNxCodecSchemaV1Types_PacketType_Interest:
            packetDictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
            break;

        case CCNxCodecSchemaV1Types_PacketType_ContentObject:
            packetDictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
            break;

        case CCNxCodecSchemaV1Types_PacketType_InterestReturn:
            // not implemented yet
            break;

        case CCNxCodecSchemaV1Types_PacketType_Control:
            packetDictionary = ccnxCodecSchemaV1TlvDictionary_CreateControl();
            break;

        default:
            // unknown type
            break;
    }

    if (packetDictionary) {
        // The packetBuffer may be padded or have extraneous content after the CCNx message.
        // Ensure that the buffer limit reflects the CCNx packet length as the decoder uses
        // the that limit, not the packetLength from the header, to determine when to stop parsing.
        size_t packetBufferLength = ccnxCodecTlvPacket_GetPacketLength(packetBuffer);
        assertTrue(packetBufferLength <= parcBuffer_Remaining(packetBuffer), "Short packet buffer");
        parcBuffer_SetLimit(packetBuffer, packetBufferLength);
        bool success = ccnxCodecSchemaV1PacketDecoder_BufferDecode(packetBuffer, packetDictionary);
        if (!success) {
            ccnxTlvDictionary_Release(&packetDictionary);
        }
    }
    return packetDictionary;
}

CCNxTlvDictionary *
ccnxCodecTlvPacket_Decode(PARCBuffer *packetBuffer)
{
    return _decodeV1(packetBuffer);
}

bool
ccnxCodecTlvPacket_BufferDecode(PARCBuffer *packetBuffer, CCNxTlvDictionary *packetDictionary)
{
    // Determine the version from the first byte of the buffer
    uint8_t version = parcBuffer_GetAtIndex(packetBuffer, 0);

    // The packetBuffer may be padded or have extraneous content after the CCNx message.
    // Ensure that the buffer limit reflects the CCNx packet length as the decoder uses
    // the that limit, not the packetLength from the header, to determine when to stop parsing.
    size_t packetBufferLength = ccnxCodecTlvPacket_GetPacketLength(packetBuffer);
    assertTrue(packetBufferLength <= parcBuffer_Remaining(packetBuffer), "Short packet buffer");
    parcBuffer_SetLimit(packetBuffer, packetBufferLength);

    bool success = false;
    switch (version) {
        case CCNxTlvDictionary_SchemaVersion_V1:
            success = ccnxCodecSchemaV1PacketDecoder_BufferDecode(packetBuffer, packetDictionary);
            break;

        default:
            // will return false
            break;
    }

    return success;
}

/*
 * We don't have an iovec based decoder yet, so linearize the memory and use a PARCBuffer
 * See case 903.
 */
bool
ccnxCodecTlvPacket_IoVecDecode(CCNxCodecNetworkBufferIoVec *vec, CCNxTlvDictionary *packetDictionary)
{
    size_t iovcnt = ccnxCodecNetworkBufferIoVec_GetCount(vec);
    const struct iovec *array = ccnxCodecNetworkBufferIoVec_GetArray(vec);

    PARCBuffer *buffer = NULL;
    if (iovcnt == 1) {
        buffer = parcBuffer_Wrap(array[0].iov_base, array[0].iov_len, 0, array[0].iov_len);
    } else if (iovcnt > 1) {
        // figure out total size, then linearize it
        size_t totalbytes = 0;
        for (int i = 0; i < iovcnt; i++) {
            totalbytes += array[i].iov_len;
        }

        buffer = parcBuffer_Allocate(totalbytes);
        for (int i = 0; i < iovcnt; i++) {
            parcBuffer_PutArray(buffer, array[i].iov_len, array[i].iov_base);
        }

        parcBuffer_Flip(buffer);
    } else {
        return false;
    }

    bool success = ccnxCodecTlvPacket_BufferDecode(buffer, packetDictionary);
    parcBuffer_Release(&buffer);
    return success;
}

CCNxCodecNetworkBufferIoVec *
ccnxCodecTlvPacket_DictionaryEncode(CCNxTlvDictionary *packetDictionary, PARCSigner *signer)
{
    CCNxTlvDictionary_SchemaVersion version = ccnxTlvDictionary_GetSchemaVersion(packetDictionary);

    CCNxCodecNetworkBufferIoVec *iovec = NULL;
    switch (version) {
        case CCNxTlvDictionary_SchemaVersion_V1:
            iovec = ccnxCodecSchemaV1PacketEncoder_DictionaryEncode(packetDictionary, signer);
            break;

        default:
            // will return NULL
            break;
    }
    return iovec;
}

size_t
ccnxCodecTlvPacket_GetPacketLength(PARCBuffer *packetBuffer)
{
    size_t length = 0;

    // Determine the version from the first byte of the buffer
    uint8_t *header = parcBuffer_Overlay(packetBuffer, 0);

    switch (header[0]) {
        case CCNxTlvDictionary_SchemaVersion_V1: { // V1 - from metis_TlvSchemaV1.c:_totalPacketLength
            CCNxCodecSchemaV1FixedHeader *headerV1 = (CCNxCodecSchemaV1FixedHeader *) header;
            length = htons(headerV1->packetLength);
            break;
        }
        default:
            break;
    }

    return length;
}

// When new versions are created they need to be incorporated here so enough header information
// can be read to determine how to proceed.
size_t
ccnxCodecTlvPacket_MinimalHeaderLength()
{
    size_t minimumHeaderLength;
    minimumHeaderLength = sizeof(CCNxCodecSchemaV1FixedHeader);
    return minimumHeaderLength;
}
