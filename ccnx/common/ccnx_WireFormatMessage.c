/*
 * Copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include "config.h"

#include <LongBow/runtime.h>

#include <ccnx/common/ccnx_WireFormatMessage.h>

void
ccnxWireFormatMessage_AssertValid(const CCNxWireFormatMessage *message)
{
    assertNotNull(message, "Must be a non-null pointer to a CCNxWireFormatMessage.");

    // Check for required fields in the underlying dictionary. Case 1036
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);
    assertNotNull(impl, "WireFormatMessage must have an valid implementation pointer.");
    if (impl->assertValid) {
        impl->assertValid(message);
    }
}

static CCNxWireFormatMessage *
_ccnxWireFormatMessage_CreateWithImpl(const CCNxWireFormatMessageInterface *impl, const PARCBuffer *wireFormatBuffer)
{
    CCNxWireFormatMessage *result = impl->create(wireFormatBuffer);
    return result;
}

CCNxWireFormatMessage *
ccnxWireFormatMessage_Create(const PARCBuffer *wireFormat)
{
    CCNxWireFormatMessage *result = NULL;

    uint8_t version = parcBuffer_GetAtIndex(wireFormat, 0);
    switch (version) {

        case CCNxTlvDictionary_SchemaVersion_V1:
            result = _ccnxWireFormatMessage_CreateWithImpl(&CCNxWireFormatFacadeV1_Implementation, wireFormat);
            break;
        default:
            // no action, will return NULL
            break;
    }

    return result;
}

static CCNxWireFormatMessageInterface *
_getImplForSchema(const CCNxTlvDictionary_SchemaVersion schemaVersion)
{
    CCNxWireFormatMessageInterface *result = NULL;
    switch (schemaVersion) {

        case CCNxTlvDictionary_SchemaVersion_V1:
            result = &CCNxWireFormatFacadeV1_Implementation;
            break;
        default:
            trapIllegalValue(data->fwd_state->nextMessage.version, "Unknown schema version: %d", schemaVersion);
    }
    return result;
}

CCNxWireFormatMessage *
ccnxWireFormatMessage_FromInterestPacketType(const CCNxTlvDictionary_SchemaVersion schemaVersion, const PARCBuffer *wireFormat)
{
    CCNxWireFormatMessageInterface *impl = _getImplForSchema(schemaVersion);
    CCNxWireFormatMessage *result = NULL;

    if (impl->fromInterestPacketType != NULL) {
        result = impl->fromInterestPacketType(wireFormat);
    }

    return result;
}

CCNxWireFormatMessage *
ccnxWireFormatMessage_FromInterestPacketTypeIoVec(const CCNxTlvDictionary_SchemaVersion schemaVersion, const CCNxCodecNetworkBufferIoVec *vec)
{
    CCNxWireFormatMessageInterface *impl = _getImplForSchema(schemaVersion);
    CCNxWireFormatMessage *result = NULL;

    if (impl->fromInterestPacketTypeIoVec != NULL) {
        result = impl->fromInterestPacketTypeIoVec(vec);
    }

    return result;
}

CCNxWireFormatMessage *
ccnxWireFormatMessage_FromContentObjectPacketType(const CCNxTlvDictionary_SchemaVersion schemaVersion, const PARCBuffer *wireFormat)
{
    CCNxWireFormatMessageInterface *impl = _getImplForSchema(schemaVersion);
    CCNxWireFormatMessage *result = NULL;

    if (impl->fromContentObjectPacketType != NULL) {
        result = impl->fromContentObjectPacketType(wireFormat);
    }

    return result;
}

CCNxWireFormatMessage *
ccnxWireFormatMessage_FromControlPacketType(const CCNxTlvDictionary_SchemaVersion schemaVersion, const PARCBuffer *wireFormat)
{
    CCNxWireFormatMessageInterface *impl = _getImplForSchema(schemaVersion);
    CCNxWireFormatMessage *result = NULL;

    if (impl->fromControlPacketType != NULL) {
        result = impl->fromControlPacketType(wireFormat);
    }

    return result;
}

CCNxTlvDictionary *
ccnxWireFormatMessage_GetDictionary(const CCNxWireFormatMessage *message)
{
    return (CCNxTlvDictionary *) message;
}


CCNxCodecNetworkBufferIoVec *
ccnxWireFormatMessage_GetIoVec(const CCNxWireFormatMessage *message)
{
    ccnxWireFormatMessage_OptionalAssertValid(message);
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);

    CCNxCodecNetworkBufferIoVec *result = NULL;

    if (impl->getIoVec != NULL) {
        result = impl->getIoVec(message);
    } else {
        trapNotImplemented("ccnxWireFormatMessage_GetIoVec");
    }

    return result;
}

bool
ccnxWireFormatMessage_PutIoVec(CCNxWireFormatMessage *message, CCNxCodecNetworkBufferIoVec *vec)
{
    ccnxWireFormatMessage_OptionalAssertValid(message);
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);

    bool result = false;

    if (impl->putIoVec != NULL) {
        result = impl->putIoVec(message, vec);
    } else {
        trapNotImplemented("ccnxWireFormatMessage_PutIoVec");
    }

    return result;
}

bool
ccnxWireFormatMessage_PutWireFormatBuffer(CCNxWireFormatMessage *message, PARCBuffer *buffer)
{
    ccnxWireFormatMessage_OptionalAssertValid(message);
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);

    bool result = false;

    if (impl->putWireFormatBuffer != NULL) {
        result = impl->putWireFormatBuffer(message, buffer);
    } else {
        trapNotImplemented("ccnxWireFormatMessage_PutWireFormatBuffer");
    }

    return result;
}

PARCBuffer *
ccnxWireFormatMessage_GetWireFormatBuffer(const CCNxWireFormatMessage *message)
{
    ccnxWireFormatMessage_OptionalAssertValid(message);
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);

    PARCBuffer *result = NULL;

    if (impl->getWireFormatBuffer != NULL) {
        result = impl->getWireFormatBuffer(message);
    } else {
        trapNotImplemented("ccnxWireFormatMessage_GetWireFormatBuffer");
    }

    return result;
}

void
ccnxWireFormatMessage_WriteToFile(const CCNxWireFormatMessage *message, const char *filename)
{
    ccnxWireFormatMessage_OptionalAssertValid(message);
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);

    if (impl->writeToFile != NULL) {
        impl->writeToFile(message, filename);
    } else {
        trapNotImplemented("ccnxWireFormatMessage_WriteToFile");
    }
}

bool
ccnxWireFormatMessage_SetProtectedRegionStart(CCNxWireFormatMessage *message, size_t startPosition)
{
    ccnxWireFormatMessage_OptionalAssertValid(message);
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);

    bool result = false;

    if (impl->setProtectedRegionStart != NULL) {
        result = impl->setProtectedRegionStart(message, startPosition);
    } else {
        trapNotImplemented("ccnxWireFormatMessage_SetProtectedRegionStart");
    }

    return result;
}

bool
ccnxWireFormatMessage_SetProtectedRegionLength(CCNxWireFormatMessage *message, size_t length)
{
    ccnxWireFormatMessage_OptionalAssertValid(message);
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);

    bool result = false;

    if (impl->setProtectedRegionLength != NULL) {
        result = impl->setProtectedRegionLength(message, length);
    } else {
        trapNotImplemented("ccnxWireFormatMessage_SetProtectedRegionLength");
    }

    return result;
}


PARCCryptoHash *
ccnxWireFormatMessage_HashProtectedRegion(const CCNxWireFormatMessage *message, PARCCryptoHasher *hasher)
{
    ccnxWireFormatMessage_OptionalAssertValid(message);
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);

    PARCCryptoHash *result = NULL;

    if (impl->hashProtectedRegion != NULL) {
        result = impl->hashProtectedRegion(message, hasher);
    } else {
        trapNotImplemented("ccnxWireFormatMessage_HashProtectedRegion");
    }

    return result;
}

PARCCryptoHash *
ccnxWireFormatMessage_CreateContentObjectHash(CCNxWireFormatMessage *message)
{
    ccnxWireFormatMessage_AssertValid(message);
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);

    PARCCryptoHash *result = NULL;

    if (impl->computeContentObjectHash != NULL) {
        result = impl->computeContentObjectHash(message);
    } else {
        trapNotImplemented("ccnxWireFormatMessage_ComputeContentObjectHash");
    }

    return result;
}

CCNxWireFormatMessage *
ccnxWireFormatMessage_Acquire(const CCNxWireFormatMessage *message)
{
    return ccnxTlvDictionary_Acquire(message);
}

void
ccnxWireFormatMessage_Release(CCNxWireFormatMessage **message)
{
    ccnxTlvDictionary_Release(message);
}

bool
ccnxWireFormatMessage_SetHopLimit(CCNxWireFormatMessage *message, uint32_t hoplimit)
{
    bool result = false;

    ccnxWireFormatMessage_OptionalAssertValid(message);
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);

    if (impl != NULL) {
        result = impl->setHopLimit(message, hoplimit);
    }

    return result;
}

bool
ccnxWireFormatMessage_ConvertInterestToInterestReturn(CCNxWireFormatMessage *message, uint8_t returnCode)
{
    bool result = false;

    ccnxWireFormatMessage_OptionalAssertValid(message);
    CCNxWireFormatMessageInterface *impl = ccnxWireFormatMessageInterface_GetInterface(message);

    if (impl != NULL) {
        result = impl->convertInterestToInterestReturn(message, returnCode);
    }
    return result;
}

