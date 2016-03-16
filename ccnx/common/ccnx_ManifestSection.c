/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#include <config.h>
#include <stdio.h>
#include <string.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_ArrayList.h>

#include <ccnx/common/ccnx_ManifestSection.h>

struct ccnx_manifest_section {
    CCNxLink *acsLink;         // optional

    PARCList *listOfNames;     // optional
    size_t numberOfNames;

    PARCList *listOfHashes;    // optional
    size_t numberOfHashes;
};

struct ccnx_manifest_section_name_entry {
    size_t chunk;
    CCNxName *mediaName;
};
typedef struct ccnx_manifest_section_name_entry _CCNxManifestSectionNameEntry;

struct ccnx_manifest_section_hash_entry {
    size_t nameIndex;
    PARCBuffer *hash;
};
typedef struct ccnx_manifest_section_hash_entry _CCNxManifestSectionHashEntry;

static void
_ccnxManifestSectionNameEntry_FinalRelease(_CCNxManifestSectionNameEntry **nameP)
{
    if ((*nameP)->mediaName != NULL) {
        ccnxName_Release(&(*nameP)->mediaName);
    }
}

static bool
_ccnxManifestSectionNameEntry_Equals(const _CCNxManifestSectionNameEntry *objectA, const _CCNxManifestSectionNameEntry *objectB)
{
    if (objectA == objectB) {
        return true;
    }
    if (objectA == NULL || objectB == NULL) {
        return false;
    }
    if (objectA->chunk == objectB->chunk) {
        if (ccnxName_Equals(objectA->mediaName, objectB->mediaName)) {
            return true;
        }
    }
    return false;
}

parcObject_ExtendPARCObject(_CCNxManifestSectionNameEntry, _ccnxManifestSectionNameEntry_FinalRelease,
                            NULL, NULL, _ccnxManifestSectionNameEntry_Equals, NULL, NULL, NULL);

static void
_ccnxManifestSectionHashEntry_FinalRelease(_CCNxManifestSectionHashEntry **hashP)
{
    if ((*hashP)->hash != NULL) {
        parcBuffer_Release(&(*hashP)->hash);
    }
}


static bool
_ccnxManifestSectionHashEntry_Equals(const _CCNxManifestSectionHashEntry *objectA, const _CCNxManifestSectionHashEntry *objectB)
{
    if (objectA == objectB) {
        return true;
    }
    if (objectA == NULL || objectB == NULL) {
        return false;
    }
    if (objectA->nameIndex == objectB->nameIndex) {
        if (parcBuffer_Equals(objectA->hash, objectB->hash)) {
            return true;
        }
    }
    return false;
}


parcObject_ExtendPARCObject(_CCNxManifestSectionHashEntry, _ccnxManifestSectionHashEntry_FinalRelease,
                            NULL, NULL, _ccnxManifestSectionHashEntry_Equals, NULL, NULL, NULL);

// Private functions

static _CCNxManifestSectionNameEntry *
_ccnxManifestSection_CreateNameEntry(size_t chunk, const CCNxName *name)
{
    _CCNxManifestSectionNameEntry *entry = parcObject_CreateAndClearInstance(_CCNxManifestSectionNameEntry);
    assertNotNull(entry, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(_CCNxManifestSectionNameEntry));

    if (entry != NULL) {
        entry->chunk = chunk;
        entry->mediaName = ccnxName_Acquire(name);
    }

    return entry;
}

static _CCNxManifestSectionHashEntry *
_ccnxManifestSection_CreateHashEntry(size_t nameIndex, const PARCBuffer *hash)
{
    _CCNxManifestSectionHashEntry *entry = parcObject_CreateAndClearInstance(_CCNxManifestSectionHashEntry);
    assertNotNull(entry, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(_CCNxManifestSectionHashEntry));

    if (entry != NULL) {
        entry->nameIndex = nameIndex;
        entry->hash = parcBuffer_Acquire(hash);
    }

    return entry;
}

static void
_ccnxManifestSectionNameEntry_Release(_CCNxManifestSectionNameEntry **entryP)
{
    parcObject_Release((void **) entryP);
}

static void
_ccnxManifestSectionHashEntry_Release(_CCNxManifestSectionHashEntry **entryP)
{
    parcObject_Release((void **) entryP);
}

static _CCNxManifestSectionNameEntry *
_getNameEntryFromHashIndex(const CCNxManifestSection *section, size_t index)
{
    _CCNxManifestSectionHashEntry *hashEntry = (_CCNxManifestSectionHashEntry *) parcList_GetAtIndex(section->listOfHashes, index);
    size_t nameIndex = hashEntry->nameIndex;
    _CCNxManifestSectionNameEntry *nameEntry = (_CCNxManifestSectionNameEntry *) parcList_GetAtIndex(section->listOfNames, nameIndex);
    return nameEntry;
}

static PARCBuffer *
_ccnxManifestSection_GetHashEntryDigest(const _CCNxManifestSectionHashEntry *entry)
{
    return entry->hash;
}

static void
_ccnxManifestSection_FinalRelease(CCNxManifestSection **sectionP)
{
    if ((*sectionP)->acsLink != NULL) {
        ccnxLink_Release(&(*sectionP)->acsLink);
    }
    if ((*sectionP)->listOfHashes != NULL) {
        parcList_Release(&(*sectionP)->listOfHashes);
    }
    if ((*sectionP)->listOfNames != NULL) {
        parcList_Release(&(*sectionP)->listOfNames);
    }
}

