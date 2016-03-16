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

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../ccnx_ManifestSection.c"

#include <inttypes.h>
#include <ccnx/common/ccnx_Manifest.h>

#include <ccnx/common/ccnx_Link.h>

#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>
#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_ArrayList.h>

LONGBOW_TEST_RUNNER(ccnx_Manifest)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_Manifest)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_Manifest)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestSection_Create_Empty);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestSection_Acquire_Release);

    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestSection_AddNameEntry);

    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestSection_GetACSLink);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestSection_GetNameCount);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestSection_GetHashCount);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestSection_GetNameFromHashIndex);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestSection_GetNameChunkFromHashIndex);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestSection_GetHash);

    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestSection_Create_ToString);

    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestSection_Equals);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxManifestSection_Create_Empty)
{
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);

    CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);

    assertNotNull(section, "Expected non-null section");

    ccnxManifestSection_Release(&section);
    ccnxLink_Release(&acsLink);
    ccnxName_Release(&acsName);
}

LONGBOW_TEST_CASE(Global, ccnxManifestSection_AddNameEntry)
{
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);

    CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);

    assertNotNull(section, "Expected non-null section");

    PARCBuffer *buffer = parcBuffer_WrapCString("test");
    CCNxName *elementName = ccnxName_CreateFromCString("lci:/foo/bar/1");
    bool result = ccnxManifestSection_AddNameEntry(section, elementName, 0, buffer);
    assertTrue(result, "Expected successful addition of new name entry");

    size_t nameCount = ccnxManifestSection_GetNameCount(section);
    size_t hashCount = ccnxManifestSection_GetHashCount(section);
    assertTrue(nameCount == 1, "Expected 1 name, got %zu", nameCount);
    assertTrue(hashCount == 1, "Expected 1 name, got %zu", hashCount);

    ccnxManifestSection_Release(&section);
    ccnxLink_Release(&acsLink);
    ccnxName_Release(&acsName);
    parcBuffer_Release(&buffer);
    ccnxName_Release(&elementName);
}

LONGBOW_TEST_CASE(Global, ccnxManifestSection_Acquire_Release)
{
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);

    CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);

    assertNotNull(section, "Expected non-null section");

    PARCReferenceCount firstReferenceCount = parcObject_GetReferenceCount(section);

    CCNxManifestSection *handle = ccnxManifestSection_Acquire(section);

    PARCReferenceCount secondReferenceCount = parcObject_GetReferenceCount(section);

    ccnxManifestSection_Release(&handle);

    PARCReferenceCount thirdReferenceCount = parcObject_GetReferenceCount(section);

    assertTrue(firstReferenceCount == thirdReferenceCount, "Expected %" PRIu64 ", got %" PRIu64, firstReferenceCount, thirdReferenceCount);
    assertTrue(firstReferenceCount + 1 == secondReferenceCount, "Expected %" PRIu64 ", got %" PRIu64, firstReferenceCount + 1, thirdReferenceCount);

    ccnxManifestSection_Release(&section);
    ccnxLink_Release(&acsLink);
    ccnxName_Release(&acsName);
}

LONGBOW_TEST_CASE(Global, ccnxManifestSection_GetACSLink)
{
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);

    CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);

    assertNotNull(section, "Expected non-null section");

    CCNxLink *linkHandle = ccnxManifestSection_GetACSLink(section);
    assertTrue(ccnxLink_Equals(acsLink, linkHandle), "Expected links to be equal");

    ccnxManifestSection_Release(&section);
    ccnxLink_Release(&acsLink);
    ccnxName_Release(&acsName);
}

LONGBOW_TEST_CASE(Global, ccnxManifestSection_GetNameCount)
{
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);

    CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);

    assertNotNull(section, "Expected non-null section");

    PARCBuffer *buffer = parcBuffer_WrapCString("test");
    CCNxName *elementName = ccnxName_CreateFromCString("lci:/foo/bar/1");
    bool result = ccnxManifestSection_AddNameEntry(section, elementName, 0, buffer);
    assertTrue(result, "Expected successful addition of new name entry");

    size_t nameCount = ccnxManifestSection_GetNameCount(section);
    assertTrue(nameCount == 1, "Expected 1 name, got %zu", nameCount);

    ccnxManifestSection_Release(&section);
    ccnxLink_Release(&acsLink);
    ccnxName_Release(&acsName);
    parcBuffer_Release(&buffer);
    ccnxName_Release(&elementName);
}

