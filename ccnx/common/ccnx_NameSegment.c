/*
 * Copyright (c) 2013-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @copyright (c) 2013-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_URI.h>
#include <parc/algol/parc_URISegment.h>
#include <parc/algol/parc_Varint.h>
#include <parc/algol/parc_DisplayIndented.h>
#include <parc/algol/parc_Hash.h>
#include <parc/algol/parc_Object.h>

#include <ccnx/common/ccnx_NameSegment.h>

struct ccnx_name_segment {
    const CCNxNameLabel *label;
    CCNxNameLabelType type;
    PARCBuffer *value;
};

static bool
_ccnxNameSegment_destructor(CCNxNameSegment **segmentP)
{
    assertNotNull(segmentP, "Parameter must be a non-null pointer to a CCNxNameSegment pointer.");

    CCNxNameSegment *segment = *segmentP;
    ccnxNameLabel_Release((CCNxNameLabel **) &(segment->label));
    parcBuffer_Release(&segment->value);
    return true;
}

parcObject_Override(CCNxNameSegment, PARCObject,
                    .destructor = (PARCObjectDestructor *) _ccnxNameSegment_destructor,
                    .copy = (PARCObjectCopy *) ccnxNameSegment_Copy,
                    .equals = (PARCObjectEquals *) ccnxNameSegment_Equals,
                    .compare = (PARCObjectCompare *) ccnxNameSegment_Compare,
                    .hashCode = (PARCObjectHashCode *) ccnxNameSegment_HashCode,
                    .toString = (PARCObjectToString *) ccnxNameSegment_ToString);

parcObject_ImplementAcquire(ccnxNameSegment, CCNxNameSegment);

parcObject_ImplementRelease(ccnxNameSegment, CCNxNameSegment);

CCNxNameSegment *
ccnxNameSegment_CreateLabelValue(const CCNxNameLabel *label, const PARCBuffer *value)
{
    CCNxNameSegment *result = parcObject_CreateInstance(CCNxNameSegment);
    if (result != NULL) {
        result->label = ccnxNameLabel_Acquire(label);
        result->type = ccnxNameLabel_GetType(label);
        result->value = parcBuffer_Acquire(value);
    }
    return result;
}

CCNxNameSegment *
ccnxNameSegment_CreateTypeValue(CCNxNameLabelType type, const PARCBuffer *value)
{
    CCNxNameSegment *result = NULL;
    CCNxNameLabel *label = ccnxNameLabel_Create(type, NULL);

    if (label != NULL) {
        result = ccnxNameSegment_CreateLabelValue(label, value);
        ccnxNameLabel_Release(&label);
    }
    return result;
}

CCNxNameSegment *
ccnxNameSegment_CreateTypeValueArray(CCNxNameLabelType type, size_t length, const char array[length])
{
    PARCBuffer *value = parcBuffer_PutArray(parcBuffer_Allocate(length), length, (const uint8_t *) array);
    parcBuffer_Flip(value);

    CCNxNameSegment *result = ccnxNameSegment_CreateTypeValue(type, value);
    parcBuffer_Release(&value);

    return result;
}

CCNxNameSegment *
ccnxNameSegment_ParseURISegment(const PARCURISegment *uriSegment)
{
    CCNxNameSegment *result = NULL;

    PARCBuffer *buffer = parcURISegment_GetBuffer(uriSegment);

    size_t originalPosition = parcBuffer_Position(buffer);
    CCNxNameLabel *label = ccnxNameLabel_Parse(buffer);

    if (ccnxNameLabel_IsValid(label)) {
        PARCBuffer *value = parcBuffer_Slice(buffer);

        CCNxNameLabelType nameType = ccnxNameLabel_GetType(label);
        if (nameType != CCNxNameLabelType_Unknown) {
            result = ccnxNameSegment_CreateLabelValue(label, value);
        }

        ccnxNameLabel_Release(&label);
        parcBuffer_Release(&value);

        parcBuffer_SetPosition(buffer, originalPosition);
    }

    return result;
}

CCNxNameSegment *
ccnxNameSegment_Copy(const CCNxNameSegment *segment)
{
    PARCBuffer *value = parcBuffer_Copy(segment->value);

    CCNxNameLabel *label = ccnxNameLabel_Copy(segment->label);

    CCNxNameSegment *result = ccnxNameSegment_CreateLabelValue(label, value);
    ccnxNameLabel_Release(&label);

    parcBuffer_Release(&value);
    return result;
}

bool
ccnxNameSegment_Equals(const CCNxNameSegment *segmentA, const CCNxNameSegment *segmentB)
{
    bool result = false;

    if (segmentA == segmentB) {
        result = true;
    } else if (segmentA == NULL || segmentB == NULL) {
        result = false;
    } else {
        if (ccnxNameLabel_Equals(segmentA->label, segmentB->label)) {
            if (parcBuffer_Equals(ccnxNameSegment_GetValue(segmentA), ccnxNameSegment_GetValue(segmentB))) {
                result = true;
            }
        }
    }

    return result;
}

int
ccnxNameSegment_Compare(const CCNxNameSegment *segmentA, const CCNxNameSegment *segmentB)
{
    if (segmentA == NULL) {
        if (segmentB == NULL) {
            return 0;
        }
        return -1;
    } else {
        if (segmentB == NULL) {
            return +1;
        }
    }

    if (ccnxNameSegment_Length(segmentA) < ccnxNameSegment_Length(segmentB)) {
        return -1;
    }
    if (ccnxNameSegment_Length(segmentA) > ccnxNameSegment_Length(segmentB)) {
        return +1;
    }

    int result = parcBuffer_Compare(ccnxNameSegment_GetValue(segmentA), ccnxNameSegment_GetValue(segmentB));
    return result;
}

CCNxNameLabelType
ccnxNameSegment_GetType(const CCNxNameSegment *segment)
{
    return ccnxNameLabel_GetType(segment->label);
}

size_t
ccnxNameSegment_Length(const CCNxNameSegment *segment)
{
    return parcBuffer_Remaining(segment->value);
}

PARCBuffer *
ccnxNameSegment_GetValue(const CCNxNameSegment *segment)
{
    return segment->value;
}

static inline char *
_ccnxNameSegment_IsEscapable(const char c)
{
    return (c == 0 || index("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~", c) == NULL);
}

static inline bool
_ccnxNameSegmentValue_IsEscaped(const PARCBuffer *value)
{
    bool result = false;

    size_t length = parcBuffer_Remaining(value);

    for (size_t i = 0; i < length; i++) {
        if (_ccnxNameSegment_IsEscapable(parcBuffer_GetAtIndex(value, i))) {
            result = true;
            break;
        }
    }

    return result;
}

PARCBufferComposer *
ccnxNameSegment_BuildString(const CCNxNameSegment *segment, PARCBufferComposer *composer)
{
    // Insert the label.  However, in the case of an unescaped Name value, the Name lable portion can be left off.

    if (ccnxNameLabel_GetType(segment->label) != CCNxNameLabelType_NAME || _ccnxNameSegmentValue_IsEscaped(segment->value)) {
        ccnxNameLabel_BuildString(segment->label, composer);
    }

    if (ccnxNameSegment_Length(segment) > 0) {
        PARCURISegment *uriSegment = parcURISegment_CreateFromBuffer(ccnxNameSegment_GetValue(segment));
        parcURISegment_BuildString(uriSegment, composer);

        parcURISegment_Release(&uriSegment);
    }

    return composer;
}

PARCHashCode
ccnxNameSegment_HashCode(const CCNxNameSegment *segment)
{
    PARCHash32Bits *hash = parcHash32Bits_Create();

    parcHash32Bits_Update(hash, &segment->type, sizeof(segment->type));
    if (parcBuffer_Remaining(segment->value) > 0) {
        parcHash32Bits_UpdateUint32(hash, parcBuffer_HashCode(segment->value));
    }

    uint32_t result = parcHash32Bits_Hash(hash);

    parcHash32Bits_Release(&hash);

    return result;
}

char *
ccnxNameSegment_ToString(const CCNxNameSegment *segment)
{
    char *result = NULL;

    PARCBufferComposer *composer = parcBufferComposer_Create();
    if (composer != NULL) {
        ccnxNameSegment_BuildString(segment, composer);
        PARCBuffer *tempBuffer = parcBufferComposer_ProduceBuffer(composer);
        result = parcBuffer_ToString(tempBuffer);
        parcBuffer_Release(&tempBuffer);
        parcBufferComposer_Release(&composer);
    }

    return result;
}

void
ccnxNameSegment_Display(const CCNxNameSegment *segment, int indentation)
{
    parcDisplayIndented_PrintLine(indentation, "CCNxNameSegment@%p {", segment);
    parcDisplayIndented_PrintLine(indentation + 1, "type=%d", segment->type);
    parcBuffer_Display(segment->value, indentation + 1);
    parcDisplayIndented_PrintLine(indentation, "}");
}

void
ccnxNameSegment_AssertValid(const CCNxNameSegment *segment)
{
    assertTrue(ccnxNameSegment_IsValid(segment), "CCNxNameSegment is invalid.");
}

bool
ccnxNameSegment_IsValid(const CCNxNameSegment *segment)
{
    bool result = false;

    if (segment != NULL) {
        if (parcBuffer_IsValid(segment->value) == true) {
            result = true;
        }
    }

    return result;
}
