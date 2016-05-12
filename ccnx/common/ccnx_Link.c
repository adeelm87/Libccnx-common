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
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>

#include <LongBow/runtime.h>

#include <stdio.h>
#include <string.h>

#include <ccnx/common/ccnx_Link.h>

#include <parc/algol/parc_Object.h>

struct ccnx_link {
    const CCNxName *name;
    PARCBuffer *keyId;
    PARCBuffer *contentHash;
};

static void
_destroy(CCNxLink **linkP)
{
    ccnxName_Release((CCNxName **) &(*linkP)->name);
    if ((*linkP)->keyId != NULL) {
        parcBuffer_Release(&(*linkP)->keyId);
    }
    if ((*linkP)->contentHash != NULL) {
        parcBuffer_Release(&(*linkP)->contentHash);
    }
}

parcObject_ExtendPARCObject(CCNxLink, _destroy, NULL, ccnxLink_ToString, ccnxLink_Equals, NULL, NULL, NULL);

parcObject_ImplementAcquire(ccnxLink, CCNxLink);

parcObject_ImplementRelease(ccnxLink, CCNxLink);

CCNxLink *
ccnxLink_Create(const CCNxName *name, PARCBuffer *keyId, PARCBuffer *contentObjectHash)
{
    CCNxLink *link = parcObject_CreateInstance(CCNxLink);

    if (link != NULL) {
        link->name = ccnxName_Acquire(name);
        link->keyId = (keyId == NULL) ? NULL : parcBuffer_Acquire(keyId);
        link->contentHash = (contentObjectHash == NULL) ? NULL : parcBuffer_Acquire(contentObjectHash);
    }

    return link;
}

CCNxLink *
ccnxLink_Copy(const CCNxLink *original)
{
    CCNxLink *link = parcObject_CreateInstance(CCNxLink);

    if (link != NULL) {
        link->name = ccnxName_Copy(original->name);
        link->keyId = (original->keyId == NULL) ? NULL : parcBuffer_Copy(original->keyId);
        link->contentHash = (original->contentHash == NULL) ? NULL : parcBuffer_Copy(original->contentHash);
    }

    return link;
}

const CCNxName *
ccnxLink_GetName(const CCNxLink *link)
{
    return link->name;
}

PARCBuffer *
ccnxLink_GetKeyID(const CCNxLink *link)
{
    return link->keyId;
}

PARCBuffer *
ccnxLink_GetContentObjectHash(const CCNxLink *link)
{
    return link->contentHash;
}

bool
ccnxLink_Equals(const CCNxLink *objectA, const CCNxLink *objectB)
{
    bool result = false;

    if (objectA == objectB) {
        result = true;
    } else if (objectA != NULL && objectB != NULL) {
        if (ccnxName_Equals(objectA->name, objectB->name)) {
            if (parcBuffer_Equals(objectA->keyId, objectB->keyId)) {
                if (parcBuffer_Equals(objectA->contentHash, objectB->contentHash)) {
                    return true;
                }
            }
        }
    }
    return result;
}

char *
ccnxLink_ToString(const CCNxLink *link)
{
    char *nameString = ccnxName_ToString(link->name);
    char *keyIdString = "NULL";
    bool nullKeyIdString = true;

    if (link->keyId != NULL) {
        keyIdString = parcBuffer_ToString(link->keyId);
        nullKeyIdString = false;
    }

    char *contentObjectHashString = NULL;
    bool nullContentObjectHashString = true;
    if (link->contentHash != NULL) {
        contentObjectHashString = parcBuffer_ToString(link->contentHash);
        nullContentObjectHashString = false;
    }

    char *string;
    int failure = asprintf(&string, "CCNxLink { .name=\"%s\", .KeyID=\"%s\", .ContentObjectHash=\"%s\" }",
                           nameString,
                           keyIdString,
                           contentObjectHashString);
    assertTrue(failure > -1, "Error asprintf");

    parcMemory_Deallocate((void **) &nameString);
    if (!nullKeyIdString) {
        parcMemory_Deallocate((void **) &keyIdString);
    }
    if (!nullContentObjectHashString) {
        parcMemory_Deallocate((void **) &contentObjectHashString);
    }

    char *result = parcMemory_StringDuplicate(string, strlen(string));
    free(string);

    return result;
}

bool
ccnxLink_IsValid(const CCNxLink *link)
{
    bool result = false;

    if (link != NULL) {
        if (ccnxName_IsValid(link->name)) {
            if (link->keyId == NULL || parcBuffer_IsValid(link->keyId)) {
                if (link->contentHash == NULL || parcBuffer_IsValid(link->contentHash)) {
                    result = true;
                }
            }
        }
    }

    return result;
}

void
ccnxLink_AssertValid(const CCNxLink *link)
{
    assertTrue(ccnxLink_IsValid(link), "CCNxLink instance is not valid.");
}
