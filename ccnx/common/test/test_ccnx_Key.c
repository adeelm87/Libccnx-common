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
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>
#include "../ccnx_Key.c"
#include <LongBow/unit-test.h>

#include <parc/algol/parc_SafeMemory.h>

LONGBOW_TEST_RUNNER(ccnx_Key)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

LONGBOW_TEST_RUNNER_SETUP(ccnx_Key)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_Key)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxKey_CreateRelease);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKey_CreateFromHexString);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKey_FromByteBuffer);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKey_ToString);
}

typedef struct {
    char *hexString;
} TestData;

static TestData*
commonSetup()
{
    char *hexString = "30819F300D06092A864886F70D010101050003818D0030818902818100A826C09E01FF4970428213C96312B46050514FD5F87E670A4784C75D8B23CD073B1CBEF328E538584E442A769DF77299192BCF3603F50F14C5664994250E5C24DF47B86EA5C7CA99B3584E9A63BC5993569FF3612C71AD46A088CDC7346B9BE021D4CA1764CF5434F993E6120363C551E2979BDB3F0345B4994BCED9CB260EEB0203010001";

    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(TestData));
    data->hexString = parcMemory_StringDuplicate(hexString, strlen(hexString));
    return data;
}

static void
commonTearDown(TestData *data)
{
    parcMemory_Deallocate((void **) &(data->hexString));
    parcMemory_Deallocate((void **) &data);
}


LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    TestData *data = commonSetup();
    longBowTestCase_SetClipBoardData(testCase, data);
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    commonTearDown(data);

    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}


LONGBOW_TEST_CASE(Global, ccnxKey_FromByteBuffer)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    PARCBuffer *hexBuf = parcBuffer_ParseHexString(data->hexString);
    CCNxKey *key = ccnxKey_Create(hexBuf);
    parcBuffer_Release(&hexBuf);
    ccnxKey_Release(&key);
}

LONGBOW_TEST_CASE(Global, ccnxKey_CreateRelease)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxKey *key = ccnxKey_CreateFromHexString(data->hexString);
    ccnxKey_AssertValid(key);

    char *string = ccnxKey_ToString(key);

    ccnxKey_Release(&key);
    assertNull(key, "Key was not nulled out after Release()");

    parcMemory_Deallocate((void **) &string);
}

LONGBOW_TEST_CASE(Global, ccnxKey_CreateFromHexString)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxKey *key = ccnxKey_CreateFromHexString(data->hexString);
    ccnxKey_AssertValid(key);

    char *string = ccnxKey_ToHexString(key);

    assertTrue(strcasecmp(data->hexString, string) == 0,
               "Expected '%s' actual '%s'", data->hexString, string);

    ccnxKey_Release(&key);
    assertNull(key, "Key was not nulled out after Release()");

    parcMemory_Deallocate((void **) &string);
}

LONGBOW_TEST_CASE(Global, ccnxKey_ToString)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxKey *key = ccnxKey_CreateFromHexString(data->hexString);
    ccnxKey_AssertValid(key);

    char *string = ccnxKey_ToString(key);

    ccnxKey_Release(&key);
    assertNull(key, "Key was not nulled out after Release()");

    parcMemory_Deallocate((void **) &string);
}

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
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_Key);
    exit(longBowMain(argc, argv, testRunner, NULL));
}
