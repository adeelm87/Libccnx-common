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
 * @file v1_cpi_add_route.h
 * @brief A hand-encoded CPI packet to add a route
 *
 * The v1 old-style control packet is a fixed header plus a tlv container 0xBEEF with a "value" of the CPI JSON string.
 * The packet type is 0xA4.
 *
 * This control packet has a CRC32C MIC on it.  Otherwise, same as v1_cpi_add_route.h
 *
 * Ground truth set derived from CRC RevEng http://reveng.sourceforge.net
 * e.g. reveng -c  -m CRC-32C 313233343536373839 gives the canonical check value 0xe306928e
 *
 * You can also calcaulate CRC32C online at http://www.zorc.breitbandkatze.de/crc.html using
 * CRC polynomial 0x1EDC6F41, init 0xFFFFFFFF, final 0xFFFFFFFF, reverse data bytes (check),
 * and reverse CRC result before final XOR (check).
 *
 * you can get the packet dump from the "write_packets" command.  here's the detailed steps.
 * The -c size of 4 in steps 4 and 7 are chosen to make it easy to delete the right number of lines.
 * there's nothing magic about the "4".
 *
 *  1) execute ./write_packets
 *  2) xxd -r -c 8 v1_cpi_add_route_crc32c.txt > y
 *  3) Delete the first 8 bytes and last 16 bytes and display has a hex string
 *       tail -c +9 y | head -c 158 | xxd -p -c 256
 *      The string should be "beef...077d7d7d"
 *  4) The string for this packet is too long for the website.  Use another tool such as reveng.
 *  5) The answer should be 78fd926a (the reveng answer will be byte reversed)
 *  6) Put the byte array from (5) in the Validation Payload.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef TransportRTA_v1_v1_cpi_AddRoute_crc32c_h
#define TransportRTA_v1_v1_cpi_AddRoute_crc32c_h

#include <ccnx/common/codec/testdata/testdata_common.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_CPISchema.h>

__attribute__((unused))
static uint8_t v1_cpi_add_route_crc32c[] = "\x01\xA4\x00\xB7"
                                    "\x00\x00\x00\x08"
                                    "\xBE\xEF\x00\x9A"
                                    "{\"CPI_REQUEST\":{\"SEQUENCE\":22,\"REGISTER\":{\"PREFIX\":\"lci:/howdie/stranger\",\"INTERFACE\":55,\"FLAGS\":0,\"PROTOCOL\":\"STATIC\",\"ROUTETYPE\":\"LONGEST\",\"COST\":200}}}"
                                    "\x00\x03\x00\x04"
                                    "\x00\x02\x00\x00"
                                    "\x00\x04\x00\x04"
                                    "\x78\xfd\x92\x6a";

__attribute__((unused))
static TruthTableEntry
TRUTHTABLENAME(v1_cpi_add_route_crc32c)[] =
{
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_CPI_PAYLOAD, .bodyManifest = true, .extent = { 12, 155 } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_CPI_ValidationAlg, .bodyManifest = true, .extent = { 171, 4 } },
    { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_CPI_SIGBITS, .bodyManifest = true, .extent = { 178, 4 } },
    { .wellKnownType = false, .indexOrKey = T_INVALID,                          .extent = { 0,  0 } },
};

#define v1_cpi_add_route_crc32c_truthTable TABLEENTRY(v1_cpi_add_route_crc32c, TLV_ERR_NO_ERROR)

#define v1_cpi_add_route_crc32c_PrefixUri "lci:/howdie/stranger"
#define v1_cpi_add_route_crc32c_Sequence  22
#define v1_cpi_add_route_crc32c_Interface 55
#endif // TransportRTA_cpi_AddRoute_h
