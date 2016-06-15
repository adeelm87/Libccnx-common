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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../ccnx_ContentObjectFacadeV1.c"
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.h>
#include <inttypes.h>

typedef struct test_data {
    CCNxName *name;
    CCNxPayloadType payloadType;
    PARCBuffer *payload;

    // a V1 dictionary but with no values set
    CCNxTlvDictionary *contentObjectV1Empty;

    // a V1 nameless Content Object
    CCNxTlvDictionary *contentObjectV1Nameless;

    // a valid V1 Content Object
    CCNxTlvDictionary *contentObjectV1;
    CCNxTlvDictionary *contentObjectVFF;
} TestData;

static TestData *
_commonSetup(void)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(TestData));

    data->name = ccnxName_CreateFromCString("lci:/foo/bar");
    data->payloadType = CCNxPayloadType_DATA;

    data->payload = parcBuffer_Flip(parcBuffer_PutArray(parcBuffer_Allocate(20), 7, (uint8_t *) "payload"));

    data->contentObjectV1Empty = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    data->contentObjectV1 = _ccnxContentObjectFacadeV1_CreateWithNameAndPayload(data->name, data->payloadType,
                                                                                data->payload);
    data->contentObjectV1Nameless = _ccnxContentObjectFacadeV1_CreateWithPayload(data->payloadType, data->payload);

    data->contentObjectVFF = ccnxTlvDictionary_Create(CCNxCodecSchemaV1TlvDictionary_MessageFastArray_END, CCNxCodecSchemaV1TlvDictionary_Lists_END);
    ccnxTlvDictionary_SetMessageType_ContentObject(data->contentObjectVFF, 0xFF);

    return data;
}

static void
_commonTeardown(TestData *data)
{
    ccnxName_Release(&data->name);
    parcBuffer_Release(&data->payload);

    ccnxTlvDictionary_Release(&data->contentObjectV1Empty);
    ccnxTlvDictionary_Release(&data->contentObjectV1);
    ccnxTlvDictionary_Release(&data->contentObjectV1Nameless);
    ccnxTlvDictionary_Release(&data->contentObjectVFF);
    parcMemory_Deallocate((void **) &data);
}


LONGBOW_TEST_RUNNER(ccnx_ContentObjectFacade)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(ImplInterface);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_ContentObjectFacade)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_ContentObjectFacade)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ======================================================================================

LONGBOW_TEST_FIXTURE(ImplInterface)
{
    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_Init);

    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetName);
    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetPayload);
    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_SetPayload);
    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_SetPayload_Link);
    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetPayloadType);
    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetPayloadType_Unset);
    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetExpiryTime);
    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_SetExpiryTime);

    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_SetSignature);
    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetKeyId);

    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_ToString);
    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_Display);
    LONGBOW_RUN_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_Equals);
}

