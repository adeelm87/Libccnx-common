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

#include <ccnx/common/ccnx_Link.c>

#include <LongBow/unit-test.h>

#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_SafeMemory.h>
#include <parc/algol/parc_ArrayList.h>
#include <parc/testing/parc_ObjectTesting.h>

LONGBOW_TEST_RUNNER(ccnx_Link)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_Link)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_Link)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxLink_Create_Full);
    LONGBOW_RUN_TEST_CASE(Global, ccnxLink_Create_EmptyKeyID);
    LONGBOW_RUN_TEST_CASE(Global, ccnxLink_Create_EmptyContentObjectHash);
    LONGBOW_RUN_TEST_CASE(Global, ccnxLink_Create_EmptyBoth);
    LONGBOW_RUN_TEST_CASE(Global, ccnxLink_AcquireRelease);

    LONGBOW_RUN_TEST_CASE(Global, ccnxLink_GetName);
    LONGBOW_RUN_TEST_CASE(Global, ccnxLink_GetKeyID);
    LONGBOW_RUN_TEST_CASE(Global, ccnxLink_GetContentObjectHash);

    LONGBOW_RUN_TEST_CASE(Global, ccnxLink_Create_ToString);

    LONGBOW_RUN_TEST_CASE(Global, ccnxLink_Equals);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxLink_Create_Full)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    PARCBuffer *keyId = parcBuffer_Allocate(10);
    PARCBuffer *contentObjectHash = parcBuffer_Allocate(10);

    CCNxLink *object = ccnxLink_Create(name, keyId, contentObjectHash);

    assertNotNull(object, "Expected non-null return value.");

    ccnxLink_Release(&object);

    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);
    parcBuffer_Release(&contentObjectHash);
}

LONGBOW_TEST_CASE(Global, ccnxLink_Create_EmptyKeyID)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    PARCBuffer *keyId = NULL;
    PARCBuffer *contentObjectHash = parcBuffer_Allocate(10);

    CCNxLink *object = ccnxLink_Create(name, keyId, contentObjectHash);

    assertNotNull(object, "Expected non-null return value.");

    ccnxLink_Release(&object);

    ccnxName_Release(&name);
    parcBuffer_Release(&contentObjectHash);
}

LONGBOW_TEST_CASE(Global, ccnxLink_Create_EmptyContentObjectHash)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    PARCBuffer *keyId = parcBuffer_Allocate(10);
    PARCBuffer *contentObjectHash = NULL;

    CCNxLink *object = ccnxLink_Create(name, keyId, contentObjectHash);

    assertNotNull(object, "Expected non-null return value.");

    ccnxLink_Release(&object);

    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);
}

LONGBOW_TEST_CASE(Global, ccnxLink_Create_EmptyBoth)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    PARCBuffer *keyId = NULL;
    PARCBuffer *contentObjectHash = NULL;

    CCNxLink *object = ccnxLink_Create(name, keyId, contentObjectHash);

    assertNotNull(object, "Expected non-null return value.");

    ccnxLink_Release(&object);

    ccnxName_Release(&name);
}

LONGBOW_TEST_CASE(Global, ccnxLink_AcquireRelease)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    PARCBuffer *keyId = parcBuffer_Allocate(10);
    PARCBuffer *contentObjectHash = parcBuffer_Allocate(10);

    CCNxLink *object = ccnxLink_Create(name, keyId, contentObjectHash);
    parcObjectTesting_AssertAcquireReleaseContract(ccnxLink_Acquire, object);

    ccnxLink_Release(&object);

    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);
    parcBuffer_Release(&contentObjectHash);
}

LONGBOW_TEST_CASE(Global, ccnxLink_GetName)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    PARCBuffer *keyId = parcBuffer_Allocate(10);
    PARCBuffer *contentObjectHash = parcBuffer_Allocate(10);

    CCNxLink *object = ccnxLink_Create(name, keyId, contentObjectHash);

    const CCNxName *actualName = ccnxLink_GetName(object);
    assertNotNull(actualName, "Expected non-null return value.");
    assertTrue(ccnxName_Equals(name, actualName), "Expected the same name back");

    ccnxLink_Release(&object);

    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);
    parcBuffer_Release(&contentObjectHash);
}

