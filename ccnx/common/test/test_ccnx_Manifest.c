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
#include "../ccnx_Manifest.c"

#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>

#include <parc/testing/parc_ObjectTesting.h>

typedef struct test_data {
    CCNxManifest *object;
} ManifestTestData;

static ManifestTestData *
_commonSetup(void)
{
    ManifestTestData *data = parcMemory_AllocateAndClear(sizeof(ManifestTestData));

    CCNxName *name = ccnxName_CreateFromCString("ccnx:/my/manifest");
    CCNxManifest *manifest = ccnxManifest_Create(name);
    ccnxName_Release(&name);

    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();

    PARCBuffer *digest1 = parcBuffer_Allocate(10);
    PARCBuffer *digest2 = parcBuffer_Allocate(10);
    ccnxManifestHashGroup_AppendPointer(group, CCNxManifestHashGroupPointerType_Data, digest1);
    ccnxManifestHashGroup_AppendPointer(group, CCNxManifestHashGroupPointerType_Manifest, digest1);
    parcBuffer_Release(&digest1);
    parcBuffer_Release(&digest2);

    CCNxName *locator = ccnxName_CreateFromCString("ccnx:/locator");
    ccnxManifestHashGroup_SetLocator(group, locator);
    ccnxName_Release(&locator);

    ccnxManifest_AddHashGroup(manifest, group);
    ccnxManifestHashGroup_Release(&group);

    data->object = manifest;

    return data;
}

static void
_commonTeardown(ManifestTestData *data)
{
    ccnxManifest_Release(&data->object);
    parcMemory_Deallocate((void **) &data);
}

LONGBOW_TEST_RUNNER(ccnx_Manifest)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_Manifest)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_Manifest)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifest_AcquireRelease);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifest_Create);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifest_AddHashGroup);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifest_GetHashGroup);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifest_GetNumberOfHashGroups);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifest_GetName);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifest_Equals);
    LONGBOW_RUN_TEST_CASE(Global, ccnxManifest_ToString);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    longBowTestCase_SetClipBoardData(testCase, _commonSetup());

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    _commonTeardown(longBowTestCase_GetClipBoardData(testCase));
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxManifest_AcquireRelease)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/my/manifest");
    CCNxManifest *manifest = ccnxManifest_Create(name);
    ccnxName_Release(&name);

    parcObjectTesting_AssertAcquireReleaseContract(ccnxManifest_Acquire, manifest);

    ccnxManifest_Release(&manifest);
}

LONGBOW_TEST_CASE(Global, ccnxManifest_Create)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/my/manifest");
    CCNxManifest *manifest = ccnxManifest_Create(name);

    assertNotNull(manifest, "Expected the Manifest to be created without errors.");
    const CCNxName *copy = ccnxManifest_GetName(manifest);

    assertTrue(ccnxName_Equals(name, copy), "Expected name to equal %s, got %s", ccnxName_ToString(name), ccnxName_ToString(copy));

    ccnxName_Release(&name);
    ccnxManifest_Release(&manifest);
}

LONGBOW_TEST_CASE(Global, ccnxManifest_AddHashGroup)
{
    ManifestTestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxManifest *manifest = data->object;

    size_t numGroups = ccnxManifest_GetNumberOfHashGroups(manifest);

    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    ccnxManifest_AddHashGroup(manifest, group);

    size_t expected = numGroups + 1;
    size_t actual = ccnxManifest_GetNumberOfHashGroups(manifest);
    assertTrue(actual == expected, "Expected %zu, got %zu", expected, actual);

    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifest_GetHashGroup)
{
    ManifestTestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxManifest *manifest = data->object;

    CCNxManifestHashGroup *group = ccnxManifest_GetHashGroupByIndex(manifest, 0);
    CCNxName *expected = ccnxName_CreateFromCString("ccnx:/locator");
    const CCNxName *actual = ccnxManifestHashGroup_GetLocator(group);
    assertTrue(ccnxName_Equals(expected, actual), "Expected %s, got %s", ccnxName_ToString(expected), ccnxName_ToString(actual));

    ccnxName_Release(&expected);
    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifest_GetNumberOfHashGroups)
{
    ManifestTestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxManifest *manifest = data->object;

    size_t before = ccnxManifest_GetNumberOfHashGroups(manifest);
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    ccnxManifest_AddHashGroup(manifest, group);

    size_t actual = ccnxManifest_GetNumberOfHashGroups(manifest);
    size_t expected = before + 1;

    assertTrue(expected == actual, "Expected %zu, got %zu", expected, actual);

    ccnxManifestHashGroup_Release(&group);
}

LONGBOW_TEST_CASE(Global, ccnxManifest_GetName)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/my/manifest");
    CCNxManifest *manifest = ccnxManifest_Create(name);

    assertNotNull(manifest, "Expected the Manifest to be created without errors.");
    const CCNxName *copy = ccnxManifest_GetName(manifest);

    assertTrue(ccnxName_Equals(name, copy), "Expected name to equal %s, got %s", ccnxName_ToString(name), ccnxName_ToString(copy));

    ccnxName_Release(&name);
    ccnxManifest_Release(&manifest);
}

LONGBOW_TEST_CASE(Global, ccnxManifest_Equals)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/my/manifest");
    CCNxManifest *x = ccnxManifest_Create(name);
    CCNxManifest *y = ccnxManifest_Create(name);
    CCNxManifest *z = ccnxManifest_Create(name);

    CCNxName *name1 = ccnxName_CreateFromCString("ccnx:/not/my/manifest");
    CCNxManifest *u1 = ccnxManifest_Create(name1);

    parcObjectTesting_AssertEqualsFunction(ccnxManifest_Equals, x, y, z, u1, NULL);

    ccnxName_Release(&name);
    ccnxName_Release(&name1);
    ccnxManifest_Release(&x);
    ccnxManifest_Release(&y);
    ccnxManifest_Release(&z);
    ccnxManifest_Release(&u1);
}

LONGBOW_TEST_CASE(Global, ccnxManifest_ToString)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/my/manifest");
    CCNxManifest *manifest = ccnxManifest_Create(name);

    assertNotNull(manifest, "Expected the Manifest to be created without errors.");
    const CCNxName *copy = ccnxManifest_GetName(manifest);

    assertTrue(ccnxName_Equals(name, copy), "Expected name to equal %s, got %s", ccnxName_ToString(name), ccnxName_ToString(copy));

    ccnxName_Release(&name);
    ccnxManifest_Release(&manifest);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_Manifest);
    int exitStatus = LONGBOW_TEST_MAIN(argc, argv, testRunner);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