LONGBOW_TEST_FIXTURE_SETUP(ImplInterface)
{
    longBowTestCase_SetClipBoardData(testCase, _commonSetup());
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(ImplInterface)
{
    _commonTeardown(longBowTestCase_GetClipBoardData(testCase));

    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

// ======================================================================================

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_Init)
{
    CCNxContentObjectInterface *impl = &CCNxContentObjectFacadeV1_Implementation;
    assertNotNull(impl->createWithNameAndPayload, "Expected CreateWithNameAndPayload to be set");
    assertNotNull(impl->createWithPayload, "Expected CreateWithPayload to be set");

    assertNotNull(impl->getName, "Expected GetName to be set");
    assertNotNull(impl->setSignature, "Expected SetSignature to be set");
    assertNotNull(impl->getKeyId, "Expected GetKeyID to be set");
    assertNotNull(impl->getPayload, "Expected GetPayload to be set");
    assertNotNull(impl->getPayloadType, "Expected GetPayloadType to be set");

    assertNotNull(impl->hasFinalChunkNumber, "Expected HasFinalChunkNumber to be set");
    assertNotNull(impl->getFinalChunkNumber, "Expected GetFinalChunkNumber to be set");
    assertNotNull(impl->setFinalChunkNumber, "Expected SetFinalChunkNumber to be set");

    assertNotNull(impl->hasExpiryTime, "Expected HasExpiryTime to be set");
    assertNotNull(impl->getExpiryTime, "Expected GetExpiryTime to be set");
    assertNotNull(impl->setExpiryTime, "Expected SetExpiryTime to be set");

    assertNotNull(impl->toString, "Expected ToString to be set");
    assertNotNull(impl->display, "Expected Display to be set");
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetName)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxName *test = _ccnxContentObjectFacadeV1_GetName(data->contentObjectV1);
    assertTrue(ccnxName_Equals(test, data->name), "Names do not match")
    {
        ccnxName_Display(test, 0);
        ccnxName_Display(data->name, 0);
    }
    test = _ccnxContentObjectFacadeV1_GetName(data->contentObjectV1Nameless);
    assertNull(test, "A nameless Content Object has no name.");
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_SetPayload)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    PARCBuffer *newPayload = parcBuffer_WrapCString("this is a new payload");

    // Shouldn't be able to set it on an already initialized ContentObject.
    // (Though this may change...)
    assertFalse(_ccnxContentObjectFacadeV1_SetPayload(data->contentObjectV1, CCNxPayloadType_DATA, newPayload),
                "Expected to not be able to re-assign a payload on an already initialized ContentObject");

    CCNxTlvDictionary *contentObject =
            _ccnxContentObjectFacadeV1_CreateWithNameAndPayload(data->name,
                                                                CCNxPayloadType_DATA,
                                                                NULL);


    bool status = _ccnxContentObjectFacadeV1_SetPayload(contentObject, CCNxPayloadType_KEY, newPayload);
    assertTrue(status, "Expected to be able to set the buffer");

    PARCBuffer *testPayload = _ccnxContentObjectFacadeV1_GetPayload(contentObject);
    assertTrue(parcBuffer_Equals(newPayload, testPayload), "payloads do not match")
    {
        parcBuffer_Display(newPayload, 0);
        parcBuffer_Display(data->payload, 0);
    }

    CCNxPayloadType testType = _ccnxContentObjectFacadeV1_GetPayloadType(contentObject);
    assertTrue(testType == CCNxPayloadType_KEY, "Expected type KEY");

    parcBuffer_Release(&newPayload);
    ccnxTlvDictionary_Release(&contentObject);
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_SetPayload_Link)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    PARCBuffer *payload = parcBuffer_WrapCString("this is a payload");

    CCNxTlvDictionary *contentObject =
            _ccnxContentObjectFacadeV1_CreateWithNameAndPayload(data->name,
                                                                CCNxPayloadType_LINK,
                                                                payload);

    PARCBuffer *testPayload = _ccnxContentObjectFacadeV1_GetPayload(contentObject);
    assertTrue(parcBuffer_Equals(payload, testPayload), "payloads do not match")
    {
        parcBuffer_Display(payload, 0);
        parcBuffer_Display(testPayload, 0);
    }

    CCNxPayloadType testType = _ccnxContentObjectFacadeV1_GetPayloadType(contentObject);
    assertTrue(testType == CCNxPayloadType_LINK, "Expected type LINK");

    parcBuffer_Release(&payload);
    ccnxTlvDictionary_Release(&contentObject);
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetPayload)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    PARCBuffer *test = _ccnxContentObjectFacadeV1_GetPayload(data->contentObjectV1);
    assertTrue(parcBuffer_Equals(test, data->payload), "payloads do not match")
    {
        parcBuffer_Display(test, 0);
        parcBuffer_Display(data->payload, 0);
    }
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetPayloadType)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxPayloadType test = _ccnxContentObjectFacadeV1_GetPayloadType(data->contentObjectV1);
    assertTrue(test == data->payloadType, "payloads do not match, got %d expected %d", test, data->payloadType)
    {
        ccnxTlvDictionary_Display(data->contentObjectV1, 0);
    }
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetPayloadType_Unset)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxPayloadType test = _ccnxContentObjectFacadeV1_GetPayloadType(data->contentObjectV1Empty);
    assertTrue(test == CCNxPayloadType_DATA, "payloads do not match, got %d expected %d", test, CCNxPayloadType_DATA)
    {
        ccnxTlvDictionary_Display(data->contentObjectV1, 0);
    }
}

