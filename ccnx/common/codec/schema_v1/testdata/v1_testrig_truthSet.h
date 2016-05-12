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
 * Contains tables of all the packets.  May be used for automated testing.  Also used by write_packets utility.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <ccnx/common/codec/testdata/testdata_common.h>

#include "v1_interest_nameA.h"
#include "v1_interest_nameA_badcrc32c.h"
#include "v1_interest_nameA_crc32c.h"
#include "v1_interest_bad_validation_alg.h"
#include "v1_interest_validation_alg_overrun.h"

#include "v1_content_nameA_crc32c.h"
#include "v1_content_nameA_keyid1_rsasha256.h"
#include "v1_content_zero_payload.h"
#include "v1_content_no_payload.h"

#include "v1_cpi_add_route.h"
#include "v1_cpi_add_route_crc32c.h"

// terminated with NULL packet entry
__attribute__((unused))
static TruthTable v1_interests_truthSet [] = {
    // tests in alphabetical order
    v1_interest_nameA_truthTable,
    v1_interest_nameA_badcrc32c_truthTable,
    v1_interest_nameA_crc32c_truthTable,
    v1_interest_bad_validation_alg_truthTable,
    v1_interest_validation_alg_overrun_truthTable,
    // the end of table marker
    { .packet = NULL, .expectedError = 0, .entry = NULL }
};

// terminated with NULL packet entry
__attribute__((unused))
static TruthTable v1_contentObject_truthSet [] = {
    v1_content_nameA_crc32c_truthTable,
    v1_content_nameA_keyid1_rsasha256_truthTable,
    v1_content_zero_payload_truthTable,
    v1_content_no_payload_truthTable,

    // the end of table marker
    { .packet = NULL, .expectedError = 0, .entry = NULL }
};

__attribute__((unused))
static TruthTable v1_cpi_truthSet [] = {
    v1_cpi_add_route_truthTable,
    v1_cpi_add_route_crc32c_truthTable,
    // the end of table marker
    { .packet = NULL, .expectedError = 0, .entry = NULL }
};
