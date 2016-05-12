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
/**
 * Decode whole packets then spot-check that fields from each section appear.  We don't need
 * to exhastively test here, as the individual decoders are exhaustively tested.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../ccnxCodecSchemaV1_PacketDecoder.c"
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_content_nameA_keyid1_rsasha256.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_content_zero_payload.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_content_no_payload.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_interest_all_fields.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_interest_nameA_crc32c.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_interest_bad_message_length.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_interest_bad_validation_alg.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_interest_validation_alg_overrun.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_cpi_add_route_crc32c.h>

#include "testrig_packetwrapper.c"

#include <ccnx/common/internal/ccnx_ContentObjectFacadeV1.h>
#include <ccnx/common/internal/ccnx_ValidationFacadeV1.h>

// =========================================================================

LONGBOW_TEST_RUNNER(ccnxCodecSchemaV1_PacketDecoder)
{
    LONGBOW_RUN_TEST_FIXTURE(ContentObject);
    LONGBOW_RUN_TEST_FIXTURE(Control);
    LONGBOW_RUN_TEST_FIXTURE(Interest);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnxCodecSchemaV1_PacketDecoder)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnxCodecSchemaV1_PacketDecoder)
{
    return LONGBOW_STATUS_SUCCEEDED;
}


// =========================================================================

LONGBOW_TEST_FIXTURE(ContentObject)
{
    LONGBOW_RUN_TEST_CASE(ContentObject, ContentObject_RsaSha256_FixedHeader);
    LONGBOW_RUN_TEST_CASE(ContentObject, ContentObject_RsaSha256_Name);
    LONGBOW_RUN_TEST_CASE(ContentObject, ContentObject_RsaSha256_ExpiryTime);
    LONGBOW_RUN_TEST_CASE(ContentObject, ContentObject_RsaSha256_ValidationAlg_KeyId);
    LONGBOW_RUN_TEST_CASE(ContentObject, ContentObject_RsaSha256_ValidationPayload);
    LONGBOW_RUN_TEST_CASE(ContentObject, ContentObject_zero_payload);
    LONGBOW_RUN_TEST_CASE(ContentObject, ContentObject_no_payload);
}

LONGBOW_TEST_FIXTURE_SETUP(ContentObject)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(ContentObject)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(ContentObject, ContentObject_RsaSha256_FixedHeader)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_content_nameA_keyid1_rsasha256, sizeof(v1_content_nameA_keyid1_rsasha256), 0, sizeof(v1_content_nameA_keyid1_rsasha256));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCBuffer *fixedHeader = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader);

    // leave the FixedHeader buffer at position 0
    parcBuffer_Rewind(fixedHeader);

    PARCBuffer *trueFixedHeader = parcBuffer_Wrap(v1_content_nameA_keyid1_rsasha256, 8, 0, 8);
    assertTrue(parcBuffer_Equals(fixedHeader, trueFixedHeader), "Buffers not equal")
    {
        printf("Expected\n");
        parcBuffer_Display(trueFixedHeader, 3);
        printf("Got\n");
        parcBuffer_Display(fixedHeader, 3);
    }

    parcBuffer_Release(&trueFixedHeader);
    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(ContentObject, ContentObject_RsaSha256_Name)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_content_nameA_keyid1_rsasha256,
                                               sizeof(v1_content_nameA_keyid1_rsasha256),
                                               0,
                                               sizeof(v1_content_nameA_keyid1_rsasha256));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    CCNxName *name = ccnxTlvDictionary_GetName(dictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_NAME);
    CCNxName *trueName = ccnxName_CreateFromCString(v1_content_nameA_keyid1_rsasha256_URI);

    assertTrue(ccnxName_Equals(name, trueName), "Buffers not equal")
    {
        printf("Expected\n");
        ccnxName_Display(trueName, 3);
        printf("Got\n");
        ccnxName_Display(name, 3);
    }

    ccnxName_Release(&trueName);
    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(ContentObject, ContentObject_RsaSha256_ExpiryTime)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_content_nameA_keyid1_rsasha256, sizeof(v1_content_nameA_keyid1_rsasha256), 0, sizeof(v1_content_nameA_keyid1_rsasha256));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    uint64_t expiryTime = ccnxTlvDictionary_GetInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_EXPIRY_TIME);

    TlvExtent expiryExtent = getTruthTableExtent(TRUTHTABLENAME(v1_content_nameA_keyid1_rsasha256), V1_MANIFEST_OBJ_EXPIRY_TIME);

    parcBuffer_SetPosition(packetBuffer, expiryExtent.offset);
    uint64_t trueTime;
    ccnxCodecTlvUtilities_GetVarInt(packetBuffer, expiryExtent.length, &trueTime);

    assertTrue(expiryTime == trueTime, "Wrong time, expected %" PRIx64 " got %" PRIx64, trueTime, expiryTime);

    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(ContentObject, ContentObject_RsaSha256_ValidationAlg_KeyId)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_content_nameA_keyid1_rsasha256, sizeof(v1_content_nameA_keyid1_rsasha256), 0, sizeof(v1_content_nameA_keyid1_rsasha256));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCBuffer *keyid = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEYID);

    TlvExtent keyidExtent = getTruthTableExtent(TRUTHTABLENAME(v1_content_nameA_keyid1_rsasha256), V1_MANIFEST_OBJ_KEYID);
    PARCBuffer *trueKeyid = parcBuffer_Wrap(v1_content_nameA_keyid1_rsasha256, sizeof(v1_content_nameA_keyid1_rsasha256), keyidExtent.offset, keyidExtent.offset + keyidExtent.length);

    assertTrue(parcBuffer_Equals(keyid, trueKeyid), "Buffers not equal")
    {
        printf("Expected\n");
        parcBuffer_Display(trueKeyid, 3);
        printf("Got\n");
        parcBuffer_Display(keyid, 3);
    }

    parcBuffer_Release(&trueKeyid);
    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(ContentObject, ContentObject_RsaSha256_ValidationPayload)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_content_nameA_keyid1_rsasha256, sizeof(v1_content_nameA_keyid1_rsasha256), 0, sizeof(v1_content_nameA_keyid1_rsasha256));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCBuffer *validationPayload = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_PAYLOAD);

    TlvExtent payloadExtent = getTruthTableExtent(TRUTHTABLENAME(v1_content_nameA_keyid1_rsasha256), V1_MANIFEST_OBJ_SIGBITS);
    PARCBuffer *truePayload = parcBuffer_Wrap(v1_content_nameA_keyid1_rsasha256, sizeof(v1_content_nameA_keyid1_rsasha256), payloadExtent.offset, payloadExtent.offset + payloadExtent.length);

    assertTrue(parcBuffer_Equals(validationPayload, truePayload), "Buffers not equal")
    {
        printf("Expected\n");
        parcBuffer_Display(truePayload, 3);
        printf("Got\n");
        parcBuffer_Display(validationPayload, 3);
    }

    parcBuffer_Release(&truePayload);
    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(ContentObject, ContentObject_zero_payload)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_content_zero_payload, sizeof(v1_content_zero_payload), 0, sizeof(v1_content_zero_payload));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    CCNxName *testName = CCNxContentObjectFacadeV1_Implementation.getName(dictionary);
    assertNotNull(testName, "Got null name on decode");

    PARCBuffer *testPayload = CCNxContentObjectFacadeV1_Implementation.getPayload(dictionary);
    assertNotNull(testPayload, "got null payload")
    assertTrue(parcBuffer_Remaining(testPayload) == 0, "Wrong length, expected 0 got %zu", parcBuffer_Remaining(testPayload));

    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(ContentObject, ContentObject_no_payload)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_content_no_payload, sizeof(v1_content_no_payload), 0, sizeof(v1_content_no_payload));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    CCNxName *test = CCNxContentObjectFacadeV1_Implementation.getName(dictionary);
    assertNotNull(test, "Got null name on decode");

    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}


// =========================================================================

LONGBOW_TEST_FIXTURE(Control)
{
    LONGBOW_RUN_TEST_CASE(Control, CPIAddRoute_Crc32c_FixedHeader);
    LONGBOW_RUN_TEST_CASE(Control, CPIAddRoute_Crc32c_Payload);
    LONGBOW_RUN_TEST_CASE(Control, CPIAddRoute_Crc32c_ValidationAlg_CryptoSuite);
    LONGBOW_RUN_TEST_CASE(Control, CPIAddRoute_Crc32c_ValidationPayload);
}

LONGBOW_TEST_FIXTURE_SETUP(Control)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Control)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Control, CPIAddRoute_Crc32c_FixedHeader)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_cpi_add_route_crc32c, sizeof(v1_cpi_add_route_crc32c), 0, sizeof(v1_cpi_add_route_crc32c));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCBuffer *fixedHeader = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader);

    // leave the FixedHeader buffer at position 0
    parcBuffer_Rewind(fixedHeader);

    PARCBuffer *trueFixedHeader = parcBuffer_Wrap(v1_cpi_add_route_crc32c, 8, 0, 8);
    assertTrue(parcBuffer_Equals(fixedHeader, trueFixedHeader), "Buffers not equal")
    {
        printf("Expected\n");
        parcBuffer_Display(trueFixedHeader, 3);
        printf("Got\n");
        parcBuffer_Display(fixedHeader, 3);
    }

    parcBuffer_Release(&trueFixedHeader);
    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(Control, CPIAddRoute_Crc32c_Payload)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_cpi_add_route_crc32c, sizeof(v1_cpi_add_route_crc32c), 0, sizeof(v1_cpi_add_route_crc32c));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCBuffer *test = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_PAYLOAD);

    TlvExtent validationPayloadExtent = getTruthTableExtent(TRUTHTABLENAME(v1_cpi_add_route_crc32c), V1_MANIFEST_CPI_SIGBITS);
    PARCBuffer *trueValidationPayload = parcBuffer_Wrap(v1_cpi_add_route_crc32c, sizeof(v1_cpi_add_route_crc32c), validationPayloadExtent.offset, validationPayloadExtent.offset + validationPayloadExtent.length);

    assertTrue(parcBuffer_Equals(test, trueValidationPayload), "Buffers not equal")
    {
        printf("Expected\n");
        parcBuffer_Display(trueValidationPayload, 3);
        printf("Got\n");
        parcBuffer_Display(test, 3);
    }

    parcBuffer_Release(&trueValidationPayload);
    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(Control, CPIAddRoute_Crc32c_ValidationAlg_CryptoSuite)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_cpi_add_route_crc32c, sizeof(v1_cpi_add_route_crc32c), 0, sizeof(v1_cpi_add_route_crc32c));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCCryptoSuite test = (PARCCryptoSuite) ccnxValidationFacadeV1_GetCryptoSuite(dictionary);
    PARCCryptoSuite trueCryptoSuite = PARCCryptoSuite_NULL_CRC32C;

    assertTrue(test == trueCryptoSuite, "Wrong crypto suite, expected %d got %d", trueCryptoSuite, test);

    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(Control, CPIAddRoute_Crc32c_ValidationPayload)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_cpi_add_route_crc32c, sizeof(v1_cpi_add_route_crc32c), 0, sizeof(v1_cpi_add_route_crc32c));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCBuffer *validationPayload = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_PAYLOAD);

    TlvExtent payloadExtent = getTruthTableExtent(TRUTHTABLENAME(v1_cpi_add_route_crc32c), V1_MANIFEST_CPI_SIGBITS);
    PARCBuffer *truePayload = parcBuffer_Wrap(v1_cpi_add_route_crc32c, sizeof(v1_cpi_add_route_crc32c), payloadExtent.offset, payloadExtent.offset + payloadExtent.length);

    assertTrue(parcBuffer_Equals(validationPayload, truePayload), "Buffers not equal")
    {
        printf("Expected\n");
        parcBuffer_Display(truePayload, 3);
        printf("Got\n");
        parcBuffer_Display(validationPayload, 3);
    }

    parcBuffer_Release(&truePayload);
    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}


// =========================================================================

LONGBOW_TEST_FIXTURE(Interest)
{
    LONGBOW_RUN_TEST_CASE(Interest, interest_bad_message_length);
    LONGBOW_RUN_TEST_CASE(Interest, interest_all_fields_FixedHeader);
    LONGBOW_RUN_TEST_CASE(Interest, interest_all_fields_Lifetime);
    LONGBOW_RUN_TEST_CASE(Interest, interest_all_fields_Name);
    LONGBOW_RUN_TEST_CASE(Interest, interest_all_fields_ValidationAlg_KeyId);
    LONGBOW_RUN_TEST_CASE(Interest, interest_all_fields_ValidationPayload);
    LONGBOW_RUN_TEST_CASE(Interest, interest_nameA_crc32c_ValidationAlg_CryptoSuite);
    LONGBOW_RUN_TEST_CASE(Interest, interest_nameA_crc32c_ValidationPayload);
    LONGBOW_RUN_TEST_CASE(Interest, ccnxCodecSchemaV1PacketDecoder_BufferDecode);
    LONGBOW_RUN_TEST_CASE(Interest, interest_bad_validation_alg);
    LONGBOW_RUN_TEST_CASE(Interest, interest_validation_alg_overrun);
}

LONGBOW_TEST_FIXTURE_SETUP(Interest)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Interest)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Interest, interest_bad_message_length)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_interest_bad_message_length, sizeof(v1_interest_bad_message_length), 0, sizeof(v1_interest_bad_message_length));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertFalse(success, "Should have seen and error on decode");

    CCNxCodecError *error = ccnxCodecTlvDecoder_GetError(decoder);
    assertNotNull(error, "Error not set when bad decode");

    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(Interest, interest_all_fields_FixedHeader)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_interest_all_fields, sizeof(v1_interest_all_fields), 0, sizeof(v1_interest_all_fields));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCBuffer *fixedHeader = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_FixedHeader);

    // leave the FixedHeader buffer at position 0
    parcBuffer_Rewind(fixedHeader);

    PARCBuffer *trueFixedHeader = parcBuffer_Wrap(v1_interest_all_fields, 8, 0, 8);
    assertTrue(parcBuffer_Equals(fixedHeader, trueFixedHeader), "Buffers not equal")
    {
        printf("Expected\n");
        parcBuffer_Display(trueFixedHeader, 3);
        printf("Got\n");
        parcBuffer_Display(fixedHeader, 3);
    }

    parcBuffer_Release(&trueFixedHeader);
    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(Interest, interest_all_fields_Lifetime)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_interest_all_fields, sizeof(v1_interest_all_fields), 0, sizeof(v1_interest_all_fields));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateContentObject();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    uint64_t lifetime = ccnxTlvDictionary_GetInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_InterestLifetime);

    TlvExtent lifetimeExtent = getTruthTableHeaderExtent(TRUTHTABLENAME(v1_interest_all_fields), V1_MANIFEST_INT_LIFETIME);

    parcBuffer_SetPosition(packetBuffer, lifetimeExtent.offset);
    uint64_t trueTime;
    ccnxCodecTlvUtilities_GetVarInt(packetBuffer, lifetimeExtent.length, &trueTime);

    assertTrue(lifetime == trueTime, "Wrong time, expected %" PRIx64 " got %" PRIx64, trueTime, lifetime);

    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(Interest, interest_all_fields_Name)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_interest_all_fields, sizeof(v1_interest_all_fields), 0, sizeof(v1_interest_all_fields));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    CCNxName *name = ccnxTlvDictionary_GetName(dictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_NAME);
    CCNxName *trueName = ccnxName_CreateFromCString(v1_interest_all_fields_URI);

    assertTrue(ccnxName_Equals(name, trueName), "Buffers not equal")
    {
        printf("Expected\n");
        ccnxName_Display(trueName, 3);
        printf("Got\n");
        ccnxName_Display(name, 3);
    }

    ccnxName_Release(&trueName);
    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

/*
 * This packet does not have a validation section, so the test is that its missing
 */
