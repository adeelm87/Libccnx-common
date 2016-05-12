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
 * @author Glenn Scott, Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <LongBow/runtime.h>

#include <ccnx/common/ccnx_KeyLocator.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Object.h>

#include <parc/security/parc_Key.h>

struct ccnx_key_locator {
    union {
        PARCKey *key;
        CCNxLink *keyLink;
    } locator;
    CCNxKeyLocatorType type;
};


static void
_Destroy(CCNxKeyLocator **keyLocatorP)
{
    if (keyLocatorP != NULL) {
        CCNxKeyLocator *keyLocator = *keyLocatorP;
        if (ccnxKeyLocator_IsKey(keyLocator)) {
            parcKey_Release(&keyLocator->locator.key);
        } else if (ccnxKeyLocator_IsKeyLink(keyLocator)) {
            ccnxLink_Release(&keyLocator->locator.keyLink);
        } else {
            assertTrue(0, "KeyLocator is not one of %s.", "Key or KeyLink");
        }
    }
}

parcObject_ExtendPARCObject(CCNxKeyLocator, _Destroy, ccnxKeyLocator_Copy, ccnxKeyLocator_ToString, ccnxKeyLocator_Equals, NULL, NULL, NULL);

parcObject_ImplementAcquire(ccnxKeyLocator, CCNxKeyLocator);

parcObject_ImplementRelease(ccnxKeyLocator, CCNxKeyLocator);

static CCNxKeyLocator *
_Create(void)
{
    CCNxKeyLocator *result = parcObject_CreateInstance(CCNxKeyLocator);
    if (NULL != result) {
        result->locator.key = NULL;
        result->type = CCNxKeyLocatorType_None;
    }
    return result;
}

CCNxKeyLocator *
ccnxKeyLocator_Copy(const CCNxKeyLocator *original)
{
    CCNxKeyLocator *copy = NULL;

    switch (original->type) {
        case CCNxKeyLocatorType_Key: {
            PARCKey *keyCopy = parcKey_Copy(original->locator.key);
            copy = ccnxKeyLocator_CreateFromKey(keyCopy);
            parcKey_Release(&keyCopy);
        }
        break;

        case CCNxKeyLocatorType_Link: {
            CCNxLink *keyLinkCopy = ccnxLink_Copy(original->locator.keyLink);
            copy = ccnxKeyLocator_CreateFromKeyLink(keyLinkCopy);
            ccnxLink_Release(&keyLinkCopy);
        }
        break;

        case CCNxKeyLocatorType_None:
            copy = _Create();
            break;
    }
    return copy;
}

CCNxKeyLocator *
ccnxKeyLocator_CreateFromKey(PARCKey *key)
{
    assertNotNull(key, "Parameter must be a non-null CCNxKey pointer.");

    CCNxKeyLocator *result = _Create();
    if (result != NULL) {
        result->locator.key = parcKey_Acquire(key);
        result->type = CCNxKeyLocatorType_Key;
    }

    return result;
}

CCNxKeyLocator *
ccnxKeyLocator_CreateFromKeyLink(CCNxLink *keyLink)
{
    CCNxKeyLocator *result = _Create();
    if (result != NULL) {
        result->locator.keyLink = ccnxLink_Acquire(keyLink);
        result->type = CCNxKeyLocatorType_Link;
    }

    return result;
}

bool
ccnxKeyLocator_Equals(const CCNxKeyLocator *a, const CCNxKeyLocator *b)
{
    if (a == b) {
        return true;
    }
    if (a == NULL || b == NULL) {
        return false;
    }

    if (a->type == b->type) {
        switch (a->type) {
            case CCNxKeyLocatorType_Key:
                return parcKey_Equals(a->locator.key, b->locator.key);

            case CCNxKeyLocatorType_Link:
                return ccnxLink_Equals(a->locator.keyLink, b->locator.keyLink);

            default:
                break;
                // fall through
        }
    }
    return false;
}

CCNxKeyLocatorType
ccnxKeyLocator_GetType(const CCNxKeyLocator *ccnxKeyLocator)
{
    assertNotNull(ccnxKeyLocator, "Parameter must be a non-null CCNxKeyLocator pointer.");

    return ccnxKeyLocator->type;
}

bool
ccnxKeyLocator_IsKey(const CCNxKeyLocator *keyLocator)
{
    return (keyLocator->type == CCNxKeyLocatorType_Key);
}

bool
ccnxKeyLocator_IsKeyLink(const CCNxKeyLocator *keyLocator)
{
    return (keyLocator->type == CCNxKeyLocatorType_Link);
}

char *
ccnxKeyLocator_ToString(const CCNxKeyLocator *keyLocator)
{
    char *result;

    if (keyLocator == NULL) {
        char *nullString = "NULL";
        result = parcMemory_StringDuplicate(nullString, strlen(nullString));
    } else {
        char *locator;
        if (ccnxKeyLocator_IsKey(keyLocator)) {
            locator = parcKey_ToString(keyLocator->locator.key);
        } else if (ccnxKeyLocator_IsKeyLink(keyLocator)) {
            locator = ccnxLink_ToString(keyLocator->locator.keyLink);
        } else {
            char *initString = "not initialised.";
            locator = parcMemory_StringDuplicate(initString, strlen(initString));
        }
        char *string;
        int failure = asprintf(&string, "KeyLocator { %s }", locator);
        assertTrue(failure > -1, "Error asprintf");

        parcMemory_Deallocate((void **) &locator);

        result = parcMemory_StringDuplicate(string, strlen(string));
        free(string);
    }

    return result;
}

CCNxLink *
ccnxKeyLocator_GetKeyLink(const CCNxKeyLocator *keyLocator)
{
    assertTrue(keyLocator->type == CCNxKeyLocatorType_Link, "Key Locator must be of type CCNxKeyLocatorType_Link");

    if (keyLocator->type == CCNxKeyLocatorType_Link) {
        return keyLocator->locator.keyLink;
    }
    return NULL;
}

PARCKey *
ccnxKeyLocator_GetKey(const CCNxKeyLocator *keyLocator)
{
    assertTrue(keyLocator->type == CCNxKeyLocatorType_Key, "Key Locator must be of type CCNxKeyLocatorType_Key");

    if (keyLocator->type == CCNxKeyLocatorType_Key) {
        return keyLocator->locator.key;
    }
    return NULL;
}

void
ccnxKeyLocator_AssertValid(const CCNxKeyLocator *keyLocator)
{
    assertNotNull(keyLocator, "CCNxKeyLocator pointer was null");
    if (ccnxKeyLocator_IsKey(keyLocator)) {
        parcKey_AssertValid(keyLocator->locator.key);
    } else if (ccnxKeyLocator_IsKeyLink(keyLocator)) {
        ccnxLink_AssertValid(keyLocator->locator.keyLink);
    } else {
        assertTrue(0, "KeyLocator is not one of %s.", "Key or Key Link");
    }
}
