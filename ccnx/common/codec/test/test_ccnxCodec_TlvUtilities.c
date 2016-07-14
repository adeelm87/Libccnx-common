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
#include "../ccnxCodec_TlvUtilities.c"
#include <parc/algol/parc_SafeMemory.h>

#include <LongBow/unit-test.h>

#include <inttypes.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_FixedHeader.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_ValidationDecoder.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.h>


typedef struct test_data {
    uint8_t *packet;
    PARCBuffer *fixedHeader;
    CCNxCodecTlvDecoder *decoder;
    CCNxTlvDictionary *dictionary;

    // truth table
    uint8_t version;
    uint8_t packetType;
    uint16_t packetLength;
    uint8_t headerLength;
} TestData;

static TestData *
_commonSetup(void)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(TestData));

    data->packet = parcMemory_Allocate(8);
    assertNotNull(data->packet, "parcMemory_Allocate(%u) returned NULL", 8);

    // Make a V1 fixed header
    memcpy(data->packet, &((uint8_t[]) {
        0x01,           // version
        0x01,           // packetType
        0x01, 0x02,     // packetLength
        0x00,           // hopLimit/hopCount
        0x00,           // returnCode
        0x03,           // flags
        0x04            // headerLength
    }), 8);


    data->fixedHeader = parcBuffer_Wrap(data->packet, 8, 0, 8);
    data->version = 1;
    data->packetType = 1;
    data->packetLength = 0x0102;
    data->headerLength = 0x04;
    data->decoder = ccnxCodecTlvDecoder_Create(data->fixedHeader);
    data->dictionary = ccnxTlvDictionary_Create(10, 10);
    return data;
}

static void
_commonTeardown(TestData *data)
{
    ccnxTlvDictionary_Release(&data->dictionary);
    ccnxCodecTlvDecoder_Destroy(&data->decoder);
    parcBuffer_Release(&data->fixedHeader);
    parcMemory_Deallocate((void **) &(data->packet));
    parcMemory_Deallocate((void **) &data);
}

LONGBOW_TEST_RUNNER(rta_TlvUtilities)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(rta_TlvUtilities)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(rta_TlvUtilities)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecTlvUtilities_GetVarInt);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecTlvUtilities_DecodeContainer);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecTlvUtilities_DecodeSubcontainer);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecTlvUtilities_PutAsName);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecTlvUtilities_PutAsBuffer);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecTlvUtilities_PutAsListBuffer);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecTlvUtilities_NestedEncode);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecTlvUtilities_EncodeCustomList);
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

LONGBOW_TEST_CASE(Global, ccnxCodecTlvUtilities_PutAsBuffer)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    uint32_t type = 1;
    uint32_t length = 8;

    bool success = ccnxCodecTlvUtilities_PutAsBuffer(data->decoder, data->dictionary, type, length,
                                                     CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader);

    assertTrue(success, "Failed to save buffer slice");

    int version = ccnxCodecSchemaV1FixedHeaderDecoder_GetVersion(data->dictionary);
    assertTrue(version == data->version, "Wrong version, got %d expected %d", version, data->version);
}

