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
#include <ccnx/common/ccnx_TimeStamp.c>

#include <inttypes.h>
#include <time.h>

#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>


LONGBOW_TEST_RUNNER(ccnx_TimeStamp)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

LONGBOW_TEST_RUNNER_SETUP(ccnx_TimeStamp)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_TimeStamp)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxTimeStamp_CreateFromCurrentUTCTime);
    LONGBOW_RUN_TEST_CASE(Global, ccnxTimeStamp_CreateFromTimespec);
    LONGBOW_RUN_TEST_CASE(Global, ccnxTimeStamp_CreateFromMillisecondsSinceEpoch);
    LONGBOW_RUN_TEST_CASE(Global, ccnxTimeStamp_Copy);
    LONGBOW_RUN_TEST_CASE(Global, ccnxTimeStamp_Equals);

    LONGBOW_RUN_TEST_CASE(Global, ccnxTimeStamp_AsNanoSeconds);
    LONGBOW_RUN_TEST_CASE(Global, ccnxTimeStamp_CreateFromNanosecondsSinceEpoch);
    LONGBOW_RUN_TEST_CASE(Global, ccnxTimeStamp_ToString);
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

LONGBOW_TEST_CASE(Global, ccnxTimeStamp_CreateFromCurrentUTCTime)
{
    CCNxTimeStamp *timeStamp = ccnxTimeStamp_CreateFromCurrentUTCTime();
    assertNotNull(timeStamp, "Expected a non-null response");

    ccnxTimeStamp_Release(&timeStamp);
    assertNull(timeStamp, "Release failed to NULL the pointer.");
}

LONGBOW_TEST_CASE(Global, ccnxTimeStamp_CreateFromTimespec)
{
    struct timespec time = { .tv_sec = 1, .tv_nsec = 1 };

    CCNxTimeStamp *timeStamp = ccnxTimeStamp_CreateFromTimespec(&time);
    assertNotNull(timeStamp, "Expected a non-null response");

    struct timespec actualTime = ccnxTimeStamp_AsTimespec(timeStamp);

    assertTrue(time.tv_sec == actualTime.tv_sec && time.tv_nsec == actualTime.tv_nsec, "Expected timespec to be equal.");

    ccnxTimeStamp_Release(&timeStamp);
    assertNull(timeStamp, "Release failed to NULL the pointer.");
}

LONGBOW_TEST_CASE(Global, ccnxTimeStamp_CreateFromMillisecondsSinceEpoch)
{
    time_t theTimeInSeconds;
    time(&theTimeInSeconds);

    CCNxTimeStamp *timeStamp = ccnxTimeStamp_CreateFromMillisecondsSinceEpoch((uint64_t) theTimeInSeconds * 1000);
    assertNotNull(timeStamp, "Expected a non-null response");

    struct timespec timeSpec = ccnxTimeStamp_AsTimespec(timeStamp);

    assertTrue(theTimeInSeconds == timeSpec.tv_sec, "Expected %ld, actual %ld", theTimeInSeconds, timeSpec.tv_sec);

    assertTrue(0 == timeSpec.tv_nsec, "Expected %d, actual %ld", 0, timeSpec.tv_nsec);

    ccnxTimeStamp_Release(&timeStamp);
    assertNull(timeStamp, "Release failed to NULL the pointer.");
}

LONGBOW_TEST_CASE(Global, ccnxTimeStamp_CreateFromNanosecondsSinceEpoch)
{
    uint64_t expected = 1099511627776ULL;

    CCNxTimeStamp *timeStamp = ccnxTimeStamp_CreateFromNanosecondsSinceEpoch(expected);

    uint64_t actual = ccnxTimeStamp_AsNanoSeconds(timeStamp);

    assertTrue(expected == actual, "Expected %" PRIu64 " actual %" PRIu64, expected, actual);

    ccnxTimeStamp_Release(&timeStamp);
}

LONGBOW_TEST_CASE(Global, ccnxTimeStamp_Equals)
{
    time_t theTimeInSeconds;
    time(&theTimeInSeconds);

    CCNxTimeStamp *x = ccnxTimeStamp_CreateFromMillisecondsSinceEpoch(theTimeInSeconds * 1000);
    CCNxTimeStamp *y = ccnxTimeStamp_CreateFromMillisecondsSinceEpoch(theTimeInSeconds * 1000);
    CCNxTimeStamp *z = ccnxTimeStamp_CreateFromMillisecondsSinceEpoch(theTimeInSeconds * 1000);
    CCNxTimeStamp *u1 = ccnxTimeStamp_CreateFromMillisecondsSinceEpoch((theTimeInSeconds + 1) * 1000);
    CCNxTimeStamp *u2 = ccnxTimeStamp_CreateFromMillisecondsSinceEpoch((theTimeInSeconds + 2) * 1000);

    assertEqualsContract(ccnxTimeStamp_Equals, x, y, z, u1, u2)

    ccnxTimeStamp_Release(&x);
    ccnxTimeStamp_Release(&y);
    ccnxTimeStamp_Release(&z);
    ccnxTimeStamp_Release(&u1);
    ccnxTimeStamp_Release(&u2);
}

LONGBOW_TEST_CASE(Global, ccnxTimeStamp_Copy)
{
    time_t theTime;
    time(&theTime);

    CCNxTimeStamp *timeStamp = ccnxTimeStamp_CreateFromMillisecondsSinceEpoch(theTime);
    assertNotNull(timeStamp, "Expected a non-null response");

    CCNxTimeStamp *copy = ccnxTimeStamp_Copy(timeStamp);

    char *expected = ccnxTimeStamp_ToString(timeStamp);
    char *actual = ccnxTimeStamp_ToString(copy);
    assertTrue(ccnxTimeStamp_Equals(timeStamp, copy),
               "Expected %s actual %s.", expected, actual);
    parcMemory_Deallocate((void **) &expected);
    parcMemory_Deallocate((void **) &actual);

    ccnxTimeStamp_Release(&timeStamp);
    ccnxTimeStamp_Release(&copy);
    assertNull(timeStamp, "Destroy failed to NULL the pointer.");
}

LONGBOW_TEST_CASE(Global, ccnxTimeStamp_AsNanoSeconds)
{
    uint64_t expected = 1099501627776ULL;

    CCNxTimeStamp *timeStamp = ccnxTimeStamp_CreateFromNanosecondsSinceEpoch(expected);

    uint64_t actual = ccnxTimeStamp_AsNanoSeconds(timeStamp);

    assertTrue(expected == actual, "Expected %" PRIu64 " actual %" PRIu64, expected, actual);

    ccnxTimeStamp_Release(&timeStamp);
}

LONGBOW_TEST_CASE(Global, ccnxTimeStamp_ToString)
{
    CCNxTimeStamp *timeStamp = ccnxTimeStamp_CreateFromCurrentUTCTime();
    assertNotNull(timeStamp, "Expected a non-null response");

    char *string = ccnxTimeStamp_ToString(timeStamp);
    assertNotNull(string, "Expected non-null result.");

    parcMemory_Deallocate((void **) &string);

    ccnxTimeStamp_Release(&timeStamp);
    assertNull(timeStamp, "Destroy failed to NULL the pointer.");
    // See case 1016
    testUnimplemented("This test is unimplemented");
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
    return LONGBOW_STATUS_SUCCEEDED;
}

int
main(int argc, char *argv[argc])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_TimeStamp);
    exit(longBowMain(argc, argv, testRunner, NULL));
}
