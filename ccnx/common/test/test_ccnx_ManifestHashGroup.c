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
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../ccnx_ManifestHashGroup.c"

#include <inttypes.h>
#include <ccnx/common/ccnx_Manifest.h>

#include <ccnx/common/ccnx_Name.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_PacketEncoder.h>
#include <ccnx/common/codec/ccnxCodec_TlvPacket.h>
#include <ccnx/common/ccnx_WireFormatMessage.h>

#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>
#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_ArrayList.h>

#include <parc/testing/parc_ObjectTesting.h>

LONGBOW_TEST_RUNNER(ccnx_ManifestHashGroup)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_ManifestHashGroup)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_ManifestHashGroup)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_AcquireRelease);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_Create);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_CreateFromJson);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_AppendGetPointer);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_PrependGetPointer);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_ToString);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_ToJson);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_IsFull);

    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_Equals);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_Iterator);

    // Metadata
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_BlockSize);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_DataSize);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_EntrySize);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_TreeHeight);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_Locator);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_OverallDataDigest);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestHashGroup_HasMetadata);
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

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_AcquireRelease)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    parcObjectTesting_AssertAcquireReleaseContract(ccnxManifestHashGroup_Acquire, group);

    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_Create)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");
    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_CreateFromJson)
{
    char *jsonString = "{ \"HashGroup\" : [ { \"type\" : 0, \"digest\" : \"FFFF\" } ] }";
    PARCJSON *json = parcJSON_ParseString(jsonString);

    CCNxManifestHashGroup *group = ccnxManifestHashGroup_CreateFromJson(json);
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    size_t actual = ccnxManifestHashGroup_GetNumberOfPointers(group);
    size_t expected = 1;

    assertTrue(actual == expected, "Expected %zu pointers, got %zu", expected, actual);

    parcJSON_Release(&json);
    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_AppendGetPointer)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    PARCBuffer *buffer1 = parcBuffer_Allocate(10);
    PARCBuffer *buffer2 = parcBuffer_Allocate(10);

    ccnxManifestHashGroup_AppendPointer(group, CCNxManifestHashGroupPointerType_Data, buffer1);
    ccnxManifestHashGroup_AppendPointer(group, CCNxManifestHashGroupPointerType_Manifest, buffer2);

    size_t expected = 2;
    size_t actual = ccnxManifestHashGroup_GetNumberOfPointers(group);

    assertTrue(expected == actual, "Expected %zu, got %zu", expected, actual);
    assertTrue(ccnxManifestHashGroupPointer_GetType(ccnxManifestHashGroup_GetPointerAtIndex(group, 0)) == CCNxManifestHashGroupPointerType_Data, "Expected data in the first slot");
    assertTrue(ccnxManifestHashGroupPointer_GetType(ccnxManifestHashGroup_GetPointerAtIndex(group, 1)) == CCNxManifestHashGroupPointerType_Manifest, "Expected data in the first slot");

    parcBuffer_Release(&buffer1);
    parcBuffer_Release(&buffer2);

    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_PrependGetPointer)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    PARCBuffer *buffer1 = parcBuffer_Allocate(10);
    PARCBuffer *buffer2 = parcBuffer_Allocate(10);

    ccnxManifestHashGroup_PrependPointer(group, CCNxManifestHashGroupPointerType_Data, buffer1);
    ccnxManifestHashGroup_PrependPointer(group, CCNxManifestHashGroupPointerType_Manifest, buffer2);

    size_t expected = 2;
    size_t actual = ccnxManifestHashGroup_GetNumberOfPointers(group);

    assertTrue(expected == actual, "Expected %zu, got %zu", expected, actual);
    assertTrue(ccnxManifestHashGroupPointer_GetType(ccnxManifestHashGroup_GetPointerAtIndex(group, 1)) == CCNxManifestHashGroupPointerType_Data, "Expected data in the first slot");
    assertTrue(ccnxManifestHashGroupPointer_GetType(ccnxManifestHashGroup_GetPointerAtIndex(group, 0)) == CCNxManifestHashGroupPointerType_Manifest, "Expected data in the first slot");

    parcBuffer_Release(&buffer1);
    parcBuffer_Release(&buffer2);

    ccnxManifestHashGroup_Release(&group);
}

