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
 * This test rig sets up test data to wrap a packet and prepare it for use in a decoder
 *
 * A hand-encoded packet, such as from the testdata directory can be passed to commonSetup and then
 * run automated tests against it based on its manifest.
 *
 * Example:
 * @code
 *
 * static uint8_t object_nameC_keyid3_protoinfo[] = {
 *      0x00, 0x02, 0x00,  110,     // ver = 0, type = object, length = 110
 *      0x00, 0x00, 0x00,    5,     // reserved = 0, header length = 5
 *      // ---------------------------
 *      // snip middle of packet
 *      // ------------------------
 *      // byte offset 76
 *      0x00, 0x03, 0x00,   26,      // Protocol Information, length = 26
 *      0x00, 0x0B, 0x00,   17,     // Object Metadata, length = 17
 *      0x00, 0x0C, 0x00, 0x01,     // Object Type, length = 1
 *      0x04,                       // LINK
 *      0x00, 0x0D, 0x00,    8,     // Creation Time
 *      0x00, 0x00, 0x01, 0x43,     // 1,388,534,400,000 msec
 *      0x4B, 0x19, 0x84, 0x00,
 *      0x00, 0x19, 0x00, 0x01,     // EndSegment, length = 1
 *      42,
 *      // ---------------------------
 *      // snip to end of packet
 *      // ---------------------------
 * };
 *
 * static TruthTableEntry TRUTHTABLENAME(object_nameC_keyid3_protoinfo)[] = {
 *    { .wellKnownType = true,  .indexOrKey = MANIFEST_OBJ_METADATA,      .bodyManifest=true,  .extent = { 80, 17} },
 *    { .wellKnownType = true,  .indexOrKey = MANIFEST_OBJ_OBJ_TYPE,      .bodyManifest=true,  .extent = { 84,  1} },
 *    { .wellKnownType = false, .indexOrKey = T_INVALID,  .extent = { 0,  0} },
 * };
 *
 * LONGBOW_TEST_FIXTURE_SETUP(Global)
 * {
 *    commonSetup(testCase, object_nameC_keyid3_protoinfo, sizeof(object_nameC_keyid3_protoinfo), object_nameC_keyid3_protoinfo_truthTableEntries, MANIFEST_OBJ_METADATA);
 *    return LONGBOW_STATUS_SUCCEEDED;
 * }
 *
 * LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV0ProtoInfo_GetEndSegmentNumber)
 * {
 *    testInt32Getter(testCase, MANIFEST_OBJ_OBJ_TYPE, ccnxCodecSchemaV0Metadata_Decode, ccnxCodecSchemaV0Metadata_GetContentType);
 * }
 *
 * @endcode
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <inttypes.h>
#include <stdio.h>

#include <ccnx/common/codec/ccnxCodec_TlvUtilities.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_NameCodec.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_testrig_truthTable.h>

/**
 * Finds a row in the truthtable where bodyManifest is TRUE and the
 * indexOrKey equals 'key'.
 */
TlvExtent
getTruthTableExtent(TruthTableEntry *ttentry, int key)
{
    for (int i = 0; ttentry[i].indexOrKey != T_INVALID; i++) {
        if (ttentry[i].bodyManifest && ttentry[i].indexOrKey == key) {
            return ttentry[i].extent;
        }
    }
    return (TlvExtent) { 0, 0 };
}

/**
 * Finds a row in the truthtable where bodyManifest is FALSE and the
 * indexOrKey equals 'key'.
 */
TlvExtent
getTruthTableHeaderExtent(TruthTableEntry *ttentry, int key)
{
    for (int i = 0; ttentry[i].indexOrKey != T_INVALID; i++) {
        if (!ttentry[i].bodyManifest && ttentry[i].indexOrKey == key) {
            return ttentry[i].extent;
        }
    }
    return (TlvExtent) { 0, 0 };
}

typedef struct test_data {
    PARCBuffer *interest;
    CCNxCodecTlvDecoder *decoder;
    CCNxTlvDictionary *dictionary;

    uint8_t *packet;
    size_t packetLength;
    TruthTableEntry *truthTable;
} TestData;

