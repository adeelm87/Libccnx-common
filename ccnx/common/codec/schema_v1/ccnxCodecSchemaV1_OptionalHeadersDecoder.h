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
 * @file ccnxCodecSchemaV1_OptionalHeadersDecoder.h
 * @brief Decode the list of optional headers
 *
 * A reference to each optional header will be stored in the provided CCNxTlvDictionary.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef TransportRTA_ccnxCodecSchemaV1_OptionalHeadersDecoder_h
#define TransportRTA_ccnxCodecSchemaV1_OptionalHeadersDecoder_h

#include <stdbool.h>

#include <parc/algol/parc_Buffer.h>

#include <ccnx/common/internal/ccnx_TlvDictionary.h>
#include <ccnx/common/codec/ccnxCodec_TlvDecoder.h>

/**
 * The decode a list of Version 1 optional headers
 *
 * The decoder should point to the first byte of the first optional header.
 * The decoder will advance until the end of the buffer.
 * It is an error for the last optional header to either go beyond the end of the
 * decoder or for it to underrun the end of the decoder. It must exactly align.
 *
 * @param [in] decoder The decoder to parse
 * @param [in] packetDictionary The results go directly in to the provided dictionary.
 *
 * @return true Fully parsed interest, no errors
 * @return false Error decoding, decoder is left pointing to the first byte of the error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxCodecSchemaV1OptionalHeadersDecoder_Decode(CCNxCodecTlvDecoder *decoder, CCNxTlvDictionary *packetDictionary);

/**
 * A convenience function to return the Interest Fragment header buffer
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] packetDictionary The packet dictionary
 *
 * @return non-null The header buffer
 * @return null The header does not exist
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCBuffer *ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestFragmentHeader(CCNxTlvDictionary *packetDictionary);

/**
 * A convenience function to return the Content Object Fragment header buffer
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] packetDictionary The packet dictionary
 *
 * @return non-null The header buffer
 * @return null The header does not exist
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCBuffer *ccnxCodecSchemaV1OptionalHeadersDecoder_GetContentObjectFragmentHeader(CCNxTlvDictionary *packetDictionary);

/**
 * A convenience function to return the Interest Lifetime header value
 *
 * Returns the byte array of the Interest Lifetime, which is encoded as a uint64_t milli-seconds
 * since the UTC epoch.  The PARCBuffer returned wraps the underlying memory so any changes to the
 * buffer will be reflected in the header.
 *
 * @param [in] packetDictionary The packet dictionary
 *
 * @return number The Interest Lifetime
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
uint64_t ccnxCodecSchemaV1OptionalHeadersDecoder_GetInterestLifetimeHeader(CCNxTlvDictionary *packetDictionary);

/**
 * A convenience function to return the Recommended Cache Time (RCT) header
 *
 * Returns the byte array of the RCT, which is encoded as a uint64_t milli-seconds
 * since the UTC epoch.  The PARCBuffer returned wraps the underlying memory so any changes to the
 * buffer will be reflected in the header.
 *
 * @param [in] packetDictionary The packet dictionary
 *
 * @return number The Recommended Cache Time
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
uint64_t ccnxCodecSchemaV1OptionalHeadersDecoder_GetRecommendedCacheTimeHeader(CCNxTlvDictionary *packetDictionary);

/**
 * Retrieves a TLV header that is not part of the V1 schema spec
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return non-NULL The header
 * @return NULL The header field does not exist
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCBuffer *ccnxCodecSchemaV1OptionalHeadersDecoder_GetCustomType(CCNxTlvDictionary *packetDictionary, uint32_t key);

#endif // TransportRTA_ccnxCodecSchemaV1_OptionalHeadersDecoder_h
