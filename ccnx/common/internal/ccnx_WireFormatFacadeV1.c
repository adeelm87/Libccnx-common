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
#include <fcntl.h>
#include <sys/stat.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_FileOutputStream.h>

#include <ccnx/common/internal/ccnx_WireFormatMessageInterface.h>
#include <ccnx/common/internal/ccnx_WireFormatFacadeV1.h>
#include <ccnx/common/ccnx_WireFormatMessage.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_Types.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_FixedHeader.h>


static CCNxTlvDictionary *
_ccnxWireFormatFacadeV1_FromInterestPacketType(const PARCBuffer *wireFormat)
{
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    ccnxTlvDictionary_PutBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_WireFormat, wireFormat);
    return dictionary;
}

static CCNxTlvDictionary *
_ccnxWireFormatFacadeV1_FromInterestPacketTypeIoVec(const CCNxCodecNetworkBufferIoVec *vec)
{
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    ccnxTlvDictionary_PutIoVec(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_WireFormat, vec);
    return dictionary;
}

static CCNxTlvDictionary *
_ccnxWireFormatFacadeV1_FromInterestReturnPacketType(const PARCBuffer *wireFormat)
{
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterestReturn();
    ccnxTlvDictionary_PutBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_WireFormat, wireFormat);
    return dictionary;
}

static CCNxTlvDictionary *
_ccnxWireFormatFacadeV1_FromContentObjectPacketType(const PARCBuffer *wireFormat)
{
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    ccnxTlvDictionary_PutBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_WireFormat, wireFormat);
    return dictionary;
}

static CCNxTlvDictionary *
_ccnxWireFormatFacadeV1_FromControlPacketType(const PARCBuffer *wireFormat)
{
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateControl();
    ccnxTlvDictionary_PutBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_WireFormat, wireFormat);
    return dictionary;
}

static CCNxCodecSchemaV1InterestHeader *
_getWireFormatFixedHeader(CCNxTlvDictionary *dictionary)
{
    // Currently there is only one of either a PARCBuffer or an IoVec ...

    CCNxCodecSchemaV1InterestHeader *header = NULL;

    // Update attached iovec
    CCNxCodecNetworkBufferIoVec *iovec = ccnxWireFormatMessage_GetIoVec(dictionary);
    if (iovec) {
        assertTrue(ccnxCodecNetworkBufferIoVec_Length(iovec) >= sizeof(CCNxCodecSchemaV1InterestHeader),
                   "IoVector smaller than required for message header");
        const struct iovec *iov = ccnxCodecNetworkBufferIoVec_GetArray(iovec);
        assertTrue(iov[0].iov_len >= sizeof(CCNxCodecSchemaV1InterestHeader),
                   "Header not contained in first element of io vector");
        header = iov[0].iov_base;
    } else {
        // Update attached buffer
        PARCBuffer *wireFormatBuffer = ccnxWireFormatMessage_GetWireFormatBuffer(dictionary);
        if (wireFormatBuffer) {
            header = parcBuffer_Overlay(wireFormatBuffer, 0);
        }
    }

    return header;
}

static bool
_ccnxWireFormatFacadeV1_SetHopLimit(CCNxTlvDictionary *dictionary, uint32_t hopLimit)
{
    bool result = false;

    CCNxCodecSchemaV1InterestHeader *header = _getWireFormatFixedHeader(dictionary);
    if (header != NULL) {
        header->hopLimit = hopLimit;
        result = true;
    }
    return result;
}

static bool
_ccnxWireFormatFacadeV1_ConvertInterestToInterestReturn(CCNxTlvDictionary *dictionary, uint8_t code)
{
    bool result = false;

    CCNxCodecSchemaV1InterestHeader *header = _getWireFormatFixedHeader(dictionary);
    if (header != NULL) {
        header->returnCode = code;
        header->packetType = CCNxCodecSchemaV1Types_PacketType_InterestReturn;
        result = true;
    }
    return result;
}

static CCNxTlvDictionary *
_ccnxWireFormatFacadeV1_CreateFromV1(const PARCBuffer *wireFormat)
{
    CCNxTlvDictionary *dictionary = NULL;
    uint8_t packetType = parcBuffer_GetAtIndex(wireFormat, 1);
    switch (packetType) {
        case CCNxCodecSchemaV1Types_PacketType_ContentObject:
            dictionary = _ccnxWireFormatFacadeV1_FromContentObjectPacketType(wireFormat);
            break;
        case CCNxCodecSchemaV1Types_PacketType_Control:
            dictionary = _ccnxWireFormatFacadeV1_FromControlPacketType(wireFormat);
            break;
        case CCNxCodecSchemaV1Types_PacketType_Interest:
            dictionary = _ccnxWireFormatFacadeV1_FromInterestPacketType(wireFormat);
            break;
        case CCNxCodecSchemaV1Types_PacketType_InterestReturn:
            dictionary = _ccnxWireFormatFacadeV1_FromInterestReturnPacketType(wireFormat);
            break;
        default:
            // will return NULL
            break;
    }
    return dictionary;
}

