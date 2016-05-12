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
 * @author Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../ccnx_ContentObject.c"

#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

#include <inttypes.h>
#include <stdio.h>

typedef struct test_data {
    CCNxContentObjectInterface impl;
    CCNxName *name;
    CCNxContentObject *contentObject;
    CCNxContentObject *namelessContentObject;
} TestData;

LONGBOW_TEST_RUNNER(ccnx_ContentObject)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(EmptyImpl);
}

static TestData *
_commonSetup(void)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));

    CCNxName *name = ccnxName_CreateFromCString("ccnx:/default/testData/content");
    PARCBuffer *payload = parcBuffer_WrapCString("hello");

    data->impl = CCNxContentObjectFacadeV1_Implementation;
    data->name = name;
    data->contentObject = ccnxContentObject_CreateWithImplAndPayload(&data->impl, name, CCNxPayloadType_DATA, payload);
    data->namelessContentObject = ccnxContentObject_CreateWithImplAndPayload(&data->impl, NULL, CCNxPayloadType_DATA, payload);

    parcBuffer_Release(&payload);
    return data;
}

static void
_commonTeardown(TestData *data)
{
    if (data->contentObject) {
        ccnxContentObject_Release(&data->contentObject);
    }
    if (data->namelessContentObject) {
        ccnxContentObject_Release(&data->namelessContentObject);
    }
    if (data->name) {
        ccnxName_Release(&data->name);
    }

    parcMemory_Deallocate((void **) &data);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_ContentObject)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_ContentObject)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_Equals);
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_SetSignature);
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_GetKeyId);
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_CreateWithNameAndPayload);
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_CreateWithPayload);

    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_HasFinalChunkNumber);
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_GetSetFinalChunkNumber);

    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_GetName);
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_GetNameWithNameless);
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_GetPayload);
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_GetPayloadType);
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_AcquireRelease);

    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_HasExpiryTime);
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_SetGetExpiryTime);

    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_Equals);
    LONGBOW_RUN_TEST_CASE(Global, ccnxContentObject_Display);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    longBowTestCase_SetClipBoardData(testCase, _commonSetup());

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    _commonTeardown(longBowTestCase_GetClipBoardData(testCase));

    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDOUT_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}


LONGBOW_TEST_CASE(Global, ccnxContentObject_CreateWithNameAndPayload)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/foo/bar");
    PARCBuffer *payload = parcBuffer_Allocate(100);

    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);
    ccnxContentObject_AssertValid(contentObject);

    ccnxName_Release(&name);
    parcBuffer_Release(&payload);
    ccnxContentObject_Release(&contentObject);
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_CreateWithPayload)
{
    PARCBuffer *payload = parcBuffer_Allocate(100);

    CCNxContentObject *contentObject = ccnxContentObject_CreateWithPayload(payload);
    ccnxContentObject_AssertValid(contentObject);

    parcBuffer_Release(&payload);
    ccnxContentObject_Release(&contentObject);
}