/**
 * Wraps the given (packet, length) in a PARCBuffer where the data->memoryRegion member will be set
 * to a given extent within that PARCBuffer.  The function will locate the truthTableKey in the truthTable and
 * use it's extent as the bounds for the wrapped packet.
 *
 * For example, if the key V1_INT_NAME has the extent {32, 12}, then the PARCBuffer will wrap the packet
 * memory and it will have and offset of 32, position 0, and a limit of 12.
 */
TestData *
commonSetup(uint8_t *packet, size_t length, TruthTableEntry *truthTable, int truthTableKey)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(TestData));

    TlvExtent extent = getTruthTableExtent(truthTable, truthTableKey);

    data->interest = parcBuffer_Wrap(packet, length, extent.offset, extent.offset + extent.length);
    data->decoder = ccnxCodecTlvDecoder_Create(data->interest);

    // content objects have more fields than interests, so use that
    data->dictionary = ccnxTlvDictionary_Create(CCNxCodecSchemaV1TlvDictionary_MessageFastArray_END, CCNxCodecSchemaV1TlvDictionary_Lists_END);

    data->packet = packet;
    data->packetLength = length;
    data->truthTable = truthTable;
    return data;
}

void
commonSetupWholePacket(LongBowTestCase *testCase, uint8_t *packet, size_t length, TruthTableEntry *truthTable)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(TestData));

    data->interest = parcBuffer_Wrap(packet, length, 0, length);
    data->decoder = ccnxCodecTlvDecoder_Create(data->interest);
    data->dictionary = ccnxTlvDictionary_Create(CCNxCodecSchemaV1TlvDictionary_MessageFastArray_END, CCNxCodecSchemaV1TlvDictionary_Lists_END);

    data->packet = packet;
    data->packetLength = length;
    data->truthTable = truthTable;

    longBowTestCase_SetClipBoardData(testCase, data);
}

void
commonTeardown(TestData *data)
{
    ccnxTlvDictionary_Release(&data->dictionary);
    ccnxCodecTlvDecoder_Destroy(&data->decoder);
    parcBuffer_Release(&data->interest);
    parcMemory_Deallocate((void **) &data);
}

/**
 * Tests that an int32_t getter returns the right value
 *
 * Given a packet byte array and a truth table (see transport/test_tools/testdata/testrig_truthTable.h),
 * check that the buffer the decoder parsed is the right buffer.
 *
 * The function will run the specified decoder on the TestData's packet and put the results in the
 * TestData's dictionary.  It will then call the specified getter and make sure its value is equal
 * to the truth table's value.
 *
 * The function will assert if the test fails.
 *
 * @param [in] testCase Used to get the clipboard data with the TestData member
 * @param [in] truthTableKey Used to match the .indexOrKey truth table member
 * @param [in] containerDecoder The decoder to use on the packet contained in TestData
 * @param [in] getter The function to call to fetch a decoded value
 *
 * Example:
 * @code
 *
 * static uint8_t object_nameC_keyid3_protoinfo[] = {
 *      0x00, 0x02, 0x00,  110,     // ver = 0, type = object, length = 110
 *      0x00, 0x00, 0x00,    5,     // reserved = 0, header length = 5
 *      // ---------------------------
 *      // snip middle of packet
 *      // ------------------------
 *      // byte offset 76
 *      0x00, 0x03, 0x00,   26,      // Protocol Information, length = 26
 *      0x00, 0x0B, 0x00,   17,     // Object Metadata, length = 17
 *      0x00, 0x0C, 0x00, 0x01,     // Object Type, length = 1
 *      0x04,                       // LINK
 *      0x00, 0x0D, 0x00,    8,     // Creation Time
 *      0x00, 0x00, 0x01, 0x43,     // 1,388,534,400,000 msec
 *      0x4B, 0x19, 0x84, 0x00,
 *      0x00, 0x19, 0x00, 0x01,     // EndSegment, length = 1
 *      42,
 *      // ---------------------------
 *      // snip to end of packet
 *      // ---------------------------
 * };
 *
 * static TruthTableEntry TRUTHTABLENAME(object_nameC_keyid3_protoinfo)[] = {
 *    { .wellKnownType = true,  .indexOrKey = MANIFEST_OBJ_METADATA,      .bodyManifest=true,  .extent = { 80, 17} },
 *    { .wellKnownType = true,  .indexOrKey = MANIFEST_OBJ_OBJ_TYPE,      .bodyManifest=true,  .extent = { 84,  1} },
 *    { .wellKnownType = false, .indexOrKey = T_INVALID,  .extent = { 0,  0} },
 * };
 *
 * LONGBOW_TEST_FIXTURE_SETUP(Global)
 * {
 *    commonSetup(testCase, object_nameC_keyid3_protoinfo, sizeof(object_nameC_keyid3_protoinfo), object_nameC_keyid3_protoinfo_truthTableEntries, MANIFEST_OBJ_METADATA);
 *    return LONGBOW_STATUS_SUCCEEDED;
 * }
 *
 * LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV0ProtoInfo_GetEndSegmentNumber)
 * {
 *    testInt32Getter(testCase, MANIFEST_OBJ_OBJ_TYPE, ccnxCodecSchemaV0Metadata_Decode, ccnxCodecSchemaV0Metadata_GetContentType);
 * }
 * * @endcode
 */
