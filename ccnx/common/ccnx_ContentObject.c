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

#include <LongBow/runtime.h>

#include <ccnx/common/internal/ccnx_ContentObjectInterface.h>

#include <ccnx/common/ccnx_ContentObject.h>

#include <ccnx/common/validation/ccnxValidation_HmacSha256.h>
#include <ccnx/common/validation/ccnxValidation_RsaSha256.h>

#include <ccnx/common/internal/ccnx_ValidationFacadeV1.h>

static const CCNxContentObjectInterface *_defaultImplementation = &CCNxContentObjectFacadeV1_Implementation;

/**
 * A CCNxContentObject is a wrapper class providing a consistent interface for applications.
 * Internally, the data of a ContentObject is stored in the underlying ccnxTlvDictionary.
 * Because we may have different schemas for a ContentObject, an CCNxContentObjectInterface pointer
 * is also stored in the underlying ccnxTlvDictionary. This provides access to the functions
 * used to create and access a particular implementation of a ContentObject.
 */

CCNxContentObject *
ccnxContentObject_CreateWithNameAndPayload(const CCNxName *contentName, const PARCBuffer *payload)
{
    return ccnxContentObject_CreateWithImplAndPayload(_defaultImplementation,
                                                      contentName,
                                                      CCNxPayloadType_DATA,
                                                      payload);
}

CCNxContentObject *
ccnxContentObject_CreateWithPayload(const PARCBuffer *payload)
{
    return ccnxContentObject_CreateWithImplAndPayload(_defaultImplementation,
                                                      NULL,
                                                      CCNxPayloadType_DATA,
                                                      payload);
}

CCNxContentObject *
ccnxContentObject_CreateWithImplAndPayload(const CCNxContentObjectInterface *impl,
                                           const CCNxName *contentName,
                                           const CCNxPayloadType payloadType,
                                           const PARCBuffer *payload)
{
    CCNxContentObject *result = NULL;

    if (impl->createWithNameAndPayload != NULL) {
        if (contentName == NULL) {
            result = impl->createWithPayload(payloadType, payload);
        } else {
            result = impl->createWithNameAndPayload(contentName, payloadType, payload);
        }

        // And set the dictionary's interface pointer to the one we just used to create this.
        ccnxTlvDictionary_SetMessageInterface(result, impl);
    } else {
        trapNotImplemented("ContentObject implementations must implement createWithNameAndPayload()");
    }

    return result;
}

bool
ccnxContentObject_SetSignature(CCNxContentObject *contentObject, const PARCBuffer *keyId,
                               const PARCSignature *signature, const CCNxKeyLocator *keyLocator)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    bool result = false;

    if (impl->setSignature != NULL) {
        result = impl->setSignature(contentObject, keyId, signature, keyLocator);
    }

    return result;
}

PARCBuffer *
ccnxContentObject_GetKeyId(const CCNxContentObject *contentObject)
{
    ccnxContentObject_OptionalAssertValid(contentObject);

    PARCBuffer *result = NULL;

    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);
    if (impl->getKeyId != NULL) {
        result = impl->getKeyId(contentObject);
    }

    return result;
}

CCNxName *
ccnxContentObject_GetName(const CCNxContentObject *contentObject)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    CCNxName *result = NULL;

    if (impl->getName != NULL) {
        result = impl->getName(contentObject);
    } else {
        trapNotImplemented("ccnxContentObject_GetName");
    }

    return result;
}

PARCBuffer *
ccnxContentObject_GetPayload(const CCNxContentObject *contentObject)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    PARCBuffer *result = NULL;

    if (impl->getPayload != NULL) {
        result = impl->getPayload(contentObject);
    } else {
        trapNotImplemented("ccnxContentObject_GetPayload");
    }

    return result;
}

CCNxPayloadType
ccnxContentObject_GetPayloadType(const CCNxContentObject *contentObject)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    CCNxPayloadType result = CCNxPayloadType_DATA;

    if (impl->getPayloadType != NULL) {
        result = impl->getPayloadType(contentObject);
    } else {
        trapNotImplemented("ccnxContentObject_GetPayloadType");
    }

    return result;
}

bool
ccnxContentObject_SetPayload(CCNxContentObject *contentObject, CCNxPayloadType payloadType, const PARCBuffer *payload)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    bool result = false;;

    if (impl->setPayload != NULL) {
        result = impl->setPayload(contentObject, payloadType, payload);;
    } else {
        trapNotImplemented("ccnxContentObject_SetPayload");
    }

    return result;
}

bool
ccnxContentObject_SetExpiryTime(CCNxContentObject *contentObject, const uint64_t expiryTIme)
{
    bool result = false;

    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    if (impl->setExpiryTime != NULL) {
        result = impl->setExpiryTime(contentObject, expiryTIme);
    } else {
        trapNotImplemented("ccnxContentObject_SetExpiryTime");
    }
    return result;
}

bool
ccnxContentObject_HasExpiryTime(const CCNxContentObject *contentObject)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    bool result = false;

    if (impl->hasExpiryTime != NULL) {
        result = impl->hasExpiryTime(contentObject);
    } else {
        return false;
    }

    return result;
}