LONGBOW_TEST_CASE(Global, ccnxCodecTlvUtilities_GetVarInt)
{
    struct test_vector {
        uint64_t value;
        bool valid;
        int length;
        uint8_t *array;
    } vectors[] = {
        // length 0 invalid
        { .value = 0,                     .valid = false, .length = 0, .array = (uint8_t[]) { 0x00 } },
        { .value = 0,                     .valid = true,  .length = 1, .array = (uint8_t[]) { 0x00 } },
        { .value = 0xFF,                  .valid = true,  .length = 1, .array = (uint8_t[]) { 0xFF } },
        { .value = 0x0001,                .valid = true,  .length = 2, .array = (uint8_t[]) { 0x00, 0x01} },
        { .value = 0xFF01,                .valid = true,  .length = 2, .array = (uint8_t[]) { 0xFF, 0x01} },
        { .value = 0x000001,              .valid = true,  .length = 3, .array = (uint8_t[]) { 0x00, 0x00, 0x01} },
        { .value = 0xFF0001,              .valid = true,  .length = 3, .array = (uint8_t[]) { 0xFF, 0x00, 0x01} },
        { .value = 0x00000001,            .valid = true,  .length = 4, .array = (uint8_t[]) { 0x00, 0x00, 0x00, 0x01} },
        { .value = 0xFF002001,            .valid = true,  .length = 4, .array = (uint8_t[]) { 0xFF, 0x00, 0x20, 0x01} },
        { .value = 0xFF00200103040506ULL, .valid = true,  .length = 8, .array = (uint8_t[]) { 0xFF, 0x00, 0x20, 0x01, 0x03, 0x04, 0x05, 0x06} },
        // length 9 invalid
        { .value = 0,                     .valid = false, .length = 9, .array = (uint8_t[]) { 0xFF, 0x00, 0x20, 0x01, 0x03, 0x04, 0x05, 0x06, 0x07} },
        // sentinal is NULL array
        { .value = 0,                     .valid = false, .length = 0, .array = NULL },
    };

    for (int i = 0; vectors[i].array != NULL; i++) {
        PARCBuffer *buffer = parcBuffer_Wrap(vectors[i].array, vectors[i].length, 0, vectors[i].length);

        uint64_t value;
        bool success = ccnxCodecTlvUtilities_GetVarInt(buffer, vectors[i].length, &value);
        parcBuffer_Release(&buffer);

        assertTrue(success == vectors[i].valid, "index %d: Wrong return, got %d expected %d", i, success, vectors[i].valid);
        if (vectors[i].valid) {
            assertTrue(value == vectors[i].value, "index %d: wrong value: got %" PRIu64 " expected %" PRIu64, i, value, vectors[i].value);
        }
    }
}


static bool
_decodeSubContainer(CCNxCodecTlvDecoder *decoder, CCNxTlvDictionary *packetDictionary)
{
    return true;
}

LONGBOW_TEST_CASE(Global, ccnxCodecTlvUtilities_DecodeSubcontainer)
{
    uint8_t metadata[] = {
        0x00, 0x0B, 0x00, 17,  // Object Metadata, length = 17
        0x00, 0x0C, 0x00, 0x01, // Object Type, length = 1
        0x04, // LINK
        0x00, 0x0D, 0x00, 8,   // Creation Time
        0x00, 0x00, 0x01, 0x43, // 1,388,534,400,000 msec
        0x4B, 0x19, 0x84, 0x00,
    };

    PARCBuffer *buffer = parcBuffer_Wrap(metadata, sizeof(metadata), 0, sizeof(metadata));

    // now decode that snippit
    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(buffer);
    CCNxTlvDictionary *dictionary = ccnxTlvDictionary_Create(20, 20);

    uint16_t key = ccnxCodecTlvDecoder_GetType(decoder);
    uint16_t length = ccnxCodecTlvDecoder_GetLength(decoder);

    bool success = ccnxCodecTlvUtilities_DecodeSubcontainer(decoder, dictionary, key, length, _decodeSubContainer);

    assertTrue(success, "Failed to decode metadata container");

    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&buffer);
}


static bool
testTypeDecoder(CCNxCodecTlvDecoder *decoder, CCNxTlvDictionary *packetDictionary, uint16_t type, uint16_t length)
{
    switch (type) {
        case 0x000C: // fallthrough
        case 0x000D:
            ccnxCodecTlvDecoder_Advance(decoder, length);
            return true;
        default:
            return false;
    }
}

LONGBOW_TEST_CASE(Global, ccnxCodecTlvUtilities_DecodeContainer)
{
    uint8_t metadataContainer[] = {
        0x00, 0x0C, 0x00, 0x01, // Object Type, length = 1
        0x04, // LINK
        0x00, 0x0D, 0x00, 8,   // Creation Time
        0x00, 0x00, 0x01, 0x43, // 1,388,534,400,000 msec
        0x4B, 0x19, 0x84, 0x00,
    };

    PARCBuffer *buffer = parcBuffer_Wrap(metadataContainer, sizeof(metadataContainer), 0, sizeof(metadataContainer));

    // now decode that snippit
    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(buffer);
    CCNxTlvDictionary *dictionary = ccnxTlvDictionary_Create(10, 10);

    bool success = ccnxCodecTlvUtilities_DecodeContainer(decoder, dictionary, testTypeDecoder);

    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&buffer);

    assertTrue(success, "The TLV types were known to us");
}

