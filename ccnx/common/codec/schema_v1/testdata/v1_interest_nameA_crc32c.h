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
 * @file interest_nameA_crc32c.h
 * @brief Interest with CRC validation
 *
 *
 * Ground truth set derived from CRC RevEng http://reveng.sourceforge.net
 * e.g. reveng -c  -m CRC-32C 313233343536373839 gives the canonical check value 0xe306928e (the output will be backwards)
 *
 * You can also calcaulate CRC32C online at http://www.zorc.breitbandkatze.de/crc.html using
 * CRC polynomial 0x1EDC6F41, init 0xFFFFFFFF, final 0xFFFFFFFF, reverse data bytes (check),
 * and reverse CRC result before final XOR (check).
 *
 * you can get the packet dump from the "write_packets" command.  here's the detailed steps.
 * The -c size of 8 in steps 4 and 7 are chosen to make it easy to delete the right number of lines.
 * there's nothing magic about the "8".
 *
 *  1) execute ./write_packets
 *  2) xxd -r -c 8 v1_interest_nameA_crc32c.txt > y
 *  3) Delete the first 24 bytes and last 8 bytes and display as a URI-escaped hex string
 *       head -c 57 y | tail -c +25 | xxd -p -c 256 | sed 's/[0-9a-f]\{2\}/%&/g'
 *      The string should be "00010015...00020000"
 *  4) Copy the hex string to the website and use the settings specified above (don't use 0x in front
 *     of any hex strings).  IMPORTANT: you need to %-escape each hex byte!! Click "compute!"
 *  5) The answer should be 6AD7B1F2
 *  6) Put the byte array from (5) in the Validation Payload.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef testdata_interest_nameA_crc32c_h
#define testdata_interest_nameA_crc32c_h

/**
 * A well formed interest with only a name
 */

#include <ccnx/common/codec/testdata/testdata_common.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_InterestSchema.h>

#define NAME_A_CRC32_PACKET(_type, _code) \
{ \
    0x01, _type, 0x00,   65,     /* ver = 1, type = interest, length = 65 */          \
    0x20, _code, 0x00,   24,     /* HopLimit = 32, reserved = 0, header length = 24*/ \
    /* ------------------------ */                                                    \
    0x00, 0x03, 0x00,   12,     /* Interest Fragment */                               \
    0x01, 0x02, 0x03, 0x04,                                                           \
    0x05, 0x06, 0x07, 0x08,     /* fragment 0x0102030405060708 */                     \
    0x05, 0xDC, 0x00, 0x00,     /* MTU 1500, fragcnt 0, fragnum 0 */                  \
    /* ------------------------ */                                                    \
    0x00, 0x01, 0x00, 0x15,     /* type = interest, length = 21 */                    \
    /* ------------------------ */                                                    \
    0x00, 0x00, 0x00, 0x11,     /* type = name, length = 17 */                        \
    0x00, 0x02, 0x00, 0x05,     /* type = binary, length = 5 */                       \
    'h',  'e',  'l',  'l',      /* "hello" */                                         \
    'o',                                                                              \
    0xF0, 0x00, 0x00, 0x04,     /* type = app, length = 4 */                          \
    'o',  'u',  'c',  'h',      /* "ouch" */                                          \
    /* ------------------------ */                                                    \
    0x00, 0x03, 0x00,    4,     /* validation alg, length = 4 */                      \
    0x00, 0x02, 0x00, 0x00,     /* CRC32C */                                          \
    /* ------------------------ */                                                    \
    0x00, 0x04, 0x00,    4,     /* validation payload */                              \
    0x6A, 0xD7, 0xB1, 0xF2      /* 6AD7B1F2 */                                        \
}

__attribute__((unused))
static uint8_t v1_interest_nameA_crc32c[] = NAME_A_CRC32_PACKET(0x00, 0x00);

__attribute__((unused))
static TruthTableEntry
TRUTHTABLENAME(v1_interest_nameA_crc32c)[] =
{
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_E2EFRAG,           .bodyManifest = false, .extent = { 12, 12 } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_INTEREST,          .bodyManifest = true,  .extent = { 24, 25 } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_NAME,              .bodyManifest = true,  .extent = { 32, 17 } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_ValidationAlg,     .bodyManifest = true,  .extent = { 53, 4  } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_INT_ValidationPayload, .bodyManifest = true,  .extent = { 61, 4  } },
    { .wellKnownType = false, .indexOrKey = T_INVALID,                         .extent       = { 0,   0 } },
};

__attribute__((unused))
static uint8_t v1_interest_nameA_crc32c_returned[] = NAME_A_CRC32_PACKET(0x02, 0x03); //InterestReturn & NoResource

#define v1_interest_nameA_crc32c_truthTable TABLEENTRY(v1_interest_nameA_crc32c, TLV_ERR_NO_ERROR)

#define v1_interest_nameA_crc32c_URI "lci:/2=hello/0xf000=ouch"

#endif
