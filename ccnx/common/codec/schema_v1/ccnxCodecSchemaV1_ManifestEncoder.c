/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * Copyright 2014-2015 Palo Alto Research Center (PARC), a Xerox company.  All Rights Reserved.
 * The content of this file, whole or in part, is subject to licensing terms.
 * If distributing this software, include this License Header Notice in each
 * file and provide the accompanying LICENSE file.
 */
/**
 * @author Marc Mosko, Computing Science Laboratory, PARC
 * @copyright 2014-2015 Palo Alto Research Center (PARC), A Xerox Company. All Rights Reserved.
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
    PARCBuffer *digest = ccnxManifestHashGroupPointer_GetDigest(ptr);
    CCNxManifestHashGroupPointerType type = ccnxManifestHashGroupPointer_GetType(ptr);

    ssize_t length = -1;
    switch (type) {
        case CCNxManifestHashGroupPointerType_Data:
            length = ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_CCNxManifestHashGroup_DataPointer, digest);
            break;
        case CCNxManifestHashGroupPointerType_Manifest:
            length = ccnxCodecTlvEncoder_AppendBuffer(encoder, CCNxCodecSchemaV1Types_CCNxManifestHashGroup_ManifestPointer, digest);
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