LONGBOW_TEST_CASE(Global, ccnxContentObject_Equals)
{
    CCNxName *nameA = ccnxName_CreateFromCString("ccnx:/foo/bar/A");
    PARCBuffer *payloadA = parcBuffer_Allocate(100);

    CCNxContentObject *objectA = ccnxContentObject_CreateWithNameAndPayload(nameA, payloadA);
    ccnxContentObject_AssertValid(objectA);

    assertTrue(ccnxContentObject_Equals(objectA, objectA), "Expected same instance to be equal");

    CCNxContentObject *objectA2 = ccnxContentObject_CreateWithNameAndPayload(nameA, payloadA);
    ccnxContentObject_AssertValid(objectA2);

    assertTrue(ccnxContentObject_Equals(objectA, objectA2), "Expected ContentObject with same payload and name to be equal");

    CCNxName *nameB = ccnxName_CreateFromCString("ccnx:/foo/bar/B");
    CCNxContentObject *objectB = ccnxContentObject_CreateWithNameAndPayload(nameB, payloadA);
    ccnxContentObject_AssertValid(objectB);

    assertFalse(ccnxContentObject_Equals(objectA, objectB), "Expected ContentObject with same payload and different name");

    ccnxName_Release(&nameA);
    ccnxName_Release(&nameB);
    parcBuffer_Release(&payloadA);

    ccnxContentObject_Release(&objectA);
    ccnxContentObject_Release(&objectA2);
    ccnxContentObject_Release(&objectB);
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_AcquireRelease)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/foo/bar");
    PARCBuffer *payload = parcBuffer_Allocate(100);

    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);
    ccnxContentObject_AssertValid(contentObject);

    CCNxContentObject *reference = ccnxContentObject_Acquire(contentObject);
    assertTrue(reference == contentObject, "Expected acquired reference to be equal to original");

    ccnxName_Release(&name);
    parcBuffer_Release(&payload);

    ccnxContentObject_AssertValid(contentObject);
    ccnxContentObject_AssertValid(reference);

    ccnxContentObject_Release(&contentObject);

    assertTrue(contentObject == NULL, "Expected contentObject pointer to be null");
    ccnxContentObject_AssertValid(reference);

    ccnxContentObject_Release(&reference);

    assertTrue(reference == NULL, "Expected contentObject pointer to be null");
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_HasFinalChunkNumber)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/foo/bar");
    PARCBuffer *payload = parcBuffer_Allocate(100);

    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);
    assertFalse(ccnxContentObject_HasFinalChunkNumber(contentObject), "Expected no final chunk number");

    ccnxContentObject_SetFinalChunkNumber(contentObject, 100);
    ccnxContentObject_AssertValid(contentObject);
    assertTrue(ccnxContentObject_HasFinalChunkNumber(contentObject), "Expected HasFinalChunkNumber to return true");
    assertTrue(ccnxContentObject_GetFinalChunkNumber(contentObject) == 100, "Expected final chunk number to be 100");

    ccnxName_Release(&name);
    parcBuffer_Release(&payload);
    ccnxContentObject_Release(&contentObject);
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_GetSetFinalChunkNumber)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/foo/bar");
    PARCBuffer *payload = parcBuffer_Allocate(100);

    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);

    ccnxContentObject_SetFinalChunkNumber(contentObject, 100);
    ccnxContentObject_AssertValid(contentObject);
    assertTrue(ccnxContentObject_GetFinalChunkNumber(contentObject) == 100, "Expected final chunk number to be 100");

    ccnxContentObject_SetFinalChunkNumber(contentObject, 20010);
    ccnxContentObject_AssertValid(contentObject);
    assertTrue(ccnxContentObject_GetFinalChunkNumber(contentObject) == 20010, "Expected final chunk number to be 20010");

    ccnxName_Release(&name);
    parcBuffer_Release(&payload);
    ccnxContentObject_Release(&contentObject);
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_GetName)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/foo/bar/baz");
    PARCBuffer *payload = parcBuffer_Allocate(100);

    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);
    ccnxContentObject_AssertValid(contentObject);

    CCNxName *actual = ccnxContentObject_GetName(contentObject);

    assertTrue(actual == name, "Expected GetName() to return the original CCNxName");

    ccnxName_Release(&name);
    parcBuffer_Release(&payload);
    ccnxContentObject_Release(&contentObject);
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_GetNameWithNameless)
{
    PARCBuffer *payload = parcBuffer_Allocate(100);

    CCNxContentObject *contentObject = ccnxContentObject_CreateWithPayload(payload);
    ccnxContentObject_AssertValid(contentObject);

    CCNxName *actual = ccnxContentObject_GetName(contentObject);

    assertNull(actual, "Nameless CCNxContentObjects have no name and must therefore be null.");

    parcBuffer_Release(&payload);
    ccnxContentObject_Release(&contentObject);
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_GetPayload)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/foo/bar");
    PARCBuffer *payload = parcBuffer_Allocate(100);

    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);
    ccnxContentObject_AssertValid(contentObject);

    PARCBuffer *actual = ccnxContentObject_GetPayload(contentObject);

    assertTrue(actual == payload, "Expected GetPayload() to return the original PARCBuffer");

    ccnxName_Release(&name);
    parcBuffer_Release(&payload);
    ccnxContentObject_Release(&contentObject);
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_GetPayloadType)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/name");
    PARCBuffer *payload = parcBuffer_Allocate(100);

    CCNxPayloadType types[] = {
        CCNxPayloadType_DATA,
        CCNxPayloadType_KEY,
        CCNxPayloadType_LINK,
        CCNxPayloadType_MANIFEST,
    };


    for (int i = 0; i < sizeof(types) / sizeof(CCNxPayloadType); i++) {
        CCNxPayloadType type = types[i];
        CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, NULL);
        ccnxContentObject_SetPayload(contentObject, type, payload);

        assertTrue(ccnxContentObject_GetPayloadType(contentObject) == type, "Unexpected PayloadType");
        ccnxContentObject_Release(&contentObject);
    }

    parcBuffer_Release(&payload);
    ccnxName_Release(&name);
}


