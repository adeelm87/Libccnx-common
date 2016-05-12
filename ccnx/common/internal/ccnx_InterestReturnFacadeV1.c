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
#include <LongBow/runtime.h>

#include <ccnx/common/internal/ccnx_InterestReturnFacadeV1.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.h>

// =====================

static void
_assertInvariants(const CCNxTlvDictionary *interestDictionary)
{
    assertNotNull(interestDictionary, "Dictionary is null");
    assertTrue(ccnxTlvDictionary_IsInterestReturn(interestDictionary), "Dictionary is not an interest");
    assertTrue(ccnxTlvDictionary_GetSchemaVersion(interestDictionary) == CCNxTlvDictionary_SchemaVersion_V1,
               "Dictionary is wrong schema InterestReturn, got %d expected %d",
               ccnxTlvDictionary_GetSchemaVersion(interestDictionary), CCNxTlvDictionary_SchemaVersion_V1);
}

static uint32_t
_fetchUint32(const CCNxTlvDictionary *interestDictionary, uint32_t key, uint32_t defaultValue)
{
    if (ccnxTlvDictionary_IsValueInteger(interestDictionary, key)) {
        return (uint32_t) ccnxTlvDictionary_GetInteger(interestDictionary, key);
    }
    return defaultValue;
}

// =====================
// Creation

static CCNxTlvDictionary *
_ccnxInterestReturnFacadeV1_Create(
    const CCNxInterest *interest,
    CCNxInterestReturn_ReturnCode code)
{
    assertNotNull(interest, "Parameter name must be non-null");

    assertTrue(ccnxInterestInterface_GetInterface(interest) == &CCNxInterestFacadeV1_Implementation,
               "Non-V1 CCNxInterest passed to V1 ccnxInterestReturn_Create()");

    CCNxInterestReturnFacadeV1_Implementation.interestImpl = CCNxInterestFacadeV1_Implementation;


    CCNxTlvDictionary *dictionary = ccnxTlvDictionary_ShallowCopy(interest);

    //Add InterestReturn specific stuff
    ccnxTlvDictionary_SetMessageType_InterestReturn(dictionary,
                                                    CCNxTlvDictionary_SchemaVersion_V1);

    ccnxTlvDictionary_PutInteger(dictionary,
                                 CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_InterestReturnCode,
                                 code);

    return dictionary;
}

static void
_ccnxInterestReturnFacadeV1_AssertValid(const CCNxTlvDictionary *interestDictionary)
{
    _assertInvariants(interestDictionary);
    assertTrue(ccnxTlvDictionary_IsValueName(interestDictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_NAME), "Name field is not a name");
}

static CCNxInterestReturn_ReturnCode
_ccnxInterestReturnFacadeV1_GetReturnCode(const CCNxTlvDictionary *interestDictionary)
{
    _assertInvariants(interestDictionary);
    CCNxInterestReturn_ReturnCode code = _fetchUint32(interestDictionary,
                                                      CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_InterestReturnCode,
                                                      0);
    assertTrue(((code > 0) && (code < CCNxInterestReturn_ReturnCode_END)), "InterestReturn ReturnCode is out of ranage");

    return code;
}

CCNxInterestReturnInterface CCNxInterestReturnFacadeV1_Implementation = {
    .Create        = &_ccnxInterestReturnFacadeV1_Create,
    .AssertValid   = &_ccnxInterestReturnFacadeV1_AssertValid,
    .GetReturnCode = &_ccnxInterestReturnFacadeV1_GetReturnCode,
};
