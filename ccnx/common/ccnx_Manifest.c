/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#include <config.h>
#include <stdio.h>
#include <string.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Object.h>

#include <ccnx/common/ccnx_Manifest.h>
#include <ccnx/common/ccnx_ContentObject.h>

static const CCNxManifestInterface *_defaultImplementation = &CCNxManifestFacadeV1_Interface;

static CCNxManifest *
_ccnxManifest_InternalCreate(const CCNxManifestInterface *impl, CCNxName *name)
{
    CCNxManifest *result = NULL;

    if (impl->create != NULL) {
        result = impl->create(name);

        // And set the dictionary's interface pointer to the one we just used to create this.
        ccnxTlvDictionary_SetMessageInterface(result, impl);
    } else {
        trapNotImplemented("Manifest implementations must implement create()");
    }

    return result;
}

parcObject_ImplementAcquire(ccnxManifest, CCNxManifest);
parcObject_ImplementRelease(ccnxManifest, CCNxManifest);

CCNxManifest *
ccnxManifest_Create(const CCNxName *name)
{
    return _ccnxManifest_InternalCreate(_defaultImplementation, name);
}

void
ccnxManifest_AddHashGroup(CCNxManifest *manifest, const CCNxManifestHashGroup *group)
{
    CCNxManifestInterface *interface = ccnxManifestInterface_GetInterface(manifest);
    interface->addHashGroup(manifest, group);
}

CCNxManifestHashGroup *
ccnxManifest_GetHashGroupByIndex(const CCNxManifest *manifest, size_t index)
{
    CCNxManifestInterface *interface = ccnxManifestInterface_GetInterface(manifest);
    return interface->getHashGroup(manifest, index);
}

size_t
ccnxManifest_GetNumberOfHashGroups(const CCNxManifest *manifest)
{
    CCNxManifestInterface *impl = ccnxManifestInterface_GetInterface(manifest);
    size_t result = 0;
    if (impl->getNumberOfHashGroups != NULL) {
        result = (impl->getNumberOfHashGroups)(manifest);
    }
    return result;
}

const CCNxName *
ccnxManifest_GetName(const CCNxManifest *manifest)
{
    CCNxManifestInterface *impl = ccnxManifestInterface_GetInterface(manifest);

    CCNxName *result = NULL;

    if (impl->getName != NULL) {
        result = impl->getName(manifest);
    } else {
        trapNotImplemented("ccnxManifest_GetName");
    }

    return result;
}

bool
ccnxManifest_Equals(const CCNxManifest *objectA, const CCNxManifest *objectB)
{
    if (objectA == objectB) {
        return true;
    }
    if (objectA == NULL || objectB == NULL) {
        return false;
    }

    CCNxManifestInterface *impl = ccnxManifestInterface_GetInterface(objectA);
    return (impl->equals)(objectA, objectB);

    return false;
}

char *
ccnxManifest_ToString(const CCNxManifest *manifest)
{
    return NULL;
}

PARCJSON *
ccnxManifest_ToJSON(const CCNxManifest *manifest)
{
    PARCJSON *root = parcJSON_Create();

    char *nameString = ccnxName_ToString(ccnxManifest_GetName(manifest));
    parcJSON_AddString(root, "locator", nameString);
    parcMemory_Deallocate(&nameString);

    PARCJSONArray *array = parcJSONArray_Create();
    for (size_t i = 0; i < ccnxManifest_GetNumberOfHashGroups(manifest); i++) {
        CCNxManifestHashGroup *group = ccnxManifest_GetHashGroupByIndex(manifest, i);
        PARCJSON *groupJson = ccnxManifestHashGroup_ToJson(group);
        PARCJSONValue *jsonValue = parcJSONValue_CreateFromJSON(groupJson);

        parcJSONArray_AddValue(array, jsonValue);

        parcJSONValue_Release(&jsonValue);
        parcJSON_Release(&groupJson);
        ccnxManifestHashGroup_Release(&group);
    }
    parcJSON_AddArray(root, "HashGroups", array);
    parcJSONArray_Release(&array);

    return root;
}

void
ccnxManifest_AssertValid(const CCNxManifest *manifest)
{
    assertNotNull(manifest, "Non-NULL manifest");
}