LONGBOW_TEST_CASE(Interest, interest_all_fields_ValidationAlg_KeyId)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_interest_all_fields, sizeof(v1_interest_all_fields), 0, sizeof(v1_interest_all_fields));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCBuffer *keyid = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_KEYID);
    assertNull(keyid, "Got a non-null keyid from a packet without one");

    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

/*
 * Packet does not have validation, test for missing
 */
LONGBOW_TEST_CASE(Interest, interest_all_fields_ValidationPayload)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_interest_all_fields, sizeof(v1_interest_all_fields), 0, sizeof(v1_interest_all_fields));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCBuffer *payload = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_PAYLOAD);
    assertNull(payload, "Got a non-null validation payload from a packet without one");

    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(Interest, interest_nameA_crc32c_ValidationAlg_CryptoSuite)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_interest_nameA_crc32c, sizeof(v1_interest_nameA_crc32c), 0, sizeof(v1_interest_nameA_crc32c));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCCryptoSuite suite = (PARCCryptoSuite) ccnxTlvDictionary_GetInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_CRYPTO_SUITE);
    PARCCryptoSuite trueSuite = PARCCryptoSuite_NULL_CRC32C;

    assertTrue(suite == trueSuite, "Wrong crypto suite, expected %d got %d", trueSuite, suite);

    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(Interest, interest_nameA_crc32c_ValidationPayload)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_interest_nameA_crc32c, sizeof(v1_interest_nameA_crc32c), 0, sizeof(v1_interest_nameA_crc32c));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertTrue(success, "Error on decode: %s", ccnxCodecError_ToString(ccnxCodecTlvDecoder_GetError(decoder)));

    PARCBuffer *validationPayload = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_PAYLOAD);

    TlvExtent payloadExtent = getTruthTableExtent(TRUTHTABLENAME(v1_interest_nameA_crc32c), V1_MANIFEST_INT_ValidationPayload);
    PARCBuffer *truePayload = parcBuffer_Wrap(v1_interest_nameA_crc32c, sizeof(v1_interest_nameA_crc32c), payloadExtent.offset, payloadExtent.offset + payloadExtent.length);

    assertTrue(parcBuffer_Equals(validationPayload, truePayload), "Buffers not equal")
    {
        printf("Expected\n");
        parcBuffer_Display(truePayload, 3);
        printf("Got\n");
        parcBuffer_Display(validationPayload, 3);
    }

    parcBuffer_Release(&truePayload);
    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(Interest, ccnxCodecSchemaV1PacketDecoder_BufferDecode)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_interest_nameA_crc32c, sizeof(v1_interest_nameA_crc32c), 0, sizeof(v1_interest_nameA_crc32c));

    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    bool success = ccnxCodecSchemaV1PacketDecoder_BufferDecode(packetBuffer, dictionary);
    assertTrue(success, "Error on decode");

    PARCBuffer *validationPayload = ccnxTlvDictionary_GetBuffer(dictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_PAYLOAD);

    TlvExtent payloadExtent = getTruthTableExtent(TRUTHTABLENAME(v1_interest_nameA_crc32c), V1_MANIFEST_INT_ValidationPayload);
    PARCBuffer *truePayload = parcBuffer_Wrap(v1_interest_nameA_crc32c, sizeof(v1_interest_nameA_crc32c), payloadExtent.offset, payloadExtent.offset + payloadExtent.length);

    assertTrue(parcBuffer_Equals(validationPayload, truePayload), "Buffers not equal")
    {
        printf("Expected\n");
        parcBuffer_Display(truePayload, 3);
        printf("Got\n");
        parcBuffer_Display(validationPayload, 3);
    }

    parcBuffer_Release(&truePayload);
    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
}

