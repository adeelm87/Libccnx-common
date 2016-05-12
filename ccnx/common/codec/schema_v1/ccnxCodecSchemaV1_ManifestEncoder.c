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
 * @author Marc Mosko, Computing Science Laboratory, PARC
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_Buffer.h>
#include <parc/algol/parc_BufferComposer.h>

#include <ccnx/common/ccnx_Manifest.h>
#include <ccnx/common/ccnx_ManifestHashGroup.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_Types.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_ManifestEncoder.h>

#include <ccnx/common/codec/ccnxCodec_TlvUtilities.h>
#include <ccnx/common/ccnx_PayloadType.h>
#include <ccnx/common/ccnx_InterestReturn.h>

static size_t
_appendPointer(CCNxCodecTlvEncoder *encoder, CCNxManifestHashGroupPointer *ptr)
{
    const PARCBuffer *digest = ccnxManifestHashGroupPointer_GetDigest(ptr);
    CCNxManifestHashGroupPointerType type = ccnxManifestHashGroupPointer_GetType(ptr);

    ssize_t length = -1;
    switch (type) {
        case CCNxManifestHashGroupPointerType_Data:
            length = ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_CCNxManifestHashGroup_DataPointer, (PARCBuffer *) digest);
            break;
        case CCNxManifestHashGroupPointerType_Manifest:
            length = ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_CCNxManifestHashGroup_ManifestPointer, (PARCBuffer *) digest);
            break;
        default:
            assertTrue(false, "Invalid pointer type %d", type);
    }

    if (length < 0) {
        CCNxCodecError *error = ccnxCodecError_Create(TLV_MISSING_MANDATORY, __func__, __LINE__, ccnxCodecTlvEncoder_Position(encoder));
        ccnxCodecTlvEncoder_SetError(encoder, error);
        ccnxCodecError_Release(&error);
    }

    return length;
}

ssize_t
_appendMetadata(CCNxCodecTlvEncoder *encoder, CCNxManifestHashGroup *group)
{
    ssize_t length = 0;

    // Pre-populate this field -- we'll come back and fill in the length after we're done
    size_t startPosition = ccnxCodecTlvEncoder_Position(encoder);
    ccnxCodecTlvEncoder_AppendContainer(encoder, CCNxCodecSchemaV1Types_CCNxManifestHashGroup_Metadata, length);

    // Now append all metadata that exists in the hash group.
    const CCNxName *locator = ccnxManifestHashGroup_GetLocator(group);
    if (locator != NULL) {
        char *nameString = ccnxName_ToString(locator);
        PARCBuffer *nameBuffer = parcBuffer_AllocateCString(nameString);
        length += ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_Locator, nameBuffer);
        parcBuffer_Release(&nameBuffer);
        parcMemory_Deallocate(&nameString);
    }

    size_t dataSize = ccnxManifestHashGroup_GetDataSize(group);
    if (dataSize > 0) {
        length += ccnxCodecTlvEncoder_AppendUint64(encoder, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_DataSize, dataSize);
    }

    size_t blockSize = ccnxManifestHashGroup_GetBlockSize(group);
    if (blockSize > 0) {
        length += ccnxCodecTlvEncoder_AppendUint64(encoder, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_BlockSize, blockSize);
    }

    size_t entrySize = ccnxManifestHashGroup_GetEntrySize(group);
    if (entrySize > 0) {
        length += ccnxCodecTlvEncoder_AppendUint64(encoder, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_EntrySize, entrySize);
    }

    size_t treeSize = ccnxManifestHashGroup_GetTreeHeight(group);
    if (treeSize > 0) {
        length += ccnxCodecTlvEncoder_AppendUint64(encoder, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_TreeHeight, treeSize);
    }

    const PARCBuffer *dataDigest = ccnxManifestHashGroup_GetOverallDataDigest(group);
    if (dataDigest != NULL) {
        length += ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_OverallDataSha256, (PARCBuffer *) dataDigest);
    }

    // Rewind back to the container opening and fill in the length
    size_t endPosition = ccnxCodecTlvEncoder_Position(encoder);
    ccnxCodecTlvEncoder_PutUint16(encoder, startPosition, CCNxCodecSchemaV1Types_CCNxManifestHashGroup_Metadata);
    ccnxCodecTlvEncoder_PutUint16(encoder, startPosition + 2, length);
    ccnxCodecTlvEncoder_SetPosition(encoder, endPosition);

    return endPosition - startPosition;
}

ssize_t
ccnxCodecSchemaV1ManifestEncoder_Encode(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary)
{
    ssize_t length = 0;

    ssize_t numHashGroups = ccnxTlvDictionary_ListSize(packetDictionary, CCNxCodecSchemaV1TlvDictionary_Lists_HASH_GROUP_LIST);
    for (size_t i = 0; i < numHashGroups; i++) {

        // Skip past the TL of the hash group to append the pointers inside
        ssize_t groupLength = 0;
        ccnxCodecTlvEncoder_AppendContainer(encoder, CCNxCodecSchemaV1Types_CCNxMessage_HashGroup, groupLength);

        CCNxManifestInterface *interface = ccnxManifestInterface_GetInterface(packetDictionary);
        CCNxManifestHashGroup *group = interface->getHashGroup(packetDictionary, i);

        // Encode any metadata, if present.
        if (ccnxManifestHashGroup_HasMetadata(group)) {
            groupLength += _appendMetadata(encoder, group);
        }

        // Append the HashGroup pointers
        size_t numPointers = ccnxManifestHashGroup_GetNumberOfPointers(group);
        for (size_t p = 0; p < numPointers; p++) {
            CCNxManifestHashGroupPointer *ptr = ccnxManifestHashGroup_GetPointerAtIndex(group, p);
            ssize_t ptrLength = _appendPointer(encoder, ptr);
            if (ptrLength < 0) {
                return ptrLength;
            }
            groupLength += ptrLength;
        }

        // Now that we know the overall length, rewind back to the start and append the TL
        // part of the container.
        size_t endPosition = ccnxCodecTlvEncoder_Position(encoder);
        ssize_t offset = endPosition - groupLength - 4;
        ccnxCodecTlvEncoder_PutUint16(encoder, offset, CCNxCodecSchemaV1Types_CCNxMessage_HashGroup);
        ccnxCodecTlvEncoder_PutUint16(encoder, offset + 2, groupLength);
        ccnxCodecTlvEncoder_SetPosition(encoder, endPosition);

        length += groupLength + 4;

        ccnxManifestHashGroup_Release(&group);
    }

    return length;
}
