/*
 * Copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1ManifestDecoder_DecodeHashGroupMetadata);
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
    uint8_t rawManifest[40] = { 0x00, 0x07, 0x00, 0x24,
                                0x00, 0x02, 0x00, 0x20,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46 };
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
    uint8_t rawManifest[40] = { 0x00, 0x07, 0x00, 0x24,
                                0x00, 0x02, 0x00, 0x20,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46 };
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
    uint8_t rawManifest[40] = { 0x00, 0x07, 0x00, 0x24,
                                0x00, 0x02, 0x00, 0x20,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46,
                                0x46, 0x46, 0x46, 0x46 };
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
    const PARCBuffer *actualPointer = ccnxManifestHashGroupPointer_GetDigest(pointer);
    assertTrue(parcBuffer_Equals(expectedPointer, actualPointer), "Expected decoded pointer to equal %s, got %s", parcBuffer_ToHexString(expectedPointer), parcBuffer_ToHexString(actualPointer));

    parcBuffer_Release(&expectedPointer);

    ccnxManifestHashGroup_Release(&group);
    ccnxManifest_Release(&dict);
    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&wireFormat);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1ManifestDecoder_DecodeHashGroupMetadata)
{
    // Re-build the expected metadata from the manifest
    CCNxName *groupLocator = ccnxName_CreateFromCString("ccnx:/locator");
    PARCBuffer *digest = parcBuffer_Allocate(16);
    for (size_t i = 0; i < parcBuffer_Limit(digest); i++) {
        parcBuffer_PutUint8(digest, 0);
    }
    parcBuffer_Flip(digest);
    size_t entrySize = 1;
    size_t dataSize = 2;
    size_t blockSize = 3;
    size_t treeHeight = 4;

    // Compute the expected size of this metadata group.
    size_t metadataSize = 4 * (4 + 8) + 4 + parcBuffer_Limit(digest) + 4 + strlen("ccnx:/locator");

    // See test_ccnxCodecSchemaV1_ManifestEncoder.c for the packet construction details.
    uint8_t rawMetadata[89] = { 0x00, CCNxCodecSchemaV1Types_CCNxManifestHashGroup_Metadata,
                                0x00, metadataSize,
                                0x00, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_Locator,
                                0x00, strlen("ccnx:/locator"),
                                'c',  'c',
                                'n',  'x',
                                ':',  '/',
                                'l',  'o',
                                'c',  'a',
                                't',  'o',
                                'r',
                                0x00, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_DataSize,
                                0x00, 0x08,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, dataSize,
                                0x00, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_BlockSize,
                                0x00, 0x08,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, blockSize,
                                0x00, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_EntrySize,
                                0x00, 0x08,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, entrySize,
                                0x00, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_TreeHeight,
                                0x00, 0x08,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, treeHeight,
                                0x00, CCNxCodecSchemaV1Types_CCNxManifestHashGroupMetadata_OverallDataSha256,
                                0x00, parcBuffer_Remaining(digest),
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, 0x00,
                                0x00, 0x00 };
    PARCBuffer *wireFormat = parcBuffer_Flip(parcBuffer_CreateFromArray(rawMetadata, sizeof(rawMetadata)));

    // Create the encoder and swallow the top level container
    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(wireFormat);
    ccnxCodecTlvDecoder_GetType(decoder); // swallow type
    uint16_t length = ccnxCodecTlvDecoder_GetLength(decoder);

    // Decode the metadata
    CCNxManifestHashGroup *group = ccnxManifestHashGroup_Create();
    bool result = _decodeHashGroupMetadata(decoder, group, length);
    assertTrue(result, "Expected hash group metadata to be decoded correctly.");

    const CCNxName *actualLocator = ccnxManifestHashGroup_GetLocator(group);
    size_t actualEntrySize = ccnxManifestHashGroup_GetEntrySize(group);
    size_t actualDataSize = ccnxManifestHashGroup_GetDataSize(group);
    size_t actualBlockSize = ccnxManifestHashGroup_GetBlockSize(group);
    size_t actualTreeHeight = ccnxManifestHashGroup_GetTreeHeight(group);
    const PARCBuffer *actualDigest = ccnxManifestHashGroup_GetOverallDataDigest(group);

    assertTrue(ccnxName_Equals(groupLocator, actualLocator), "Expected decoded locator to equal %s, got %s", ccnxName_ToString(groupLocator), ccnxName_ToString(actualLocator));
    assertTrue(entrySize == actualEntrySize, "Expected %zu entry size, got %zu", entrySize, actualEntrySize);
    assertTrue(dataSize == actualDataSize, "Expected %zu data size, got %zu", dataSize, actualDataSize);
    assertTrue(blockSize == actualBlockSize, "Expected %zu block size, got %zu", blockSize, actualBlockSize);
    assertTrue(treeHeight == actualTreeHeight, "Expected %zu tree height, got %zu", treeHeight, actualTreeHeight);
    assertTrue(parcBuffer_Equals(digest, actualDigest), "Expected %s digest, got %s", parcBuffer_ToHexString(digest), parcBuffer_ToHexString(actualDigest));

    parcBuffer_Release(&digest);
    ccnxName_Release(&groupLocator);

    ccnxManifestHashGroup_Release(&group);
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