LONGBOW_TEST_CASE_EXPECTS(ImplInterface, ccnxContentObjectFacadeV1_ToString, .event = &LongBowTrapNotImplemented)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _ccnxContentObjectFacadeV1_ToString(data->contentObjectV1);
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_Display)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    _ccnxContentObjectFacadeV1_Display(data->contentObjectV1, 0);
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_Equals)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    assertTrue(_ccnxContentObjectFacadeV1_Equals(data->contentObjectV1, data->contentObjectV1),
               "Expected equals to be true");
    assertFalse(_ccnxContentObjectFacadeV1_Equals(data->contentObjectV1, data->contentObjectV1Empty),
                "Expected equals to be false");
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetExpiryTime)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    uint64_t truth = 12345;
    ccnxTlvDictionary_PutInteger(data->contentObjectV1, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_EXPIRY_TIME, truth);

    assertTrue(_ccnxContentObjectFacadeV1_HasExpiryTime(data->contentObjectV1), "Expected HasExpiryTime() to return true");
    uint64_t expiryTime = _ccnxContentObjectFacadeV1_GetExpiryTime(data->contentObjectV1);
    assertTrue(truth == expiryTime, "times do not match, expected %" PRIu64 ", got %" PRIu64, truth, expiryTime);
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_SetExpiryTime)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    uint64_t truth = 12345;
    _ccnxContentObjectFacadeV1_SetExpiryTime(data->contentObjectV1, truth);
    uint64_t expiryTime = _ccnxContentObjectFacadeV1_GetExpiryTime(data->contentObjectV1);
    assertTrue(truth == expiryTime, "times do not match, expected %" PRIu64 ", got %" PRIu64, truth, expiryTime);
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_SetSignature)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/baz");
    PARCBuffer *payload = parcBuffer_Allocate(100);

    CCNxTlvDictionary *contentObject = _ccnxContentObjectFacadeV1_CreateWithNameAndPayload(name, CCNxPayloadType_DATA,
                                                                                           payload);

    PARCBuffer *keyId = parcBuffer_WrapCString("keyhash");
    char *rawsig = "siggybits";
    PARCBuffer *sigbits = parcBuffer_CreateFromArray((void *) rawsig, strlen(rawsig));

    PARCSignature *signature = parcSignature_Create(PARCSigningAlgorithm_RSA, PARCCryptoHashType_SHA256, parcBuffer_Flip(sigbits));
    parcBuffer_Release(&sigbits);

    assertTrue(_ccnxContentObjectFacadeV1_SetSignature(contentObject, keyId, signature, NULL),
               "Expected to be able to set the signature");

    ccnxName_Release(&name);
    parcBuffer_Release(&payload);
    parcSignature_Release(&signature);
    parcBuffer_Release(&keyId);
    ccnxTlvDictionary_Release(&contentObject);
}

LONGBOW_TEST_CASE(ImplInterface, ccnxContentObjectFacadeV1_GetKeyId)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/baz");
    PARCBuffer *payload = parcBuffer_Allocate(100);

    CCNxTlvDictionary *contentObject = _ccnxContentObjectFacadeV1_CreateWithNameAndPayload(name, CCNxPayloadType_DATA,
                                                                                           payload);

    assertNull(_ccnxContentObjectFacadeV1_GetKeyId(contentObject), "Expect key ID to be NULL");

    PARCBuffer *testKeyId = parcBuffer_WrapCString("keyhash");
    char *rawsig = "siggybits";
    PARCBuffer *sigbits = parcBuffer_CreateFromArray((void *) rawsig, strlen(rawsig));

    PARCSignature *signature = parcSignature_Create(PARCSigningAlgorithm_RSA, PARCCryptoHashType_SHA256, parcBuffer_Flip(sigbits));
    parcBuffer_Release(&sigbits);

    assertTrue(_ccnxContentObjectFacadeV1_SetSignature(contentObject, testKeyId, signature, NULL),
               "Expected to be able to set the signature");

    PARCBuffer *keyId = _ccnxContentObjectFacadeV1_GetKeyId(contentObject);

    assertTrue(parcBuffer_Equals(keyId, testKeyId), "Expect key ID's to be the same");

    ccnxName_Release(&name);
    parcBuffer_Release(&payload);
    parcSignature_Release(&signature);
    parcBuffer_Release(&keyId);
    ccnxTlvDictionary_Release(&contentObject);
}

// ======================================================================================

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_ContentObjectFacade);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