void
testInt32Getter(LongBowTestCase *testCase, int truthTableKey, bool containerDecoder(CCNxCodecTlvDecoder *, CCNxTlvDictionary *),
                int32_t (*getter)(CCNxTlvDictionary *))
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    containerDecoder(data->decoder, data->dictionary);
    int32_t testvalue = getter(data->dictionary);

    // look up the true name buffer from the truth table
    TlvExtent extent = getTruthTableExtent(data->truthTable, truthTableKey);
    PARCBuffer *truthbuffer = parcBuffer_Wrap(data->packet, data->packetLength, extent.offset, extent.offset + extent.length);
    uint64_t truthvalue = -2;
    ccnxCodecTlvUtilities_GetVarInt(truthbuffer, parcBuffer_Remaining(truthbuffer), &truthvalue);

    parcBuffer_Release(&truthbuffer);

    assertTrue(testvalue == (int32_t) truthvalue, "Wrong value, got %d expected %d", testvalue, (int32_t) truthvalue);
}

/**
 * Tests that an int64_t getter returns the right value
 *
 * Given a packet byte array and a truth table (see transport/test_tools/testdata/testrig_truthTable.h),
 * check that the buffer the decoder parsed is the right buffer.
 *
 * The function will run the specified decoder on the TestData's packet and put the results in the
 * TestData's dictionary.  It will then call the specified getter and make sure its value is equal
 * to the truth table's value.
 *
 * The function will assert if the test fails.
 *
 * @param [in] testCase Used to get the clipboard data with the TestData member
 * @param [in] truthTableKey Used to match the .indexOrKey truth table member
 * @param [in] containerDecoder The decoder to use on the packet contained in TestData
 * @param [in] getter The function to call to fetch a decoded value
 *
 * Example:
 * @code
 *
 * static uint8_t object_nameC_keyid3_protoinfo[] = {
 *      0x00, 0x02, 0x00,  110,     // ver = 0, type = object, length = 110
 *      0x00, 0x00, 0x00,    5,     // reserved = 0, header length = 5
 *      // ---------------------------
 *      // snip middle of packet
 *      // ------------------------
 *      // byte offset 76
 *      0x00, 0x03, 0x00,   26,      // Protocol Information, length = 26
 *      0x00, 0x0B, 0x00,   17,     // Object Metadata, length = 17
 *      0x00, 0x0C, 0x00, 0x01,     // Object Type, length = 1
 *      0x04,                       // LINK
 *      0x00, 0x0D, 0x00,    8,     // Creation Time
 *      0x00, 0x00, 0x01, 0x43,     // 1,388,534,400,000 msec
 *      0x4B, 0x19, 0x84, 0x00,
 *      0x00, 0x19, 0x00, 0x01,     // EndSegment, length = 1
 *      42,
 *      // ---------------------------
 *      // snip to end of packet
 *      // ---------------------------
 * };
 *
 * static TruthTableEntry TRUTHTABLENAME(object_nameC_keyid3_protoinfo)[] = {
 *    { .wellKnownType = true,  .indexOrKey = MANIFEST_OBJ_PROTOINFO,     .bodyManifest=true,  .extent = { 76, 26} },
 *    { .wellKnownType = true,  .indexOrKey = MANIFEST_OBJ_ENDSEGMENT,    .bodyManifest=true,  .extent = {101,  1} },
 *    { .wellKnownType = false, .indexOrKey = T_INVALID,  .extent = { 0,  0} },
 * };
 *
 * LONGBOW_TEST_FIXTURE_SETUP(Global)
 * {
 *    commonSetup(testCase, object_nameC_keyid3_protoinfo, sizeof(object_nameC_keyid3_protoinfo), object_nameC_keyid3_protoinfo_truthTableEntries, MANIFEST_OBJ_PROTOINFO);
 *    return LONGBOW_STATUS_SUCCEEDED;
 * }
 *
 * LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV0ProtoInfo_GetEndSegmentNumber)
 * {
 *    testInt64Getter(testCase, MANIFEST_OBJ_ENDSEGMENT, ccnxCodecSchemaV0ProtoInfo_Decode, ccnxCodecSchemaV0ProtoInfo_GetEndSegmentNumber);
 * }
 *
 * @endcode
 */
