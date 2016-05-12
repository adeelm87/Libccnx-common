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
#include "../ccnxCodecSchemaV1_FixedHeaderDecoder.c"
#include <parc/algol/parc_SafeMemory.h>

#include <LongBow/unit-test.h>

typedef struct test_data {
    uint8_t *packet;
    PARCBuffer *fixedHeader;
    CCNxCodecTlvDecoder *decoder;
    CCNxTlvDictionary *dictionary;

    // truth table
    uint8_t version;
    uint8_t packetType;
    uint16_t packetLength;
    uint8_t hopLimit;
    uint8_t returnCode;
    uint8_t flags;
    uint8_t headerLength;
} TestData;

static TestData *
_commonSetup(void)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(TestData));

    data->packet = parcMemory_Allocate(8);
    assertNotNull(data->packet, "parcMemory_Allocate(%d) returned NULL", 8);
    memcpy(data->packet, &((uint8_t[]) {0x00, 0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08}), 8);

    data->fixedHeader = parcBuffer_Wrap(data->packet, 8, 0, 8);
    data->version = 0;
    data->packetType = 1;
    data->packetLength = 0x0102;
    data->hopLimit = 3;
    data->returnCode = 4;
    data->flags = 5;
    data->headerLength = 8;
    data->decoder = ccnxCodecTlvDecoder_Create(data->fixedHeader);
    data->dictionary = ccnxTlvDictionary_Create(CCNxCodecSchemaV1TlvDictionary_MessageFastArray_END,
                                                CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_END);

    return data;
}

static void
_commonTeardown(TestData *data)
{
//    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    ccnxTlvDictionary_Release(&data->dictionary);
    ccnxCodecTlvDecoder_Destroy(&data->decoder);
    parcBuffer_Release(&data->fixedHeader);
    parcMemory_Deallocate((void **) &(data->packet));
    parcMemory_Deallocate((void **) &data);
}

LONGBOW_TEST_RUNNER(ccnxCodecSchemaV1_FixedHeaderDecoder)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

LONGBOW_TEST_RUNNER_SETUP(ccnxCodecSchemaV1_FixedHeaderDecoder)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_RUNNER_TEARDOWN(ccnxCodecSchemaV1_FixedHeaderDecoder)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_Decode_Underrun);

    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetHeaderLength);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketType);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketLength);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetVersion);

    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetHopLimit);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetReturnCode);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetFlags);

    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetHeaderLength_Missing);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketType_Missing);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketLength_Missing);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetVersion_Missing);

    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_PacketLengthTooShort);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_HeaderLengthTooShort);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_PacketLengthLessHeaderLength);
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

/**
 * Successful decode is tested in all the _GetX functions.  We only test
 * when the buffer is too small here
 */
LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_Decode_Underrun)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    // advance the buffer so its too small
    ccnxCodecTlvDecoder_Advance(data->decoder, 1);

    size_t beforePosition = ccnxCodecTlvDecoder_Position(data->decoder);
    bool success = ccnxCodecSchemaV1FixedHeaderDecoder_Decode(data->decoder, data->dictionary);
    size_t afterPosition = ccnxCodecTlvDecoder_Position(data->decoder);

    assertFalse(success, "Should have failed with too small a buffer");
    assertTrue(beforePosition == afterPosition, "Wrong postion, got %zu expected %zu", afterPosition, beforePosition);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetHeaderLength)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1FixedHeaderDecoder_Decode(data->decoder, data->dictionary);
    int headerLength = ccnxCodecSchemaV1FixedHeaderDecoder_GetHeaderLength(data->dictionary);
    assertTrue(headerLength == data->headerLength, "Wrong headerLength, got %d expected %d", headerLength,
               data->headerLength);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketType)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1FixedHeaderDecoder_Decode(data->decoder, data->dictionary);
    int packetType = ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketType(data->dictionary);
    assertTrue(packetType == data->packetType, "Wrong packetType, got %d expected %d", packetType, data->packetType);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketLength)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1FixedHeaderDecoder_Decode(data->decoder, data->dictionary);
    int packetLength = ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketLength(data->dictionary);
    assertTrue(packetLength == data->packetLength, "Wrong payloadLength, got %d expected %d", packetLength,
               data->packetLength);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetVersion)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1FixedHeaderDecoder_Decode(data->decoder, data->dictionary);
    int version = ccnxCodecSchemaV1FixedHeaderDecoder_GetVersion(data->dictionary);
    assertTrue(version == data->version, "Wrong version, got %d expected %d", version, data->version);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetHopLimit)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1FixedHeaderDecoder_Decode(data->decoder, data->dictionary);
    int hopLimit = ccnxCodecSchemaV1FixedHeaderDecoder_GetHopLimit(data->dictionary);
    assertTrue(hopLimit == data->hopLimit, "Wrong hopLimit, got %d expected %d", hopLimit, data->hopLimit);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetReturnCode)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1FixedHeaderDecoder_Decode(data->decoder, data->dictionary);
    int returnCode = ccnxCodecSchemaV1FixedHeaderDecoder_GetReturnCode(data->dictionary);
    assertTrue(returnCode == data->returnCode, "Wrong returnCode, got %d expected %d", returnCode, data->returnCode);

    // Check that the InterestReturnCode was set in the fast array, too.
    uint8_t test =
        ccnxTlvDictionary_GetInteger(data->dictionary,
                                     CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_InterestReturnCode);
    assertTrue(test == data->returnCode, "Expected the dictionary to have the return code set");
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetFlags)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1FixedHeaderDecoder_Decode(data->decoder, data->dictionary);
    int flags = ccnxCodecSchemaV1FixedHeaderDecoder_GetFlags(data->dictionary);
    assertTrue(flags == data->flags, "Wrong flags, got %d expected %d", flags, data->flags);
}


