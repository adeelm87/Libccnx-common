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
 * @copyright 2015-2016, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../ccnx_ManifestFacadeV1.c"
#include <parc/algol/parc_SafeMemory.h>
#include <parc/algol/parc_Memory.h>
#include <LongBow/unit-test.h>

#include <ccnx/common/ccnx_Manifest.h>

typedef struct test_data {
    CCNxTlvDictionary *manifest;

    CCNxName *name;
} TestData;

static TestData *
_commonSetup(void)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(TestData));

    data->name = ccnxName_CreateFromCString("lci:/once/upon/a/time");
    data->manifest = _ccnxManifestFacadeV1_Create(data->name);

    return data;
}

static void
_commonTeardown(TestData *data)
{
    ccnxName_Release(&data->name);
    ccnxTlvDictionary_Release(&data->manifest);

    parcMemory_Deallocate((void **) &data);
}

LONGBOW_TEST_RUNNER(ccnx_ManifestFacadeV1)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_ManifestFacadeV1)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_ManifestFacadeV1)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ========================================================================================

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestFacadeV1_Create);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestFacadeV1_GetName);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestFacadeV1_AddHashGroup);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestFacadeV1_GetHashGroup);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifestFacadeV1_Equals);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    longBowTestCase_SetClipBoardData(testCase, _commonSetup());

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    _commonTeardown(longBowTestCase_GetClipBoardData(testCase));

    if (parcSafeMemory_ReportAllocation(STDOUT_FILENO) != 0) {
        printf("('%s' leaks memory by %d (allocs - frees)) ", longBowTestCase_GetName(testCase), parcMemory_Outstanding());
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxManifestFacadeV1_Create)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxTlvDictionary *manifest= _ccnxManifestFacadeV1_Create(data->name);

    CCNxName *test = _ccnxManifestFacadeV1_GetName(manifest);
    assertTrue(ccnxName_Equals(test, data->name), "Names do not match");
    ccnxTlvDictionary_Release(&manifest);
}

LONGBOW_TEST_CASE(Global, ccnxManifestFacadeV1_AddHashGroup)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxTlvDictionary *manifest= _ccnxManifestFacadeV1_Create(data->name);

    int actual = 0;
    int numberOfGroups = _ccnxManifestFacadeV1_GetNumberOfHashGroups(manifest);
    assertTrue(numberOfGroups == actual, "Expected %d, got %d", actual, numberOfGroups);

    CCNxManifestHashGroup *hashGroup = ccnxManifestHashGroup_Create();
    _ccnxManifestFacadeV1_AddHashGroup(manifest, hashGroup);
    actual++;
    numberOfGroups = _ccnxManifestFacadeV1_GetNumberOfHashGroups(manifest);

    assertTrue(numberOfGroups == actual, "Expected %d, got %d", actual, numberOfGroups);

    ccnxManifestHashGroup_Release(&hashGroup);
    ccnxTlvDictionary_Release(&manifest);
}

LONGBOW_TEST_CASE(Global, ccnxManifestFacadeV1_GetHashGroup)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxTlvDictionary *manifest= _ccnxManifestFacadeV1_Create(data->name);

    CCNxManifestHashGroup *hashGroup = ccnxManifestHashGroup_Create();
    _ccnxManifestFacadeV1_AddHashGroup(manifest, hashGroup);

    CCNxManifestHashGroup *firstGroup = _ccnxManifestFacadeV1_GetHashGroup(manifest, 0);
    assertTrue(ccnxManifestHashGroup_Equals(hashGroup, firstGroup), "Expected the HashGroups to be equal");

    ccnxManifestHashGroup_Release(&hashGroup);
    ccnxManifestHashGroup_Release(&firstGroup);
    ccnxTlvDictionary_Release(&manifest);
}

LONGBOW_TEST_CASE(Global, ccnxManifestFacadeV1_GetNumberOfHashGroups)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxTlvDictionary *manifest= _ccnxManifestFacadeV1_Create(data->name);

    CCNxName *test = _ccnxManifestFacadeV1_GetName(manifest);
    assertTrue(ccnxName_Equals(test, data->name), "Names do not match");
    ccnxTlvDictionary_Release(&manifest);
}


LONGBOW_TEST_CASE(Global, ccnxManifestFacadeV1_GetName)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxName *test = _ccnxManifestFacadeV1_GetName(data->manifest);
    assertTrue(ccnxName_Equals(test, data->name), "Names do not match")
    {
        printf("\ngot     : \n"); ccnxName_Display(test, 3);
        printf("\nexpected: \n"); ccnxName_Display(data->name, 3);
    }
}

LONGBOW_TEST_CASE(Global, ccnxManifestFacadeV1_Equals)
{
    CCNxName *name1 = ccnxName_CreateFromCString("lci:/name/1");
    CCNxName *name2 = ccnxName_CreateFromCString("lci:/name/2");

    CCNxTlvDictionary *x = _ccnxManifestFacadeV1_Create(name1);
    CCNxTlvDictionary *y = _ccnxManifestFacadeV1_Create(name1);
    CCNxTlvDictionary *z = _ccnxManifestFacadeV1_Create(name1);

    CCNxTlvDictionary *diff = _ccnxManifestFacadeV1_Create(name2);

    assertEqualsContract(_ccnxManifestFacadeV1_Equals, x, y, z, diff, NULL);

    ccnxTlvDictionary_Release(&x);
    ccnxTlvDictionary_Release(&y);
    ccnxTlvDictionary_Release(&z);
    ccnxTlvDictionary_Release(&diff);

    ccnxName_Release(&name1);
    ccnxName_Release(&name2);
}

// ========================================================================================

LONGBOW_TEST_FIXTURE(Local)
{
}

LONGBOW_TEST_FIXTURE_SETUP(Local)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Local)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_ManifestFacadeV1);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