LONGBOW_TEST_CASE(Interest, interest_bad_validation_alg)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_interest_bad_validation_alg, sizeof(v1_interest_bad_validation_alg), 0, sizeof(v1_interest_bad_validation_alg));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertFalse(success, "Should have seen and error on decode");

    CCNxCodecError *error = ccnxCodecTlvDecoder_GetError(decoder);
    assertNotNull(error, "Error not set when bad decode");

    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

LONGBOW_TEST_CASE(Interest, interest_validation_alg_overrun)
{
    PARCBuffer *packetBuffer = parcBuffer_Wrap(v1_interest_validation_alg_overrun, sizeof(v1_interest_validation_alg_overrun), 0, sizeof(v1_interest_validation_alg_overrun));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(packetBuffer);
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateInterest();
    bool success = ccnxCodecSchemaV1PacketDecoder_Decode(decoder, dictionary);
    assertFalse(success, "Should have seen and error on decode");

    CCNxCodecError *error = ccnxCodecTlvDecoder_GetError(decoder);
    assertNotNull(error, "Error not set when bad decode");

    parcBuffer_Release(&packetBuffer);
    ccnxTlvDictionary_Release(&dictionary);
    ccnxCodecTlvDecoder_Destroy(&decoder);
}

// =========================================================================


int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnxCodecSchemaV1_PacketDecoder);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
