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
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include <parc/algol/parc_Object.h>
#include <LongBow/runtime.h>

#include <ccnx/common/ccnx_NameLabel.h>

struct ccnx_name_label {
    CCNxNameLabelType type;
    PARCBuffer *parameter;
};

static struct CCNxNameLabelMnemonic {
    const char *mnemonic;
    const CCNxNameLabelType type;
} CCNxNameLabelMnemonic[] = {
    { CCNxNameLabel_Name,              CCNxNameLabelType_NAME      },
    { CCNxNameLabel_InterestPayloadId, CCNxNameLabelType_PAYLOADID },
    { CCNxNameLabel_Serial,            CCNxNameLabelType_SERIAL    },
    { CCNxNameLabel_Chunk,             CCNxNameLabelType_CHUNK     },
    { CCNxNameLabel_ChunkMeta,         CCNxNameLabelType_CHUNKMETA },
    { CCNxNameLabel_App,               CCNxNameLabelType_APP0      },
    { CCNxNameLabel_Time,              CCNxNameLabelType_TIME      },
    { NULL,                            CCNxNameLabelType_Unknown   },
};

static bool
_ccnxNameLabel_Destructor(CCNxNameLabel **labelP)
{
    assertNotNull(labelP, "Parameter must be a non-null pointer to a CCNxNameLabels pointer.");

    CCNxNameLabel *label = *labelP;
    if (label->parameter != NULL) {
        parcBuffer_Release(&label->parameter);
    }
    return true;
}

parcObject_Override(CCNxNameLabel, PARCObject,
                    .destructor = (PARCObjectDestructor *) _ccnxNameLabel_Destructor);

parcObject_ImplementAcquire(ccnxNameLabel, CCNxNameLabel);

parcObject_ImplementRelease(ccnxNameLabel, CCNxNameLabel);

CCNxNameLabel *
ccnxNameLabel_Create(CCNxNameLabelType type, const PARCBuffer *parameter)
{
    CCNxNameLabel *result = NULL;

    if (type != CCNxNameLabelType_BADNAME && type != CCNxNameLabelType_Unknown) {
        result = parcObject_CreateInstance(CCNxNameLabel);
        if (result != NULL) {
            result->type = type;
            result->parameter = parameter == NULL ? NULL : parcBuffer_Acquire(parameter);
        }
    }

    return result;
}

CCNxNameLabelType
ccnxNameLabel_GetType(const CCNxNameLabel *label)
{
    ccnxNameLabel_OptionalAssertValid(label);

    return label->type;
}

PARCBuffer *
ccnxNameLabel_GetParameter(const CCNxNameLabel *label)
{
    ccnxNameLabel_OptionalAssertValid(label);
    return label->parameter;
}

static const char *
_ccnxNameLabelType_ToMnemonic(CCNxNameLabelType type)
{
    const char *result = NULL;

    for (struct CCNxNameLabelMnemonic *p = &CCNxNameLabelMnemonic[0]; p->mnemonic != NULL; p++) {
        if (p->type == type) {
            result = p->mnemonic;
            break;
        }
    }

    return result;
}

static CCNxNameLabelType
_ccnxNameLabelType_ResolveNumeric(PARCBuffer *label)
{
    CCNxNameLabelType result = (CCNxNameLabelType) parcBuffer_ParseNumeric(label);

    return result;
}

static CCNxNameLabelType
_ccnxNameLabelType_ResolveMnemonic(const PARCBuffer *label)
{
    CCNxNameLabelType result = CCNxNameLabelType_Unknown;

    size_t labelLength = parcBuffer_Remaining(label);
    char *labelAsBytes = parcBuffer_Overlay((PARCBuffer *) label, 0);

    for (struct CCNxNameLabelMnemonic *p = &CCNxNameLabelMnemonic[0]; p->mnemonic != NULL; p++) {
        if (strncasecmp(p->mnemonic, labelAsBytes, labelLength) == 0) {
            result = p->type;
            break;
        }
    }

    return result;
}

static CCNxNameLabelType
_ccnxNameLabelType_Resolve(PARCBuffer *label)
{
    CCNxNameLabelType result = CCNxNameLabelType_NAME;

    if (label != NULL) {
        if (parcBuffer_Remaining(label) > 0) {
            if (isdigit(parcBuffer_PeekByte(label))) {
                result = _ccnxNameLabelType_ResolveNumeric(label);
            } else {
                result = _ccnxNameLabelType_ResolveMnemonic(label);
            }
        } else {
            result = CCNxNameLabelType_NAME;
        }
    }

    return result;
}

