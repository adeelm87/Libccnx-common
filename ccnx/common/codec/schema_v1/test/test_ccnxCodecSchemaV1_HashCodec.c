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
#include "../ccnxCodecSchemaV1_HashCodec.c"
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

#include <ccnx/common/codec/ccnxCodec_Error.h>

LONGBOW_TEST_RUNNER(ccnxTlvCodec_Hash)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnxTlvCodec_Hash)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnxTlvCodec_Hash)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxTlvCodecHash_DecodeValue);
    // LONGBOW_RUN_TEST_CASE(Global, ccnxTlvCodecHash_DecodeValue_InvalidHash);
    // LONGBOW_RUN_TEST_CASE(Global, ccnxTlvCodecHash_DecodeValue_InvalidLength);

    LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1LinkCodec_Encode);
    // LONGBOW_RUN_TEST_CASE(Global, ccnxCodecSchemaV1LinkCodec_Encode_InvalidLength);
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

LONGBOW_TEST_CASE(Global, ccnxTlvCodecHash_DecodeValue)
{
    // -- 32-byte hash
    uint8_t encoded[] = {
        0x00, CCNxCodecSchemaV1Types_HashType_SHA256, 0x00, 0x20,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    PARCBuffer *tlvBuffer = parcBuffer_Wrap(encoded, sizeof(encoded), 0, sizeof(encoded));
    PARCBuffer *payloadBuffer = parcBuffer_Wrap(encoded + 4, sizeof(encoded) - 4, 0, sizeof(encoded) - 4);

    CCNxCodecTlvDecoder *decoder = ccnxCodecTlvDecoder_Create(tlvBuffer);
    PARCCryptoHash *hash = ccnxCodecSchemaV1LinkCodec_DecodeValue(decoder, sizeof(encoded));
    assertNotNull(hash, "got non-null link when it should have been an error (null)");

    assertTrue(parcCryptoHash_GetDigestType(hash) == PARCCryptoHash_SHA256, "Expected to decode the correct hash type.");
    assertTrue(parcBuffer_Equals(payloadBuffer, parcCryptoHash_GetDigest(hash)), "Expected the digest to match.");

    CCNxCodecError *error = ccnxCodecTlvDecoder_GetError(decoder);
    assertNull(error, "Got null error when it should have been set");

    parcCryptoHash_Release(&hash);
    ccnxCodecTlvDecoder_Destroy(&decoder);
    parcBuffer_Release(&tlvBuffer);
}

LONGBOW_TEST_CASE(Global, ccnxCodecSchemaV1LinkCodec_Encode)
{
    uint8_t encoded[] = {
        0x00, CCNxCodecSchemaV1Types_HashType_SHA256, 0x00, 0x20,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    PARCBuffer *trueEncoding = parcBuffer_Wrap(encoded, sizeof(encoded), 0, sizeof(encoded));

    // Create the hash
    PARCBuffer *payloadBuffer = parcBuffer_Allocate(0x20);
    PARCCryptoHash *expectedHash = parcCryptoHash_Create(PARCCryptoHash_SHA256, payloadBuffer);

    // Encode it
    CCNxCodecTlvEncoder *encoder = ccnxCodecTlvEncoder_Create();
    ssize_t length = ccnxCodecSchemaV1LinkCodec_Encode(encoder, link);
    assertFalse(length < 0, "Got error on encode: %s", ccnxCodecError_ToString(ccnxCodecTlvEncoder_GetError(encoder)));
    assertTrue(length == sizeof(encoded), "Wrong length, expected %zd got %zd", sizeof(encoded), length);

    // Check for equality
    ccnxCodecTlvEncoder_Finalize(encoder);
    PARCBuffer *testEncoding = ccnxCodecTlvEncoder_CreateBuffer(encoder);
    assertTrue(parcBuffer_Equals(trueEncoding, testEncoding), "The hash was encoded incorrectly.")
    {
        printf("Expected\n");
        parcBuffer_Display(trueEncoding, 3);
        printf("Got\n");
        parcBuffer_Display(testEncoding, 3);
    }

    parcBuffer_Release(&testEncoding);
    parcBuffer_Release(&trueEncoding);

    parcCryptoHash_Release(&expectedHash);
    parcBuffer_Release(&payloadBuffer);

    ccnxCodecTlvEncoder_Destroy(&encoder);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnxTlvCodec_Hash);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
