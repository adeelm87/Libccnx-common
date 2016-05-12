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
 * @author Glenn Scott, Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>
#include <stdio.h>

#include <LongBow/runtime.h>

#include <ccnx/common/ccnx_InterestReturn.h>
#include <ccnx/common/ccnx_Interest.h>
#include <ccnx/common/internal/ccnx_InterestReturnFacadeV1.h>

#include <parc/algol/parc_Memory.h>

#include <parc/algol/parc_Object.h>

static const CCNxInterestReturnInterface *_defaultImpl = &CCNxInterestReturnFacadeV1_Implementation;

CCNxInterestReturn *
ccnxInterestReturn_CreateWithImpl(const CCNxInterestReturnInterface *impl,
                                  const CCNxInterest *interest,
                                  CCNxInterestReturn_ReturnCode returnCode)
{
    //The impl must do an Acquire

    CCNxInterestReturn *result = impl->Create(interest, returnCode);

    // And set the dictionary's interface pointer to the one we just used to create this.
    ccnxTlvDictionary_SetMessageInterface(result, impl);

    return result;
}


// Canonical Functions
CCNxInterestReturn *
ccnxInterestReturn_Create(const CCNxInterest *interest, CCNxInterestReturn_ReturnCode returnCode)
{
    CCNxInterestReturn *result =
        ccnxInterestReturn_CreateWithImpl(_defaultImpl, interest, returnCode);
    return result;
}

void
ccnxInterestReturn_AssertValid(const CCNxInterestReturn *interestReturn)
{
    assertNotNull(interestReturn, "Must be a non-null pointer to a CCNxInterestReturn.");
    // Check for required fields in the underlying dictionary. Case 1036
    CCNxInterestReturnInterface *impl = ccnxInterestReturnInterface_GetInterface(interestReturn);
    assertNotNull(impl, "Interest must have an valid implementation pointer.");

    if (impl->AssertValid) {
        impl->AssertValid(interestReturn);
    } else {
        trapNotImplemented("ccnxInterest_GetName");
    }
}

CCNxInterestReturn *
ccnxInterestReturn_Acquire(const CCNxInterestReturn *instance)
{
    return ccnxTlvDictionary_Acquire(instance);
}

void
ccnxInterestReturn_Release(CCNxInterestReturn **instance)
{
    ccnxTlvDictionary_Release(instance);
}

bool
ccnxInterestReturn_Equals(const CCNxInterestReturn *a, const CCNxInterestReturn *b)
{
    if (a == NULL || b == NULL) {
        return false;
    }

    if (a == b) {
        return true;
    }

    CCNxInterestReturnInterface *impl = ccnxInterestReturnInterface_GetInterface(a);
    CCNxInterestReturnInterface *implB = ccnxInterestReturnInterface_GetInterface(b);

    if (impl != implB) {
        return false;
    }

    if (impl->GetReturnCode(a) != impl->GetReturnCode(b)) {
        return false;
    }

    return ccnxInterest_Equals(a, b);
}

char *
ccnxInterestReturn_ToString(const CCNxInterestReturn *interestReturn)
{
    ccnxInterestReturn_AssertValid(interestReturn);

    char *name = ccnxName_ToString(ccnxInterest_GetName(interestReturn));

    CCNxInterestReturnInterface *impl = ccnxInterestReturnInterface_GetInterface(interestReturn);
    CCNxInterestReturn_ReturnCode code = impl->GetReturnCode(interestReturn);

    char *string;
    int len = asprintf(&string, "CCNxInterestReturn{.code=%dms .name=\"%s\"}", code, name);
    if (len < 0) {
        //We have serious problems.
        return name;
    }

    parcMemory_Deallocate((void **) &name);

    char *result = parcMemory_StringDuplicate(string, strlen(string));
    free(string);

    return result;
}

// Accessors Functions
CCNxInterestReturn_ReturnCode
ccnxInterestReturn_GetReturnCode(const CCNxInterestReturn *interestReturn)
{
    ccnxInterestReturn_AssertValid(interestReturn);
    CCNxInterestReturnInterface *impl = ccnxInterestReturnInterface_GetInterface(interestReturn);
    CCNxInterestReturn_ReturnCode code = impl->GetReturnCode(interestReturn);
    return code;
}