static CCNxManifestHashGroup *
_createHashGroup(CCNxName *locator, size_t n, size_t blockSize, size_t dataSize, size_t entrySize, size_t treeHeight)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();

    if (locator != NULL) {
        ccnxManifestHashGroup_SetLocator(group, locator);
    }

    for (size_t i = 0; i < n; i++) {
        PARCBuffer *buffer = parcBuffer_AllocateCString("random");
        ccnxManifestHashGroup_AppendPointer(group, CCNxManifestHashGroupPointerType_Data, buffer);
        parcBuffer_Release(&buffer);
    }

    if (blockSize != 0) {
        ccnxManifestHashGroup_SetBlockSize(group, blockSize);
    }

    if (dataSize != 0) {
        ccnxManifestHashGroup_SetDataSize(group, dataSize);
    }

    if (entrySize != 0) {
        ccnxManifestHashGroup_SetEntrySize(group, entrySize);
    }

    if (treeHeight != 0) {
        ccnxManifestHashGroup_SetTreeHeight(group, treeHeight);
    }

    return group;
}


LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_Equals)
{
    CCNxName *locator = ccnxName_CreateFromCString("ccnx:/my/manifest");

    CCNxManifestHashGroup *x = _createHashGroup(locator, 10, 0, 0, 0, 0);
    CCNxManifestHashGroup *y = _createHashGroup(locator, 10, 0, 0, 0, 0);
    CCNxManifestHashGroup *z = _createHashGroup(locator, 10, 0, 0, 0, 0);

    CCNxManifestHashGroup *u1 = _createHashGroup(locator, 5, 0, 0, 0, 0);
    CCNxManifestHashGroup *u2 = _createHashGroup(NULL, 10, 0, 0, 0, 0);
    CCNxManifestHashGroup *u3 = _createHashGroup(locator, 10, 1, 0, 0, 0);
    CCNxManifestHashGroup *u4 = _createHashGroup(locator, 10, 0, 1, 0, 0);
    CCNxManifestHashGroup *u5 = _createHashGroup(locator, 10, 0, 0, 1, 0);
    CCNxManifestHashGroup *u6 = _createHashGroup(locator, 10, 0, 0, 0, 1);

    parcObjectTesting_AssertEqualsFunction(ccnxManifestHashGroup_Equals, x, y, z, u1, u2, u3, u4, u5, u6, NULL);

    ccnxManifestHashGroup_Release(&x);
    ccnxManifestHashGroup_Release(&y);
    ccnxManifestHashGroup_Release(&z);

    ccnxManifestHashGroup_Release(&u1);
    ccnxManifestHashGroup_Release(&u2);
    ccnxManifestHashGroup_Release(&u3);
    ccnxManifestHashGroup_Release(&u4);
    ccnxManifestHashGroup_Release(&u5);
    ccnxManifestHashGroup_Release(&u6);

    ccnxName_Release(&locator);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_ToString)
{
    char *jsonString = "{ \"HashGroup\" : [ { \"type\" : 0, \"digest\" : \"617364617364617364\" } ] }";
    PARCJSON *json = parcJSON_ParseString(jsonString);

    CCNxManifestHashGroup *group = ccnxManifestHashGroup_CreateFromJson(json);

    char *stringForm = ccnxManifestHashGroup_ToString(group);
    assertTrue(strcmp(jsonString, stringForm) == 0, "Expected %s and actual %s should be equal.", jsonString, stringForm);

    parcMemory_Deallocate(&stringForm);
    parcJSON_Release(&json);
    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_ToJson)
{
    char *jsonString = "{ \"HashGroup\" : [ { \"type\" : 0, \"digest\" : \"617364617364617364\" } ] }";
    PARCJSON *json = parcJSON_ParseString(jsonString);

    CCNxManifestHashGroup *group = ccnxManifestHashGroup_CreateFromJson(json);
    PARCJSON *expected = ccnxManifestHashGroup_ToJson(group);

    assertTrue(parcJSON_Equals(json, expected), "Expected the input and output JSON to be identical");

    parcJSON_Release(&expected);
    parcJSON_Release(&json);
    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_IsFull)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    for (size_t i = 0; i < MAX_NUMBER_OF_POINTERS; i++) {
        PARCBuffer *buffer = parcBuffer_Allocate(10);
        assertTrue(ccnxManifestHashGroup_AppendPointer(group, CCNxManifestHashGroupPointerType_Data, buffer), "Expected the insertion to succeed");
        parcBuffer_Release(&buffer);
    }

    PARCBuffer *buffer = parcBuffer_Allocate(10);
    assertFalse(ccnxManifestHashGroup_AppendPointer(group, CCNxManifestHashGroupPointerType_Data, buffer), "Expected the insertion to fail since the HashGroup is full.");
    parcBuffer_Release(&buffer);

    bool isFull = ccnxManifestHashGroup_IsFull(group);
    assertTrue(isFull, "Expected the group to be full after %ul pointers", MAX_NUMBER_OF_POINTERS);

    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_BlockSize)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    size_t blockSize = 10;
    ccnxManifestHashGroup_SetBlockSize(group, blockSize);
    size_t actual = ccnxManifestHashGroup_GetBlockSize(group);

    assertTrue(blockSize == actual, "Expected %zu, got %zu", blockSize, actual);

    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_DataSize)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    size_t dataSize = 10;
    ccnxManifestHashGroup_SetDataSize(group, dataSize);
    size_t actual = ccnxManifestHashGroup_GetDataSize(group);

    assertTrue(dataSize  == actual, "Expected %zu, got %zu", dataSize , actual);

    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_EntrySize)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    size_t entrySize = 10;
    ccnxManifestHashGroup_SetEntrySize(group, entrySize);
    size_t actual = ccnxManifestHashGroup_GetEntrySize(group);

    assertTrue(entrySize  == actual, "Expected %zu, got %zu", entrySize , actual);

    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_TreeHeight)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    size_t treeHeight = 10;
    ccnxManifestHashGroup_SetTreeHeight(group, treeHeight);
    size_t actual = ccnxManifestHashGroup_GetTreeHeight(group);

    assertTrue(treeHeight  == actual, "Expected %zu, got %zu", treeHeight , actual);

    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_OverallDataDigest)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    PARCBuffer *digest = parcBuffer_Allocate(10);
    ccnxManifestHashGroup_SetOverallDataDigest(group, digest);
    const PARCBuffer *actual = ccnxManifestHashGroup_GetOverallDataDigest(group);

    assertTrue(parcBuffer_Equals(digest, actual) == true, "Expected %s, got %s", parcBuffer_ToHexString(digest), parcBuffer_ToHexString(actual));

    parcBuffer_Release(&digest);
    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_Locator)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    CCNxName *expected = ccnxName_CreateFromCString("ccnx:/flic/manifest");
    ccnxManifestHashGroup_SetLocator(group, expected);
    const CCNxName *actual = ccnxManifestHashGroup_GetLocator(group);

    assertTrue(ccnxName_Equals(expected, actual) == true, "Expected %s, got %s", ccnxName_ToString(expected), ccnxName_ToString(actual));

    ccnxName_Release(&expected);
    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_HasMetadata)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    assertFalse(ccnxManifestHashGroup_HasMetadata(group), "Expected an empty HashGroup to have no metadata");

    CCNxName *expected = ccnxName_CreateFromCString("ccnx:/flic/manifest");
    ccnxManifestHashGroup_SetLocator(group, expected);

    assertTrue(ccnxManifestHashGroup_HasMetadata(group), "Expected a HashGroup with a locator to have metadata");

    ccnxName_Release(&expected);
    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifestHashGroup_Iterator)
{
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    assertNotNull(group, "Expected non-null CCNxManifestHashGroup");

    for (size_t i = 0; i < MAX_NUMBER_OF_POINTERS; i++) {
        PARCBuffer *buffer = parcBuffer_Allocate(10);
        parcBuffer_Flip(parcBuffer_PutUint32(buffer, i));
        assertTrue(ccnxManifestHashGroup_AppendPointer(group, CCNxManifestHashGroupPointerType_Data, buffer), "Expected the insertion to succeed");
        parcBuffer_Release(&buffer);
    }

    PARCIterator *itr = ccnxManifestHashGroup_Iterator(group);

    size_t i = 0;
    while (parcIterator_HasNext(itr)) {
        CCNxManifestHashGroupPointer *ptr = (CCNxManifestHashGroupPointer *) parcIterator_Next(itr);
        const PARCBuffer *digest = ccnxManifestHashGroupPointer_GetDigest(ptr);
        size_t index = parcBuffer_GetUint32((PARCBuffer *) digest);
        assertTrue(index == i, "Expected the right digest pointer to be extracted, got %zu, expected %zu", index, i);
        i++;
    }

    parcIterator_Release(&itr);

    bool isFull = ccnxManifestHashGroup_IsFull(group);
    assertTrue(isFull, "Expected the group to be full after %ul pointers", MAX_NUMBER_OF_POINTERS);

    ccnxManifestHashGroup_Release(&group);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_ManifestHashGroup);
    int exitStatus = LONGBOW_TEST_MAIN(argc, argv, testRunner);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