LONGBOW_TEST_CASE(Global, ccnxCodecTlvUtilities_PutAsName)
{
    // A list of 2 TLV containers (types 0x000C and 0x000D)
    uint8_t nameContainer[] = {
        0x00, 0x00, 0x00, 9,        // type = name, length = 9
        0x00, 0x03, 0x00, 5,        // type = binary, length = 5
        'h',  'e',  'l', 'l',       // "hello"
        'o',
    };

    PARCBuffer *buffer = parcBuffer_Wrap(nameContainer, sizeof(nameContainer), 0, sizeof(nameContainer));

    // now decode that snippit
    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(buffer);
    CCNxTlvDictionary *dictionary = ccnxTlvDictionary_Create(10, 10);

    uint16_t tlvtype = ccnxCodecTlvDecoder_GetType(decoder);
    uint16_t tlvlength = ccnxCodecTlvDecoder_GetLength(decoder);

    // Saves "lci:/3=hello"
    bool success = ccnxCodecTlvUtilities_PutAsName(decoder, dictionary, tlvtype, tlvlength, 1);

    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&buffer);

    assertTrue(success, "The Name failed to decode or some other error");

    CCNxName *truth = ccnxName_CreateFromCString("lci:/3=hello");
    CCNxName *test = ccnxTlvDictionary_GetName(dictionary, 1);
    assertTrue(ccnxName_Equals(truth, test), "Names not equal")
    {
        ccnxName_Display(test, 3);
        ccnxName_Display(truth, 3);
        ccnxName_Release(&truth);
        ccnxTlvDictionary_Release(&dictionary);
    }

    ccnxName_Release(&truth);
    ccnxTlvDictionary_Release(&dictionary);
}

LONGBOW_TEST_CASE(Global, ccnxCodecTlvUtilities_PutAsListBuffer)
{
    uint8_t array[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    PARCBuffer *buffer = parcBuffer_Wrap(array, sizeof(array), 0, sizeof(array));

    PARCBuffer *truth[3];
    truth[0] = parcBuffer_Wrap(array, sizeof(array), 0, 2);
    truth[1] = parcBuffer_Wrap(array, sizeof(array), 2, 3);
    truth[2] = parcBuffer_Wrap(array, sizeof(array), 3, 6);

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(buffer);
    CCNxTlvDictionary *dictionary = ccnxTlvDictionary_Create(10, 10);

    // put 3 buffers of {0x01, 0x02} and {0x03} and {0x04, 0x05, x06} on the list
    int listkey = 1;
    ccnxCodecTlvUtilities_PutAsListBuffer(decoder, dictionary, 0, 2, listkey);
    ccnxCodecTlvUtilities_PutAsListBuffer(decoder, dictionary, 1, 1, listkey);
    ccnxCodecTlvUtilities_PutAsListBuffer(decoder, dictionary, 2, 3, listkey);

    assertTrue(ccnxTlvDictionary_ListSize(dictionary, listkey) == 3,
               "Wrong list size, got %zu expected %u",
               ccnxTlvDictionary_ListSize(dictionary, listkey), 3);

    // now make sure they are right
    for (int i = 0; i < ccnxTlvDictionary_ListSize(dictionary, listkey); i++) {
        PARCBuffer *test = ccnxTlvDictionary_ListGetByType(dictionary, listkey, i);
        assertNotNull(test, "Failed to get index %d", i);

        assertTrue(parcBuffer_Equals(truth[i], test), "Buffers not equal for index %d", i)
        {
            parcBuffer_Display(test, 3);
            parcBuffer_Display(truth[i], 3);
        }
    }

    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&truth[0]);
    parcBuffer_Release(&truth[1]);
    parcBuffer_Release(&truth[2]);
    parcBuffer_Release(&buffer);
    ccnxTlvDictionary_Release(&dictionary);
}



