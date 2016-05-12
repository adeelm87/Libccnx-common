/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../ccnxCodecSchemaV1_NameCodec.c"
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

LONGBOW_TEST_RUNNER(ccnxTlvCodec_Name)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnxTlvCodec_Name)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnxTlvCodec_Name)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxTlvCodecName_Decode_RightType);
    LONGBOW_RUN_TEST_CASE(Global, ccnxTlvCodecName_Decode_WrongType);
    LONGBOW_RUN_TEST_CASE(Global, ccnxTlvCodecName_Encode);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    if (parcSafeMemory_ReportAllocation(STDOUT_FILENO) != 0) {
        printf("('%s' leaks memory by %d (allocs - frees)) ", longBowTestCase_GetName(testCase), parcMemory_Outstanding());
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxTlvCodecName_Decode_RightType)
{
    PARCBuffer *buffer = parcBuffer_WrapCString("brandywine");
    CCNxNameSegment *segment = ccnxNameSegment_CreateTypeValue(CCNxNameLabelType_NAME, buffer);
    CCNxName *truth = ccnxName_Append(ccnxName_Create(), segment);
    ccnxNameSegment_Release(&segment);
    parcBuffer_Release(&buffer);

    uint8_t decodeBytes[] = { 0x10, 0x20, 0x00, 0x0E, 0x00, CCNxNameLabelType_NAME, 0x00, 0x0A, 'b', 'r', 'a', 'n', 'd', 'y', 'w', 'i', 'n', 'e' };
    PARCBuffer *decodeBuffer = parcBuffer_Wrap(decodeBytes, sizeof(decodeBytes), 0, sizeof(decodeBytes));
    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(decodeBuffer);
    CCNxName *test = ccnxCodecSchemaV1NameCodec_Decode(decoder, 0x1020);

    assertTrue(ccnxName_Equals(truth, test), "Name segments do not match");

    ccnxName_Release(&test);
    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&decodeBuffer);
    ccnxName_Release(&truth);
}

LONGBOW_TEST_CASE(Global, ccnxTlvCodecName_Decode_WrongType)
{
    uint8_t decodeBytes[] = { 0x10, 0x20, 0x00, 0x0E, 0x00, CCNxNameLabelType_NAME, 0x00, 0x0A, 'b', 'r', 'a', 'n', 'd', 'y', 'w', 'i', 'n', 'e' };
    PARCBuffer *decodeBuffer = parcBuffer_Wrap(decodeBytes, sizeof(decodeBytes), 0, sizeof(decodeBytes));
    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(decodeBuffer);
    CCNxName *test = ccnxCodecSchemaV1NameCodec_Decode(decoder, 0xFFFF);

    assertNull(test, "Name should have returned NULL because the name type does not match");
    assertTrue(ccnxCodecTlvDecoder_Position(decoder) == 0, "Position should not have moved, expected 0, got %zu", ccnxCodecTlvDecoder_Position(decoder));

    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&decodeBuffer);
}

LONGBOW_TEST_CASE(Global, ccnxTlvCodecName_Encode)
{
    uint8_t truthBytes[] = { 0x10, 0x20, 0x00, 0x0E, 0x00, CCNxNameLabelType_NAME, 0x00, 0x0A, 'b', 'r', 'a', 'n', 'd', 'y', 'w', 'i', 'n', 'e' };
    PARCBuffer *truth = parcBuffer_Wrap(truthBytes, sizeof(truthBytes), 0, sizeof(truthBytes));

    CCNxCodecTlvEncoder *encoder = ccnxCodecTlvEncoder_Create();
    ccnxCodecTlvEncoder_Initialize(encoder);

    PARCBuffer *buffer = parcBuffer_WrapCString("brandywine");
    CCNxNameSegment *segment = ccnxNameSegment_CreateTypeValue(CCNxNameLabelType_NAME, buffer);

    CCNxName *name = ccnxName_Append(ccnxName_Create(), segment);
    ccnxNameSegment_Release(&segment);
    parcBuffer_Release(&buffer);

    ccnxCodecSchemaV1NameCodec_Encode(encoder, 0x1020, name);

    ccnxCodecTlvEncoder_Finalize(encoder);
    PARCBuffer *test = ccnxCodecTlvEncoder_CreateBuffer(encoder);

    if (!parcBuffer_Equals(truth, test)) {
        printf("Buffers do not match\n");
        printf("Excpected:\n");
        parcBuffer_Display(truth, 3);
        printf("Got:\n");
        parcBuffer_Display(test, 3);
        assertTrue(parcBuffer_Equals(truth, test), "Buffers do not match");
    }

    ccnxName_Release(&name);
    parcBuffer_Release(&test);
    ccnxCodecTlvEncoder_Destroy(&encoder);
    parcBuffer_Release(&truth);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnxTlvCodec_Name);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