LONGBOW_TEST_CASE(Global, ccnxManifestSection_GetHashCount)
{
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);

    CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);

    assertNotNull(section, "Expected non-null section");

    PARCBuffer *buffer = parcBuffer_WrapCString("test");
    CCNxName *elementName = ccnxName_CreateFromCString("lci:/foo/bar/1");
    bool result = ccnxManifestSection_AddNameEntry(section, elementName, 0, buffer);
    assertTrue(result, "Expected successful addition of new name entry");

    size_t hashCount = ccnxManifestSection_GetHashCount(section);
    assertTrue(hashCount == 1, "Expected 1 hash entry, got %zu", hashCount);

    ccnxManifestSection_Release(&section);
    ccnxLink_Release(&acsLink);
    ccnxName_Release(&acsName);
    parcBuffer_Release(&buffer);
    ccnxName_Release(&elementName);
}

LONGBOW_TEST_CASE(Global, ccnxManifestSection_GetHash)
{
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);

    CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);

    assertNotNull(section, "Expected non-null section");

    PARCBuffer *buffer = parcBuffer_WrapCString("test");
    CCNxName *elementName = ccnxName_CreateFromCString("lci:/foo/bar/1");
    bool result = ccnxManifestSection_AddNameEntry(section, elementName, 0, buffer);
    assertTrue(result, "Expected successful addition of new name entry");

    PARCBuffer *handle = ccnxManifestSection_GetHashAtIndex(section, 0);
    assertTrue(parcBuffer_Equals(buffer, handle), "Expected PARCBuffer instances to be equal");

    ccnxManifestSection_Release(&section);
    ccnxLink_Release(&acsLink);
    ccnxName_Release(&acsName);
    parcBuffer_Release(&buffer);
    ccnxName_Release(&elementName);
}

LONGBOW_TEST_CASE(Global, ccnxManifestSection_GetNameFromHashIndex)
{
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);

    CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);

    assertNotNull(section, "Expected non-null section");

    PARCBuffer *buffer = parcBuffer_WrapCString("test");
    CCNxName *elementName = ccnxName_CreateFromCString("lci:/foo/bar/1");
    bool result = ccnxManifestSection_AddNameEntry(section, elementName, 0, buffer);
    assertTrue(result, "Expected successful addition of new name entry");

    CCNxName *mediaName = ccnxManifestSection_GetNameFromHashIndex(section, 0);
    assertTrue(ccnxName_Equals(elementName, mediaName), "Expected names to be equal");

    ccnxManifestSection_Release(&section);
    ccnxLink_Release(&acsLink);
    ccnxName_Release(&acsName);
    parcBuffer_Release(&buffer);
    ccnxName_Release(&elementName);
}

LONGBOW_TEST_CASE(Global, ccnxManifestSection_GetNameChunkFromHashIndex)
{
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);

    CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);

    assertNotNull(section, "Expected non-null section");

    PARCBuffer *buffer = parcBuffer_WrapCString("test");
    CCNxName *elementName = ccnxName_CreateFromCString("lci:/foo/bar/1");
    bool result = ccnxManifestSection_AddNameEntry(section, elementName, 10, buffer);
    assertTrue(result, "Expected successful addition of new name entry");

    size_t chunk = ccnxManifestSection_GetNameChunkFromHashIndex(section, 0);
    assertTrue(chunk == 10, "Expected chunk 10, got %zu", chunk);

    ccnxManifestSection_Release(&section);
    ccnxLink_Release(&acsLink);
    ccnxName_Release(&acsName);
    parcBuffer_Release(&buffer);
    ccnxName_Release(&elementName);
}

LONGBOW_TEST_CASE(Global, ccnxManifestSection_Create_ToString)
{
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);

    CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);

    assertNotNull(section, "Expected non-null section");

    PARCBuffer *buffer = parcBuffer_WrapCString("test");
    CCNxName *elementName = ccnxName_CreateFromCString("lci:/foo/bar/1");
    bool result = ccnxManifestSection_AddNameEntry(section, elementName, 0, buffer);
    assertTrue(result, "Expected successful addition of new name entry");

    char *string = ccnxManifestSection_ToString(section);
    assertNotNull(string, "Expected non-null string.");
    parcMemory_Deallocate((void **) &string);

    ccnxManifestSection_Release(&section);
    ccnxLink_Release(&acsLink);
    ccnxName_Release(&acsName);
    parcBuffer_Release(&buffer);
    ccnxName_Release(&elementName);
}

