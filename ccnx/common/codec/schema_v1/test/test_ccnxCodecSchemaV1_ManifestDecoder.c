/*
 * Copyright (c) 2016, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../ccnxCodecSchemaV1_ManifestDecoder.c"
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

#include "testrig_encoder.c"

#include <ccnx/common/ccnx_Manifest.h>

// =========================================================================

LONGBOW_TEST_RUNNER(ccnxCodecSchemaV1_ManifestDecoder)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnxCodecSchemaV1_ManifestDecoder)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnxCodecSchemaV1_ManifestDecoder)
{
    return LONGBOW_STATUS_SUCCEEDED;
}


// =========================================================================

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1ManifestDecoder_Decode);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1ManifestDecoder_DecodeType);
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1ManifestDecoder_DecodeHashGroup);
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

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1ManifestDecoder_Decode)
{
    // TODO: construction comment
    uint8_t rawManifest[40] = {0x00,0x07,0x00,0x24,0x00,0x02,0x00,0x20,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46};
    PARCBuffer *wireFormat = parcBuffer_Flip(parcBuffer_CreateFromArray(rawManifest, 40));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(wireFormat);
    CCNxTlvDictionary *dict = ccnxCodecSchemaV1TlvDictionary_CreateManifest();

    bool result = ccnxCodecSchemaV1ManifestDecoder_Decode(decoder, dict);
    assertTrue(result, "Expected the manifest to be decoded correctly");

    ccnxTlvDictionary_Release(&dict);
    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&wireFormat);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1ManifestDecoder_DecodeType)
{
    // TODO: construction comment
    uint8_t rawManifest[40] = {0x00,0x07,0x00,0x24,0x00,0x02,0x00,0x20,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46};
    PARCBuffer *wireFormat = parcBuffer_Flip(parcBuffer_CreateFromArray(rawManifest, 40));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(wireFormat);
    CCNxTlvDictionary *dict = ccnxCodecSchemaV1TlvDictionary_CreateManifest();

    uint16_t type = ccnxCodecTlvDecoder_GetType(decoder);
    uint16_t length = ccnxCodecTlvDecoder_GetLength(decoder);

    bool result = _decodeType(decoder, dict, type, length);
    assertTrue(result, "Expected the manifest type to be correctly decoded at the top level");

    ccnxManifest_Release(&dict);
    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&wireFormat);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1ManifestDecoder_DecodeHashGroup)
{
    // TODO: construction comment
    uint8_t rawManifest[40] = {0x00,0x07,0x00,0x24,0x00,0x02,0x00,0x20,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46};
    PARCBuffer *wireFormat = parcBuffer_Flip(parcBuffer_CreateFromArray(rawManifest, 40));

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(wireFormat);
    CCNxTlvDictionary *dict = ccnxCodecSchemaV1TlvDictionary_CreateManifest();

    ccnxCodecTlvDecoder_GetType(decoder); // swallow type
    uint16_t length = ccnxCodecTlvDecoder_GetLength(decoder);

    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    bool result = _decodeHashGroup(decoder, dict, group, length);
    assertTrue(result, "Expected hash group to be decoded correctly.");

    PARCBuffer *expectedPointer = parcBuffer_AllocateCString("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    CCNxManifestHashGroupPointer *pointer = ccnxManifestHashGroup_GetPointerAtIndex(group, 0);
    PARCBuffer *actualPointer = ccnxManifestHashGroupPointer_GetDigest(pointer);
    assertTrue(parcBuffer_Equals(expectedPointer, actualPointer), "Expected decoded pointer to equal %s, got %s", parcBuffer_ToHexString(expectedPointer), parcBuffer_ToHexString(actualPointer));

    parcBuffer_Release(&expectedPointer);

    ccnxManifestHashGroup_Release(&group);
    ccnxManifest_Release(&dict);
    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&wireFormat);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnxCodecSchemaV1_ManifestDecoder);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
