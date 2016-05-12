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
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>

#include <LongBow/runtime.h>

#include <ccnx/common/ccnx_InterestPayloadId.h>
#include <ccnx/common/ccnx_NameSegment.h>

#include <parc/algol/parc_Buffer.h>
#include <parc/algol/parc_Object.h>

#include <parc/security/parc_CryptoHashType.h>
#include <parc/security/parc_CryptoHash.h>
#include <parc/security/parc_CryptoHasher.h>

struct ccnx_interest_payload_id {
    CCNxNameSegment *nameSegment;
    uint8_t type;
};

static void
_destroy(CCNxInterestPayloadId **idP)
{
    CCNxInterestPayloadId *id = *idP;
    ccnxNameSegment_Release(&id->nameSegment);
}

parcObject_ExtendPARCObject(CCNxInterestPayloadId, _destroy, ccnxInterestPayloadId_Copy, ccnxInterestPayloadId_ToString,
                            ccnxInterestPayloadId_Equals, ccnxInterestPayloadId_Compare, NULL, NULL);

parcObject_ImplementAcquire(ccnxInterestPayloadId, CCNxInterestPayloadId);

parcObject_ImplementRelease(ccnxInterestPayloadId, CCNxInterestPayloadId);

CCNxInterestPayloadId *
ccnxInterestPayloadId_Create(const PARCBuffer *data, uint8_t type)
{
    CCNxInterestPayloadId *result = parcObject_CreateInstance(CCNxInterestPayloadId);
    parcBuffer_AssertValid(data);

    PARCBuffer *buffer = parcBuffer_Allocate(parcBuffer_Capacity(data) + 1);
    assertTrue(type > CCNxInterestPayloadId_TypeCode_App, "App type must be greater than 0x80");

    parcBuffer_PutUint8(buffer, type);
    parcBuffer_PutBuffer(buffer, data);
    parcBuffer_Flip(buffer);
    result->nameSegment = ccnxNameSegment_CreateTypeValue(CCNxNameLabelType_PAYLOADID, buffer);
    parcBuffer_Release(&buffer);

    return result;
}

CCNxInterestPayloadId *
ccnxInterestPayloadId_CreateAsSHA256Hash(const PARCBuffer *data)
{
    CCNxInterestPayloadId *result = parcObject_CreateInstance(CCNxInterestPayloadId);

    PARCCryptoHasher *hasher = parcCryptoHasher_Create(PARC_HASH_SHA256);
    parcCryptoHasher_Init(hasher);
    parcCryptoHasher_UpdateBuffer(hasher, data);
    PARCCryptoHash *hash = parcCryptoHasher_Finalize(hasher);
    parcCryptoHasher_Release(&hasher);

    PARCBuffer *hashData = parcCryptoHash_GetDigest(hash);
    PARCBuffer *codedHash = parcBuffer_Allocate(parcBuffer_Capacity(hashData) + 1);
    parcBuffer_PutUint8(codedHash, CCNxInterestPayloadId_TypeCode_RFC6920_SHA256);
    parcBuffer_PutBuffer(codedHash, hashData);
    parcBuffer_Flip(codedHash);

    result->nameSegment =
        ccnxNameSegment_CreateTypeValue(CCNxNameLabelType_PAYLOADID, codedHash);

    parcBuffer_Release(&codedHash);
    parcCryptoHash_Release(&hash);

    return result;
}

static CCNxInterestPayloadId *
_ccnxInterestPayloadId_CreateFromNameSegment(const CCNxNameSegment *nameSegment)
{
    ccnxNameSegment_AssertValid(nameSegment);
    assertTrue(ccnxNameSegment_GetType(nameSegment) == CCNxNameLabelType_PAYLOADID,
               "ccnxInterestPayloadId_CreateFromNameSegment: supplied nameSegment is not a PayloadId");
    if (ccnxNameSegment_GetType(nameSegment) != CCNxNameLabelType_PAYLOADID) {
        return NULL;
    }
    CCNxInterestPayloadId *result = parcObject_CreateInstance(CCNxInterestPayloadId);
    result->nameSegment = ccnxNameSegment_Acquire(nameSegment);

    return result;
}

CCNxInterestPayloadId *
ccnxInterestPayloadId_CreateFromSegmentInName(const CCNxName *name)
{
    ccnxName_AssertValid(name);

    CCNxInterestPayloadId *result = NULL;
    size_t count = ccnxName_GetSegmentCount(name);
    for (size_t i = 0; i < count; ++i) {
        CCNxNameSegment *segment = ccnxName_GetSegment(name, i);
        if (ccnxNameSegment_GetType(segment) == CCNxNameLabelType_PAYLOADID) {
            result = _ccnxInterestPayloadId_CreateFromNameSegment(segment);
            break;
        }
    }

    return result;
}



const CCNxNameSegment *
ccnxInterestPayloadId_GetNameSegment(const CCNxInterestPayloadId *id)
{
    return id->nameSegment;
}

PARCBuffer *
ccnxInterestPayloadId_GetValue(const CCNxInterestPayloadId *id)
{
    PARCBuffer *data = ccnxNameSegment_GetValue(id->nameSegment);
    parcBuffer_Rewind(data);
    parcBuffer_SetPosition(data, 1);
    return data;
}

uint8_t
ccnxInterestPayloadId_GetType(const CCNxInterestPayloadId *id)
{
    PARCBuffer *data = ccnxNameSegment_GetValue(id->nameSegment);
    parcBuffer_Rewind(data);
    uint8_t type = parcBuffer_GetUint8(data);
    return type;
}


void
ccnxInterestPayloadId_AssertValid(const CCNxInterestPayloadId *id)
{
    trapIllegalValueIf(ccnxInterestPayloadId_IsValid(id) == false, "CCNxName instance is not valid.");
}

bool
ccnxInterestPayloadId_IsValid(const CCNxInterestPayloadId *id)
{
    bool result = false;
    if (ccnxNameSegment_IsValid(id->nameSegment)) {
        result = true;
    }
    return result;
}

char *
ccnxInterestPayloadId_ToString(const CCNxInterestPayloadId *id)
{
    ccnxInterestPayloadId_AssertValid(id);

    return ccnxNameSegment_ToString(id->nameSegment);
}

CCNxInterestPayloadId *
ccnxInterestPayloadId_Copy(const CCNxInterestPayloadId *sourceId)
{
    ccnxInterestPayloadId_AssertValid(sourceId);

    CCNxInterestPayloadId *result =
        _ccnxInterestPayloadId_CreateFromNameSegment(sourceId->nameSegment);
    return result;
}

bool
ccnxInterestPayloadId_Equals(const CCNxInterestPayloadId *id1, const CCNxInterestPayloadId *id2)
{
    ccnxInterestPayloadId_AssertValid(id1);
    ccnxInterestPayloadId_AssertValid(id2);

    bool result = ((id1 == id2) ||
                   (ccnxNameSegment_Equals(id1->nameSegment, id2->nameSegment)));
    return result;
}

uint32_t
ccnxInterestPayloadId_HashCode(const CCNxInterestPayloadId *id)
{
    ccnxInterestPayloadId_AssertValid(id);
    return ccnxNameSegment_HashCode(id->nameSegment);
}


int
ccnxInterestPayloadId_Compare(const CCNxInterestPayloadId *id1, const CCNxInterestPayloadId *id2)
{
    ccnxInterestPayloadId_AssertValid(id1);
    ccnxInterestPayloadId_AssertValid(id2);

    int result = ccnxNameSegment_Compare(id1->nameSegment, id2->nameSegment);
    return result;
}