LONGBOW_TEST_CASE(Global, ccnxContentObject_SetSignature)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/hello/dolly");
    PARCBuffer *payload = parcBuffer_WrapCString("hello");

    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);

    PARCBuffer *keyId = parcBuffer_WrapCString("keyhash");
    PARCBuffer *sigbits = parcBuffer_WrapCString("siggybits");
    PARCSignature *signature = parcSignature_Create(PARCSigningAlgorithm_RSA, PARC_HASH_SHA256, parcBuffer_Flip(sigbits));

    ccnxContentObject_SetSignature(contentObject, keyId, signature, NULL);

    parcBuffer_Release(&payload);
    parcBuffer_Release(&sigbits);
    parcBuffer_Release(&keyId);
    parcSignature_Release(&signature);
    ccnxName_Release(&name);
    ccnxContentObject_Release(&contentObject);
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_GetKeyId)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/hello/dolly");
    PARCBuffer *payload = parcBuffer_WrapCString("hello");

    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);

    assertNull(ccnxContentObject_GetKeyId(contentObject), "Expect NULL for KeyId here");

    PARCBuffer *testKeyId = parcBuffer_WrapCString("keyhash");
    PARCBuffer *sigbits = parcBuffer_WrapCString("siggybits");
    PARCSignature *signature = parcSignature_Create(PARCSigningAlgorithm_RSA, PARC_HASH_SHA256, parcBuffer_Flip(sigbits));

    ccnxContentObject_SetSignature(contentObject, testKeyId, signature, NULL);

    PARCBuffer *keyId = ccnxContentObject_GetKeyId(contentObject);

    assertTrue(parcBuffer_Equals(keyId, testKeyId), "Expect key ids to match");

    parcBuffer_Release(&payload);
    parcBuffer_Release(&sigbits);
    parcBuffer_Release(&keyId);
    parcSignature_Release(&signature);
    ccnxName_Release(&name);
    ccnxContentObject_Release(&contentObject);
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_HasExpiryTime)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/hello/dolly");
    PARCBuffer *payload = parcBuffer_WrapCString("hello");

    // Use a V1 ContentObject, as V0 doesn't support ExpiryTime
    CCNxContentObject *contentObject =
        ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
                                                   name, CCNxPayloadType_DATA, payload);


    assertFalse(ccnxContentObject_HasExpiryTime(contentObject), "Expected no expiration time by default");

    parcBuffer_Release(&payload);
    ccnxName_Release(&name);
    ccnxContentObject_Release(&contentObject);
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_SetGetExpiryTime)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/hello/dolly");
    PARCBuffer *payload = parcBuffer_WrapCString("hello");

    // Use a V1 ContentObject, as V0 doesn't support ExpiryTime
    CCNxContentObject *contentObject =
        ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
                                                   name, CCNxPayloadType_DATA, payload);

    assertFalse(ccnxContentObject_HasExpiryTime(contentObject), "Expected no expiration time by default");

    uint64_t expiryTime = 1010101ULL;
    ccnxContentObject_SetExpiryTime(contentObject, expiryTime);

    assertTrue(ccnxContentObject_HasExpiryTime(contentObject), "Expected the expiryTime to be set");
    uint64_t retrievedTime = ccnxContentObject_GetExpiryTime(contentObject);
    assertTrue(expiryTime == retrievedTime, "Did not retrieve expected expiryTime from ContentObject");

    parcBuffer_Release(&payload);
    ccnxName_Release(&name);
    ccnxContentObject_Release(&contentObject);
}

LONGBOW_TEST_CASE_EXPECTS(Global, ccnxContentObject_GetExpiryTimeWithNoExpiryTime, .event = &LongBowTrapUnexpectedStateEvent)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/hello/dolly");
    PARCBuffer *payload = parcBuffer_WrapCString("hello");

    // Use a V1 ContentObject, as V0 doesn't support ExpiryTime
    CCNxContentObject *contentObject =
        ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
                                                   name, CCNxPayloadType_DATA, payload);

    // This should throw.
    uint64_t retrievedTime = ccnxContentObject_GetExpiryTime(contentObject);
    trapUnexpectedState("Expected to have thrown an exception when calling GetExpiryTime(), which returned %" PRIu64, retrievedTime);
    parcBuffer_Release(&payload);
    ccnxName_Release(&name);
    ccnxContentObject_Release(&contentObject);
}

