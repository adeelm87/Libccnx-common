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
#include "../ccnx_NameSegmentNumber.c"

#include <LongBow/unit-test.h>

#include <stdio.h>
#include <inttypes.h>

#include <parc/algol/parc_SafeMemory.h>

LONGBOW_TEST_RUNNER(test_ccnx_NameSegmentNumber)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(test_ccnx_NameSegmentNumber)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(test_ccnx_NameSegmentNumber)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_Create64bits);
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_Create56bits);
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_Create48bits);
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_Create40bits);
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_Create32bits);
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_Create24bits);
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_Create16bits);
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_Create8bits);
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_BorderCases);
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_IsValid);
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_IsValid_False);
    LONGBOW_RUN_TEST_CASE(Global, ccnxNameSegmentNumber_AssertValid);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDOUT_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_Create64bits)
{
    uint64_t expected = 0x123456789ABCDEF0;
    CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);

    uint64_t actual = ccnxNameSegmentNumber_Value(segment);

    assertTrue(expected == actual, "Expected 0x%" PRIX64 " actual 0x%" PRIX64 "", expected, actual);

    ccnxNameSegment_Release(&segment);
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_Create56bits)
{
    uint64_t expected = 0x123456789ABCDE;
    CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);

    uint64_t actual = ccnxNameSegmentNumber_Value(segment);

    assertTrue(expected == actual, "Expected 0x%" PRIX64 " actual 0x%" PRIX64 "", expected, actual);
    ccnxNameSegment_Release(&segment);
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_Create48bits)
{
    uint64_t expected = 0x123456789ABC;
    CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);

    uint64_t actual = ccnxNameSegmentNumber_Value(segment);

    assertTrue(expected == actual, "Expected 0x%" PRIX64 " actual 0x%" PRIX64 "", expected, actual);
    ccnxNameSegment_Release(&segment);
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_Create40bits)
{
    uint64_t expected = 0x123456789A;
    CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);

    uint64_t actual = ccnxNameSegmentNumber_Value(segment);

    assertTrue(expected == actual, "Expected 0x%" PRIX64 " actual 0x%" PRIX64 "", expected, actual);
    ccnxNameSegment_Release(&segment);
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_Create32bits)
{
    uint64_t expected = 0x12345678;
    CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);

    uint64_t actual = ccnxNameSegmentNumber_Value(segment);

    assertTrue(expected == actual, "Expected 0x%" PRIX64 " actual 0x%" PRIX64 "", expected, actual);
    ccnxNameSegment_Release(&segment);
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_Create24bits)
{
    uint64_t expected = 0x123456;
    CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);

    uint64_t actual = ccnxNameSegmentNumber_Value(segment);

    assertTrue(expected == actual, "Expected 0x%" PRIX64 " actual 0x%" PRIX64 "", expected, actual);
    ccnxNameSegment_Release(&segment);
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_Create16bits)
{
    uint64_t expected = 0x1234;
    CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);

    uint64_t actual = ccnxNameSegmentNumber_Value(segment);

    assertTrue(expected == actual, "Expected 0x%" PRIX64 " actual 0x%" PRIX64 "", expected, actual);
    ccnxNameSegment_Release(&segment);
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_Create8bits)
{
    uint64_t expected = 0x12;
    CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);

    uint64_t actual = ccnxNameSegmentNumber_Value(segment);

    assertTrue(expected == actual, "Expected 0x%" PRIX64 " actual 0x%" PRIX64 "", expected, actual);
    ccnxNameSegment_Release(&segment);
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_BorderCases)
{
    struct test_struct {
        uint64_t value;
        size_t length;
        uint8_t *encoded;
    } test_vector[] = {
        { .value = 0x0000000000000000ULL, .length = 1, .encoded = (uint8_t[1]) { 0x00 } },
        { .value = 0x0000000000000001ULL, .length = 1, .encoded = (uint8_t[1]) { 0x01 } },
        { .value = 0x00000000000000FFULL, .length = 1, .encoded = (uint8_t[1]) { 0xFF } },
        { .value = 0x0000000000000100ULL, .length = 2, .encoded = (uint8_t[2]) { 0x01, 0x00} },
        { .value = 0x0100000000000100ULL, .length = 8, .encoded = (uint8_t[8]) { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00} },
        { .value = 0x8000000000000100ULL, .length = 8, .encoded = (uint8_t[8]) { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00} },
        { .value = 0xFFFFFFFFFFFFFFFFULL, .length = 8, .encoded = (uint8_t[8]) { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} },
        { .value = 0,                     .length = 0, .encoded = NULL }
    };

    for (int i = 0; test_vector[i].encoded != NULL; i++) {
        PARCBuffer *buffer =
            parcBuffer_Wrap(test_vector[i].encoded, test_vector[i].length, 0, test_vector[i].length);
        CCNxNameSegment *expected = ccnxNameSegment_CreateTypeValue(CCNxNameLabelType_NAME, buffer);

        CCNxNameSegment *actual = ccnxNameSegmentNumber_Create(CCNxNameLabelType_NAME, test_vector[i].value);

        assertTrue(ccnxNameSegment_Equals(expected, actual),
                   "Buffers do not match: test_vector[%d] value %" PRIX64 " Expected %" PRIX64 " actual %" PRIX64 "",
                   i,
                   test_vector[i].value,
                   ccnxNameSegmentNumber_Value(expected),
                   ccnxNameSegmentNumber_Value(actual));

        ccnxNameSegment_Release(&expected);
        ccnxNameSegment_Release(&actual);
        parcBuffer_Release(&buffer);
    }
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_AssertValid)
{
    uint64_t expected = 0x12;
    CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);

    ccnxNameSegmentNumber_AssertValid(segment);
    ccnxNameSegment_Release(&segment);
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_IsValid)
{
    uint64_t expected = 0x12;
    CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);

    assertTrue(ccnxNameSegmentNumber_IsValid(segment), "Expected the CCNxNameSegment to be valid.");
    ccnxNameSegment_Release(&segment);
}

LONGBOW_TEST_CASE(Global, ccnxNameSegmentNumber_IsValid_False)
{
    uint64_t expected = 0x12;
    CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);

    PARCBuffer *value = ccnxNameSegment_GetValue(segment);
    parcBuffer_SetPosition(value, parcBuffer_Limit(value)); // Wreck the buffer by making it zero length.

    assertFalse(ccnxNameSegmentNumber_IsValid(segment), "Expected the CCNxNameSegment to be valid.");
    ccnxNameSegment_Release(&segment);
}

int
main(int argc, char *argv[argc])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(test_ccnx_NameSegmentNumber);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