void
testInt64Getter(TestData *data, int truthTableKey, bool containerDecoder(CCNxCodecTlvDecoder *, CCNxTlvDictionary *),
                int64_t (*getter)(CCNxTlvDictionary *))
{
    containerDecoder(data->decoder, data->dictionary);
    int64_t testvalue = getter(data->dictionary);

    // look up the true name buffer from the truth table
    TlvExtent extent = getTruthTableExtent(data->truthTable, truthTableKey);
    PARCBuffer *truthbuffer = parcBuffer_Wrap(data->packet, data->packetLength, extent.offset, extent.offset + extent.length);
    uint64_t truthvalue = -2;
    ccnxCodecTlvUtilities_GetVarInt(truthbuffer, parcBuffer_Remaining(truthbuffer), &truthvalue);

    parcBuffer_Release(&truthbuffer);

    assertTrue(testvalue == (int64_t) truthvalue, "Wrong value, got %" PRId64 " expected %" PRId64, testvalue, (int64_t) truthvalue);
}

/**
 * Tests that a buffer getter returns the right buffer
 *
 * Given a packet byte array and a truth table (see transport/test_tools/testdata/testrig_truthTable.h),
 * check that the buffer the decoder parsed is the right buffer.
 *
 * The function will run the specified decoder on the TestData's packet and put the results in the
 * TestData's dictionary.  It will then call the specified getter and make sure its value is equal
 * to the truth table's value.
 *
 * The function will assert if the test fails.
 *
 * @param [in] testCase Used to get the clipboard data with the TestData member
 * @param [in] truthTableKey Used to match the .indexOrKey truth table member
 * @param [in] containerDecoder The decoder to use on the packet contained in TestData
 * @param [in] getter The function to call to fetch a decoded value
 *
 * Example:
 * @code
 *
 * static uint8_t object_nameC_keyid3_protoinfo[] = {
 *      0x00, 0x02, 0x00,  110,     // ver = 0, type = object, length = 110
 *      0x00, 0x00, 0x00,    5,     // reserved = 0, header length = 5
 *      // ---------------------------
 *      // snip middle of packet
 *      // ------------------------
 *      // byte offset = 117
 *      0x00, 0x05, 0x00, 0x06,      // signature block, length = 6
 *      0x00, 0x0E, 0x00, 0x02,      // signature bits, length = 2
 *      0xBE, 0xEF                   // value = 0xBEEF
 * };
 *
 * static TruthTableEntry TRUTHTABLENAME(object_nameC_keyid3_protoinfo)[] = {
 *    { .wellKnownType = true,  .indexOrKey = MANIFEST_OBJ_SIGBLOCK,      .bodyManifest=true,  .extent = {117,  6} },
 *    { .wellKnownType = true,  .indexOrKey = MANIFEST_OBJ_SIGBITS,       .bodyManifest=true,  .extent = {121,  2} },
 *    { .wellKnownType = false, .indexOrKey = T_INVALID,  .extent = { 0,  0} },
 * };
 *
 * LONGBOW_TEST_FIXTURE_SETUP(Global)
 * {
 *    commonSetup(testCase, object_nameC_keyid3_protoinfo, sizeof(object_nameC_keyid3_protoinfo), object_nameC_keyid3_protoinfo_truthTableEntries, MANIFEST_OBJ_SIGBLOCK);
 *    return LONGBOW_STATUS_SUCCEEDED;
 * }
 *
 * LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV0SigBlock_GetSignatureBits)
 * {
 *    testBufferGetter(testCase, MANIFEST_OBJ_SIGBITS, ccnxCodecSchemaV0SigBlock_Decode, ccnxCodecSchemaV0SigBlock_GetSignatureBits);
 * }
 *
 * @endcode
 */