LONGBOW_TEST_CASE(Global, ccnxCodecTlvUtilities_NestedEncode)
{
//  TODO: This test needs to be updated with V1 data.
//        See BugzID: 3919


//    uint8_t metadata[] = {
//        0x00, 0x0B, 0x00, 17,  // Object Metadata, length = 17
//        0x00, 0x0C, 0x00, 0x01, // Object Type, length = 1
//        0x04, // LINK
//        0x00, 0x0D, 0x00, 8,   // Creation Time
//        0x00, 0x00, 0x01, 0x43, // 1,388,534,400,000 msec
//        0x4B, 0x19, 0x84, 0x00,
//    };
//
//
//    PARCBuffer *truth = parcBuffer_Wrap(metadata, sizeof(metadata), 0, sizeof(metadata));
//
//    // now decode that snippit
//    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(truth);
//    CCNxTlvDictionary *dictionary = ccnxTlvDictionary_Create(20, 20);
//    ccnxCodecTlvDecoder_Advance(decoder, 4);
//    ccnxCodecTlvUtilities_DecodeContainer(decoder, dictionary, _ccnxCodecSchemaV0MetadataDecoder_DecodeType);
//
//    // the dictionary should now be ready for encoding
//    ccnxCodecTlvDecoder_Destroy(&decoder);
//
//    CCNxCodecTlvEncoder *encoder = ccnxCodecTlvEncoder_Create();
//    ssize_t length = ccnxCodecTlvUtilities_NestedEncode(encoder, dictionary, 0x000B, ccnxCodecSchemaV0MetadataEncoder_Encode);
//
//    assertTrue(length == sizeof(metadata), "Wrong size, got %zu expected %zu", length, sizeof(metadata));
//
//    ccnxCodecTlvEncoder_Finalize(encoder);
//    PARCBuffer *test = ccnxCodecTlvEncoder_CreateBuffer(encoder);
//    assertTrue(parcBuffer_Equals(test, truth), "Buffers do not match")
//    {
//        parcBuffer_Display(test, 3);
//        parcBuffer_Display(truth, 3);
//    }
//
//    ccnxCodecTlvEncoder_Destroy(&encoder);
//    ccnxTlvDictionary_Release(&dictionary);
//    parcBuffer_Release(&truth);
//    parcBuffer_Release(&test);
}

LONGBOW_TEST_CASE(Global, ccnxCodecTlvUtilities_EncodeCustomList)
{
    uint8_t truthArray[] = { 0x00, 0x00, 0x00, 0x02, 0x01, 0x02,
                             0x00, 0x01, 0x00, 0x01, 0x03,
                             0x00, 0x02, 0x00, 0x03, 0x04, 0x05, 0x06 };

    PARCBuffer *truth = parcBuffer_Wrap(truthArray, sizeof(truthArray), 0, sizeof(truthArray));

    PARCBuffer *buffers[3];
    buffers[0] = parcBuffer_Wrap(truthArray, sizeof(truthArray), 4, 6);
    buffers[1] = parcBuffer_Wrap(truthArray, sizeof(truthArray), 10, 11);
    buffers[2] = parcBuffer_Wrap(truthArray, sizeof(truthArray), 15, 18);

    CCNxTlvDictionary *dictionary = ccnxTlvDictionary_Create(10, 10);

    // put 3 buffers of {0x01, 0x02} and {0x03} and {0x04, 0x05, x06} on the list
    int listkey = 1;
    ccnxTlvDictionary_PutListBuffer(dictionary, listkey, 2, buffers[2]);
    ccnxTlvDictionary_PutListBuffer(dictionary, listkey, 1, buffers[1]);
    ccnxTlvDictionary_PutListBuffer(dictionary, listkey, 0, buffers[0]);

    CCNxCodecTlvEncoder *encoder = ccnxCodecTlvEncoder_Create();
    ccnxCodecTlvUtilities_EncodeCustomList(encoder, dictionary, listkey);

    ccnxCodecTlvEncoder_Finalize(encoder);
    PARCBuffer *test = ccnxCodecTlvEncoder_CreateBuffer(encoder);

    assertTrue(parcBuffer_Equals(test, truth), "Buffers not equal")
    {
        parcBuffer_Display(test, 3);
        parcBuffer_Display(truth, 3);
    }

    ccnxCodecTlvEncoder_Destroy(&encoder);
    parcBuffer_Release(&buffers[0]);
    parcBuffer_Release(&buffers[1]);
    parcBuffer_Release(&buffers[2]);
    parcBuffer_Release(&truth);
    parcBuffer_Release(&test);
    ccnxTlvDictionary_Release(&dictionary);
}

// ====================================================================================

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
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(rta_TlvUtilities);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
