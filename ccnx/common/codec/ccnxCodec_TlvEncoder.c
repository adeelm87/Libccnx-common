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
 * We use a 2-byte T and a 2-byte L
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <LongBow/runtime.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Buffer.h>
#include <ccnx/common/codec/ccnxCodec_NetworkBuffer.h>

#include <ccnx/common/codec/ccnxCodec_TlvEncoder.h>

#define NONE_SET  0
#define START_SET 1
#define END_SET   2
#define BOTH_SET  3

struct ccnx_codec_tlv_encoder {
    CCNxCodecNetworkBuffer *buffer;

    // OR of NONE_SET, START_SET, END_SET
    int signatureStartEndSet;

    size_t signatureStart;
    size_t signatureEnd;

    CCNxCodecError *error;
    PARCSigner *signer;
};

CCNxCodecTlvEncoder *
ccnxCodecTlvEncoder_Create(void)
{
    CCNxCodecTlvEncoder *encoder = parcMemory_AllocateAndClear(sizeof(CCNxCodecTlvEncoder));
    assertNotNull(encoder, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(CCNxCodecTlvEncoder));

    encoder->buffer = ccnxCodecNetworkBuffer_Create(&ParcMemoryMemoryBlock, NULL);
    encoder->signatureStartEndSet = NONE_SET;
    encoder->error = NULL;

    return encoder;
}

void
ccnxCodecTlvEncoder_Destroy(CCNxCodecTlvEncoder **encoderPtr)
{
    assertNotNull(encoderPtr, "Parameter must be non-null double pointer");
    assertNotNull(*encoderPtr, "Parameter must dereferecne to non-null pointer");
    CCNxCodecTlvEncoder *encoder = *encoderPtr;

    ccnxCodecNetworkBuffer_Release(&encoder->buffer);

    if (encoder->error) {
        ccnxCodecError_Release(&encoder->error);
    }

    if (encoder->signer) {
        parcSigner_Release(&encoder->signer);
    }

    parcMemory_Deallocate((void **) &encoder);
    *encoderPtr = NULL;
}

CCNxCodecTlvEncoder *
ccnxCodecTlvEncoder_Initialize(CCNxCodecTlvEncoder *encoder)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    return encoder;
}

size_t
ccnxCodecTlvEncoder_AppendBuffer(CCNxCodecTlvEncoder *encoder, uint16_t type, PARCBuffer *value)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    assertTrue(parcBuffer_Remaining(value) <= UINT16_MAX, "Value length too long, got %zu maximum %u\n", parcBuffer_Remaining(value), UINT16_MAX);

    size_t bytes = 4 + parcBuffer_Remaining(value);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, type);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, parcBuffer_Remaining(value));
    ccnxCodecNetworkBuffer_PutBuffer(encoder->buffer, value);

    return bytes;
}

size_t
ccnxCodecTlvEncoder_AppendArray(CCNxCodecTlvEncoder *encoder, uint16_t type, uint16_t length, const uint8_t array[length])
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, type);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, length);
    ccnxCodecNetworkBuffer_PutArray(encoder->buffer, length, array);
    return length + 4;
}

size_t
ccnxCodecTlvEncoder_AppendContainer(CCNxCodecTlvEncoder *encoder, uint16_t type, uint16_t length)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");

    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, type);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, length);
    return 4;
}

size_t
ccnxCodecTlvEncoder_AppendUint8(CCNxCodecTlvEncoder *encoder, uint16_t type, uint8_t value)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, type);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, 1);
    ccnxCodecNetworkBuffer_PutUint8(encoder->buffer, value);
    return 5;
}

size_t
ccnxCodecTlvEncoder_AppendUint16(CCNxCodecTlvEncoder *encoder, uint16_t type, uint16_t value)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, type);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, 2);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, value);
    return 6;
}

size_t
ccnxCodecTlvEncoder_AppendUint32(CCNxCodecTlvEncoder *encoder, uint16_t type, uint32_t value)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, type);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, 4);
    ccnxCodecNetworkBuffer_PutUint32(encoder->buffer, value);
    return 8;
}

size_t
ccnxCodecTlvEncoder_AppendUint64(CCNxCodecTlvEncoder *encoder, uint16_t type, uint64_t value)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, type);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, 8);
    ccnxCodecNetworkBuffer_PutUint64(encoder->buffer, value);
    return 12;
}

static unsigned
_ccnxCodecTlvEncoder_ComputeVarIntLength(uint64_t value)
{
    unsigned length = 8;
    if (value <= 0x00000000000000FFULL) {
        length = 1;
    } else if (value <= 0x000000000000FFFFULL) {
        length = 2;
    } else if (value <= 0x0000000000FFFFFFULL) {
        length = 3;
    } else if (value <= 0x00000000FFFFFFFFULL) {
        length = 4;
    } else if (value <= 0x000000FFFFFFFFFFULL) {
        length = 5;
    } else if (value <= 0x0000FFFFFFFFFFFFULL) {
        length = 6;
    } else if (value <= 0x00FFFFFFFFFFFFFFULL) {
        length = 7;
    }

    return length;
}

size_t
ccnxCodecTlvEncoder_AppendVarInt(CCNxCodecTlvEncoder *encoder, uint16_t type, uint64_t value)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");

    unsigned length = _ccnxCodecTlvEncoder_ComputeVarIntLength(value);

    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, type);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, length);

    // See case 1007
    bool mustContinue = false;
    for (int byte = 7; byte >= 0; byte--) {
        uint8_t b = (value >> (byte * 8)) & 0xFF;
        if (b != 0 || byte == 0 || mustContinue) {
            ccnxCodecNetworkBuffer_PutUint8(encoder->buffer, b);
            mustContinue = true;
        }
    }

    return length + 4;
}