parcObject_ExtendPARCObject(CCNxManifestSection, _ccnxManifestSection_FinalRelease, NULL, ccnxManifestSection_ToString, ccnxManifestSection_Equals, NULL, NULL, NULL);

parcObject_ImplementAcquire(ccnxManifestSection, CCNxManifestSection);

parcObject_ImplementRelease(ccnxManifestSection, CCNxManifestSection);

// Public functions

CCNxManifestSection *
ccnxManifestSection_Create(CCNxLink *acsLink)
{
    CCNxManifestSection *section = parcObject_CreateAndClearInstance(CCNxManifestSection);

    if (section != NULL) {
        section->acsLink = acsLink == NULL ? NULL : ccnxLink_Acquire(acsLink);

        PARCArrayList *hashList = parcArrayList_Create((void (*)(void **))_ccnxManifestSectionHashEntry_Release);
        section->listOfHashes = parcList((PARCObject *) hashList, PARCArrayListAsPARCList);
        section->numberOfHashes = 0;

        PARCArrayList *nameList = parcArrayList_Create((void (*)(void **))_ccnxManifestSectionNameEntry_Release);
        section->listOfNames = parcList((PARCObject *) nameList, PARCArrayListAsPARCList);
        section->numberOfNames = 0;
    }

    return section;
}

CCNxName *
ccnxManifestSection_GetNameFromHashIndex(const CCNxManifestSection *section, size_t index)
{
    _CCNxManifestSectionNameEntry *nameEntry = _getNameEntryFromHashIndex(section, index);
    return nameEntry->mediaName;
}

size_t
ccnxManifestSection_GetNameChunkFromHashIndex(const CCNxManifestSection *section, size_t index)
{
    _CCNxManifestSectionNameEntry *nameEntry = _getNameEntryFromHashIndex(section, index);
    return nameEntry->chunk;
}

CCNxLink *
ccnxManifestSection_GetACSLink(const CCNxManifestSection *section)
{
    return section->acsLink;
}

size_t
ccnxManifestSection_GetNameCount(const CCNxManifestSection *section)
{
    return section->numberOfNames;
}

size_t
ccnxManifestSection_GetHashCount(const CCNxManifestSection *section)
{
    return section->numberOfHashes;
}

PARCBuffer *
ccnxManifestSection_GetHashAtIndex(const CCNxManifestSection *section, size_t index)
{
    _CCNxManifestSectionHashEntry *hashEntry =
        (_CCNxManifestSectionHashEntry *) parcList_GetAtIndex(section->listOfHashes, index);
    PARCBuffer *retBuffer = _ccnxManifestSection_GetHashEntryDigest(hashEntry);
    return retBuffer;
}

bool
ccnxManifestSection_AddNameEntry(CCNxManifestSection *section, const CCNxName *name, size_t chunk, const PARCBuffer *digest)
{
    _CCNxManifestSectionNameEntry *nameEntry = _ccnxManifestSection_CreateNameEntry(chunk, name);
    _CCNxManifestSectionHashEntry *hashEntry = _ccnxManifestSection_CreateHashEntry(section->numberOfNames, digest);

    // Add elements to both lists
    bool successful = parcList_Add(section->listOfNames, (void *) nameEntry);
    if (successful) {
        section->numberOfNames++;
    }
    successful = parcList_Add(section->listOfHashes, (void *) hashEntry);
    if (successful) {
        section->numberOfHashes++;
    }

    return successful;
}

bool
ccnxManifestSection_Equals(const CCNxManifestSection *objectA, const CCNxManifestSection *objectB)
{
    if (objectA == objectB) {
        return true;
    }
    if (objectA == NULL || objectB == NULL) {
        return false;
    }
    if (ccnxLink_Equals(objectA->acsLink, objectB->acsLink)) {
        if (objectA->numberOfHashes == objectB->numberOfHashes) {
            if (objectA->numberOfNames == objectB->numberOfNames) {
                for (size_t i = 0; i < parcList_Size(objectA->listOfHashes); i++) {
                    _CCNxManifestSectionHashEntry *entryA =
                        (_CCNxManifestSectionHashEntry *) parcList_GetAtIndex(objectA->listOfHashes, i);
                    _CCNxManifestSectionHashEntry *entryB =
                        (_CCNxManifestSectionHashEntry *) parcList_GetAtIndex(objectB->listOfHashes, i);
                    if (!_ccnxManifestSectionHashEntry_Equals(entryA, entryB)) {
                        return false;
                    }
                }
                for (size_t i = 0; i < parcList_Size(objectA->listOfNames); i++) {
                    _CCNxManifestSectionNameEntry *entryA =
                        (_CCNxManifestSectionNameEntry *) parcList_GetAtIndex(objectA->listOfNames, i);
                    _CCNxManifestSectionNameEntry *entryB =
                        (_CCNxManifestSectionNameEntry *) parcList_GetAtIndex(objectB->listOfNames, i);
                    if (!_ccnxManifestSectionNameEntry_Equals(entryA, entryB)) {
                        return false;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

char *
ccnxManifestSection_ToString(const CCNxManifestSection *section)
{
    char *acsString = ccnxLink_ToString(section->acsLink);

    char *string;
    int failure = asprintf(&string, "CCNxManifestSection { .acsLink=\"%s\" }", acsString);
    assertTrue(failure > -1, "Error asprintf");

    parcMemory_Deallocate((void **) &acsString);

    char *result = parcMemory_StringDuplicate(string, strlen(string));
    free(string);

    return result;
}

void
ccnxManifestSection_AssertValid(const CCNxManifestSection *section)
{
    assertNotNull(section, "Parameter must be a non-null CCNxManifestSection pointer.");
}
