/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @brief A structure of functions to enable accessing a CCNxTlvDictionary as a
 *        wire format object.
 *
 * @author Marc Mosko, Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef __CCNx_Common__ccnx_WireFormatMessage__
#define __CCNx_Common__ccnx_WireFormatMessage__

#include <ccnx/common/internal/ccnx_WireFormatMessageInterface.h>
#include <ccnx/common/internal/ccnx_TlvDictionary.h>

#include <parc/security/parc_CryptoHash.h>

typedef CCNxTlvDictionary CCNxWireFormatMessage;

/**
 * Creates a new CCNxWireFormatMessage instance from the `wireFormat` buffer passed in.
 * The schema version and the message type are determined from `wireFormat`.
 *
 * @param wireFormat - a buffer, in wire format, representing the message to created.
 * @return A new instance of CCNxWireFormatMessage that must eventually be released by calling
 *          {@link ccnxWireFormatMessage_Release}.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 * @see ccnxWireFormatMessage_Release
 */
CCNxWireFormatMessage *ccnxWireFormatMessage_Create(const PARCBuffer *wireFormat);

/**
 * Creates dictionary of Interest type from the wire format
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
CCNxWireFormatMessage *ccnxWireFormatMessage_FromInterestPacketType(const CCNxTlvDictionary_SchemaVersion schemaVersion, const PARCBuffer *wireFormat);

/**
 * Creates dictionary of Interest type from the wire format
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
CCNxWireFormatMessage *ccnxWireFormatMessage_FromInterestPacketTypeIoVec(const CCNxTlvDictionary_SchemaVersion schemaVersion, const CCNxCodecNetworkBufferIoVec *vec);

/**
 * Creates a dictionary of ContentObject type from the wire format
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
CCNxWireFormatMessage *ccnxWireFormatMessage_FromContentObjectPacketType(const CCNxTlvDictionary_SchemaVersion schemaVersion, const PARCBuffer *wireFormat);

/**
 * Creates a dictionary of Control type from the wire format
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
CCNxWireFormatMessage *ccnxWireFormatMessage_FromControlPacketType(const CCNxTlvDictionary_SchemaVersion schemaVersion, const PARCBuffer *wireFormat);

#ifdef Libccnx_DISABLE_VALIDATION
#  define ccnxWireFormatMessage_OptionalAssertValid(_instance_)
#else
#  define ccnxWireFormatMessage_OptionalAssertValid(_instance_) ccnxWireFormatMessage_AssertValid(_instance_)
#endif

/**
 * Assert that the given `CCNxWireFormatMessage` instance is valid.
 *
 * @param [in] instance A pointer to a valid CCNxWireFormatMessage instance.
 *
 * Example:
 * @code
 * {
 *     CCNxWireFormatMessage *a = ccnxWireFormatMessage_Create();
 *
 *     ccnxWireFormatMessage_AssertValid(a);
 *
 *     printf("Instance is valid.\n");
 *
 *     ccnxWireFormatMessage_Release(&b);
 * }
 * @endcode
 */
void ccnxWireFormatMessage_AssertValid(const CCNxWireFormatMessage *message);

/**
 * Returns the PARCBuffer that wraps the entire wireformat representation
 *
 * May be NULL if there is no wire format yet (e.g. you're going down the stack before the codec)
 *
 * @param [<#in#> | <#out#> | <#in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 */
PARCBuffer *ccnxWireFormatMessage_GetWireFormatBuffer(const CCNxWireFormatMessage *dictionary);

/**
 * Returns the CCNxCodecNetworkBufferIoVec that wraps the entire wireformat representation
 *
 * May be NULL if there is no wire format yet (e.g. you're going down the stack before the codec), or it
 * may be NULL because the wireformat is wrapped in a PARCBuffer instead.
 *
 * @param [<#in#> | <#out#> | <#in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 */
CCNxCodecNetworkBufferIoVec *ccnxWireFormatMessage_GetIoVec(const CCNxWireFormatMessage *dictionary);


/**
 * Sets the wire format buffer in a Dictionary
 *
 * You can only put the wire format once.  Will store its down reference to the wire format, so
 * caller should release their copy when done with it.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return true Value was put in the dictionary
 * @return false Value not put, likely already existed
 *
 * Example:
 * @code
 * {
 *
 * }
 * @endcode
 */
bool ccnxWireFormatMessage_PutWireFormatBuffer(CCNxWireFormatMessage *dictionary, PARCBuffer *buffer);

/**
 * Sets the wire format buffer in a Dictionary
 *
 * You can only put the wire format once.  Will store its down reference to the wire format, so
 * caller should release their copy when done with it.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return true Value was put in the dictionary
 * @return false Value not put, likely already existed
 *
 * Example:
 * @code
 * {
 *
 * }
 * @endcode
 */
bool ccnxWireFormatMessage_PutIoVec(CCNxWireFormatMessage *dictionary, CCNxCodecNetworkBufferIoVec *vec);

