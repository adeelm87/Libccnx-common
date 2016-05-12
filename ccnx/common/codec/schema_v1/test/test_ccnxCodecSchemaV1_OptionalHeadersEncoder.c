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
#include "../ccnxCodecSchemaV1_OptionalHeadersEncoder.c"
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

#include <ccnx/common/codec/schema_v1/testdata/v1_interest_nameA.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_content_nameA_crc32c.h>

#include "testrig_encoder.c"

static TruthTableEntry
TRUTHTABLENAME(interest_optional_headers)[] =
{
    { .wellKnownType = false, .indexOrKey = V1_MANIFEST_INT_OPTHEAD,  .bodyManifest = true,  .extent = { 8,  28 } }, // index = 0
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_E2EFRAG,  .bodyManifest = false, .extent = { 12, 12 } }, // index = 1
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_LIFETIME, .bodyManifest = false, .extent = { 28, 8  } }, // index = 2
    { .wellKnownType = false, .indexOrKey = T_INVALID,                .extent       = { 0,   0 } },
};

static TruthTableEntry
TRUTHTABLENAME(contentobject_optional_headers)[] =
{
    { .wellKnownType = false, .indexOrKey = V1_MANIFEST_OBJ_OPTHEAD,              .bodyManifest = true,  .extent = { 8,  36 } }, // 0
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_OBJ_E2EFRAG,              .bodyManifest = false, .extent = { 12, 20 } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_OBJ_RecommendedCacheTime, .bodyManifest = false, .extent = { 36, 8  } },
    { .wellKnownType = false, .indexOrKey = T_INVALID,                            .extent       = { 0,   0 } },
};


LONGBOW_TEST_RUNNER(ccnxCodecSchemaV0_OptionalHeadersEncoder)
{
    LONGBOW_RUN_TEST_FIXTURE(Interest);
    LONGBOW_RUN_TEST_FIXTURE(ContentObject);
    LONGBOW_RUN_TEST_FIXTURE(UnknownType);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnxCodecSchemaV0_OptionalHeadersEncoder)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnxCodecSchemaV0_OptionalHeadersEncoder)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ==================================================================================

LONGBOW_TEST_FIXTURE(Interest)
{
    LONGBOW_RUN_TEST_CASE(Interest, ccnxCodecSchemaV1OptionalHeadersEncoder_Encode);
}

LONGBOW_TEST_FIXTURE_SETUP(Interest)
{
    longBowTestCase_SetClipBoardData(testCase,
                                 commonSetup(v1_interest_nameA,
                                             sizeof(v1_interest_nameA),
                                             TRUTHTABLENAME(interest_optional_headers),
                                             V1_MANIFEST_INT_OPTHEAD));

    // commonSetup will not add headers...
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    addBuffer(data, data->dictionary, TRUTHTABLENAME(interest_optional_headers)[1].extent.offset, TRUTHTABLENAME(interest_optional_headers)[1].extent.offset + TRUTHTABLENAME(interest_optional_headers)[1].extent.length, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_INTFRAG);

    addBuffer(data, data->dictionary, TRUTHTABLENAME(interest_optional_headers)[2].extent.offset, TRUTHTABLENAME(interest_optional_headers)[2].extent.offset + TRUTHTABLENAME(interest_optional_headers)[2].extent.length, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_InterestLifetime);

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Interest)
{
    testrigencoder_CommonTeardown(longBowTestCase_GetClipBoardData(testCase));

    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Interest, ccnxCodecSchemaV1OptionalHeadersEncoder_Encode)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ssize_t length = ccnxCodecSchemaV1OptionalHeadersEncoder_Encode(data->encoder, data->dictionary);
    assertTrue(length >= 0, "Error on Encode: length %zd", length);
    testCompareEncoderToBuffer(data->encoder, data->memoryRegion);
}

// ==================================================================================

LONGBOW_TEST_FIXTURE(ContentObject)
{
    LONGBOW_RUN_TEST_CASE(ContentObject, ccnxCodecSchemaV1OptionalHeadersEncoder_Encode);
}

LONGBOW_TEST_FIXTURE_SETUP(ContentObject)
{
    longBowTestCase_SetClipBoardData(testCase,
                                 commonSetup(v1_content_nameA_crc32c,
                                             sizeof(v1_content_nameA_crc32c),
                                             TRUTHTABLENAME(contentobject_optional_headers),
                                             V1_MANIFEST_OBJ_OPTHEAD));

    // commonSetup will not add headers...
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    addBuffer(data, data->dictionary, TRUTHTABLENAME(contentobject_optional_headers)[1].extent.offset, TRUTHTABLENAME(contentobject_optional_headers)[1].extent.offset + TRUTHTABLENAME(contentobject_optional_headers)[1].extent.length, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_OBJFRAG);

    addBuffer(data, data->dictionary, TRUTHTABLENAME(contentobject_optional_headers)[2].extent.offset, TRUTHTABLENAME(contentobject_optional_headers)[2].extent.offset + TRUTHTABLENAME(contentobject_optional_headers)[2].extent.length, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_RecommendedCacheTime);


    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(ContentObject)
{
    testrigencoder_CommonTeardown(longBowTestCase_GetClipBoardData(testCase));

    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(ContentObject, ccnxCodecSchemaV1OptionalHeadersEncoder_Encode)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    ssize_t length = ccnxCodecSchemaV1OptionalHeadersEncoder_Encode(data->encoder, data->dictionary);
    assertTrue(length >= 0, "Error on Encode: length %zd", length);
    testCompareEncoderToBuffer(data->encoder, data->memoryRegion);
}

// ==================================================================================

LONGBOW_TEST_FIXTURE(UnknownType)
{
    LONGBOW_RUN_TEST_CASE(UnknownType, Unknown);
}

LONGBOW_TEST_FIXTURE_SETUP(UnknownType)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(UnknownType)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(UnknownType, Unknown)
{
    CCNxTlvDictionary *unknown = ccnxTlvDictionary_Create(CCNxCodecSchemaV1TlvDictionary_MessageFastArray_END, 1);

    CCNxCodecTlvEncoder *encoder = ccnxCodecTlvEncoder_Create();
    ssize_t length = ccnxCodecSchemaV1OptionalHeadersEncoder_Encode(encoder, unknown);
    assertTrue(length < 0, "Did not get error return when encoding unknown type");

    CCNxCodecError *error = ccnxCodecTlvEncoder_GetError(encoder);
    assertNotNull(error, "encoder did not set the error");

    ccnxCodecTlvEncoder_Destroy(&encoder);
    ccnxTlvDictionary_Release(&unknown);
}

// ==================================================================================


int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnxCodecSchemaV0_OptionalHeadersEncoder);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