uint64_t
ccnxContentObject_GetExpiryTime(const CCNxContentObject *contentObject)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    if (impl->hasExpiryTime != NULL && !impl->hasExpiryTime((CCNxTlvDictionary *) contentObject)) {
        trapUnexpectedState("ContentObject has no ExpiryTime. Call HasExpiryTime() first.");
    }

    if (impl->getExpiryTime != NULL) {
        return impl->getExpiryTime(contentObject);
    } else {
        trapNotImplemented("ccnxContentObject_HasExpiryTime");
    }
}

bool
ccnxContentObject_SetMessageId(CCNxContentObject *contentObject, const uint32_t messageId)
{
    bool result = false;

    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    if (impl->setMessageId != NULL) {
        result = impl->setMessageId(contentObject, messageId);
    } else {
        trapNotImplemented("ccnxContentObject_SetMessageId");
    }
    return result;
}

bool
ccnxContentObject_HasMessageId(const CCNxContentObject *contentObject)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    bool result = false;

    if (impl->hasMessageId != NULL) {
        result = impl->hasMessageId(contentObject);
    } else {
        return false;
    }

    return result;
}

uint32_t
ccnxContentObject_GetMessageId(const CCNxContentObject *contentObject)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    if (impl->hasMessageId != NULL && !impl->hasMessageId((CCNxTlvDictionary *) contentObject)) {
        trapUnexpectedState("ContentObject has no MessageId. Call HasMessageId() first.");
    }

    if (impl->getMessageId != NULL) {
        return impl->getMessageId(contentObject);
    } else {
        trapNotImplemented("ccnxContentObject_HasMessageId");
    }
}

bool
ccnxContentObject_SetFinalChunkNumber(CCNxContentObject *contentObject, const uint64_t finalChunkNumber)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    bool result = false;

    if (impl->setFinalChunkNumber != NULL) {
        result = impl->setFinalChunkNumber(contentObject, finalChunkNumber);
    } else {
        trapNotImplemented("ccnxContentObject_SetFinalChunkNumber");
    }
    return result;
}

bool
ccnxContentObject_HasFinalChunkNumber(const CCNxContentObject *contentObject)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    bool result = false;

    if (impl->hasFinalChunkNumber != NULL) {
        result = impl->hasFinalChunkNumber(contentObject);
    } else {
        trapNotImplemented("ccnxContentObject_HasFinalChunkNumber");
    }

    return result;
}

uint64_t
ccnxContentObject_GetFinalChunkNumber(const CCNxContentObject *contentObject)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    if (impl->hasFinalChunkNumber != NULL && !impl->hasFinalChunkNumber((CCNxTlvDictionary *) contentObject)) {
        trapUnexpectedState("ContentObject has no final chunk number. Call ccnxContentObject_HasFinalChunkNumber() first.");
    }

    if (impl->getFinalChunkNumber != NULL) {
        return impl->getFinalChunkNumber(contentObject);
    } else {
        trapNotImplemented("ccnxContentObject_GetFinalChunkNumber");
    }
}

void
ccnxContentObject_Display(const CCNxContentObject *contentObject, int indentation)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    if (impl->display != NULL) {
        impl->display(contentObject, indentation);
    } else {
        ccnxTlvDictionary_Display(contentObject, indentation);
    }
}

char *
ccnxContentObject_ToString(const CCNxContentObject *contentObject)
{
    ccnxContentObject_OptionalAssertValid(contentObject);
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    char *result = NULL;

    if (impl->toString != NULL) {
        result = impl->toString(contentObject);
    } else {
        trapNotImplemented("ccnxContentObject_ToString");
    }
    return result;
}

bool
ccnxContentObject_Equals(const CCNxContentObject *objectA, const CCNxContentObject *objectB)
{
    CCNxContentObjectInterface *implA = ccnxContentObjectInterface_GetInterface(objectA);
    CCNxContentObjectInterface *implB = ccnxContentObjectInterface_GetInterface(objectB);

    assertNotNull(implA, "ContentObject must have an valid implementation pointer.");
    assertNotNull(implB, "ContentObject must have an valid implementation pointer.");

    if (implA != implB) {
        return false;
    }

    if (implA->equals != NULL) {
        return implA->equals(objectA, objectB);
    } else {
        trapNotImplemented("ccnxContentObject_Equals");
    }
}

CCNxContentObject *
ccnxContentObject_Acquire(const CCNxContentObject *contentObject)
{
    return ccnxTlvDictionary_Acquire(contentObject);
}

void
ccnxContentObject_Release(CCNxContentObject **contentObjectP)
{
    ccnxTlvDictionary_Release(contentObjectP);
}

void
ccnxContentObject_AssertValid(const CCNxContentObject *contentObject)
{
    assertNotNull(contentObject, "Parameter must be a non-null CCNxContentObject pointer");
    CCNxContentObjectInterface *impl = ccnxContentObjectInterface_GetInterface(contentObject);

    assertNotNull(impl, "ContentObject must have a non-NUll implementation");
    if (impl->assertValid != NULL) {
        impl->assertValid(contentObject);
    }
}
