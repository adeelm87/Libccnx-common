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
#include "../ccnxCodec_Error.c"
#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>

LONGBOW_TEST_RUNNER(tlv_Errors)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(tlv_Errors)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(tlv_Errors)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecError_Create_Destroy);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecError_GetByteOffset);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecError_GetErrorCode);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecError_GetLine);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecError_GetFunction);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecError_GetErrorMessage);
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

LONGBOW_TEST_CASE(Global, ccnxCodecError_Create_Destroy)
{
    CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_NO_ERROR, "apple", 10, 100);
    ccnxCodecError_Release(&error);
    assertTrue(parcMemory_Outstanding() == 0, "memory imbalance after create/destroy");
}

LONGBOW_TEST_CASE(Global, ccnxCodecError_GetByteOffset)
{
    CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_NO_ERROR, "apple", 10, 100);
    assertTrue(ccnxCodecError_GetByteOffset(error) == 100,
               "Wrong offset, expected %u got %zu",
               100,
               ccnxCodecError_GetByteOffset(error));
    ccnxCodecError_Release(&error);
}

LONGBOW_TEST_CASE(Global, ccnxCodecError_GetErrorCode)
{
    CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_NO_ERROR, "apple", 10, 100);
    assertTrue(ccnxCodecError_GetErrorCode(error) == TLV_ERR_NO_ERROR,
               "Wrong error code, expected %d got %d",
               TLV_ERR_NO_ERROR,
               ccnxCodecError_GetErrorCode(error));
    ccnxCodecError_Release(&error);
}

LONGBOW_TEST_CASE(Global, ccnxCodecError_GetLine)
{
    CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_NO_ERROR, "apple", 10, 100);
    assertTrue(ccnxCodecError_GetLine(error) == 10,
               "Wrong line number, expected %d got %d",
               10,
               ccnxCodecError_GetLine(error));
    ccnxCodecError_Release(&error);
}

LONGBOW_TEST_CASE(Global, ccnxCodecError_GetFunction)
{
    char *apple = "apple";

    CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_NO_ERROR, apple, 10, 100);
    assertTrue(ccnxCodecError_GetFunction(error) == apple,
               "Wrong function string, expected %p got %p",
               apple,
               ccnxCodecError_GetFunction(error));
    ccnxCodecError_Release(&error);
}

LONGBOW_TEST_CASE(Global, ccnxCodecError_GetErrorMessage)
{
    char *apple = "apple";
    const char *truth = ccnxCodecErrors_ErrorMessage(TLV_ERR_NO_ERROR);

    CCNxCodecError *error = ccnxCodecError_Create(TLV_ERR_NO_ERROR, apple, 10, 100);
    assertTrue(ccnxCodecError_GetErrorMessage(error) == truth,
               "Wrong function string, expected %p got %p",
               truth,
               ccnxCodecError_GetErrorMessage(error));
    ccnxCodecError_Release(&error);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(tlv_Errors);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