LONGBOW_TEST_CASE(Global, ccnxLink_GetKeyID)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    PARCBuffer *keyId = parcBuffer_Allocate(10);
    PARCBuffer *contentObjectHash = parcBuffer_Allocate(20);

    CCNxLink *object = ccnxLink_Create(name, keyId, contentObjectHash);

    PARCBuffer *buffer = ccnxLink_GetKeyID(object);
    assertNotNull(buffer, "Expected non-null return value.");
    assertTrue(parcBuffer_Capacity(buffer) == 10, "Expected the same buffer size back");

    ccnxLink_Release(&object);

    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);
    parcBuffer_Release(&contentObjectHash);
}

LONGBOW_TEST_CASE(Global, ccnxLink_GetContentObjectHash)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    PARCBuffer *keyId = parcBuffer_Allocate(10);
    PARCBuffer *contentObjectHash = parcBuffer_Allocate(20);

    CCNxLink *object = ccnxLink_Create(name, keyId, contentObjectHash);

    PARCBuffer *buffer = ccnxLink_GetContentObjectHash(object);
    assertNotNull(buffer, "Expected non-null return value.");
    assertTrue(parcBuffer_Capacity(buffer) == 20, "Expected the same buffer size back");

    ccnxLink_Release(&object);

    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);
    parcBuffer_Release(&contentObjectHash);
}

LONGBOW_TEST_CASE(Global, ccnxLink_Equals)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    PARCBuffer *keyId = parcBuffer_Allocate(10);
    PARCBuffer *contentObjectHash = parcBuffer_Allocate(20);
    CCNxLink *x = ccnxLink_Create(name, keyId, contentObjectHash);
    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);
    parcBuffer_Release(&contentObjectHash);

    name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    keyId = parcBuffer_Allocate(10);
    contentObjectHash = parcBuffer_Allocate(20);
    CCNxLink *y = ccnxLink_Create(name, keyId, contentObjectHash);
    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);
    parcBuffer_Release(&contentObjectHash);

    name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    keyId = parcBuffer_Allocate(10);
    contentObjectHash = parcBuffer_Allocate(20);
    CCNxLink *z = ccnxLink_Create(name, keyId, contentObjectHash);
    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);
    parcBuffer_Release(&contentObjectHash);

    name = ccnxName_CreateFromCString("lci:/foo/bar/othername");
    keyId = parcBuffer_Allocate(10);
    contentObjectHash = parcBuffer_Allocate(20);
    CCNxLink *unequal1 = ccnxLink_Create(name, keyId, contentObjectHash);
    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);
    parcBuffer_Release(&contentObjectHash);

    name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    keyId = NULL;
    contentObjectHash = parcBuffer_Allocate(20);
    CCNxLink *unequal2 = ccnxLink_Create(name, keyId, contentObjectHash);
    ccnxName_Release(&name);
    parcBuffer_Release(&contentObjectHash);

    name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    keyId = parcBuffer_Allocate(10);
    contentObjectHash = NULL;
    CCNxLink *unequal3 = ccnxLink_Create(name, keyId, contentObjectHash);
    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);

    assertEqualsContract(ccnxLink_Equals, x, y, z, unequal1, unequal2, unequal3);

    ccnxLink_Release(&x);
    ccnxLink_Release(&y);
    ccnxLink_Release(&z);
    ccnxLink_Release(&unequal1);
    ccnxLink_Release(&unequal2);
    ccnxLink_Release(&unequal3);
}

LONGBOW_TEST_CASE(Global, ccnxLink_Create_ToString)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/name");
    PARCBuffer *keyId = parcBuffer_Allocate(10);
    PARCBuffer *contentObjectHash = parcBuffer_Allocate(20);
    CCNxLink *object = ccnxLink_Create(name, keyId, contentObjectHash);

    char *string = ccnxLink_ToString(object);
    assertNotNull(string, "Expected non-null string.");
    parcMemory_Deallocate((void **) &string);

    ccnxLink_Release(&object);

    ccnxName_Release(&name);
    parcBuffer_Release(&keyId);
    parcBuffer_Release(&contentObjectHash);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_Link);
    int exitStatus = LONGBOW_TEST_MAIN(argc, argv, testRunner);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
