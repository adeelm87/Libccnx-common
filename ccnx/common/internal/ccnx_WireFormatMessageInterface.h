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
 * @brief The definition of the interface used to call into a CCNxWireFormatFacade implementation.
 *
 * @author Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef CCNx_Common_ccnx_internal_WireFormatMessageInterface_h
#define CCNx_Common_ccnx_internal_WireFormatMessageInterface_h

#include <ccnx/common/internal/ccnx_TlvDictionary.h>

typedef struct ccnx_wireformatmessage_interface {
    char              *description;      // A human-readable label for this implementation

    /** @see ccnxWireFormatMessage_Create */
    CCNxTlvDictionary *(*create)(const PARCBuffer * wireFormat);

    /** @see ccnxWireFormatMessage_FromInterestPacketType */
    CCNxTlvDictionary *(*fromInterestPacketType)(const PARCBuffer * wireFormat);

    /** @see ccnxWireFormatMessage_FromInterestPacketTypeIoVec */
    CCNxTlvDictionary *(*fromInterestPacketTypeIoVec)(const CCNxCodecNetworkBufferIoVec * vec);

    /** @see ccnxWireFormatMessage_FromContentObjectPacketType */
    CCNxTlvDictionary *(*fromContentObjectPacketType)(const PARCBuffer * wireFormat);

    /** @see ccnxWireFormatMessage_FromControlPacketType */
    CCNxTlvDictionary *(*fromControlPacketType)(const PARCBuffer * wireFormat);

    /** @see ccnxWireFormatMessage_GetWireFormatBuffer */
    PARCBuffer        *(*getWireFormatBuffer)(const CCNxTlvDictionary * dictionary);

    /** @see ccnxWireFormatMessage_GetIoVec */
    CCNxCodecNetworkBufferIoVec *(*getIoVec)(const CCNxTlvDictionary * dictionary);

    /** @see ccnxWireFormatMessage_PutWireFormatBuffer */
    bool (*putWireFormatBuffer)(CCNxTlvDictionary *dictionary, PARCBuffer *buffer);

    /** @see ccnxWireFormatMessage_PutIoVec */
    bool (*putIoVec)(CCNxTlvDictionary *dictionary, CCNxCodecNetworkBufferIoVec *vec);

    /** @see ccnxWireFormatMessage_WriteToFile */
    void (*writeToFile)(const CCNxTlvDictionary *dictionary, const char *filename);

    /** @see ccnxWireFormatMessage_SetProtectedRegionStart */
    bool (*setProtectedRegionStart)(CCNxTlvDictionary *dictionary, size_t startPosition);

    /** @see ccnxWireFormatMessage_SetProtectedRegionLength */
    bool (*setProtectedRegionLength)(CCNxTlvDictionary *dictionary, size_t length);

    /** @see ccnxWireFormatMessage_SetContentObjectHashRegionStart */
    bool (*setContentObjectHashRegionStart)(CCNxTlvDictionary *dictionary, size_t startPosition);

    /** @see ccnxWireFormatMessage_SetContentObjectHashRegionLength */
    bool (*setContentObjectHashRegionLength)(CCNxTlvDictionary *dictionary, size_t length);

    /** @see ccnxWireFormatMessage_HashProtectedRegion */
    PARCCryptoHash    *(*hashProtectedRegion)(const CCNxTlvDictionary * dictionary, PARCCryptoHasher * hasher);

    /** @see ccnxWireFormatMessage_SetHopLimit */
    bool (*setHopLimit)(CCNxTlvDictionary *dictionary, uint32_t hopLimit);

    /** @see ccnxWireFormatMessage_AssertValid*/
    void (*assertValid)(const CCNxTlvDictionary *dictionary);

    /** @see ccnxWireFormatMessage_CreateContentObjectHash */
    PARCCryptoHash    *(*computeContentObjectHash)(CCNxTlvDictionary * dictionary);

    /** @see ccnxWireFormatMessage_ConvertInterestToInterestReturn */
    bool (*convertInterestToInterestReturn)(CCNxTlvDictionary *dictionary, uint8_t returnCode);
} CCNxWireFormatMessageInterface;

/**
 * The SchemaV0 WireFormatMessage implementaton
 */
extern CCNxWireFormatMessageInterface CCNxWireFormatFacadeV0_Implementation;

/**
 * The SchemaV1 WireFormatMessage implementaton
 */
extern CCNxWireFormatMessageInterface CCNxWireFormatFacadeV1_Implementation;

/**
 * Given a CCNxTlvDictionary representing a CCNxWireFormatMessage, return the address of the CCNxWireFormatMessageInterface
 * instance that should be used to access the WireFormatMessage.
 *
 * @param interestDictionary - a {@link CCNxTlvDictionary} representing a CCNxInterestReturn.
 * @return the address of the `CCNxWireFormatMessageInterface` instance that should be used to access the CCNxWireFormatMessage.
 *
 * Example:
 * @code
 * {
 *
 *     CCNxWireFormatMessage *message = ccnxWireFormatMessage_FromInterestPacketType(schemaVersion, wireFormatBuffer);
 *
 *     //V1 test
 *     if (ccnxWireFormatMessageInterface_GetInterface(interestReturn) == &CCNxWireFormatFacadeV1_Implementation) {
 *         printf("Using a V1 CCNxWireFormatFacadeV1_Implementation \n");
 *     }
 *
 *     ...
 *
 *     ccnxWireFormatMessage_Release(&message);
 * } * @endcode
 */
CCNxWireFormatMessageInterface *ccnxWireFormatMessageInterface_GetInterface(const CCNxTlvDictionary *contentDictionary);
#endif
