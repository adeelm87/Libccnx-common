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
 * @author Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../ccnx_KeyLocator.c"

#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

#include <parc/algol/parc_SafeMemory.h>

LONGBOW_TEST_RUNNER(ccnx_KeyLocator)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_KeyLocator)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_KeyLocator)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxKeyLocator_Copy);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKeyLocator_Destroy);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKeyLocator_Equals);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKeyLocator_FromKey);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKeyLocator_FromKeyLink);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKeyLocator_GetKey);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKeyLocator_GetKeyName);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKeyLocator_GetType);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKeyLocator_IsKey);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKeyLocator_IsKeyName);
    LONGBOW_RUN_TEST_CASE(Global, ccnxKeyLocator_ToString);
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

LONGBOW_TEST_CASE(Global, ccnxKeyLocator_Copy)
{
    CCNxName *keyURIName = ccnxName_CreateFromCString("lci://name");
    CCNxLink *keyLink = ccnxLink_Create(keyURIName, NULL, NULL);
    CCNxKeyLocator *keyLocator = ccnxKeyLocator_CreateFromKeyLink(keyLink);

    ccnxKeyLocator_AssertValid(keyLocator);

    CCNxKeyLocator *copy = ccnxKeyLocator_Copy(keyLocator);
    assertTrue(ccnxKeyLocator_Equals(copy, keyLocator), "Expected orig and copy to be the same");

    ccnxKeyLocator_Release(&keyLocator);
    ccnxKeyLocator_Release(&copy);

    ccnxName_Release(&keyURIName);
    ccnxLink_Release(&keyLink);

    // Try FromHexString, too, for yucks.
    PARCBuffer *keyBuffer = parcBuffer_WrapCString("hello world");
    PARCKeyId *keyId = parcKeyId_Create(keyBuffer);
    PARCKey *key = parcKey_CreateFromDerEncodedPublicKey(keyId, PARCSigningAlgorithm_RSA, keyBuffer);
    parcKeyId_Release(&keyId);
    parcBuffer_Release(&keyBuffer);
    CCNxKeyLocator *keyLocator2 = ccnxKeyLocator_CreateFromKey(key);

    CCNxKeyLocator *copy2 = ccnxKeyLocator_Copy(keyLocator2);
    assertTrue(ccnxKeyLocator_Equals(copy, keyLocator), "Expected orig and copy to be the same");

    parcKey_Release(&key);
    ccnxKeyLocator_Release(&keyLocator2);
    ccnxKeyLocator_Release(&copy2);
}

LONGBOW_TEST_CASE(Global, ccnxKeyLocator_Destroy)
{
    CCNxName *keyURIName = ccnxName_CreateFromCString("lci://name");
    CCNxLink *keyLink = ccnxLink_Create(keyURIName, NULL, NULL);
    CCNxKeyLocator *keyLocator = ccnxKeyLocator_CreateFromKeyLink(keyLink);

    ccnxKeyLocator_AssertValid(keyLocator);

    ccnxKeyLocator_Release(&keyLocator);
    assertNull(keyLocator, "keyLocator was not nulled out by Release()");

    ccnxLink_Release(&keyLink);
    ccnxName_Release(&keyURIName);
}

LONGBOW_TEST_CASE(Global, ccnxKeyLocator_Equals)
{
    char *hexString = "ABCDEF1234";
    PARCBuffer *keyBuffer = parcBuffer_WrapCString(hexString);
    PARCKeyId *keyId = parcKeyId_Create(keyBuffer);
    PARCKey *key1 = parcKey_CreateFromDerEncodedPublicKey(keyId, PARCSigningAlgorithm_RSA, keyBuffer);
    CCNxKeyLocator *keyLocator1 = ccnxKeyLocator_CreateFromKey(key1);
    parcKeyId_Release(&keyId);
    parcBuffer_Release(&keyBuffer);

    CCNxKeyLocator *keyLocator1Copy = ccnxKeyLocator_Copy(keyLocator1);

    PARCBuffer *keyBuffer2 = parcBuffer_WrapCString(hexString);
    PARCKeyId *keyId2 = parcKeyId_Create(keyBuffer2);
    PARCKey *key2 = parcKey_CreateFromDerEncodedPublicKey(keyId2, PARCSigningAlgorithm_RSA, keyBuffer2);
    CCNxKeyLocator *keyLocator2 = ccnxKeyLocator_CreateFromKey(key2);
    parcBuffer_Release(&keyBuffer2);
    parcKeyId_Release(&keyId2);

    CCNxName *keyURIName = ccnxName_CreateFromCString("lci://name");
    CCNxLink *keyLink = ccnxLink_Create(keyURIName, NULL, NULL);
    CCNxKeyLocator *keyLocatorDiff = ccnxKeyLocator_CreateFromKeyLink(keyLink);

    ccnxKeyLocator_AssertValid(keyLocator1);
    ccnxKeyLocator_AssertValid(keyLocator1Copy);
    ccnxKeyLocator_AssertValid(keyLocator2);
    ccnxKeyLocator_AssertValid(keyLocatorDiff);

    assertEqualsContract(ccnxKeyLocator_Equals, keyLocator1, keyLocator1Copy, keyLocator2, keyLocatorDiff);

    parcKey_Release(&key1);
    ccnxKeyLocator_Release(&keyLocator1);
    ccnxKeyLocator_Release(&keyLocator1Copy);

    parcKey_Release(&key2);
    ccnxKeyLocator_Release(&keyLocator2);

    ccnxName_Release(&keyURIName);
    ccnxLink_Release(&keyLink);
    ccnxKeyLocator_Release(&keyLocatorDiff);
}

