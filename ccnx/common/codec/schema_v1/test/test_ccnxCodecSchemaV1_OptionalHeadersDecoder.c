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
#include "../ccnxCodecSchemaV1_OptionalHeadersDecoder.c"
#include <parc/algol/parc_SafeMemory.h>

#include <inttypes.h>

#include <LongBow/unit-test.h>

typedef struct test_data {
    PARCBuffer *optionalHeader;
    CCNxCodecTlvDecoder *decoder;
    CCNxTlvDictionary *dictionary;

    // truth table
    PARCBuffer *interestLifetime;
    PARCBuffer *cacheTime;
    PARCBuffer *interestFrag;
    PARCBuffer *objectFrag;
    PARCBuffer *customHeader;

    // the key for the customHeader
    uint16_t customHeaderType;
} TestData;

/**
 * A packet with all the defined optional headers plus one custom header
 * This is not actually a packet one would see in real life as it has
 * headers from both an Interest and a Content Object
 */
static uint8_t packet_with_headers[] = {
    0x01, 0x01, 0x00, 120,  // ver = 1, type = interest, length = 120
    0x01, 0x00, 0x00, 88,   // hopLimit = 1, reserved = 0, header length = 88
    // ------------------------
    // byte 8
    0x00, 0x01, 0x00, 0x08, // Interest Lifetime (type 1)
    0x20, 0x30, 0x40, 0x50, // 0x2030405060708090
    0x60, 0x70, 0x80, 0x90,
    // ------------------------
    // byte 20
    0x00, 0x02, 0x00, 0x08, // Recommended Cache Time (type 2)
    0x21, 0x31, 0x41, 0x51, // 0x2030405060708090
    0x61, 0x71, 0x81, 0x91,
    // ------------------------
    // byte 32
    0x00, 0x03, 0x00, 0x0C, // Interest Fragment (type 3)
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    // ------------------------
    // byte 48
    0x00, 0x04, 0x00, 20,   // ContentObject Fragment (type 4)
    0xC1, 0xC2, 0xC3, 0xC4,
    0xC5, 0xC6, 0xC7, 0xC8, // fragment 0xC1C2C3C4C5C6C7C8
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    0xD1, 0xD2, 0xD3, 0xD4,
    0xD5, 0xD6, 0xD7, 0xD8, // fragment 0xD1D2D3D4D5D6D7D8
    // ------------------------
    // byte 72
    0x01, 0x00, 0x00, 12,   // Custom header (type 256), length 12
    0xA0, 0xA1, 0xA2, 0xA3,
    0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xAB,
    // ------------------------
    // byte 88
    0x00, 0x01, 0x00, 29,   // type = interest, length = 29
    // ------------------------
    0x00, 0x00, 0x00, 0x10, // type = name, length = 16
    0x00, 0x02, 0x00, 0x04, // type = binary, length = 5
    'h',  'e',  'l',  'l',  // "hello"
    0xF0, 0x00, 0x00, 0x04, // type = app, length = 4
    'o',  'u',  'c',  'h',  // "ouch"
    // ------------------------
    0x00, 0x01, 0x00, 0x04, // type = keyid, length = 4
    0xA0, 0xB0, 0xC0, 0xD0, // 0xA0B0C0D0
    // ------------------------
    // byte 120
};

static TestData *
_commonSetup(void)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(TestData));

    // setup the decoder and decode the optional headers
    data->optionalHeader = parcBuffer_Wrap(packet_with_headers, sizeof(packet_with_headers), 8, 88);
    data->decoder = ccnxCodecTlvDecoder_Create(data->optionalHeader);
    data->dictionary = ccnxTlvDictionary_Create(10, 5);

    // setup the truth values
    data->interestLifetime = parcBuffer_Wrap(packet_with_headers, sizeof(packet_with_headers), 12, 20);
    data->cacheTime = parcBuffer_Wrap(packet_with_headers, sizeof(packet_with_headers), 24, 32);

    data->interestFrag = parcBuffer_Wrap(packet_with_headers, sizeof(packet_with_headers), 36, 48);
    data->objectFrag = parcBuffer_Wrap(packet_with_headers, sizeof(packet_with_headers), 52, 72);

    data->customHeader = parcBuffer_Wrap(packet_with_headers, sizeof(packet_with_headers), 76, 88);
    data->customHeaderType = 0x0100;

    return data;
}

static void
_commonTeardown(TestData *data)
{
    ccnxTlvDictionary_Release(&data->dictionary);
    ccnxCodecTlvDecoder_Destroy(&data->decoder);
    parcBuffer_Release(&data->optionalHeader);

    parcBuffer_Release(&data->interestLifetime);
    parcBuffer_Release(&data->cacheTime);
    parcBuffer_Release(&data->interestFrag);
    parcBuffer_Release(&data->objectFrag);
    parcBuffer_Release(&data->customHeader);

    parcMemory_Deallocate((void **) &data);
}