static PARCBuffer *
_ccnxWireFormatFacadeV1_GetWireFormatBuffer(const CCNxTlvDictionary *dictionary)
{
    PARCBuffer *wireFormat = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_WireFormat);
    return wireFormat;
}

static bool
_ccnxWireFormatFacadeV1_PutWireFormatBuffer(CCNxTlvDictionary *dictionary, PARCBuffer *wireFormat)
{
    bool success = ccnxTlvDictionary_PutBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_WireFormat, wireFormat);
    return success;
}

static CCNxCodecNetworkBufferIoVec *
_ccnxWireFormatFacadeV1_GetIoVec(const CCNxTlvDictionary *dictionary)
{
    CCNxCodecNetworkBufferIoVec *vec = ccnxTlvDictionary_GetIoVec(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_WireFormat);
    return vec;
}

static bool
_ccnxWireFormatFacadeV1_PutIoVec(CCNxTlvDictionary *dictionary, CCNxCodecNetworkBufferIoVec *vec)
{
    bool success = ccnxTlvDictionary_PutIoVec(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_WireFormat, vec);
    return success;
}

static void
_ccnxWireFormatFacadeV1_WriteToFile(const CCNxTlvDictionary *dictionary, const char *filename)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    PARCFileOutputStream *fos = parcFileOutputStream_Create(fd);

    PARCBuffer *wireFormat = _ccnxWireFormatFacadeV1_GetWireFormatBuffer(dictionary);
    if (wireFormat) {
        parcFileOutputStream_Write(fos, wireFormat);
        parcBuffer_Rewind(wireFormat);
    }

    // this will close the file descriptor
    parcFileOutputStream_Release(&fos);
}

static bool
_ccnxWireFormatFacadeV1_SetProtectedRegionStart(CCNxTlvDictionary *dictionary, size_t startPosition)
{
    assertNotNull(dictionary, "Parameter dictionary must be non-null");
    // ccnx packets work on 16-bit lengths
    assertTrue(startPosition <= UINT16_MAX, "Start position beyond UINT16_MAX: %zu", startPosition);

    bool success = ccnxTlvDictionary_PutInteger(dictionary,
                                                CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ProtectedStart,
                                                startPosition);
    return success;
}

static bool
_ccnxWireFormatFacadeV1_SetProtectedRegionLength(CCNxTlvDictionary *dictionary, size_t length)
{
    assertNotNull(dictionary, "Parameter dictionary must be non-null");
    // ccnx packets work on 16-bit lengths
    assertTrue(length <= UINT16_MAX, "Length position beyond UINT16_MAX: %zu", length);

    bool success = ccnxTlvDictionary_PutInteger(dictionary,
                                                CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ProtectedLength,
                                                length);
    return success;
}

static bool
_ccnxWireFormatFacadeV1_SetContentObjectHashRegionStart(CCNxTlvDictionary *dictionary, size_t startPosition)
{
    assertNotNull(dictionary, "Parameter dictionary must be non-null");
    // ccnx packets work on 16-bit lengths
    assertTrue(startPosition <= UINT16_MAX, "Start position beyond UINT16_MAX: %zu", startPosition);

    bool success = ccnxTlvDictionary_PutInteger(dictionary,
                                                CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ContentObjectHashRegionStart,
                                                startPosition);
    return success;
}

static bool
_ccnxWireFormatFacadeV1_SetContentObjectHashRegionLength(CCNxTlvDictionary *dictionary, size_t length)
{
    assertNotNull(dictionary, "Parameter dictionary must be non-null");
    // ccnx packets work on 16-bit lengths
    assertTrue(length <= UINT16_MAX, "Length position beyond UINT16_MAX: %zu", length);

    bool success = ccnxTlvDictionary_PutInteger(dictionary,
                                                CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ContentObjectHashRegionLength,
                                                length);
    return success;
}