LONGBOW_TEST_CASE(Global, ccnxKeyLocator_FromKey)
{
    char *hexString = "ABCDEF1234";
    PARCBuffer *keyBuffer = parcBuffer_WrapCString(hexString);
    PARCKeyId *keyId = parcKeyId_Create(keyBuffer);

    PARCKey *key = parcKey_CreateFromDerEncodedPublicKey(keyId, PARCSigningAlgorithm_RSA, keyBuffer);
    CCNxKeyLocator *keyLocator = ccnxKeyLocator_CreateFromKey(key);
    parcKeyId_Release(&keyId);
    parcBuffer_Release(&keyBuffer);

    ccnxKeyLocator_AssertValid(keyLocator);

    ccnxKeyLocator_Release(&keyLocator);
    parcKey_Release(&key);
}

LONGBOW_TEST_CASE(Global, ccnxKeyLocator_FromKeyLink)
{
    CCNxName *keyURIName = ccnxName_CreateFromCString("lci://name");
    CCNxLink *keyLink = ccnxLink_Create(keyURIName, NULL, NULL);
    CCNxKeyLocator *keyLocator = ccnxKeyLocator_CreateFromKeyLink(keyLink);

    ccnxKeyLocator_AssertValid(keyLocator);

    ccnxKeyLocator_Release(&keyLocator);
    ccnxName_Release(&keyURIName);
    ccnxLink_Release(&keyLink);
}

LONGBOW_TEST_CASE(Global, ccnxKeyLocator_GetKey)
{
    char *hexString = "ABCDEF1234";
    PARCBuffer *keyBuffer = parcBuffer_WrapCString(hexString);
    PARCKeyId *keyId = parcKeyId_Create(keyBuffer);

    PARCKey *key = parcKey_CreateFromDerEncodedPublicKey(keyId, PARCSigningAlgorithm_RSA, keyBuffer);
    CCNxKeyLocator *keyLocator = ccnxKeyLocator_CreateFromKey(key);

    parcKeyId_Release(&keyId);
    parcBuffer_Release(&keyBuffer);

    ccnxKeyLocator_AssertValid(keyLocator);
    PARCKey *actual = ccnxKeyLocator_GetKey(keyLocator);
    assertTrue(actual == key, "Actual certificate didn't match expected");

    ccnxKeyLocator_Release(&keyLocator);
    parcKey_Release(&key);
}

LONGBOW_TEST_CASE(Global, ccnxKeyLocator_GetKeyName)
{
    CCNxName *keyURIName = ccnxName_CreateFromCString("lci://name/test");
    CCNxLink *keyLink = ccnxLink_Create(keyURIName, NULL, NULL);
    CCNxKeyLocator *keyLocator = ccnxKeyLocator_CreateFromKeyLink(keyLink);

    ccnxKeyLocator_AssertValid(keyLocator);
    const CCNxName *actual = ccnxLink_GetName(ccnxKeyLocator_GetKeyLink(keyLocator));
    assertTrue(ccnxName_Equals(actual, keyURIName), "Actual keyName did not match the one returned by GetKeyName()");

    ccnxKeyLocator_Release(&keyLocator);
    ccnxName_Release(&keyURIName);
    ccnxLink_Release(&keyLink);
}

