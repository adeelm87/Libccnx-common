/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * Copyright 2015 Palo Alto Research Center, Inc. (PARC), a Xerox company.  All Rights Reserved.
 * The content of this file, whole or in part, is subject to licensing terms.
 * If distributing this software, include this License Header Notice in each
 * file and provide the accompanying LICENSE file.
 */
/**
 * @brief A structure of functions representing a Manifest implementation.
 *
 * The underlying implementation should support the CCNxManifest API.
 *
 * @author Christopher A. Wood, Computing Science Laboratory, PARC
 * @copyright 2015 Palo Alto Research Center, Inc. (PARC), A Xerox Company. All Rights Reserved.
 */

#ifndef CCNx_Common_ccnx_internal_ManifestImpl_h
#define CCNx_Common_ccnx_internal_ManifestImpl_h

#include <ccnx/common/internal/ccnx_TlvDictionary.h>
#include <ccnx/common/ccnx_ManifestHashGroup.h>
#include <ccnx/common/ccnx_Name.h>
#include <ccnx/common/ccnx_KeyLocator.h>

#include "ccnx_TlvDictionary.h"

typedef struct ccnx_manifest_interface {
    /** A human-readable label for this implementation */
    char                 *description;

    /** @see ccnxManifest_Create */
    CCNxTlvDictionary *(*create) (const CCNxName *name);

    /** @see ccnxManifest_AddHashGroup */
    void (*addHashGroup)(CCNxTlvDictionary *dict, const CCNxManifestHashGroup *group);

    /** @see ccnxManifest_GetHashGroup */
    CCNxManifestHashGroup *(*getHashGroup)(const CCNxTlvDictionary *dict, size_t index);

    /** @see ccnxManifest_GetNumberOfHashGroups */
    size_t (*getNumberOfHashGroups)(const CCNxTlvDictionary *dict);

    /** @see ccnxManifest_Equals */
    bool (*equals)(const CCNxTlvDictionary *objectA, const CCNxTlvDictionary *objectB);

    /** @see ccnxManifest_GetName */
    const CCNxName *(*getName)(const CCNxTlvDictionary *dict);
} CCNxManifestInterface;

CCNxManifestInterface *ccnxManifestInterface_GetInterface(const CCNxTlvDictionary *dictionary);

/**
 * The SchemaV1 Manifest implementaton
 */
extern CCNxManifestInterface CCNxManifestFacadeV1_Interface;

#endif
