/*
 * Copyright (c) 2016, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @file v1_content_nameless_nosig.h
 * @brief A v1 content object without a name
 *
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright 2016, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef v1_content_nameless_nosig_h
#define v1_content_nameless_nosig_h

#include <ccnx/common/codec/testdata/testdata_common.h>
#include <ccnx/common/codec/schema_v1/testdata/v1_ContentObjectSchema.h>

/**
 * A well formed nameless V1 Content Object
 */
__attribute__((unused))
static uint8_t v1_content_nameless_nosig[] = {
        0x01, 0x01, 0x00, 0x31,     // ver = 1, type = content object, length = 0x31
        0x00, 0x00, 0x00, 0x08,     // HopLimit = 0, reserved = 0, header length = 8
        // ------------------------
        0x00, 0x02, 0x00,   37,     // type = content object, length = 37
        // ------------------------
        0x00, 0x05, 0x00,    1,     // PayloadType
        1,                          // type 1 = key
        0x00, 0x06, 0x00, 0x08,     // expiry time in msec
        0x00, 0x00, 0x01, 0x43,     // 1,388,534,400,000 msec
        0x4B, 0x19, 0x84, 0x00,
        0x00, 0x19, 0x00,    4,     // end chunk number
        0x06, 0x05, 0x04, 0x03,
        // ------------------------
        0x00, 0x01, 0x00,    8,     // payload, length = 8
        0x73, 0x75, 0x72, 0x70,
        0x72, 0x69, 0x73, 0x65,
};

__attribute__((unused))
static TruthTableEntry TRUTHTABLENAME(v1_content_nameless_nosig)[] = {
        { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_OBJ_CONTENTOBJECT,  .bodyManifest=true,  .extent = {12, 37} },
        { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_OBJ_PAYLOADTYPE,    .bodyManifest=true,  .extent = {17, 1} },
        { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_OBJ_EXPIRY_TIME,    .bodyManifest=true,  .extent = {22, 8} },
        { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_OBJ_ENDSEGMENT,     .bodyManifest=true,  .extent = {30, 4} },
        { .wellKnownType = true,  .indexOrKey = V1_MANIFEST_OBJ_PAYLOAD,        .bodyManifest=true,  .extent = {41, 8} },
        { .wellKnownType = false, .indexOrKey = T_INVALID,                      .extent = { 0,  0} },
};

#define v1_content_nameless_nosig_truthTable TABLEENTRY(v1_content_nameless_nosig, TLV_ERR_NO_ERROR)

#endif // v1_content_nameless_nosig_h