void
testBufferGetter(TestData *data, int truthTableKey, bool containerDecoder(CCNxCodecTlvDecoder *, CCNxTlvDictionary *),
                 PARCBuffer *(*getter)(const CCNxTlvDictionary *))
{
    containerDecoder(data->decoder, data->dictionary);
    PARCBuffer *test = getter(data->dictionary);

    // look up the true name buffer from the truth table
    TlvExtent extent = getTruthTableExtent(data->truthTable, truthTableKey);
    PARCBuffer *truth = parcBuffer_Wrap(data->packet, data->packetLength, extent.offset, extent.offset + extent.length);

    assertTrue(parcBuffer_Equals(test, truth), "Buffers not equal") {
        printf("Expected:\n");
        parcBuffer_Display(truth, 3);
        printf("Got:\n");
        parcBuffer_Display(test, 3);
    }

    parcBuffer_Release(&truth);
}

void
testNameGetter(TestData *data, int truthTableKey, bool containerDecoder(CCNxCodecTlvDecoder *, CCNxTlvDictionary *),
               CCNxName *(*getter)(CCNxTlvDictionary *))
{
    containerDecoder(data->decoder, data->dictionary);
    CCNxName *test = getter(data->dictionary);

    // look up the true name buffer from the truth table
    TlvExtent extent = getTruthTableExtent(data->truthTable, truthTableKey);

    // we need to backup 4 bytes to get the TLV container
    PARCBuffer *truthBuffer =
        parcBuffer_Wrap(data->packet, data->packetLength, extent.offset - 4, extent.offset + extent.length);

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(truthBuffer);
    CCNxName *truthName = ccnxCodecSchemaV1NameCodec_Decode(decoder, CCNxCodecSchemaV1Types_CCNxMessage_Name);
    ccnxCodecTlvDecoder_Destroy(&decoder);

    assertTrue(ccnxName_Equals(test, truthName), "Names not equal") {
        printf("Expected:\n");
        ccnxName_Display(truthName, 3);
        printf("Got:\n");
        ccnxName_Display(test, 3);
    }

    ccnxName_Release(&truthName);
    parcBuffer_Release(&truthBuffer);
}

void
testMissingInt32Getter(LongBowTestCase *testCase, int32_t (*getter)(CCNxTlvDictionary *))
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    int32_t test = getter(data->dictionary);

    assertTrue(test == -1, "Wrong value, got %d expected %d", test, -1);
}

/**
 * Tried to retrieve execute the getter on the dictionary and ensures that
 * the field is missing.
 */
void
testMissingInt64Getter(LongBowTestCase *testCase, int64_t (*getter)(CCNxTlvDictionary *))
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    int64_t test = getter(data->dictionary);

    assertTrue(test == -1, "Wrong value, got %" PRId64 " expected %d", test, -1);
}

/**
 * Tried to retrieve execute the getter on the dictionary and ensures that
 * the field is missing.
 */
void
testMissingNameGetter(LongBowTestCase *testCase, CCNxName *(*getter)(CCNxTlvDictionary *))
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxName *test = getter(data->dictionary);

    assertNull(test, "Should have gotten null for missing field, got %p", (void *) test);
}

/**
 * Tried to retrieve execute the getter on the dictionary and ensures that
 * the field is missing.
 */
void
testMissingBufferGetter(LongBowTestCase *testCase, PARCBuffer *(*getter)(CCNxTlvDictionary *))
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    PARCBuffer *test = getter(data->dictionary);

    assertNull(test, "Should have gotten null for missing field, got %p", (void *) test);
}

/**
 * Tried to retrieve execute the getter on the dictionary and ensures that
 * the field is missing.
 */
void
testMissingDictionaryGetter(LongBowTestCase *testCase, CCNxTlvDictionary *(*getter)(CCNxTlvDictionary *))
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    CCNxTlvDictionary *test = getter(data->dictionary);

    assertNull(test, "Should have gotten null for missing field, got %p", (void *) test);
}
