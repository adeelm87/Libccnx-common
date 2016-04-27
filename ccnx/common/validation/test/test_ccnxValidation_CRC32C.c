/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
/**
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../ccnxValidation_CRC32C.c"
#include <parc/algol/parc_SafeMemory.h>

#include <LongBow/unit-test.h>
#include "testrig_validation.c"

#include <sys/time.h>

/*
 * Ground truth set derived from CRC RevEng http://reveng.sourceforge.net
 * e.g. reveng -c  -m CRC-32C 313233343536373839 gives the canonical check value 0xe306928e
 *
 * You can also calcaulate them online at http://www.zorc.breitbandkatze.de/crc.html using
 * CRC polynomial 0x1EDC6F41, init 0xFFFFFFFF, final 0xFFFFFFFF, reverse data bytes (check),
 * and reverse CRC result before final XOR (check).
 *
 */
struct test_vector {
    uint32_t crc32c;
    int length;
    uint8_t *buffer;
} vectors[] = {
    { .crc32c = 0xe3069283, .length = 9,  .buffer = (uint8_t []) { '1',  '2', '3', '4', '5', '6', '7', '8', '9' } },
    { .crc32c = 0xddb65633, .length = 1,  .buffer = (uint8_t []) { 0x3D } },
    { .crc32c = 0xc203c1fd, .length = 2,  .buffer = (uint8_t []) { 0x3D, 0x41 } },
    { .crc32c = 0x80a9d169, .length = 3,  .buffer = (uint8_t []) { 'b',  'e', 'e' } },
    { .crc32c = 0xa099f534, .length = 4,  .buffer = (uint8_t []) { 'h',  'e', 'l', 'l' } },
    { .crc32c = 0x9a71bb4c, .length = 5,  .buffer = (uint8_t []) { 'h',  'e', 'l', 'l', 'o' } },
    { .crc32c = 0x2976E503, .length = 6,  .buffer = (uint8_t []) { 'g',  'r', 'u', 'm', 'p', 'y' } },
    { .crc32c = 0xe627f441, .length = 7,  .buffer = (uint8_t []) { 'a',  'b', 'c', 'd', 'e', 'f', 'g' } },
    { .crc32c = 0x2d265c1d, .length = 13, .buffer = (uint8_t []) { 'a',  'b', 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'c', 'd', 'e', 'f'} },
    { .crc32c = 0,          .length = 0,  .buffer = NULL }
};

LONGBOW_TEST_RUNNER(ccnxValidation_CRC32C)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnxValidation_CRC32C)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnxValidation_CRC32C)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ===========================================================

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxValidationCRC32C_Set);
    LONGBOW_RUN_TEST_CASE(Global, ccnxValidationCRC32C_CreateSigner);
    LONGBOW_RUN_TEST_CASE(Global, ccnxValidationCRC32C_CreateVerifier);
    LONGBOW_RUN_TEST_CASE(Global, ccnxValidationCRC32C_DictionaryCryptoSuiteValue);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    longBowTestCase_SetClipBoardData(testCase, commonSetup());
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    commonTeardown(longBowTestCase_GetClipBoardData(testCase));

    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxValidationCRC32C_Set)
{
    // do not test on V0 packets, no support
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    testValidationSet_NoParam(data, ccnxValidationCRC32C_Set, ccnxValidationCRC32C_Test, false, true);
}

LONGBOW_TEST_CASE(Global, ccnxValidationCRC32C_CreateSigner)
{
    PARCSigner *signer = ccnxValidationCRC32C_CreateSigner();
    assertNotNull(signer, "Got null signer");

    // now run all the test vectors through it

    for (int i = 0; vectors[i].buffer != NULL; i++) {
        PARCCryptoHasher *hasher = parcSigner_GetCryptoHasher(signer);

        parcCryptoHasher_Init(hasher);
        parcCryptoHasher_UpdateBytes(hasher, vectors[i].buffer, vectors[i].length);
        PARCCryptoHash *hash = parcCryptoHasher_Finalize(hasher);

        PARCSignature *sig = parcSigner_SignDigest(signer, hash);
        PARCBuffer *sigbits = parcSignature_GetSignature(sig);
        uint32_t testCrc = parcBuffer_GetUint32(sigbits);
        assertTrue(testCrc == vectors[i].crc32c,
                   "CRC32C values wrong, index %d got 0x%08x expected 0x%08x\n",
                   i, testCrc, vectors[i].crc32c);

        parcSignature_Release(&sig);
        parcCryptoHash_Release(&hash);
    }

    parcSigner_Release(&signer);
}

LONGBOW_TEST_CASE(Global, ccnxValidationCRC32C_CreateVerifier)
{
    PARCSigner *signer = ccnxValidationCRC32C_CreateSigner();
    assertNotNull(signer, "Got null signer");

    PARCVerifier *verifier = ccnxValidationCRC32C_CreateVerifier();
    assertNotNull(verifier, "Got null verifier");

    for (int i = 0; vectors[i].buffer != NULL; i++) {
        // Produce the signature
        PARCSignature *sig = NULL;
        {
            PARCCryptoHasher *signingHasher = parcSigner_GetCryptoHasher(signer);
            parcCryptoHasher_Init(signingHasher);
            parcCryptoHasher_UpdateBytes(signingHasher, vectors[i].buffer, vectors[i].length);
            PARCCryptoHash *signingHash = parcCryptoHasher_Finalize(signingHasher);
            sig = parcSigner_SignDigest(signer, signingHash);
            parcCryptoHash_Release(&signingHash);
        }

        // Now do the verification stage
        PARCCryptoHash *verifierHash = NULL;
        {
            PARCCryptoHasher *verifyHasher = parcVerifier_GetCryptoHasher(verifier, NULL, PARC_HASH_CRC32C);
            parcCryptoHasher_Init(verifyHasher);
            parcCryptoHasher_UpdateBytes(verifyHasher, vectors[i].buffer, vectors[i].length);
            verifierHash = parcCryptoHasher_Finalize(verifyHasher);
        }

        bool success = parcVerifier_VerifyDigestSignature(verifier, NULL, verifierHash, PARCCryptoSuite_NULL_CRC32C, sig);

        assertTrue(success,
                   "Failed to verify signature, index %d expected 0x%08x\n",
                   i, vectors[i].crc32c);

        parcSignature_Release(&sig);
        parcCryptoHash_Release(&verifierHash);
    }
    parcSigner_Release(&signer);
    parcVerifier_Release(&verifier);
}

LONGBOW_TEST_CASE(Global, ccnxValidationCRC32C_DictionaryCryptoSuiteValue)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    CCNxTlvDictionary *dictionary = ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
                                                                               data->keyname,
                                                                               CCNxPayloadType_DATA,
                                                                               NULL);
    ccnxValidationCRC32C_Set(dictionary);
    uint64_t cryptosuite = ccnxTlvDictionary_GetInteger(dictionary, CCNxCodecSchemaV1TlvDictionary_ValidationFastArray_CRYPTO_SUITE);
    assertTrue(cryptosuite == PARCCryptoSuite_NULL_CRC32C, "Unexpected PARCCryptoSuite value in dictionary");

    ccnxTlvDictionary_Release(&dictionary);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnxValidation_CRC32C);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