LONGBOW_TEST_CASE(Global, ccnxContentObject_Display)
{
    CCNxName *name = ccnxName_CreateFromCString("ccnx:/hello/dolly");
    PARCBuffer *payload = parcBuffer_WrapCString("hello");

    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);

    ccnxContentObject_Display(contentObject, 0);

    parcBuffer_Release(&payload);
    ccnxName_Release(&name);
    ccnxContentObject_Release(&contentObject);
}

///////////////////////////////////////////////////////////////////////////
// Empty Implementation Tests
///////////////////////////////////////////////////////////////////////////

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_GetPayloadType, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.getPayloadType = NULL;

    CCNxPayloadType type = ccnxContentObject_GetPayloadType(data->contentObject);
    printf("We shouldn't get here. Payload = %d", type);
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_GetPayload, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.getPayload = NULL;

    PARCBuffer *payload = ccnxContentObject_GetPayload(data->contentObject);
    printf("We shouldn't get here. Payload = %p", (void *) payload);
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_SetPayload, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.setPayload = NULL;

    ccnxContentObject_SetPayload(data->contentObject, CCNxPayloadType_DATA, NULL);
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_GetName, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.getName = NULL;

    CCNxName *name = ccnxContentObject_GetName(data->contentObject);
    printf("We shouldn't get here. Name = %p", (void *) name);
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_SetFinalChunkNumber, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.setFinalChunkNumber = NULL;

    ccnxContentObject_SetFinalChunkNumber(data->contentObject, 100);
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_GetFinalChunkNumber, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.getFinalChunkNumber = NULL;

    ccnxContentObject_SetFinalChunkNumber(data->contentObject, 100);
    ccnxContentObject_GetFinalChunkNumber(data->contentObject);
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_GetFinalChunkNumberNoHas, .event = &LongBowTrapUnexpectedStateEvent)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.getFinalChunkNumber = NULL;

    ccnxContentObject_GetFinalChunkNumber(data->contentObject);
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_HasFinalChunkNumber, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.hasFinalChunkNumber = NULL;

    if (ccnxContentObject_HasFinalChunkNumber(data->contentObject)) {
        printf("Shouldn't get here");
    }
}

LONGBOW_TEST_CASE(EmptyImpl, empty_HasExpiryTime)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.hasExpiryTime = NULL;

    assertFalse(ccnxContentObject_HasExpiryTime(data->contentObject), "If no expiry time implementation, return false.");
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_SetExpiryTime, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.setExpiryTime = NULL;

    ccnxContentObject_SetExpiryTime(data->contentObject, 100);
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_GetExpiryTime, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.getExpiryTime = NULL;

    ccnxContentObject_SetExpiryTime(data->contentObject, 100);
    uint64_t expiryTime = ccnxContentObject_GetExpiryTime(data->contentObject);
    printf("We shouldn't get here, with expiryTime = %" PRIu64, expiryTime);
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_GetExpiryTimeNoHas, .event = &LongBowTrapUnexpectedStateEvent)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.getExpiryTime = NULL;

    uint64_t expiryTime = ccnxContentObject_GetExpiryTime(data->contentObject);
    printf("We shouldn't get here, with expiryTime = %" PRIu64, expiryTime);
}

LONGBOW_TEST_CASE(EmptyImpl, empty_Display)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.display = NULL;

    ccnxContentObject_Display(data->contentObject, 2);
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_ToString, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.toString = NULL;

    const char *expectedString = ccnxContentObject_ToString(data->contentObject);
    if (expectedString != NULL) {
        parcMemory_Deallocate((void **) &expectedString);
    }
}

LONGBOW_TEST_CASE_EXPECTS(EmptyImpl, empty_Equals, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    data->impl.equals = NULL;

    if (ccnxContentObject_Equals(data->contentObject, data->contentObject)) {
        printf("Shouldn't get here");
    }
}

LONGBOW_TEST_FIXTURE(EmptyImpl)
{
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_Display);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_HasExpiryTime);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_GetExpiryTime);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_GetExpiryTimeNoHas);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_SetExpiryTime);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_SetFinalChunkNumber);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_GetFinalChunkNumber);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_GetFinalChunkNumberNoHas);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_HasFinalChunkNumber);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_GetPayload);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_GetPayloadType);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_SetPayload);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_GetName);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_ToString);
    LONGBOW_RUN_TEST_CASE(EmptyImpl, empty_Equals);
}

LONGBOW_TEST_FIXTURE_SETUP(EmptyImpl)
{
    longBowTestCase_SetClipBoardData(testCase, _commonSetup());

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(EmptyImpl)
{
    _commonTeardown(longBowTestCase_GetClipBoardData(testCase));

    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDOUT_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}


int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_ContentObject);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