/*
 * Parse the 'label [":" param] ' portion of an lpv-segment
 *
 *  lpv-segment      = label [":" param] "=" s-value
 *  v-segment        = s-value
 *  label            = alpha-t / num-t
 *  param            = alpha-t / num-t
 *  s-value          = *(s-pchar)
 *
 * number =
 * number
 */
CCNxNameLabel *
ccnxNameLabel_Parse(PARCBuffer *buffer)
{
    PARCBuffer *label = NULL;
    PARCBuffer *parameter = NULL;

    // When complete, the buffer's position will be set to the first byte of the value portion.
    if (parcBuffer_SkipTo(buffer, 1, (uint8_t *) "=")) {
        label = parcBuffer_Flip(parcBuffer_Duplicate(buffer));
        if (parcBuffer_Remaining(label) > 0) {
            parcBuffer_SetPosition(buffer, parcBuffer_Position(buffer) + 1);

            if (parcBuffer_SkipTo(label, 1, (uint8_t *) ":")) {
                size_t colon = parcBuffer_Position(label);
                parcBuffer_SetPosition(label, colon + 1);
                parameter = parcBuffer_Slice(label);

                parcBuffer_SetPosition(label, colon);
            }

            label = parcBuffer_Flip(label);
        } else {
            parcBuffer_Release(&label);
            return NULL;
        }
    } else {
        parcBuffer_Rewind(buffer);
    }

    CCNxNameLabelType type = CCNxNameLabelType_NAME;

    if (label != NULL) {
        type = _ccnxNameLabelType_Resolve(label);
        if (type == CCNxNameLabelType_App(0)) {
            if (parameter != NULL) {
                type = CCNxNameLabelType_App(parcBuffer_ParseNumeric(parameter));
                parcBuffer_Release(&parameter);
            }
        }
        parcBuffer_Release(&label);
    }

    CCNxNameLabel *result = ccnxNameLabel_Create(type, parameter);

    if (parameter != NULL) {
        parcBuffer_Release(&parameter);
    }

    return result;
}

bool
ccnxNameLabel_Equals(const CCNxNameLabel *x, const CCNxNameLabel *y)
{
    bool result = false;

    if (x == y) {
        result = true;
    } else if (x == NULL || y == NULL) {
        result = false;
    } else {
        if (x->type == y->type) {
            if (parcBuffer_Equals(x->parameter, y->parameter)) {
                result = true;
            }
        }
    }

    return result;
}

CCNxNameLabel *
ccnxNameLabel_Copy(const CCNxNameLabel *label)
{
    ccnxNameLabel_OptionalAssertValid(label);

    PARCBuffer *parameter = (label->parameter == NULL) ? NULL : parcBuffer_Copy(label->parameter);
    CCNxNameLabel *result = ccnxNameLabel_Create(label->type, parameter);
    if (parameter != NULL) {
        parcBuffer_Release(&parameter);
    }

    return result;
}

bool
ccnxNameLabel_IsValid(const CCNxNameLabel *label)
{
    bool result = false;

    if (label != NULL) {
        if (label->type != CCNxNameLabelType_Unknown && label->type != CCNxNameLabelType_BADNAME) {
            if (label->parameter != NULL) {
                if (parcBuffer_IsValid(label->parameter)) {
                    result = true;
                }
            } else {
                result = true;
            }
        }
    }
    return result;
}

void
ccnxNameLabel_AssertValid(const CCNxNameLabel *label)
{
    trapIllegalValueIf(ccnxNameLabel_IsValid(label) == false, "Encountered an invalid CCNxNameLabel instance.");
}

PARCBufferComposer *
ccnxNameLabel_BuildString(const CCNxNameLabel *label, PARCBufferComposer *composer)
{
    ccnxNameLabel_OptionalAssertValid(label);

    if (label->type >= CCNxNameLabelType_App(0) && label->type <= CCNxNameLabelType_App(4096)) {
        parcBufferComposer_Format(composer, "%s:%u=", CCNxNameLabel_App, label->type - CCNxNameLabelType_App(0));
    } else {
        const char *mnemonic = _ccnxNameLabelType_ToMnemonic(label->type);
        if (mnemonic == NULL) {
            parcBufferComposer_Format(composer, "%u", label->type);
        } else {
            parcBufferComposer_PutString(composer, mnemonic);
        }

        if (label->parameter != NULL) {
            parcBufferComposer_PutString(composer, ":");
            parcBufferComposer_PutBuffer(composer, label->parameter);
        }
        parcBufferComposer_PutString(composer, "=");
    }

    return composer;
}

char *
ccnxNameLabel_ToString(const CCNxNameLabel *label)
{
    PARCBufferComposer *composer = parcBufferComposer_Create();
    ccnxNameLabel_BuildString(label, composer);

    char *result = parcBufferComposer_ToString(composer);

    parcBufferComposer_Release(&composer);

    return result;
}