/*
 * Compute the hash over the specified region.  This is a static function, so we know the caller
 * has verified all the parameters before giving them to us.
 * The caller must also verify that (start + length <= totalLength).
 *
 * As we walk through the iovecs, the variables will look like this
 * +-----------+-----------+-----------+-----------+-----------+
 *    iov[0]       iov[1]      iov[2]      iov[3]
 * +-----------+-----------+-----------+-----------+-----------+
 *             ^       ^   ^                  ^
 *             |       |   |                  |
 *             |     start |                 end
 *          iovStart     iovEnd
 *             |-------|---|
 *               offset
 *                     remaining
 *
 * +-----------+-----------+-----------+-----------+-----------+
 *    iov[0]       iov[1]      iov[2]      iov[3]
 * +-----------+-----------+-----------+-----------+-----------+
 *                         ^           ^      ^
 *                         |           |      |
 *                       start         |     end
 *                      iovStart     iovEnd
 *                         |-----------|
 *                      offset (0)
 *                           remaining (iov_len)
 *
 * +-----------+-----------+-----------+-----------+-----------+
 *    iov[0]       iov[1]      iov[2]      iov[3]
 * +-----------+-----------+-----------+-----------+-----------+
 *                                     ^      ^
 *                                     |      |
 *                                   start   end
 *                                 iovStart iovEnd
 *                                     |------|
 *                                  offset (0)
 *                                        remaining (end-start)
 *
 */
static PARCCryptoHash *
_hashProtectedRegionIoVec(CCNxCodecNetworkBufferIoVec *vec, PARCCryptoHasher *hasher, size_t start, size_t length)
{
    int failure = parcCryptoHasher_Init(hasher);
    assertFalse(failure, "Error initializing the hasher");

    const struct iovec *iov = ccnxCodecNetworkBufferIoVec_GetArray(vec);
    int iovcnt = ccnxCodecNetworkBufferIoVec_GetCount(vec);

    // iovStart is the beginning position of the current iovec
    size_t iovStart = 0;
    size_t end = start + length;

    // Once iovStart is past end, we no longer need to loop, we're done
    for (int i = 0; i < iovcnt && iovStart < end; i++) {
        size_t iovEnd = iovStart + iov[i].iov_len;
        iovEnd = (iovEnd > end) ? end : iovEnd;

        // is "start" contained in this iovec?
        if (iovStart <= start && start < iovEnd) {
            size_t offset = start - iovStart;
            size_t remaining = iovEnd - start;

            failure = parcCryptoHasher_UpdateBytes(hasher, (const void *) ((uint8_t *) iov[i].iov_base + offset), remaining);
            assertFalse(failure, "Error updating hasher iovec %d offset %zu remaining %zu", i, offset, remaining)
            {
                ccnxCodecNetworkBufferIoVec_Display(vec, 3);
            }

            // keep moving start to the next iovec
            start += remaining;
        }

        iovStart += iov[i].iov_len;
    }

    PARCCryptoHash *hash = parcCryptoHasher_Finalize(hasher);

    return hash;
}


/*
 * Compute the hash over the specified region.  This is a static function, so we know the caller
 * has verified all the parameters before giving them to us.
 * The caller must also verify that (start + length <= parcBuffer_Limit).
 *
 * +-----------+-----------+-----------+-----------+-----------+
 *                          PARCBuffer
 * +-----------+-----------+-----------+-----------+-----------+
 * ^                   ^                      ^         ^
 * |                   |                      |         |
 * 0                 start                   end      Limit
 */
static PARCCryptoHash *
_ccnxWireFormatFacadeV1_ComputeBufferHash(PARCBuffer *wireFormat, PARCCryptoHasher *hasher, size_t start, size_t length)
{
    int failure = parcCryptoHasher_Init(hasher);
    assertTrue(failure == 0, "Error initializing the hasher");

    parcBuffer_SetPosition(wireFormat, start);
    uint8_t *overlay = parcBuffer_Overlay(wireFormat, 0);

    failure = parcCryptoHasher_UpdateBytes(hasher, overlay, length);
    assertTrue(failure == 0, "Error updating hasher start %zu length %zu", start, length)
    {
        parcBuffer_Display(wireFormat, 3);
    }

    parcBuffer_Rewind(wireFormat);

    PARCCryptoHash *hash = parcCryptoHasher_Finalize(hasher);
    return hash;
}

static PARCCryptoHash *
_ccnxWireFormatFacadeV1_HashProtectedRegion(const CCNxTlvDictionary *dictionary, PARCCryptoHasher *hasher)
{
    assertNotNull(dictionary, "Parameter dictionary must be non-null");
    assertNotNull(hasher, "Parameter hasher must be non-null");

    PARCCryptoHash *hash = NULL;

    if (ccnxTlvDictionary_IsValueInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ProtectedStart)) {
        if (ccnxTlvDictionary_IsValueInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ProtectedLength)) {
            size_t startPosition = ccnxTlvDictionary_GetInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ProtectedStart);
            size_t length = ccnxTlvDictionary_GetInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ProtectedLength);

            CCNxCodecNetworkBufferIoVec *vec = _ccnxWireFormatFacadeV1_GetIoVec(dictionary);
            if (vec) {
                size_t totalLength = ccnxCodecNetworkBufferIoVec_Length(vec);
                if (startPosition + length <= totalLength) {
                    hash = _hashProtectedRegionIoVec(vec, hasher, startPosition, length);
                }
            } else {
                PARCBuffer *wireFormat = _ccnxWireFormatFacadeV1_GetWireFormatBuffer(dictionary);
                if (wireFormat) {
                    size_t totalLength = parcBuffer_Remaining(wireFormat);
                    if (startPosition + length <= totalLength) {
                        hash = _ccnxWireFormatFacadeV1_ComputeBufferHash(wireFormat, hasher, startPosition, length);
                    }
                }
            }
        }
    }

    return hash;
}


