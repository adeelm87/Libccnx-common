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
#include "../ccnx_InterestReturn.c"
#include "../ccnx_Interest.h"
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

#include <ccnx/common/ccnx_Interest.h>
#include <ccnx/common/ccnx_InterestReturn.h>
#include <ccnx/common/ccnx_PayloadType.h>

typedef struct test_data {
    CCNxTlvDictionary *interest;
    CCNxInterestInterface *interestImpl;

    CCNxName *name;
    PARCBuffer *keyid;
    PARCBuffer *contentObjectHash;
    PARCBuffer *payload;

    // allocated data
    uint8_t keyidArray[32];
    uint8_t contentObjectHashArray[32];
    uint8_t payloadArray[128];

    uint32_t lifetime;
    uint32_t hoplimit;
    CCNxPayloadType payloadType;
} TestData;


LONGBOW_TEST_RUNNER(ccnx_InterestReturnV1)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_InterestReturnV1)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_InterestReturnV1)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ========================================================================================

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxInterestReturnV1_Create);
    LONGBOW_RUN_TEST_CASE(Global, ccnxInterestReturnV1_GetReturnCode);
    LONGBOW_RUN_TEST_CASE(Global, ccnxInterestReturnV1_AcquireRelease);
    LONGBOW_RUN_TEST_CASE(Global, ccnxInterestReturnV1_Equals);
    LONGBOW_RUN_TEST_CASE(Global, ccnxInterestReturnV1_NotEquals);
    LONGBOW_RUN_TEST_CASE(Global, ccnxInterestReturnV1_ToString);
    LONGBOW_RUN_TEST_CASE(Global, ccnxInterestReturnV1_GetInterestFields);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%lu) returned NULL", sizeof(TestData));
    data->name = ccnxName_CreateFromCString("lci:/once/upon/a/time");

    for (int i = 0; i < 32; i++) {
        data->keyidArray[i] = i * 7;
        data->contentObjectHashArray[i] = i * 11;
    }

    for (int i = 0; i < 128; i++) {
        data->payloadArray[i] = i * 13;
    }

    data->interestImpl = &CCNxInterestFacadeV1_Implementation;
    data->keyid = parcBuffer_Wrap(data->keyidArray, 32, 0, 32);
    data->contentObjectHash = parcBuffer_Wrap(data->contentObjectHashArray, 32, 0, 32);
    data->payloadType = CCNxPayloadType_DATA;
    data->payload = parcBuffer_Wrap(data->payloadArray, 128, 0, 128);

    data->lifetime = 900;
    data->hoplimit = 77;

    data->interest = ccnxInterest_CreateWithImpl(data->interestImpl,
                                                 data->name,
                                                 data->lifetime,
                                                 data->keyid,
                                                 data->contentObjectHash,
                                                 data->hoplimit);

    ccnxInterest_SetPayload(data->interest, data->payload);

    longBowTestCase_SetClipBoardData(testCase, data);

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxName_Release(&data->name);
    parcBuffer_Release(&data->keyid);
    parcBuffer_Release(&data->contentObjectHash);
    parcBuffer_Release(&data->payload);
    ccnxTlvDictionary_Release(&data->interest);

    parcMemory_Deallocate((void **) &data);

    if (parcSafeMemory_ReportAllocation(STDOUT_FILENO) != 0) {
        printf("('%s' leaks memory by %d (allocs - frees)) ", longBowTestCase_GetName(testCase), parcMemory_Outstanding());
        return LONGBOW_STATUS_TEARDOWN_FAILED;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxInterestReturnV1_Create)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxInterestReturn *interestReturn =
        ccnxInterestReturn_Create(data->interest, CCNxInterestReturn_ReturnCode_NoRoute);

    ccnxInterestReturn_AssertValid(interestReturn);

    //assertTrue(ccnxInterestReturn_IsValid(interestReturn), "InterestReturn is not valid");
    ccnxTlvDictionary_Release(&interestReturn);
}


LONGBOW_TEST_CASE(Global, ccnxInterestReturnV1_AcquireRelease)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxInterestReturn *interestReturn =
        ccnxInterestReturn_Create(data->interest, CCNxInterestReturn_ReturnCode_NoRoute);

    assertNotNull(interestReturn, "Expect non-NULL interestReturn");

    CCNxInterestReturn *testInterestReturn = ccnxInterestReturn_Acquire(interestReturn);
    assertNotNull(testInterestReturn, "Expected a non-NULL testInterestReturn");

    ccnxInterestReturn_Release(&testInterestReturn);
    assertNull(testInterestReturn, "Expected a NULL testInterestReturn");

    ccnxInterestReturn_Release(&interestReturn);
    assertNull(interestReturn, "Expected a NULL testInterestReturn");
}

