/*
 * Copyright (c) 2015-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @author Christopher A. Wood, Computing Science Laboratory, PARC
 * @copyright (c) 2015-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_JSON.h>

#include <ccnx/common/ccnx_Manifest.h>
#include <ccnx/common/internal/ccnx_ManifestInterface.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.h>

static size_t _ccnxManifestFacadeV1_GetNumberOfHashGroups(const CCNxTlvDictionary *dict);

static CCNxTlvDictionary *
_ccnxManifestFacadeV1_Create(const CCNxName *name)
{
    CCNxTlvDictionary *dictionary = ccnxCodecSchemaV1TlvDictionary_CreateManifest();

    if (dictionary != NULL) {
        if (name != NULL) {
            ccnxTlvDictionary_PutName(dictionary, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_NAME, name);
        }
    } else {
        trapOutOfMemory("Could not allocate an Manifest");
    }

    return dictionary;
}

static const CCNxName *
_ccnxManifestFacadeV1_GetName(const CCNxTlvDictionary *dict)
{
    return ccnxTlvDictionary_GetName(dict, CCNxCodecSchemaV1TlvDictionary_MessageFastArray_NAME);
}

static void
_ccnxManifestFacadeV1_AddHashGroup(CCNxTlvDictionary *dict, const CCNxManifestHashGroup *group)
{
    PARCJSON *json = ccnxManifestHashGroup_ToJson(group);
    size_t numGroups = _ccnxManifestFacadeV1_GetNumberOfHashGroups(dict);

    char *jsonString = parcJSON_ToString(json);
    PARCBuffer *buffer = parcBuffer_AllocateCString(jsonString);
    parcMemory_Deallocate(&jsonString);
    parcJSON_Release(&json);

    ccnxTlvDictionary_PutListBuffer(dict, CCNxCodecSchemaV1TlvDictionary_Lists_HASH_GROUP_LIST, (uint32_t) numGroups, buffer);
    parcBuffer_Release(&buffer);
}

static CCNxManifestHashGroup *
_ccnxManifestFacadeV1_GetHashGroup(const CCNxTlvDictionary *dict, size_t index)
{
    PARCBuffer *buffer = NULL;
    uint32_t key;
    ccnxTlvDictionary_ListGetByPosition(dict, CCNxCodecSchemaV1TlvDictionary_Lists_HASH_GROUP_LIST, index, &buffer, &key);

    char *jsonString = parcBuffer_ToString(buffer);
    PARCJSON *json = parcJSON_ParseString(jsonString);
    parcMemory_Deallocate(&jsonString);

    CCNxManifestHashGroup *group = ccnxManifestHashGroup_CreateFromJson(json);
    parcJSON_Release(&json);

    return group;
}

static size_t
_ccnxManifestFacadeV1_GetNumberOfHashGroups(const CCNxTlvDictionary *dict)
{
    size_t numHashGroups = ccnxTlvDictionary_ListSize(dict, CCNxCodecSchemaV1TlvDictionary_Lists_HASH_GROUP_LIST);
    return numHashGroups;
}

static bool
_ccnxManifestFacadeV1_Equals(const CCNxTlvDictionary *dictA, const CCNxTlvDictionary *dictB)
{
    if (dictA == dictB) {
        return true;
    }
    if (dictA == NULL || dictB == NULL) {
        return false;
    }

    if (ccnxName_Equals(_ccnxManifestFacadeV1_GetName(dictA), _ccnxManifestFacadeV1_GetName(dictB))) {
        if (_ccnxManifestFacadeV1_GetNumberOfHashGroups(dictA) == _ccnxManifestFacadeV1_GetNumberOfHashGroups(dictB)) {
            for (size_t i = 0; i < _ccnxManifestFacadeV1_GetNumberOfHashGroups(dictA); i++) {
                CCNxManifestHashGroup *groupA = _ccnxManifestFacadeV1_GetHashGroup(dictA, i);
                CCNxManifestHashGroup *groupB = _ccnxManifestFacadeV1_GetHashGroup(dictB, i);

                if (!ccnxManifestHashGroup_Equals(groupA, groupB)) {
                    ccnxManifestHashGroup_Release(&groupA);
                    ccnxManifestHashGroup_Release(&groupB);
                    return false;
                }
            }
            return true;
        }
    }
    return false;
};

CCNxManifestInterface CCNxManifestFacadeV1_Interface = {
    .description                     = "CCNxManifestFacadeV1_Implementation",
    .create                          = &_ccnxManifestFacadeV1_Create,
    .getName                         = &_ccnxManifestFacadeV1_GetName,
    .addHashGroup                    = &_ccnxManifestFacadeV1_AddHashGroup,
    .getHashGroup                    = &_ccnxManifestFacadeV1_GetHashGroup,
    .getNumberOfHashGroups           = &_ccnxManifestFacadeV1_GetNumberOfHashGroups,
    .equals                          = &_ccnxManifestFacadeV1_Equals,
};