static PARCCryptoHash  *
_ccnxWireFormatFacadeV1_ComputeContentObjectHash(CCNxTlvDictionary *dictionary)
{
    // This assumes the dictionary has been passed through something like the V1 packet decoder,
    // (e.g. ccnxCodecSchemaV1PacketDecoder_Decode) and has had the protected region extents set.
    // This will be the case for Athena. Metis has its own TLV parsing.

    assertTrue(ccnxTlvDictionary_IsContentObject(dictionary) || ccnxTlvDictionary_IsManifest(dictionary), "Message must be a ContentObject or Manifest");

    PARCCryptoHash *result = NULL;

    if (ccnxTlvDictionary_IsValueInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ContentObjectHashRegionStart)
        && ccnxTlvDictionary_IsValueInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ContentObjectHashRegionLength)) {
        PARCCryptoHasher *hasher = parcCryptoHasher_Create(PARCCryptoHashType_SHA256);
        size_t startPosition = ccnxTlvDictionary_GetInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ContentObjectHashRegionStart);
        size_t length = ccnxTlvDictionary_GetInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_ContentObjectHashRegionLength);

        CCNxCodecNetworkBufferIoVec *vec = _ccnxWireFormatFacadeV1_GetIoVec(dictionary);
        if (vec) {
            size_t totalLength = ccnxCodecNetworkBufferIoVec_Length(vec);
            if (startPosition + length <= totalLength) {
                result = _hashProtectedRegionIoVec(vec, hasher, startPosition, length);
            }
        } else {
            PARCBuffer *wireFormat = _ccnxWireFormatFacadeV1_GetWireFormatBuffer(dictionary);
            if (wireFormat) {
                size_t totalLength = parcBuffer_Remaining(wireFormat);
                if (startPosition + length <= totalLength) {
                    result = _ccnxWireFormatFacadeV1_ComputeBufferHash(wireFormat, hasher, startPosition, length);
                }
            }
        }
        parcCryptoHasher_Release(&hasher);
    }

    return result; // Could be NULL
}


/**
 * `CCNxWireFormatFacadeV1_Implementation` is the structure containing the pointers to the
 * V1 schema WireFormatMessage implementation.
 */
CCNxWireFormatMessageInterface CCNxWireFormatFacadeV1_Implementation = {
    .description                      = "CCNxWireFormatFacadeV1_Implementation",

    .create                           = &_ccnxWireFormatFacadeV1_CreateFromV1,

    .fromInterestPacketType           = &_ccnxWireFormatFacadeV1_FromInterestPacketType,

    .fromInterestPacketTypeIoVec      = &_ccnxWireFormatFacadeV1_FromInterestPacketTypeIoVec,

    .fromContentObjectPacketType      = &_ccnxWireFormatFacadeV1_FromContentObjectPacketType,

    .fromControlPacketType            = &_ccnxWireFormatFacadeV1_FromControlPacketType,

    .getWireFormatBuffer              = &_ccnxWireFormatFacadeV1_GetWireFormatBuffer,

    .getIoVec                         = &_ccnxWireFormatFacadeV1_GetIoVec,

    .putWireFormatBuffer              = &_ccnxWireFormatFacadeV1_PutWireFormatBuffer,

    .putIoVec                         = &_ccnxWireFormatFacadeV1_PutIoVec,

    .writeToFile                      = &_ccnxWireFormatFacadeV1_WriteToFile,

    .setProtectedRegionStart          = &_ccnxWireFormatFacadeV1_SetProtectedRegionStart,

    .setProtectedRegionLength         = &_ccnxWireFormatFacadeV1_SetProtectedRegionLength,

    .hashProtectedRegion              = &_ccnxWireFormatFacadeV1_HashProtectedRegion,

    .setContentObjectHashRegionStart  = &_ccnxWireFormatFacadeV1_SetContentObjectHashRegionStart,

    .setContentObjectHashRegionLength = &_ccnxWireFormatFacadeV1_SetContentObjectHashRegionLength,

    .computeContentObjectHash         = &_ccnxWireFormatFacadeV1_ComputeContentObjectHash,

    .setHopLimit                      = &_ccnxWireFormatFacadeV1_SetHopLimit,

    .convertInterestToInterestReturn  = &_ccnxWireFormatFacadeV1_ConvertInterestToInterestReturn,

};