/**
 * Writes the wire format to the specified file
 *
 * The file will be truncated to 0.  If there is no wire format, the file will remain at 0 bytes.
 *
 * You can view the packet with the Wireshark plugin.  First convert it to a TCP enapsulation
 * (the program text2pcap is included as part of WireShark).  The part "- filename.pcap" means that
 * text2pcap will read from stdin (the "-") and write to "filename.pcap".  Text2pcap requires a
 * text format derived from "od" or from "hexdump".
 *
 *     od -Ax -tx1 -v filename | text2pcap -T 9695,9695 - filename.pcap
 *     tshark -r filename.pcap -V
 *
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
void ccnxWireFormatMessage_WriteToFile(const CCNxWireFormatMessage *dictionary, const char *filename);

/**
 * Write to the dictionary the start of the protection region
 *
 * The protected region is the byte range that is protected by the signature (validation payload).
 * This sets the validation region in the dictionary.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return false A validation region is already set, cannot overwrite
 * @return true The validation region was set in the dictionary
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxWireFormatMessage_SetProtectedRegionStart(CCNxWireFormatMessage *dictionary, size_t startPosition);

/**
 * Write to the dictionary the length of the protection region
 *
 * The protected region is the byte range that is protected by the signature (validation payload).
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return false A validation region is already set, cannot overwrite
 * @return true The validation region was set in the dictionary
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxWireFormatMessage_SetProtectedRegionLength(CCNxWireFormatMessage *dictionary, size_t length);

/**
 * Runs a hasher over the protected part of the wire format message
 *
 * The protected part of the wire format message is from the start of the CCNx Message
 * through the end of the Validation Algorithm.
 *
 * If no validation algorithm block exists, then there is no protection region and
 * we cannot compute the hash.
 *
 * The protection region is computed by the CCNxCodecTlvDecoder and stored in the dictionary.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return null There is no protection region
 * @return non-null The computed hash
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCCryptoHash *ccnxWireFormatMessage_HashProtectedRegion(const CCNxWireFormatMessage *dictionary, PARCCryptoHasher *hasher);

/**
 * Calculates the ContentObject Hash, which is the SHA256 hash of the protected part of the wire format message.
 *
 * The protected part of the wire format message is from the start of the CCNx Message
 * through the end of the Validation Algorithm.
 *
 * The protection region is computed by the CCNxCodecTlvDecoder and stored in the dictionary.
 *
 * This function must only be called on a dictionary that contains a ContentObject and a WireFormat buffer.
 *
 * @param [in] dictionary The ContentObject message on which to calculate its hash.
 *
 * @return null if we could not calculate the hash. This could mean the protected region wasn't set, or it wasn't a ContentObject.
 * @return non-null The computed hash
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */

PARCCryptoHash *ccnxWireFormatMessage_CreateContentObjectHash(CCNxWireFormatMessage *dictionary);

/**
 * Returns a pointer to the CCNxTlvDictionary underlying the specified CCNxWireFormatMessage.
 *
 * @param message - The CCNxWireFormatMessage instance from which to retrieve the CCNxTlvDictionary.
 * @return A pointer to the underlying CCNxTlvDictionary
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxTlvDictionary *ccnxWireFormatMessage_GetDictionary(const CCNxWireFormatMessage *message);

/**
 * Increase the number of references to a `CCNxWireFormatMessage`.
 *
 * Note that a new `CCNxWireFormatMessage` is not created,
 * only that the given `CCNxWireFormatMessage` reference count is incremented.
 * Discard the reference by invoking {@link ccnxWireFormatMessage_Release}.
 *
 * @param [in] message A pointer to the original instance.
 * @return The value of the input parameter @p instance.
 *
 * Example:
 * @code
 * {
 *
 *     CCNxWireFormat *reference = ccnxWireFormatMessage_Acquire(message);
 *
 *     ...
 *
 *     ccnxWireFormatMessage_Release(&reference);
 *
 * }
 * @endcode
 *
 * @see {@link ccnxWireFormatMessage_Release}
 */
CCNxWireFormatMessage *ccnxWireFormatMessage_Acquire(const CCNxWireFormatMessage *message);

/**
 * Release a previously acquired reference to the specified instance,
 * decrementing the reference count for the instance.
 *
 * The pointer to the instance is set to NULL as a side-effect of this function.
 *
 * If the invocation causes the last reference to the instance to be released,
 * the instance is deallocated and the instance's implementation will perform
 * additional cleanup and release other privately held references.
 *
 * @param [in,out] messageP A pointer to a pointer to the instance to release.
 *
 * Example:
 * @code
 * {
 *     CCNxWireFormat *reference = ccnxWireFormatMessage_Acquire(message);
 *
 *     ...
 *
 *     ccnxWireFormatMessage_Release(&reference);
 *
 * }
 * @endcode
 *
 * @see {@link ccnxWireFormatMessage_Acquire}
 */
void ccnxWireFormatMessage_Release(CCNxWireFormatMessage **messageP);

/**
 * Write a hoplimit to a messages attached wire format io vectors or buffers
 *
 * @param [in] message
 * @param [in] hoplimit
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxWireFormatMessage_SetHopLimit(CCNxWireFormatMessage *message, uint32_t hoplimit);

// WALENDO: doc
bool ccnxWireFormatMessage_ConvertInterestToInterestReturn(CCNxWireFormatMessage *message, uint8_t returnCode);

#endif /* defined(__CCNx_Common__ccnx_WireFormatMessage__) */
