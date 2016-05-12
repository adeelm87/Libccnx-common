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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <parc/algol/parc_Memory.h>
#include <LongBow/runtime.h>

#include <ccnx/common/codec/ccnxCodec_Error.h>

struct error_messages {
    CCNxCodecErrorCodes code;
    const char *message;
} TlvErrorMessages[] = {
    { .code = TLV_ERR_NO_ERROR,               .message = "No error"                                                        },
    { .code = TLV_ERR_VERSION,                .message = "Unsupported version"                                             },
    { .code = TLV_ERR_PACKETTYPE,             .message = "Unsupported packet type"                                         },
    { .code = TLV_ERR_BEYOND_PACKET_END,      .message = "Field goes beyond end of packet"                                 },
    { .code = TLV_ERR_TOO_LONG,               .message = "Length too long for parent container"                            },
    { .code = TLV_ERR_NOT_FIXED_SIZE,         .message = "Fixed size Type wrong Length"                                    },
    { .code = TLV_ERR_DUPLICATE_FIELD,        .message = "Duplicate field"                                                 },
    { .code = TLV_ERR_EMPTY_SPACE,            .message = "The sum of child TLVs did not add up to parent container length" },

    // missing mandatory field errors
    { .code = TLV_MISSING_MANDATORY,          .message = "Missing mandatory field"                                         },

    { .code = TLV_ERR_DECODE,                 .message = "Decoding error"                                                  },

    { .code = TLV_ERR_PACKETLENGTH_TOO_SHORT, .message = "Packet length less than 8"                                       },
    { .code = TLV_ERR_HEADERLENGTH_TOO_SHORT, .message = "Header length less than 8"                                       },
    { .code = TLV_ERR_PACKETLENGTHSHORTER,    .message = "Packet length less than header length"                           },


    // end of list sentinel, the NULL determines the end of list
    { .code = UINT16_MAX,                     .message = NULL                                                              }
};


const char *
ccnxCodecErrors_ErrorMessage(CCNxCodecErrorCodes code)
{
    for (int i = 0; TlvErrorMessages[i].message != NULL; i++) {
        if (TlvErrorMessages[i].code == code) {
            return TlvErrorMessages[i].message;
        }
    }
    return "No error message found";
}

// ==========================================================================

struct ccnx_codec_error {
    CCNxCodecErrorCodes code;
    const char *functionName;
    int line;
    size_t byteOffset;
    unsigned refcount;
    char *toString;
};

CCNxCodecError *
ccnxCodecError_Create(CCNxCodecErrorCodes code, const char *func, int line, size_t byteOffset)
{
    CCNxCodecError *error = parcMemory_AllocateAndClear(sizeof(CCNxCodecError));
    assertNotNull(error, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(CCNxCodecError));
    error->code = code;
    error->functionName = func;
    error->line = line;
    error->byteOffset = byteOffset;
    error->toString = NULL;      // computed on the fly
    error->refcount = 1;
    return error;
}

CCNxCodecError *
ccnxCodecError_Acquire(CCNxCodecError *error)
{
    assertNotNull(error, "Parameter error must be non-null");
    assertTrue(error->refcount > 0, "Parameter has 0 refcount, not valid");

    error->refcount++;
    return error;
}

void
ccnxCodecError_Release(CCNxCodecError **errorPtr)
{
    assertNotNull(errorPtr, "Parameter must be non-null double pointer");
    assertNotNull(*errorPtr, "Parameter must derefernece to non-null pointer");
    CCNxCodecError *error = *errorPtr;

    assertTrue(error->refcount > 0, "Parameter has 0 refcount, not valid");
    error->refcount--;
    if (error->refcount == 0) {
        if (error->toString) {
            // this is asprintf generated
            free(error->toString);
        }
        parcMemory_Deallocate((void **) &error);
    }
    *errorPtr = NULL;
}

size_t
ccnxCodecError_GetByteOffset(const CCNxCodecError *error)
{
    assertNotNull(error, "Parameter must be non-null");
    return error->byteOffset;
}


CCNxCodecErrorCodes
ccnxCodecError_GetErrorCode(const CCNxCodecError *error)
{
    assertNotNull(error, "Parameter must be non-null");
    return error->code;
}

const char *
ccnxCodecError_GetFunction(const CCNxCodecError *error)
{
    assertNotNull(error, "Parameter must be non-null");
    return error->functionName;
}

int
ccnxCodecError_GetLine(const CCNxCodecError *error)
{
    assertNotNull(error, "Parameter must be non-null");
    return error->line;
}

const char *
ccnxCodecError_GetErrorMessage(const CCNxCodecError *error)
{
    assertNotNull(error, "Parameter must be non-null");
    return ccnxCodecErrors_ErrorMessage(error->code);
}

const char *
ccnxCodecError_ToString(CCNxCodecError *error)
{
    assertNotNull(error, "Parameter must be non-null");
    if (error->toString) {
        return error->toString;
    }

    int failure = asprintf(&error->toString, "TLV error: %s:%d offset %zu: %s",
             error->functionName,
             error->line,
             error->byteOffset,
             ccnxCodecError_GetErrorMessage(error));
	assertTrue(failure > -1, "Error asprintf");

    return error->toString;
}
