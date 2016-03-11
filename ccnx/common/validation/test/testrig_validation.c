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
 * Common routines to test validators
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#include <ccnx/common/ccnx_KeyLocator.h>
#include <ccnx/common/internal/ccnx_ValidationFacadeV1.h>

#include <ccnx/common/ccnx_ContentObject.h>

#include <parc/algol/parc_Buffer.h>

typedef struct test_data {
    PARCBuffer   *keyid;
    PARCBuffer   *key;
    PARCBuffer   *cert;
    CCNxName     *keyname;

    CCNxKeyLocator *locatorByKey;
    CCNxKeyLocator *locatorByName;
} TestData;

PARCBuffer *
bufferFromString(size_t length, const char string[length])
{
    return parcBuffer_Flip(parcBuffer_PutArray(parcBuffer_Allocate(length), length, (const uint8_t *) string));
}

TestData *
testData_Create(void)
{
    char keyidString[] = "the keyid";
    char keyString[] = "Memory, all alone in the moonlight";
    char certString[] = "The quick brown fox";

    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(TestData));

    data->keyid = bufferFromString(sizeof(keyidString), keyidString);
    data->key = bufferFromString(sizeof(keyString), keyString);
    data->cert = bufferFromString(sizeof(certString), certString);
    data->keyname = ccnxName_CreateFromCString("lci:/lazy/dog");

    PARCBuffer *bb_id = parcBuffer_Wrap("choo choo", 9, 0, 9);
    PARCKeyId *keyid = parcKeyId_Create(bb_id);
    parcBuffer_Release(&bb_id);

    PARCKey *key = parcKey_CreateFromDerEncodedPublicKey(keyid, PARCSigningAlgorithm_RSA, data->key);

    data->locatorByKey = ccnxKeyLocator_CreateFromKey(key);
    parcKey_Release(&key);
    parcKeyId_Release(&keyid);

    CCNxLink *link = ccnxLink_Create(data->keyname, NULL, NULL);
    data->locatorByName = ccnxKeyLocator_CreateFromKeyLink(link);
    ccnxLink_Release(&link);

    return data;
}

void
testData_Release(TestData **dataPtr)
{
    TestData *data = *dataPtr;

    ccnxKeyLocator_Release(&data->locatorByKey);
    ccnxKeyLocator_Release(&data->locatorByName);
    ccnxName_Release(&data->keyname);
    parcBuffer_Release(&data->cert);
    parcBuffer_Release(&data->key);
    parcBuffer_Release(&data->keyid);

    parcMemory_Deallocate((void **) &data);
    *dataPtr = NULL;
}

TestData *
commonSetup(void)
{
    TestData *data = testData_Create();
    return data;
}

int
commonTeardown(TestData *data)
{
    testData_Release(&data);
    return 0;
}

// === V1

void
testValidationSetV1_NoParam(TestData *data, bool (*set)(CCNxTlvDictionary *message), bool (*test)(const CCNxTlvDictionary *message))
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/parc/validation/test");
    CCNxTlvDictionary *packetV1 = ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
                                                                             name,
                                                                             CCNxPayloadType_DATA,
                                                                             NULL);
    bool success = set(packetV1);
    assertTrue(success, "Failed to set on V1");

    bool testResult = test(packetV1);
    assertTrue(testResult, "Test function failed on V1 packet");

    ccnxName_Release(&name);
    ccnxTlvDictionary_Release(&packetV1);
}

void
testValidationSetV1_KeyId_Null(TestData *data, bool (*set)(CCNxTlvDictionary *message, const PARCBuffer *keyid), bool (*test)(const CCNxTlvDictionary *message))
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/parc/validation/test");
    CCNxTlvDictionary *packetV1 = ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
                                                                             name,
                                                                             CCNxPayloadType_DATA,
                                                                             NULL);
    bool success = set(packetV1, NULL);
    assertTrue(success, "Failed to set on V1");

    bool testResult = test(packetV1);
    assertTrue(testResult, "Test function failed on V1 packet");

    ccnxName_Release(&name);
    ccnxTlvDictionary_Release(&packetV1);
}

void
testValidationSetV1_KeyId_KeyId(TestData *data, bool (*set)(CCNxTlvDictionary *message, const PARCBuffer *keyid), bool (*test)(const CCNxTlvDictionary *message))
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/parc/validation/test");
    CCNxTlvDictionary *packetV1 = ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
                                                                             name,
                                                                             CCNxPayloadType_DATA,
                                                                             NULL);
    bool success = set(packetV1, data->keyid);
    assertTrue(success, "Failed to set on V1");

    bool testResult = test(packetV1);
    assertTrue(testResult, "Test function failed on V1 packet");

    PARCBuffer *testKeyId = ccnxValidationFacadeV1_GetKeyId(packetV1);
    assertTrue(parcBuffer_Equals(testKeyId, data->keyid), "keyid not equal");

    ccnxName_Release(&name);
    ccnxTlvDictionary_Release(&packetV1);
}

void
testValidationSetV1_KeyId_KeyLocator_Null_Null(TestData *data,
                                               bool (*set)(CCNxTlvDictionary *message, const PARCBuffer *keyid,
                                                           const CCNxKeyLocator *keyLocator),
                                               bool (*test)(const CCNxTlvDictionary *message))
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/parc/validation/test");
    CCNxTlvDictionary *packetV1 = ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
                                                                             name,
                                                                             CCNxPayloadType_DATA,
                                                                             NULL);
    bool success = set(packetV1, NULL, NULL);
    assertTrue(success, "Failed to set on V1");

    bool testResult = test(packetV1);
    assertTrue(testResult, "Test function failed on V1 packet");

    ccnxName_Release(&name);
    ccnxTlvDictionary_Release(&packetV1);
}

