/*
 * Copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @file ccnxCodecSchemaV1_HashCodec.h
 * @brief A cryptographic hash digest encoded
 *
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef __CCNx_Common__ccnxCodecSchemaV1_HashCodec__
#define __CCNx_Common__ccnxCodecSchemaV1_HashCodec__

#include <ccnx/common/codec/ccnxCodec_TlvEncoder.h>
#include <ccnx/common/codec/ccnxCodec_TlvDecoder.h>

#include <parc/security/parc_CryptoHash.h>

/**
 * Encodes the hash, but without a "TL" container
 *
 * Will append the Link in it's well-known TLV format, but without any
 * "TL" container.
 *
 * If the link does not have a name, will return -1 with the error TLV_MISSING_MANDATORY.
 *
 * @param [in] encoder The hash will be appended to the encoder
 * @param [in] hash The hash to append
 *
 * @retval non-negative The number of bytes appended to the encoder
 * @retval negative An error, look at the CCNxCodecError of the encoder
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
ssize_t ccnxCodecSchemaV1HashCodec_Encode(CCNxCodecTlvEncoder *encoder, const PARCCryptoHash *hash);

/**
 * The decoder points to the first byte of the "value" of something that is a Link
 *
 * For a KeyName, decoder should be pointed to the "value" of the KeyName.  for a ContentObject
 * of type Link, it should be the first byte of the Payload.
 *
 * A link is the tuple {Name, [KeyId], [Hash]}, where KeyId is the keyIdRestriction and
 * Hash is the ContentObjectHash restriction to use in an Interest for Name.
 * No additional fields are allowed in the Link.
 *
 * @param [in] decoder The Tlv Decoder pointing to the start of the Name value
 * @param [in] length the length of the Hash value
 *
 * @return non-null A parsed name
 * @return null An error, check the decoder's error message
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCCryptoHash *ccnxCodecSchemaV1HashCodec_DecodeValue(CCNxCodecTlvDecoder *decoder, size_t length);

#endif // __CCNx_Common__ccnxCodecSchemaV1_HashCodec__
