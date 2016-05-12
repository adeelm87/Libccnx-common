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
 * @file ccnxCodec_Error.h
 * @brief Wraps an error condition in the Tlv codec
 *
 * <#Detailed Description#>
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef libccnx_ccnxCodec_Error_h
#define libccnx_ccnxCodec_Error_h

#include <stdlib.h>
#include <ccnx/common/codec/ccnxCodec_ErrorCodes.h>

struct ccnx_codec_error;
typedef struct ccnx_codec_error CCNxCodecError;

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const char *ccnxCodecError_ErrorMessage(CCNxCodecErrorCodes code);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxCodecError *ccnxCodecError_Create(CCNxCodecErrorCodes code, const char *func, int line, size_t byteOffset);

/**
 * Returns a reference counted copy of the error
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return non-null A reference counted copy
 * @return null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxCodecError *ccnxCodecError_Acquire(CCNxCodecError *error);

/**
 * Releases a reference count
 *
 * When the reference count reaches 0, the object is destroyed.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void ccnxCodecError_Release(CCNxCodecError **errorPtr);

/**
 * The byte offset of the error
 *
 *   Primarily for decoding errors.  It will contain the byte offset of the first byte
 *   of the field causing the error.  For encoding, it will be the byte offer of the
 *   partially-encoded buffer, but the error is usually in the native format, not the
 *   partially encoded buffer.
 *
 * @param <#param1#>
 * @return The byte offset of the error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t ccnxCodecError_GetByteOffset(const CCNxCodecError *error);


/**
 * If there was a decode error, return the error code
 *
 *   A text message is available from <code>tlvErrors_ErrorMessage()</code>.
 *
 * @param <#param1#>
 * @return Returns the error code, or TVL_ERR_NO_ERROR for successful decode
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxCodecErrorCodes ccnxCodecError_GetErrorCode(const CCNxCodecError *error);

/**
 * The function where the error occured
 *
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const char *ccnxCodecError_GetFunction(const CCNxCodecError *error);

/**
 * The line where the error occured
 *
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
int ccnxCodecError_GetLine(const CCNxCodecError *error);

/**
 * Descriptive error message
 *
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return A static text string
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const char *ccnxCodecError_GetErrorMessage(const CCNxCodecError *error);

/**
 * A string representation of the entire error
 *
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return An internally allocated string, do not destroy it
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const char *ccnxCodecError_ToString(CCNxCodecError *error);
#endif // libccnx_ccnx_TlvError_h
