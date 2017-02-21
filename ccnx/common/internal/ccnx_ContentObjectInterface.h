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
 * @brief A structure of functions representing a ContentObject implementation.
 *
 * The underlying implementation should support the CCNxContentObject API.
 *
 * @author Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef CCNx_Common_ccnx_internal_ContentObjectInterface_h
#define CCNx_Common_ccnx_internal_ContentObjectInterface_h

#include <parc/algol/parc_Buffer.h>

#include <ccnx/common/internal/ccnx_TlvDictionary.h>

#include <ccnx/common/ccnx_Name.h>
#include <ccnx/common/ccnx_KeyLocator.h>
#include <ccnx/common/ccnx_PayloadType.h>

typedef struct ccnx_contentobject_interface {
    char               *description;      // A human-readable label for this implementation

    /** @see ccnxContentObject_CreateWithNameAndPayload */
    CCNxTlvDictionary  *(*createWithNameAndPayload)(const CCNxName * contentName,
                                                    const CCNxPayloadType contentType,
                                                    const PARCBuffer * payload);

    /** @see ccnxContentObject_CreateWithPayload */
    CCNxTlvDictionary  *(*createWithPayload)(const CCNxPayloadType contentType,
                                             const PARCBuffer * payload);

    /** @see ccnxContentObject_GetName */
    CCNxName           *(*getName)(const CCNxTlvDictionary * dict);

    /** @see ccnxContentObject_SetSignature */
    bool                (*setSignature)(CCNxTlvDictionary *dict,
                                        const PARCBuffer *keyId,
                                        const PARCSignature *signature,
                                        const CCNxKeyLocator *keyLocator);

    /** @see ccnxContentObject_GetKeyId */
    PARCBuffer         *(*getKeyId)(const CCNxTlvDictionary * dict);

    /** @see ccnxContentObject_GetPayload */
    PARCBuffer         *(*getPayload)(const CCNxTlvDictionary * dict);

    /** @see ccnxContentObject_GetPayloadType */
    CCNxPayloadType     (*getPayloadType)(const CCNxTlvDictionary *dict);

    /** @see ccnxContentObject_SetPayload */
    bool                (*setPayload)(CCNxTlvDictionary *dict, CCNxPayloadType payloadType, const PARCBuffer *payload);

    /** @see ccnxContentObject_SetFinalChunkNumber */
    bool                (*setFinalChunkNumber)(CCNxTlvDictionary *dict, const uint64_t finalChunkNumber);

    /** @see ccnxContentObject_GetFinalChunkNumber */
    uint64_t            (*getFinalChunkNumber)(const CCNxTlvDictionary *dict);

    /** @see ccnxContentObject_HasFinalChunkNumber */
    bool                (*hasFinalChunkNumber)(const CCNxTlvDictionary *dict);

    /** @see ccnxContentObject_SetExpiryTime */
    bool                (*setExpiryTime)(CCNxTlvDictionary *dict, const uint64_t expiryTime);

    /** @see ccnxContentObject_GetExpiryTime */
    uint64_t            (*getExpiryTime)(const CCNxTlvDictionary *dict);

    /** @see ccnxContentObject_HasExpiryTime */
    bool                (*hasExpiryTime)(const CCNxTlvDictionary *dict);

    /** @see ccnxContentObject_SetMessageId */
    bool                (*setMessageId)(CCNxTlvDictionary *dict, const uint32_t messageId);

    /** @see ccnxContentObject_GetMessageId */
    uint32_t            (*getMessageId)(const CCNxTlvDictionary *dict);

    /** @see ccnxContentObject_HasMessageId */
    bool                (*hasMessageId)(const CCNxTlvDictionary *dict);    

    /** @see ccnxContentObject_Equals */
    bool                (*equals)(const CCNxTlvDictionary *objectA, const CCNxTlvDictionary *objectB);

    /** @see ccnxContentObject_AssertValid */
    void                (*assertValid)(const CCNxTlvDictionary *dict);

    /** @see ccnxContentObject_ToString */
    char               *(*toString)(const CCNxTlvDictionary * dict);

    /** @see ccnxContentObject_CreateWithPayload */
    void                (*display)(const CCNxTlvDictionary *interestDictionary, size_t indentation);
} CCNxContentObjectInterface;

/**
 * The SchemaV1 ContentObject implementaton
 */
extern CCNxContentObjectInterface CCNxContentObjectFacadeV1_Implementation;

/**
 * Given a CCNxTlvDictionary representing a CCNxContentObject, return the address of the CCNxContentObjectInterface
 * instance that should be used to access the ContentObject. This will also update the CCNxTlvDictionary's implementation
 * pointer for future references.
 *
 * @param contentDictionary - a {@link CCNxTlvDictionary} representing a CCNxContentObject.
 * @return the address of the `CCNxContentObjectInterface` instance that should be used to access the CCNxContentObject.
 *
 * Example:
 * @code
 * {
 *      CCNxName *name = ccnxName_CreateFromCString("lci:/boose/roo/pie");
 *
 *      CCNxContentObject *contentObjectV0 =
 *          ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV0_Implementation,
 *                                                     name,
 *                                                     CCNxPayloadType_DATA,
 *                                                     NULL);
 *
 *      CCNxContentObject *contentObjectV1 =
 *          ccnxContentObject_CreateWithImplAndPayload(&CCNxContentObjectFacadeV1_Implementation,
 *                                                     name,
 *                                                     CCNxPayloadType_DATA,
 *                                                     NULL);
 *
 *      assertTrue(ccnxContentObjectInterface_GetInterface(contentObjectV0) == &CCNxContentObjectFacadeV0_Implementation,
 *                 "Expected V0 Implementation");
 *
 *      assertTrue(ccnxContentObjectInterface_GetInterface(contentObjectV1) == &CCNxContentObjectFacadeV1_Implementation,
 *                 "Expected V1 Implementation");
 *
 *      ccnxName_Release(&name);
 *      ccnxContentObject_Release(&contentObjectV0);
 *      ccnxContentObject_Release(&contentObjectV1);
 * }
 * @endcode
 */
CCNxContentObjectInterface *ccnxContentObjectInterface_GetInterface(const CCNxTlvDictionary *contentDictionary);
#endif
