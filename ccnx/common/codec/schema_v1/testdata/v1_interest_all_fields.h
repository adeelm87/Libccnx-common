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
 * @file v1_interest_all_fields.h
 * @brief A hand-encoded v1 interest in wireformat with all Interest fields.
 *
 * <#Detailed Description#>
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef CCNx_Common_v1_interest_all_fields_h
#define CCNx_Common_v1_interest_all_fields_h

#include <ccnx/common/codec/testdata/testdata_common.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_InterestSchema.h>

/**
 * A well formed interest with all allowed Interest fields
 */
__attribute__((unused))
static uint8_t v1_interest_all_fields[] = {
    0x01, 0x00, 0x00, 156,      // ver = 1, type = interest, length = 156
    0x20, 0x00, 0x11, 14,       // HopLimit = 32, reserved = 0, flags = 0x11, header length = 14
    // ------------------------
    0x00, 0x01, 0x00, 2,        // Interest Lifetime (2 bytes)
    0xEA, 0xEB,
    // ------------------------
    0x00, 0x01, 0x00, 138,      // type = interest, length = 138
    // ------------------------
    0x00, 0x00, 0x00, 45,       // type = name, length = 45
    0x00, 0x02, 0x00, 4,        // type = binary, length = 4
    'c',  'o',  'o',  'l',      // "cool"
    // ----- segment 2 --------
    0x00, 0x08, 0x00, 33,       // type = payload id, length = 33
    0x01, // payloadID type = sha256
    0x89, 0x87, 0x69, 0xfc,     // hash bytes based on payload
    0x8c, 0xff, 0x16, 0xff,
    0x3d, 0xfc, 0xe7, 0xfa,
    0x02, 0xd2, 0x6d, 0x26,
    0xf0, 0x91, 0x86, 0x27,
    0xcf, 0x18, 0xc1, 0x9b,
    0x0b, 0x5f, 0xe3, 0x93,
    0xce, 0x1a, 0xa3, 0x56,
    // ------------------------
    0x00, 0x02, 0x00, 36,       // type = keyid restriction, length = 36
    0x00, 0x01, 0x00, 0x20,     // SHA256 hash, length 32
    0xa0, 0xa1, 0xa2, 0xa3,
    0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xab,
    0xac, 0xad, 0xae, 0xaf,
    0xa0, 0xa1, 0xa2, 0xa3,
    0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xab,
    0xac, 0xad, 0xae, 0xaf,
    // ------------------------
    0x00, 0x03, 0x00, 36,       // type = hash restriction, length = 36
    0x00, 0x01, 0x00, 0x20,     // SHA256 hash, length 32
    0xb0, 0xb1, 0xb2, 0xb3,
    0xb4, 0xb5, 0xb6, 0xb7,
    0xb8, 0xb9, 0xba, 0xbb,
    0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb,
    0xcc, 0xcd, 0xce, 0xcf,
    // ------------------------
    0x00, 0x01, 0x00, 5,        // type = payload, length = 5
    0xD0, 0xD1, 0xD2, 0xD3,
    0xD4,
};

__attribute__((unused))
static TruthTableEntry
TRUTHTABLENAME(v1_interest_all_fields)[] =
{
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_LIFETIME, .bodyManifest = false, .extent = { 12,  2   } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_INTEREST, .bodyManifest = true,  .extent = { 18,  138 } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_NAME,     .bodyManifest = true,  .extent = { 22,  45  } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_KEYID,    .bodyManifest = true,  .extent = { 71,  36  } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_OBJHASH,  .bodyManifest = true,  .extent = { 111, 36  } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_PAYLOAD,  .bodyManifest = true,  .extent = { 151, 5   } },
    { .wellKnownType = false, .indexOrKey = T_INVALID,                .extent       = { 0,   0 } },
};

#define v1_interest_all_fields_truthTable TABLEENTRY(v1_interest_all_fields, TLV_ERR_NO_ERROR)

#define v1_interest_all_fields_URI "lci:/2=cool/8=\x01\x89\x87\x69\xfc\x8c\xff\x16\xff\x3d\xfc\xe7\xfa\x02\xd2\x6d\x26\xf0\x91\x86\x27\xcf\x18\xc1\x9b\x0b\x5f\xe3\x93\xce\x1a\xa3\x56"
#define v1_interest_all_fields_Lifetime 0xEAEB
#endif
