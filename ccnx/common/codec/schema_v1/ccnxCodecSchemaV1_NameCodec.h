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
 * @file ccnxCodecSchemaV1_NameCodec.h
 * @brief TLV codec for CCNx types
 *
 * <#Detailed Description#>
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef CCNxCodecSchemaV1_NameCodec_h
#define CCNxCodecSchemaV1_NameCodec_h

#include <ccnx/common/codec/ccnxCodec_TlvEncoder.h>
#include <ccnx/common/codec/ccnxCodec_TlvDecoder.h>
#include <ccnx/common/ccnx_Name.h>

/**
 * Encodes the name to the TLV Encoder
 *
 * Will append the Name after the current encoder location
 *
 * @param [in] type The TLV type to use for the Name container
 *
 * @return bytes The number of bytes appended to the encoder
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t ccnxCodecSchemaV1NameCodec_Encode(CCNxCodecTlvEncoder *encoder, uint16_t type, const CCNxName *name);

/**
 * Decode the buffer as a CCNxName beginning at the current position
 *
 * The buffer must be pointing to the beginnig of the "type".  The decoder will
 * verify that the type matches `type'.  If it does not match, it will return NULL.
 *
 * @param [in] decoder The decoder
 * @param [in] type The TLV type that the decoder should currently be pointing at
 *
 * @return non-null The CCNxName decoded
 * @return null An error: either type did not match or some other error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxName *ccnxCodecSchemaV1NameCodec_Decode(CCNxCodecTlvDecoder *decoder, uint16_t type);

/**
 * The decoder points to the first byte of the Name "value"
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] decoder The Tlv Decoder pointing to the start of the Name value
 * @param [in] length the length of the Name value
 *
 * @return non-null A parsed name
 * @return null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxName *ccnxCodecSchemaV1NameCodec_DecodeValue(CCNxCodecTlvDecoder *decoder, uint16_t length);
#endif // CCNxCodecSchemaV1_NameCodec_h