static CCNxManifestSection *
_createFullManifestSection()
{
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);

    CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);

    assertNotNull(section, "Expected non-null section");

    PARCBuffer *buffer = parcBuffer_WrapCString("test");
    CCNxName *elementName = ccnxName_CreateFromCString("lci:/foo/bar/1");
    ccnxManifestSection_AddNameEntry(section, elementName, 0, buffer);

    ccnxLink_Release(&acsLink);
    ccnxName_Release(&acsName);
    parcBuffer_Release(&buffer);
    ccnxName_Release(&elementName);

    return section;
}

LONGBOW_TEST_CASE(Global, ccnxManifestSection_Equals)
{
    CCNxManifestSection *x = _createFullManifestSection();
    CCNxManifestSection *y = _createFullManifestSection();
    CCNxManifestSection *z = _createFullManifestSection();

    CCNxManifestSection *u1 = _createFullManifestSection();
    ccnxLink_Release(&u1->acsLink);
    CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/different");
    CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);
    u1->acsLink = acsLink;

    CCNxManifestSection *u2 = _createFullManifestSection();
    u2->numberOfHashes = 100;

    CCNxManifestSection *u3 = _createFullManifestSection();
    u3->numberOfNames = 100;

    // different hash entry
    CCNxManifestSection *u4 = _createFullManifestSection();
    parcList_Release(&u4->listOfHashes);
    parcList_Release(&u4->listOfNames);
    PARCArrayList *hashList4 = parcArrayList_Create((void (*)(void **))_ccnxManifestSectionHashEntry_Release);
    u4->listOfHashes = parcList(hashList4, PARCArrayListAsPARCList);
    PARCArrayList *nameList4 = parcArrayList_Create((void (*)(void **))_ccnxManifestSectionHashEntry_Release);
    u4->listOfNames = parcList(nameList4, PARCArrayListAsPARCList);
    PARCBuffer *buffer4 = parcBuffer_WrapCString("THIS HASH IS DIFFERENT");
    CCNxName *elementName4 = ccnxName_CreateFromCString("lci:/foo/bar/1");
    ccnxManifestSection_AddNameEntry(u4, elementName4, 0, buffer4);
    parcBuffer_Release(&buffer4);
    ccnxName_Release(&elementName4);

    CCNxManifestSection *u5 = _createFullManifestSection();
    parcList_Release(&u5->listOfHashes);
    parcList_Release(&u5->listOfNames);
    PARCArrayList *hashList5 = parcArrayList_Create((void (*)(void **))_ccnxManifestSectionHashEntry_Release);
    u5->listOfHashes = parcList(hashList5, PARCArrayListAsPARCList);
    PARCArrayList *nameList5 = parcArrayList_Create((void (*)(void **))_ccnxManifestSectionHashEntry_Release);
    u5->listOfNames = parcList(nameList5, PARCArrayListAsPARCList);
    PARCBuffer *buffer5 = parcBuffer_WrapCString("test");
    CCNxName *elementName5 = ccnxName_CreateFromCString("lci:/foo/bar/somethingdifferent");
    ccnxManifestSection_AddNameEntry(u5, elementName5, 0, buffer5);
    parcBuffer_Release(&buffer5);
    ccnxName_Release(&elementName5);

    assertEqualsContract(ccnxManifestSection_Equals, x, y, z, u1, u2, u3, u4, u5, NULL);

    ccnxName_Release(&acsName);
    ccnxManifestSection_Release(&x);
    ccnxManifestSection_Release(&y);
    ccnxManifestSection_Release(&z);
    ccnxManifestSection_Release(&u1);
    ccnxManifestSection_Release(&u2);
    ccnxManifestSection_Release(&u3);
    ccnxManifestSection_Release(&u4);
    ccnxManifestSection_Release(&u5);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_Manifest);
    int exitStatus = LONGBOW_TEST_MAIN(argc, argv, testRunner);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
