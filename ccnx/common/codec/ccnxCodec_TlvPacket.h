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
 * @file rta_TlvPacket.h
 * @brief Encode and decode a packet using the TLV 1.1 codec
 *
 * Will choose the appropriate schema based on the packet version
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef TransportRTA_rta_TlvPacketDecoder_h
#define TransportRTA_rta_TlvPacketDecoder_h

#include <parc/algol/parc_Buffer.h>
#include <parc/security/parc_Signer.h>

#include <ccnx/common/codec/ccnxCodec_NetworkBuffer.h>

#include <ccnx/common/internal/ccnx_TlvDictionary.h>

/**
 * Decodes a packet in to a dictionary
 *
 * The buffer must point to byte 0 of the FixedHeader.  It may extend beyond the
 * end of the packet.
 *
 * @param [in] packetBuffer The wire format representation of a packet
 *
 * @retval non-null An allocated dictionary
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxTlvDictionary *ccnxCodecTlvPacket_Decode(PARCBuffer *packetBuffer);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxCodecTlvPacket_BufferDecode(PARCBuffer *packetBuffer, CCNxTlvDictionary *packetDictionary);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxCodecTlvPacket_IoVecDecode(CCNxCodecNetworkBufferIoVec *vec, CCNxTlvDictionary *packetDictionary);


/**
 * Encode the packetDictionary to wire format
 *
 * Will only use the PacketType from FixedHeader in the dictionary, if provided.  The packet Version is based
 * on the dictionary schema version, and the length fields of the fixed header are calculated.
 * If the FixedHeaderDictionary is not provided, the
 * PacketType is inferred from the type of CCNx message.
 *
 * The signer is not stored beyond the call to DictionaryEncode.
 * If the dictionary already has a ValidationAlg and ValidationPayload, those are used, not the Signer.
 * Otherwise, if the signer is not null, it is used to sign the wire format.
 *
 * @param [in] packetDictionary The dictionary representation of the packet to encode
 * @param [in] signer If not NULL will be used to sign the wire format
 *
 * @retval non-null An IoVec that can be written to the network
 * @retval null an error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxCodecNetworkBufferIoVec *ccnxCodecTlvPacket_DictionaryEncode(CCNxTlvDictionary *packetDictionary, PARCSigner *signer);

/**
 * Return the length of the wire format packet based on information in the header
 *
 * @param [in] packetBuffer a PARCBuffer containing, at least, the wire format header
 * @return length of the message in bytes
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t ccnxCodecTlvPacket_GetPacketLength(PARCBuffer *packetBuffer);

/**
 * Return the minimal header that must be read to determine type and packet size
 *
 * @return length of the header which must be read in bytes
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t ccnxCodecTlvPacket_MinimalHeaderLength();

#endif // TransportRTA_rta_TlvPacketDecoder_h