LONGBOW_TEST_RUNNER(ccnxCodecSchemaV1_OptionalHeadersDecoder)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnxCodecSchemaV1_OptionalHeadersDecoder)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnxCodecSchemaV1_OptionalHeadersDecoder)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_Decode_TooLong);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetContentObjectFragmentHeader);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestFragmentHeader);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestLifetimeHeader);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetRecommendedCacheTimeHeader);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetCustomHeader);

    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetContentObjectFragmentHeader_Missing);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestFragmentHeader_Missing);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestLifetimeHeader_Missing);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetRecommendedCacheTimeHeader_Missing);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetCustomHeader_Missing);
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
 * One of the TLVs will extend beyond the end of the buffer.  Should fail.
 */
LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_Decode_TooLong)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    // Make one of the fields 255 bytes
    uint8_t original = packet_with_headers[10];
    packet_with_headers[10] = 0xFF;
    bool success = ccnxCodecSchemaV1OptionalHeadersDecoder_Decode(data->decoder, data->dictionary);

    // now set it back
    packet_with_headers[10] = original;

    assertFalse(success, "Should have failed to parse when a TLV exceeds bounary");
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetContentObjectFragmentHeader)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1OptionalHeadersDecoder_Decode(data->decoder, data->dictionary);
    PARCBuffer *test = ccnxCodecSchemaV1OptionalHeadersDecoder_GetContentObjectFragmentHeader(data->dictionary);
    assertTrue(parcBuffer_Equals(test, data->objectFrag), "Wrong value")
    {
        printf("Expected: \n");
        parcBuffer_Display(data->objectFrag, 3);
        printf("Got:\n");
        parcBuffer_Display(test, 3);
    }
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestFragmentHeader)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1OptionalHeadersDecoder_Decode(data->decoder, data->dictionary);
    PARCBuffer *test = ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestFragmentHeader(data->dictionary);
    assertTrue(parcBuffer_Equals(test, data->interestFrag), "Wrong value")
    {
        printf("Expected: \n");
        parcBuffer_Display(data->interestFrag, 3);
        printf("Got:\n");
        parcBuffer_Display(test, 3);
    }
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestLifetimeHeader)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1OptionalHeadersDecoder_Decode(data->decoder, data->dictionary);
    uint64_t lifetime = ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestLifetimeHeader(data->dictionary);

    uint64_t trueLifetime = 0;
    ccnxCodecTlvUtilities_GetVarInt(data->interestLifetime, parcBuffer_Remaining(data->interestLifetime), &trueLifetime);

    assertTrue(trueLifetime == lifetime, "wrong value, expected %" PRIx64 " got %" PRIx64, trueLifetime, lifetime);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetRecommendedCacheTimeHeader)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1OptionalHeadersDecoder_Decode(data->decoder, data->dictionary);
    uint64_t cachetime = ccnxCodecSchemaV1OptionalHeadersDecoder_GetRecommendedCacheTimeHeader(data->dictionary);

    uint64_t trueCachetime = 0;
    ccnxCodecTlvUtilities_GetVarInt(data->cacheTime, parcBuffer_Remaining(data->cacheTime), &trueCachetime);

    assertTrue(trueCachetime == cachetime, "wrong value, expected %" PRIx64 " got %" PRIx64, trueCachetime, cachetime);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetCustomHeader)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ccnxCodecSchemaV1OptionalHeadersDecoder_Decode(data->decoder, data->dictionary);
    PARCBuffer *test = ccnxCodecSchemaV1OptionalHeadersDecoder_GetCustomType(data->dictionary, data->customHeaderType);
    assertTrue(parcBuffer_Equals(test, data->customHeader), "Wrong value for header type %02X", data->customHeaderType)
    {
        printf("Expected: \n");
        parcBuffer_Display(data->customHeader, 3);
        printf("Got:\n");
        parcBuffer_Display(test, 3);
    }
}

// ========
// test for missing values

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetContentObjectFragmentHeader_Missing)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    PARCBuffer *test = ccnxCodecSchemaV1OptionalHeadersDecoder_GetContentObjectFragmentHeader(data->dictionary);
    assertNull(test, "Did not get null buffer for missing field, got %p", (void *) test);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestFragmentHeader_Missing)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    PARCBuffer *test = ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestFragmentHeader(data->dictionary);
    assertNull(test, "Did not get null buffer for missing field, got %p", (void *) test);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestLifetimeHeader_Missing)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    bool exists = ccnxTlvDictionary_IsValueInteger(data->dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_InterestLifetime);
    assertFalse(exists, "Dictionary reports it has a missing field");
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetRecommendedCacheTimeHeader_Missing)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    bool exists = ccnxTlvDictionary_IsValueInteger(data->dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_RecommendedCacheTime);
    assertFalse(exists, "Dictionary reports it has a missing field");
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1OptionalHeadersDecoder_GetCustomHeader_Missing)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    PARCBuffer *test = ccnxCodecSchemaV1OptionalHeadersDecoder_GetCustomType(data->dictionary, data->customHeaderType);
    assertNull(test, "Did not get null buffer for missing field, got %p", (void *) test);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnxCodecSchemaV1_OptionalHeadersDecoder);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