LONGBOW_TEST_CASE(Global, ccnxKeyLocator_GetType)
{
    // Try FromKey
    char *hexString = "ABCDEF1234";
    PARCBuffer *keyBuffer = parcBuffer_WrapCString(hexString);
    PARCKeyId *keyId = parcKeyId_Create(keyBuffer);

    PARCKey *key = parcKey_CreateFromDerEncodedPublicKey(keyId, PARCSigningAlgorithm_RSA, keyBuffer);
    CCNxKeyLocator *keyLocator = ccnxKeyLocator_CreateFromKey(key);
    parcKeyId_Release(&keyId);
    parcBuffer_Release(&keyBuffer);

    ccnxKeyLocator_AssertValid(keyLocator);
    assertTrue(ccnxKeyLocator_GetType(keyLocator) == CCNxKeyLocatorType_Key, "Actual certificate type didn't match expected type");

    ccnxKeyLocator_Release(&keyLocator);
    parcKey_Release(&key);

    // Try KeyName
    CCNxName *keyURIName = ccnxName_CreateFromCString("lci://name/test");
    CCNxLink *keyLink = ccnxLink_Create(keyURIName, NULL, NULL);
    keyLocator = ccnxKeyLocator_CreateFromKeyLink(keyLink);

    ccnxKeyLocator_AssertValid(keyLocator);
    assertTrue(ccnxKeyLocator_GetType(keyLocator) == CCNxKeyLocatorType_Link, "Actual certificate type didn't match expected type");
    assertFalse(ccnxKeyLocator_GetType(keyLocator) == CCNxKeyLocatorType_Key, "Actual certificate type didn't match expected type");

    ccnxKeyLocator_Release(&keyLocator);
    ccnxName_Release(&keyURIName);
    ccnxLink_Release(&keyLink);
}

LONGBOW_TEST_CASE(Global, ccnxKeyLocator_IsKey)
{
    char *hexString = "ABCDEF1234";
    PARCBuffer *keyBuffer = parcBuffer_WrapCString(hexString);
    PARCKeyId *keyId = parcKeyId_Create(keyBuffer);

    PARCKey *key = parcKey_CreateFromDerEncodedPublicKey(keyId, PARCSigningAlgorithm_RSA, keyBuffer);
    CCNxKeyLocator *keyLocator = ccnxKeyLocator_CreateFromKey(key);
    parcKeyId_Release(&keyId);
    parcBuffer_Release(&keyBuffer);

    ccnxKeyLocator_AssertValid(keyLocator);
    assertTrue(ccnxKeyLocator_IsKey(keyLocator), "Expected Iskey to be true");
    assertFalse(ccnxKeyLocator_IsKeyLink(keyLocator), "Expected IsKeyLink to be false");

    ccnxKeyLocator_Release(&keyLocator);
    parcKey_Release(&key);
}

LONGBOW_TEST_CASE(Global, ccnxKeyLocator_IsKeyName)
{
    CCNxName *keyURIName = ccnxName_CreateFromCString("lci://name/test");
    CCNxLink *keyLink = ccnxLink_Create(keyURIName, NULL, NULL);
    CCNxKeyLocator *keyLocator = ccnxKeyLocator_CreateFromKeyLink(keyLink);

    ccnxKeyLocator_AssertValid(keyLocator);
    assertFalse(ccnxKeyLocator_IsKey(keyLocator), "Expected Iskey to be false");
    assertTrue(ccnxKeyLocator_IsKeyLink(keyLocator), "Expected IsKeyLink to be true");

    ccnxKeyLocator_Release(&keyLocator);
    ccnxName_Release(&keyURIName);
    ccnxLink_Release(&keyLink);
}

LONGBOW_TEST_CASE(Global, ccnxKeyLocator_ToString)
{
    CCNxName *keyURIName = ccnxName_CreateFromCString("lci://name/test");
    CCNxLink *keyLink = ccnxLink_Create(keyURIName, NULL, NULL);
    CCNxKeyLocator *keyLocator = ccnxKeyLocator_CreateFromKeyLink(keyLink);

    ccnxKeyLocator_AssertValid(keyLocator);

    char *actualString = ccnxKeyLocator_ToString(keyLocator);

    assertTrue(0 == strncmp("KeyLocator", actualString, strlen("KeyLocator")), "ToString() did not return the expected prefix");

    parcMemory_Deallocate((void **) &actualString);

    ccnxLink_Release(&keyLink);
    ccnxName_Release(&keyURIName);
    ccnxKeyLocator_Release(&keyLocator);
}

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, ccnxKeyLocator_Create);
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

LONGBOW_TEST_CASE(Local, ccnxKeyLocator_Create)
{
    CCNxName *keyURIName = ccnxName_CreateFromCString("lci://name");
    CCNxLink *keyLink = ccnxLink_Create(keyURIName, NULL, NULL);
    CCNxKeyLocator *keyLocator = ccnxKeyLocator_CreateFromKeyLink(keyLink);

    ccnxKeyLocator_AssertValid(keyLocator);

    ccnxKeyLocator_Release(&keyLocator);
    ccnxName_Release(&keyURIName);
    ccnxLink_Release(&keyLink);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_KeyLocator);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