size_t
ccnxCodecTlvEncoder_Position(CCNxCodecTlvEncoder *encoder)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    return ccnxCodecNetworkBuffer_Position(encoder->buffer);
}

size_t
ccnxCodecTlvEncoder_SetPosition(CCNxCodecTlvEncoder *encoder, size_t position)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    assertTrue(position <= ccnxCodecNetworkBuffer_Limit(encoder->buffer),
               "position beyond end of buffer, got %zu maximum %zu",
               position, ccnxCodecNetworkBuffer_Limit(encoder->buffer));
    ccnxCodecNetworkBuffer_SetPosition(encoder->buffer, position);
    return position;
}

void
ccnxCodecTlvEncoder_SetContainerLength(CCNxCodecTlvEncoder *encoder, size_t offset, uint16_t length)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");

    size_t currentPosition = ccnxCodecNetworkBuffer_Position(encoder->buffer);

    // +2 because we skip over the Type field
    ccnxCodecNetworkBuffer_SetPosition(encoder->buffer, offset + 2);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, length);

    ccnxCodecNetworkBuffer_SetPosition(encoder->buffer, currentPosition);
}

void
ccnxCodecTlvEncoder_Finalize(CCNxCodecTlvEncoder *encoder)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");

    // set the limit to whatever our current possition is.  That will truncate the
    // packet in case we wrote beyond where we are now.

    ccnxCodecNetworkBuffer_Finalize(encoder->buffer);
}

PARCBuffer *
ccnxCodecTlvEncoder_CreateBuffer(CCNxCodecTlvEncoder *encoder)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");

    PARCBuffer *output = ccnxCodecNetworkBuffer_CreateParcBuffer(encoder->buffer);
    return output;
}

CCNxCodecNetworkBufferIoVec *
ccnxCodecTlvEncoder_CreateIoVec(CCNxCodecTlvEncoder *encoder)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    return ccnxCodecNetworkBuffer_CreateIoVec(encoder->buffer);
}

size_t
ccnxCodecTlvEncoder_AppendRawArray(CCNxCodecTlvEncoder *encoder, size_t length, uint8_t array[length])
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    ccnxCodecNetworkBuffer_PutArray(encoder->buffer, length, array);
    return length;
}

size_t
ccnxCodecTlvEncoder_PutUint8(CCNxCodecTlvEncoder *encoder, size_t offset, uint8_t value)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    size_t position = ccnxCodecNetworkBuffer_Position(encoder->buffer);
    ccnxCodecNetworkBuffer_SetPosition(encoder->buffer, offset);
    ccnxCodecNetworkBuffer_PutUint8(encoder->buffer, value);
    ccnxCodecNetworkBuffer_SetPosition(encoder->buffer, position);
    return 1;
}

size_t
ccnxCodecTlvEncoder_PutUint16(CCNxCodecTlvEncoder *encoder, size_t offset, uint16_t value)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    size_t position = ccnxCodecNetworkBuffer_Position(encoder->buffer);
    ccnxCodecNetworkBuffer_SetPosition(encoder->buffer, offset);
    ccnxCodecNetworkBuffer_PutUint16(encoder->buffer, value);
    ccnxCodecNetworkBuffer_SetPosition(encoder->buffer, position);
    return 2;
}

void
ccnxCodecTlvEncoder_MarkSignatureStart(CCNxCodecTlvEncoder *encoder)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    encoder->signatureStart = ccnxCodecNetworkBuffer_Position(encoder->buffer);
    encoder->signatureStartEndSet |= START_SET;
}

void
ccnxCodecTlvEncoder_MarkSignatureEnd(CCNxCodecTlvEncoder *encoder)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    encoder->signatureEnd = ccnxCodecNetworkBuffer_Position(encoder->buffer);
    encoder->signatureStartEndSet |= END_SET;
}

PARCSignature *
ccnxCodecTlvEncoder_ComputeSignature(CCNxCodecTlvEncoder *encoder)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    assertTrue(encoder->signatureStartEndSet == BOTH_SET, "Did not set both start and end positions");

    return ccnxCodecNetworkBuffer_ComputeSignature(encoder->buffer, encoder->signatureStart, encoder->signatureEnd, encoder->signer);
}

bool
ccnxCodecTlvEncoder_HasError(const CCNxCodecTlvEncoder *encoder)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    if (encoder->error) {
        return true;
    }

    return false;
}

bool
ccnxCodecTlvEncoder_SetError(CCNxCodecTlvEncoder *encoder, CCNxCodecError *error)
{
    if (ccnxCodecTlvEncoder_HasError(encoder)) {
        return false;
    }

    encoder->error = ccnxCodecError_Acquire(error);
    return true;
}

void
ccnxCodecTlvEncoder_ClearError(CCNxCodecTlvEncoder *encoder)
{
    if (ccnxCodecTlvEncoder_HasError(encoder)) {
        ccnxCodecError_Release(&encoder->error);
    }
}

CCNxCodecError *
ccnxCodecTlvEncoder_GetError(const CCNxCodecTlvEncoder *encoder)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    return encoder->error;
}

void
ccnxCodecTlvEncoder_SetSigner(CCNxCodecTlvEncoder *encoder, PARCSigner *signer)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    if (encoder->signer) {
        parcSigner_Release(&encoder->signer);
    }

    if (signer) {
        encoder->signer = parcSigner_Acquire(signer);
    } else {
        encoder->signer = NULL;
    }
}

PARCSigner *
ccnxCodecTlvEncoder_GetSigner(const CCNxCodecTlvEncoder *encoder)
{
    assertNotNull(encoder, "Parameter encoder must be non-null");
    return encoder->signer;
}