void
testValidationSetV1_KeyId_KeyLocator_KeyId_Null(TestData *data,
                                                bool (*set)(CCNxTlvDictionary *message, const PARCBuffer *keyid,
                                                            const CCNxKeyLocator *keyLocator),
                                                bool (*test)(const CCNxTlvDictionary *message))
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/parc/validation/test");
    CCNxTlvDictionary *packetV1 = ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
                                                                             name,
                                                                             CCNxPayloadType_DATA,
                                                                             NULL);
    bool success = set(packetV1, data->keyid, NULL);
    assertTrue(success, "Failed to set on V1");

    bool testResult = test(packetV1);
    assertTrue(testResult, "Test function failed on V1 packet");

    PARCBuffer *testKeyId = ccnxValidationFacadeV1_GetKeyId(packetV1);
    assertTrue(parcBuffer_Equals(testKeyId, data->keyid), "keyid not equal");

    ccnxName_Release(&name);
    ccnxTlvDictionary_Release(&packetV1);
}

void
testValidationSetV1_KeyId_KeyLocator_KeyId_Key(TestData *data,
                                               bool (*set)(CCNxTlvDictionary *message, const PARCBuffer *keyid,
                                                           const CCNxKeyLocator *keyLocator),
                                               bool (*test)(const CCNxTlvDictionary *message))
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/parc/validation/test");
    CCNxTlvDictionary *packetV1 = ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
                                                                             name,
                                                                             CCNxPayloadType_DATA,
                                                                             NULL);
    bool success = set(packetV1, data->keyid, data->locatorByKey);
    assertTrue(success, "Failed to set on V1");

    bool testResult = test(packetV1);
    assertTrue(testResult, "Test function failed on V1 packet");

    PARCBuffer *testKeyId = ccnxValidationFacadeV1_GetKeyId(packetV1);
    assertTrue(parcBuffer_Equals(testKeyId, data->keyid), "keyid not equal");

    PARCBuffer *testKey = ccnxValidationFacadeV1_GetPublicKey(packetV1);
    assertTrue(parcBuffer_Equals(testKey, data->key), "keys not equal");

    ccnxName_Release(&name);
    ccnxTlvDictionary_Release(&packetV1);
}

void
testValidationSetV1_KeyId_KeyLocator_KeyId_KeyName(TestData *data,
                                                   bool (*set)(CCNxTlvDictionary *message, const PARCBuffer *keyid,
                                                               const CCNxKeyLocator *keyLocator),
                                                   bool (*test)(const CCNxTlvDictionary *message))
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/parc/validation/test");
    CCNxTlvDictionary *packetV1 = ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
                                                                             name,
                                                                             CCNxPayloadType_DATA,
                                                                             NULL);
    bool success = set(packetV1, data->keyid, data->locatorByName);
    assertTrue(success, "Failed to set on V1");

    bool testResult = test(packetV1);
    assertTrue(testResult, "Test function failed on V1 packet");

    PARCBuffer *testKeyId = ccnxValidationFacadeV1_GetKeyId(packetV1);
    assertTrue(parcBuffer_Equals(testKeyId, data->keyid), "keyid not equal");

    // XXX: TODO: GetKeyName() returns a Link, so it should be GetLink().
    //            It also creates a new object (the CCNxLink), so... needs thinking about.
    //            See BugzId: 3322

    CCNxLink *testLink = ccnxValidationFacadeV1_GetKeyName(packetV1);
    assertTrue(ccnxName_Equals(ccnxLink_GetName(testLink), data->keyname), "Keynames not equal");
    ccnxLink_Release(&testLink);

    ccnxName_Release(&name);
    ccnxTlvDictionary_Release(&packetV1);
}

// === General test for public key algs

void
testValidationSet_KeyId_KeyLocator(TestData *data, bool (*set)(CCNxTlvDictionary *message, const PARCBuffer *keyid, const CCNxKeyLocator *keyLocator), bool (*test)(const CCNxTlvDictionary *message), bool v0ok, bool v1ok)
{
    if (v1ok) {
        testValidationSetV1_KeyId_KeyLocator_Null_Null(data, set, test);
        testValidationSetV1_KeyId_KeyLocator_KeyId_Null(data, set, test);
        testValidationSetV1_KeyId_KeyLocator_KeyId_Key(data, set, test);
        testValidationSetV1_KeyId_KeyLocator_KeyId_KeyName(data, set, test);
    }
}

void
testValidationSet_KeyId(TestData *data, bool (*set)(CCNxTlvDictionary *message, const PARCBuffer *keyid), bool (*test)(const CCNxTlvDictionary *message), bool v0ok, bool v1ok)
{
    if (v1ok) {
        testValidationSetV1_KeyId_Null(data, set, test);
        testValidationSetV1_KeyId_KeyId(data, set, test);
    }
}

void
testValidationSet_NoParam(TestData *data, bool (*set)(CCNxTlvDictionary *message), bool (*test)(const CCNxTlvDictionary *message), bool v0ok, bool v1ok)
{
    if (v1ok) {
        testValidationSetV1_NoParam(data, set, test);
        testValidationSetV1_NoParam(data, set, test);
    }
}