// ==============================
// Tests for missing values

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetHeaderLength_Missing)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    // don't decode the buffer, so we're operating with an empty dictionary
    int version = ccnxCodecSchemaV1FixedHeaderDecoder_GetHeaderLength(data->dictionary);
    assertTrue(version == -1, "Wrong HeaderLength, got %d expected %d", version, -1);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketType_Missing)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    // don't decode the buffer, so we're operating with an empty dictionary
    int version = ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketType(data->dictionary);
    assertTrue(version == -1, "Wrong PacketType, got %d expected %d", version, -1);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketLength_Missing)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    // don't decode the buffer, so we're operating with an empty dictionary
    int version = ccnxCodecSchemaV1FixedHeaderDecoder_GetPacketLength(data->dictionary);
    assertTrue(version == -1, "Wrong payloadLength, got %d expected %d", version, -1);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_GetVersion_Missing)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    // don't decode the buffer, so we're operating with an empty dictionary
    int version = ccnxCodecSchemaV1FixedHeaderDecoder_GetVersion(data->dictionary);
    assertTrue(version == -1, "Wrong version, got %d expected %d", version, -1);
}


/**
 * Packet length must be at least 8 bytes
 */
LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_PacketLengthTooShort)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxCodecSchemaV1InterestHeader header = {
        .version      = 1,
        .packetType   = CCNxCodecSchemaV1Types_PacketType_Interest,
        .packetLength = htons(3),
        .hopLimit     = 4,
        .returnCode   = 7,
        .flags        = 8,
        .headerLength = 9,
    };

    PARCBuffer *fixedHeader = parcBuffer_Wrap(&header, 8, 0, 8);
    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(fixedHeader);

    bool success = ccnxCodecSchemaV1FixedHeaderDecoder_Decode(decoder, data->dictionary);
    assertFalse(success, "Did not fail on packet length too short");

    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&fixedHeader);
}

/**
 * Header length must be at least 8 bytes
 */
LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_HeaderLengthTooShort)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxCodecSchemaV1InterestHeader header = {
        .version      = 1,
        .packetType   = CCNxCodecSchemaV1Types_PacketType_Interest,
        .packetLength = htons(12),
        .hopLimit     = 4,
        .returnCode   = 7,
        .flags        = 8,
        .headerLength = 6,
    };

    PARCBuffer *fixedHeader = parcBuffer_Wrap(&header, 8, 0, 8);
    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(fixedHeader);

    bool success = ccnxCodecSchemaV1FixedHeaderDecoder_Decode(decoder, data->dictionary);
    assertFalse(success, "Did not fail on header length too short");

    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&fixedHeader);
}

/**
 * Packet length must be no less than Header Length
 */
LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1FixedHeaderDecoder_PacketLengthLessHeaderLength)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxCodecSchemaV1InterestHeader header = {
        .version      = 1,
        .packetType   = CCNxCodecSchemaV1Types_PacketType_Interest,
        .packetLength = htons(12),
        .hopLimit     = 4,
        .returnCode   = 7,
        .flags        = 8,
        .headerLength = 18,
    };

    PARCBuffer *fixedHeader = parcBuffer_Wrap(&header, 8, 0, 8);
    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(fixedHeader);

    bool success = ccnxCodecSchemaV1FixedHeaderDecoder_Decode(decoder, data->dictionary);
    assertFalse(success, "Did not fail on packet length less than header length");

    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&fixedHeader);
}

// ======================

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnxCodecSchemaV1_FixedHeaderDecoder);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