LONGBOW_TEST_CASE(Global, ccnxInterestReturnV1_Equals)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxInterestReturn *interestReturn =
        ccnxInterestReturn_Create(data->interest, CCNxInterestReturn_ReturnCode_NoRoute);
    assertTrue(ccnxInterestReturn_Equals(interestReturn, interestReturn), "Expect same interestReturn pointers to be equal");

    CCNxInterestReturn *acquiredIR = ccnxInterestReturn_Acquire(interestReturn);
    assertTrue(ccnxInterestReturn_Equals(interestReturn, acquiredIR), "Expect acquired interestReturn to be equal to original");
    ccnxInterestReturn_Release(&acquiredIR);

    CCNxInterestReturn *identicalIR =
        ccnxInterestReturn_Create(data->interest, CCNxInterestReturn_ReturnCode_NoRoute);
    assertTrue(ccnxInterestReturn_Equals(interestReturn, identicalIR), "Expect identical interestReturn to be equal to original");

    ccnxInterestReturn_Release(&identicalIR);
    ccnxInterestReturn_Release(&interestReturn);
}

LONGBOW_TEST_CASE(Global, ccnxInterestReturnV1_NotEquals)
{
    assertFalse(ccnxInterestReturn_Equals(NULL, NULL), "Expect two NULL interests to not be equal");

    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxInterestReturn *interestReturn =
        ccnxInterestReturn_Create(data->interest, CCNxInterestReturn_ReturnCode_NoRoute);
    assertFalse(ccnxInterestReturn_Equals(interestReturn, NULL), "Expect a NULL interest to not be equal");

    CCNxInterestReturn *testIR =
        ccnxInterestReturn_Create(data->interest, CCNxInterestReturn_ReturnCode_MTUTooLarge);
    assertFalse(ccnxInterestReturn_Equals(interestReturn, testIR), "Expect interestReturn's with different return codes to be !=");

    ccnxInterestReturn_Release(&testIR);
    ccnxInterestReturn_Release(&interestReturn);
}

LONGBOW_TEST_CASE(Global, ccnxInterestReturnV1_ToString)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxInterestReturn *interestReturn =
        ccnxInterestReturn_Create(data->interest, CCNxInterestReturn_ReturnCode_NoRoute);

    const char *string = ccnxInterestReturn_ToString(interestReturn);
    assertNotNull(string, "Expected non-null result from ccnxInterestReturn_ToString.");

    parcMemory_Deallocate((void **) &string);

    ccnxInterestReturn_Release(&interestReturn);
}

LONGBOW_TEST_CASE(Global, ccnxInterestReturnV1_GetReturnCode)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxInterestReturn *interestReturn =
        ccnxInterestReturn_Create(data->interest, CCNxInterestReturn_ReturnCode_NoRoute);

    CCNxInterestReturn_ReturnCode code =
        ccnxInterestReturn_GetReturnCode(interestReturn);

    assertTrue((CCNxInterestReturn_ReturnCode_NoRoute == code), "InterestReturn wrong Return Code");
    ccnxInterestReturn_Release(&interestReturn);
}

LONGBOW_TEST_CASE(Global, ccnxInterestReturnV1_GetInterestFields)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxInterestReturn *interestReturn =
        ccnxInterestReturn_Create(data->interest, CCNxInterestReturn_ReturnCode_NoRoute);

    CCNxName *name = ccnxInterest_GetName(interestReturn);
    assertTrue(ccnxName_Equals(name, data->name), "Names do not match")
    {
        printf("\ngot     : \n"); ccnxName_Display(name, 3);
        printf("\nexpected: \n"); ccnxName_Display(data->name, 3);
    }

    uint32_t hopLimit = ccnxInterest_GetHopLimit(interestReturn);
    assertTrue(hopLimit == data->hoplimit, "Wrong hoplimit: got %u expected %u", hopLimit, data->hoplimit);

    uint32_t lifetime = (uint32_t) ccnxInterest_GetLifetime(interestReturn);
    assertTrue(lifetime == data->lifetime, "Wrong lifetime: got %u expected %u", lifetime, data->lifetime);

    PARCBuffer *buff = ccnxInterest_GetKeyIdRestriction(interestReturn);
    assertTrue(parcBuffer_Equals(buff, data->keyid), "KeyIDs do not match")
    {
        printf("\ngot     : \n"); parcBuffer_Display(buff, 3);
        printf("\nexpected: \n"); parcBuffer_Display(data->keyid, 3);
    }

    buff = ccnxInterest_GetPayload(interestReturn);
    assertTrue(parcBuffer_Equals(buff, data->payload), "Payloads do not match")
    {
        printf("\ngot     : \n"); parcBuffer_Display(buff, 3);
        printf("\nexpected: \n"); parcBuffer_Display(data->payload, 3);
    }

    ccnxInterestReturn_Release(&interestReturn);
}


int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_InterestReturnV1);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
