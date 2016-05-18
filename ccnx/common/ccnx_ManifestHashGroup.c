/*
 * Copyright (c) 2015-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2015-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>
#include <stdio.h>
#include <string.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_LinkedList.h>

#include <ccnx/common/ccnx_Interest.h>
#include <ccnx/common/ccnx_ManifestHashGroup.h>
#include <ccnx/common/internal/ccnx_WireFormatMessageInterface.h>

#define MAX_NUMBER_OF_POINTERS 1500 // loose upper bound imposed by packet format

struct ccnx_manifest_hash_group {
    PARCLinkedList *pointers;

    // Metadata
    const PARCBuffer *overallDataDigest; // overall *application data* digest
    size_t dataSize;   // size for all pointers (last pointer might not be entrySize unless perfectly balanced)
    size_t entrySize;  // size per pointer
    size_t blockSize;  // size of nodes used in the tree (e.g., 4K for each Manifest or Data node)
    size_t treeHeight; // height of sub-tree referred to by each pointer
    const CCNxName *locator; // locator for the hash group
};

struct ccnx_manifest_hash_group_pointer {
    CCNxManifestHashGroupPointerType pointerType;
    PARCBuffer *digest;
};

static bool
CCNxManifestHashGroupPointer_FinalRelease(CCNxManifestHashGroupPointer **ptrP)
{
    if ((*ptrP)->digest != NULL) {
        parcBuffer_Release(&(*ptrP)->digest);
    }
    return true;
}

static bool
_CCNxManifestHashGroupPointer_Equals(const CCNxManifestHashGroupPointer *objectA, const CCNxManifestHashGroupPointer *objectB)
{
    if (objectA == objectB) {
        return true;
    }
    if (objectA == NULL || objectB == NULL) {
        return false;
    }
    if (objectA->pointerType == objectB->pointerType) {
        if (parcBuffer_Equals(objectA->digest, objectB->digest)) {
            return true;
        }
    }
    return false;
}

parcObject_Override(CCNxManifestHashGroupPointer, PARCObject,
                    .destructor = (PARCObjectDestructor *) CCNxManifestHashGroupPointer_FinalRelease,
                    .equals = (PARCObjectEquals *) _CCNxManifestHashGroupPointer_Equals);

parcObject_ImplementAcquire(ccnxManifestHashGroupPointer, CCNxManifestHashGroupPointer);
parcObject_ImplementRelease(ccnxManifestHashGroupPointer, CCNxManifestHashGroupPointer);

CCNxManifestHashGroupPointer *
ccnxManifestHashGroupPointer_Create(CCNxManifestHashGroupPointerType type, const PARCBuffer *digest)
{
    CCNxManifestHashGroupPointer *ptr = parcObject_CreateAndClearInstance(CCNxManifestHashGroupPointer);
    if (ptr != NULL) {
        ptr->pointerType = type;
        ptr->digest = parcBuffer_Acquire(digest);
    }
    return ptr;
}

CCNxManifestHashGroupPointerType
ccnxManifestHashGroupPointer_GetType(const CCNxManifestHashGroupPointer *ptr)
{
    return ptr->pointerType;
}

const PARCBuffer *
ccnxManifestHashGroupPointer_GetDigest(const CCNxManifestHashGroupPointer *ptr)
{
    return ptr->digest;
}

static bool
_ccnxManifestHashGroup_Destructor(CCNxManifestHashGroup **groupP)
{
    if ((*groupP)->pointers != NULL) {
        parcLinkedList_Release(&(*groupP)->pointers);
    }
    if ((*groupP)->overallDataDigest != NULL) {
        parcBuffer_Release((PARCBuffer **) &(*groupP)->overallDataDigest);
    }
    if ((*groupP)->locator != NULL) {
        ccnxName_Release((CCNxName **) &(*groupP)->locator);
    }
    return true;
}

parcObject_Override(CCNxManifestHashGroup, PARCObject,
                    .destructor = (PARCObjectDestructor *) _ccnxManifestHashGroup_Destructor,
                    .toString = (PARCObjectToString *) ccnxManifestHashGroup_ToString,
                    .equals = (PARCObjectEquals *) ccnxManifestHashGroup_Equals);

parcObject_ImplementAcquire(ccnxManifestHashGroup, CCNxManifestHashGroup);
parcObject_ImplementRelease(ccnxManifestHashGroup, CCNxManifestHashGroup);

CCNxManifestHashGroup *
ccnxManifestHashGroup_Create(void)
{
    CCNxManifestHashGroup *section = parcObject_CreateAndClearInstance(CCNxManifestHashGroup);

    if (section != NULL) {
        section->pointers = parcLinkedList_Create();

        section->overallDataDigest = NULL;
        section->dataSize = 0;
        section->entrySize = 0;
        section->blockSize = 0;
        section->treeHeight = 0;
        section->locator = NULL;
    }

    return section;
}

bool
ccnxManifestHashGroup_AppendPointer(CCNxManifestHashGroup *group, CCNxManifestHashGroupPointerType type, const PARCBuffer *buffer)
{
    if (!ccnxManifestHashGroup_IsFull(group)) {
        CCNxManifestHashGroupPointer *ptr = ccnxManifestHashGroupPointer_Create(type, buffer);
        parcLinkedList_Append(group->pointers, ptr);
        ccnxManifestHashGroupPointer_Release(&ptr);
        return true;
    }
    return false;
}

bool
ccnxManifestHashGroup_PrependPointer(CCNxManifestHashGroup *group, CCNxManifestHashGroupPointerType type, const PARCBuffer *buffer)
{
    if (!ccnxManifestHashGroup_IsFull(group)) {
        CCNxManifestHashGroupPointer *ptr = ccnxManifestHashGroupPointer_Create(type, buffer);
        parcLinkedList_Prepend(group->pointers, ptr);
        ccnxManifestHashGroupPointer_Release(&ptr);
        return true;
    }
    return false;
}

void
ccnxManifestHashGroup_SetOverallDataDigest(CCNxManifestHashGroup *group, const PARCBuffer *digest)
{
    group->overallDataDigest = parcBuffer_Acquire(digest);
}

const PARCBuffer *
ccnxManifestHashGroup_GetOverallDataDigest(const CCNxManifestHashGroup *group)
{
    return group->overallDataDigest;
}

size_t
ccnxManifestHashGroup_GetTotalSize(const CCNxManifestHashGroup *group)
{
    return group->dataSize;
}

size_t
ccnxManifestHashGroup_GetChildBlockSize(const CCNxManifestHashGroup *group)
{
    return group->entrySize;
}

void
ccnxManifestHashGroup_SetLocator(CCNxManifestHashGroup *group, const CCNxName *locator)
{
    group->locator = ccnxName_Acquire(locator);
}

const CCNxName *
ccnxManifestHashGroup_GetLocator(const CCNxManifestHashGroup *group)
{
    return group->locator;
}

size_t
ccnxManifestHashGroup_GetNumberOfPointers(const CCNxManifestHashGroup *group)
{
    return parcLinkedList_Size(group->pointers);
}

CCNxManifestHashGroupPointer *
ccnxManifestHashGroup_GetPointerAtIndex(const CCNxManifestHashGroup *group, size_t index)
{
    CCNxManifestHashGroupPointer *entry = parcLinkedList_GetAtIndex(group->pointers, index);
    return entry;
}

CCNxManifestHashGroupPointerType
ccnxManifestHashGroup_GetPointerTypeAtIndex(const CCNxManifestHashGroup *group, size_t index)
{
    CCNxManifestHashGroupPointer *entry = parcLinkedList_GetAtIndex(group->pointers, index);
    return entry->pointerType;
}

PARCBuffer *
ccnxManifestHashGroup_GetPointerDigestAtIndex(const CCNxManifestHashGroup *group, size_t index)
{
    CCNxManifestHashGroupPointer *entry = parcLinkedList_GetAtIndex(group->pointers, index);
    return entry->digest;
}

bool
ccnxManifestHashGroup_IsFull(const CCNxManifestHashGroup *group)
{
    size_t size = parcLinkedList_Size(group->pointers);
    return size >= MAX_NUMBER_OF_POINTERS;
}

bool
ccnxManifestHashGroup_Equals(const CCNxManifestHashGroup *objectA, const CCNxManifestHashGroup *objectB)
{
    if (objectA == objectB) {
        return true;
    }
    if (objectA == NULL || objectB == NULL) {
        return false;
    }

    if (objectA->dataSize == objectB->dataSize) {
        if (objectA->entrySize == objectB->entrySize) {
            if (objectA->blockSize == objectB->blockSize) {
                if (objectA->treeHeight == objectB->treeHeight) {
                    if (ccnxName_Equals(objectA->locator, objectB->locator)) {
                        if (parcBuffer_Equals(objectA->overallDataDigest, objectB->overallDataDigest)) {
                            if (parcLinkedList_Size(objectA->pointers) == parcLinkedList_Size(objectB->pointers)) {
                                for (size_t i = 0; i < parcLinkedList_Size(objectA->pointers); i++) {
                                    CCNxManifestHashGroupPointer *ptrA = parcLinkedList_GetAtIndex(objectA->pointers, i);
                                    CCNxManifestHashGroupPointer *ptrB = parcLinkedList_GetAtIndex(objectB->pointers, i);
                                    if (!_CCNxManifestHashGroupPointer_Equals(ptrA, ptrB)) {
                                        return false;
                                    }
                                }
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

char *
ccnxManifestHashGroup_ToString(const CCNxManifestHashGroup *section)
{
    PARCJSON *json = ccnxManifestHashGroup_ToJson(section);
    char *stringRep = parcJSON_ToString(json);
    parcJSON_Release(&json);
    return stringRep;
}

PARCJSON *
ccnxManifestHashGroup_ToJson(const CCNxManifestHashGroup *group)
{
    PARCJSON *root = parcJSON_Create();

    PARCJSONArray *ptrList = parcJSONArray_Create();
    for (size_t i = 0; i < parcLinkedList_Size(group->pointers); i++) {
        CCNxManifestHashGroupPointer *ptr = (CCNxManifestHashGroupPointer *) parcLinkedList_GetAtIndex(group->pointers, i);
        PARCJSON *ptrJson = parcJSON_Create();

        // Type.
        parcJSON_AddInteger(ptrJson, "type", ccnxManifestHashGroupPointer_GetType(ptr));

        // Digest.
        char *digestString = parcBuffer_ToHexString(ptr->digest);
        parcJSON_AddString(ptrJson, "digest", digestString);
        parcMemory_Deallocate(&digestString);

        // Add the tuple to the list.
        PARCJSONValue *val = parcJSONValue_CreateFromJSON(ptrJson);
        parcJSONArray_AddValue(ptrList, val);

        // Cleanup
        parcJSONValue_Release(&val);
        parcJSON_Release(&ptrJson);
    }
    root = parcJSON_AddArray(root, "HashGroup", ptrList);
    parcJSONArray_Release(&ptrList);

    if (group->overallDataDigest != NULL) {
        char *digestString = parcBuffer_ToHexString(group->overallDataDigest);
        root = parcJSON_AddString(root, "overallDataDigest", digestString);
        parcMemory_Deallocate((void **) &digestString);
    }

    if (group->locator != NULL) {
        char *locatorString = ccnxName_ToString(group->locator);
        root = parcJSON_AddString(root, "locator", locatorString);
        parcMemory_Deallocate((void **) &locatorString);
    }

    if (group->entrySize > 0) {
        root = parcJSON_AddInteger(root, "entrySize", group->entrySize);
    }

    if (group->dataSize > 0) {
        root = parcJSON_AddInteger(root, "dataSize", group->dataSize);
    }

    if (group->blockSize > 0) {
        root = parcJSON_AddInteger(root, "blockSize", group->blockSize);
    }

    if (group->treeHeight > 0) {
        root = parcJSON_AddInteger(root, "treeHeight", group->treeHeight);
    }

    return root;
}

CCNxManifestHashGroup *
ccnxManifestHashGroup_CreateFromJson(const PARCJSON *json)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();

    PARCJSONValue *ptrListValue = parcJSON_GetValueByName(json, "HashGroup");
    PARCJSONArray *ptrList = parcJSONValue_GetArray(ptrListValue);
    size_t numberOfPointers = parcJSONArray_GetLength(ptrList);
    for (size_t i = 0; i < numberOfPointers; i++) {
        PARCJSONValue *pointerValue = parcJSONArray_GetValue(ptrList, i);

        PARCJSON *typeJson = parcJSONValue_GetJSON(pointerValue);
        PARCJSONValue *typeValue = parcJSON_GetValueByName(typeJson, "type");
        CCNxManifestHashGroupPointerType type;

        if (parcJSONValue_GetInteger(typeValue) == 0) {
            type = CCNxManifestHashGroupPointerType_Data;
        } else {
            type = CCNxManifestHashGroupPointerType_Manifest;
        }

        PARCJSON *digestJson = parcJSONValue_GetJSON(pointerValue);
        PARCJSONValue *digestValue = parcJSON_GetValueByName(digestJson, "digest");
        PARCBuffer *digestHex = parcJSONValue_GetString(digestValue);

        char *hexString = parcBuffer_ToString(digestHex);
        PARCBuffer *digest = parcBuffer_Flip(parcBuffer_ParseHexString(hexString));
        parcMemory_Deallocate(&hexString);

        ccnxManifestHashGroup_AppendPointer(group, type, digest);
        parcBuffer_Release(&digest);
    }

    if (parcJSON_GetPairByName(json, "overallDataDigest") != NULL) {
        PARCJSONValue *overallDataDigestValue = parcJSON_GetValueByName(json, "overallDataDigest");
        PARCBuffer *digestHex = parcJSONValue_GetString(overallDataDigestValue);

        char *hexString = parcBuffer_ToString(digestHex);
        group->overallDataDigest = parcBuffer_Flip(parcBuffer_ParseHexString(hexString));
        parcMemory_Deallocate(&hexString);
    }

    if (parcJSON_GetPairByName(json, "locator") != NULL) {
        PARCJSONValue *locatorValue = parcJSON_GetValueByName(json, "locator");
        PARCBuffer *buffer = parcJSONValue_GetString(locatorValue);
        char *locator = parcBuffer_ToString(buffer);
        group->locator = ccnxName_CreateFromCString(locator);
        parcMemory_Deallocate(&locator);
    }

    if (parcJSON_GetPairByName(json, "entrySize") != NULL) {
        PARCJSONValue *childBlockNodeSizeValue = parcJSON_GetValueByName(json, "entrySize");
        group->entrySize = parcJSONValue_GetInteger(childBlockNodeSizeValue);
    }

    if (parcJSON_GetPairByName(json, "dataSize") != NULL) {
        PARCJSONValue *totalSizeValue = parcJSON_GetValueByName(json, "dataSize");
        group->dataSize = parcJSONValue_GetInteger(totalSizeValue);
    }

    if (parcJSON_GetPairByName(json, "blockSize") != NULL) {
        PARCJSONValue *blockSizeValue = parcJSON_GetValueByName(json, "blockSize");
        group->blockSize = parcJSONValue_GetInteger(blockSizeValue);
    }

    if (parcJSON_GetPairByName(json, "treeHeight") != NULL) {
        PARCJSONValue *treeHeightValue = parcJSON_GetValueByName(json, "treeHeight");
        group->treeHeight = parcJSONValue_GetInteger(treeHeightValue);
    }

    return group;
}

struct _hashgroup_ccnxManifestHashGroupIterator_state {
    size_t pointerNumber;
    bool atEnd;
};
typedef struct _hashgroup_ccnxManifestHashGroupIterator_state _HashgroupIteratorState;

static void *
_ccnxManifestHashGroupIterator_Init(CCNxManifestHashGroup *group)
{
    _HashgroupIteratorState *state = parcMemory_Allocate(sizeof(_HashgroupIteratorState));
    state->pointerNumber = 0;
    state->atEnd = false;
    return state;
}

static bool
_ccnxManifestHashGroupIterator_HasNext(CCNxManifestHashGroup *group, void *voidstate)
{
    _HashgroupIteratorState *state = (_HashgroupIteratorState *) voidstate;
    return !state->atEnd;
}

static void *
_ccnxManifestHashGroupIterator_Next(CCNxManifestHashGroup *group, void *state)
{
    _HashgroupIteratorState *thestate = (_HashgroupIteratorState *) state;
    thestate->pointerNumber++;

    if (thestate->pointerNumber == parcLinkedList_Size(group->pointers)) {
        thestate->atEnd = true;
    }

    return thestate;
}

static void
_ccnxManifestHashGroupIterator_RemoveAt(CCNxManifestHashGroup *group, void **state)
{
    // pass
}

static void *
_ccnxManifestHashGroupIterator_GetElement(CCNxManifestHashGroup *group, void *state)
{
    _HashgroupIteratorState *thestate = (_HashgroupIteratorState *) state;
    CCNxManifestHashGroupPointer *ptr =
        (CCNxManifestHashGroupPointer *) parcLinkedList_GetAtIndex(group->pointers, thestate->pointerNumber - 1);
    return ptr;
}

static void
_ccnxManifestHashGroupIterator_Finish(CCNxManifestHashGroup *group, void *state)
{
    _HashgroupIteratorState *thestate = (_HashgroupIteratorState *) state;
    parcMemory_Deallocate(&thestate);
}

static void
_ccnxManifestHashGroupIterator_AssertValid(const void *state)
{
    // pass
}

PARCIterator *
ccnxManifestHashGroup_Iterator(const CCNxManifestHashGroup *group)
{
    PARCIterator *iterator = parcIterator_Create((void *) group,
                                                 (void *(*)(PARCObject *)) _ccnxManifestHashGroupIterator_Init,
                                                 (bool  (*)(PARCObject *, void *)) _ccnxManifestHashGroupIterator_HasNext,
                                                 (void *(*)(PARCObject *, void *)) _ccnxManifestHashGroupIterator_Next,
                                                 (void  (*)(PARCObject *, void **)) _ccnxManifestHashGroupIterator_RemoveAt,
                                                 (void *(*)(PARCObject *, void *)) _ccnxManifestHashGroupIterator_GetElement,
                                                 (void  (*)(PARCObject *, void *)) _ccnxManifestHashGroupIterator_Finish,
                                                 (void  (*)(const void *)) _ccnxManifestHashGroupIterator_AssertValid);

    return iterator;
}

size_t
ccnxManifestHashGroup_GetBlockSize(const CCNxManifestHashGroup *group)
{
    return group->blockSize;
}

void
ccnxManifestHashGroup_SetBlockSize(CCNxManifestHashGroup *group, size_t blockSize)
{
    group->blockSize = blockSize;
}

size_t
ccnxManifestHashGroup_GetDataSize(const CCNxManifestHashGroup *group)
{
    return group->dataSize;
}

void
ccnxManifestHashGroup_SetDataSize(CCNxManifestHashGroup *group, size_t dataSize)
{
    group->dataSize = dataSize;
}

size_t
ccnxManifestHashGroup_GetEntrySize(const CCNxManifestHashGroup *group)
{
    return group->entrySize;
}

void
ccnxManifestHashGroup_SetEntrySize(CCNxManifestHashGroup *group, size_t entrySize)
{
    group->entrySize = entrySize;
}

size_t
ccnxManifestHashGroup_GetTreeHeight(const CCNxManifestHashGroup *group)
{
    return group->treeHeight;
}

void
ccnxManifestHashGroup_SetTreeHeight(CCNxManifestHashGroup *group, size_t treeHeight)
{
    group->treeHeight = treeHeight;
}

bool
ccnxManifestHashGroup_HasMetadata(const CCNxManifestHashGroup *group)
{
    // Check the existence of each metadata value.
    if (group->blockSize > 0) {
        return true;
    }
    if (group->dataSize > 0) {
        return true;
    }
    if (group->entrySize > 0) {
        return true;
    }
    if (group->locator != NULL) {
        return true;
    }
    if (group->overallDataDigest != NULL) {
        return true;
    }

    return false;
}

PARCLinkedList *
ccnxManifestHashGroup_CreateInterestList(const CCNxManifestHashGroup *group, const CCNxName *locator)
{
    PARCLinkedList *interestList = parcLinkedList_Create();

    PARCIterator *itr = ccnxManifestHashGroup_Iterator(group);
    while (parcIterator_HasNext(itr)) {
        // Extract the name and digest
        CCNxManifestHashGroupPointer *ptr = parcIterator_Next(itr);
        const PARCBuffer *digest = ccnxManifestHashGroupPointer_GetDigest(ptr);
        const CCNxName *name = group->locator == NULL ? locator : group->locator;

        // Build the interest and append it to the running list
        if (name != NULL) {
            CCNxInterest *interest = ccnxInterest_CreateSimple(name);
            ccnxInterest_SetContentObjectHashRestriction(interest, digest);
            parcLinkedList_Append(interestList, interest);
            ccnxInterest_Release(&interest);
        }
    }
    parcIterator_Release(&itr);

    return interestList;
}
