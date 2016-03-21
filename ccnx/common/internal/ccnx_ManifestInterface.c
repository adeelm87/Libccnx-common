/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * Copyright 2015 Palo Alto Research Center, Inc. (PARC), a Xerox company.  All Rights Reserved.
 * The content of this file, whole or in part, is subject to licensing terms.
 * If distributing this software, include this License Header Notice in each
 * file and provide the accompanying LICENSE file.
 */
/**
 * @author Christopher A. Wood, Computing Science Laboratory, PARC
 * @copyright 2015 Palo Alto Research Center (PARC), A Xerox Company. All Rights Reserved.
 */
#include <config.h>

#include <LongBow/runtime.h>

#include <ccnx/common/ccnx_PayloadType.h>
#include <ccnx/common/internal/ccnx_ManifestInterface.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.h>

CCNxManifestInterface *
ccnxManifestInterface_GetInterface(const CCNxTlvDictionary *dictionary)
{
    assertTrue(ccnxTlvDictionary_IsManifest(dictionary), "Expected a Manifest");

    CCNxManifestInterface *impl = (CCNxManifestInterface *) ccnxTlvDictionary_GetMessageInterface(dictionary);

    if (impl == NULL) {
        // If we're here, we need to update the implementation pointer.
        // Figure out what the typeImplementation should be, based on the attributes we know.
        int schemaVersion = ccnxTlvDictionary_GetSchemaVersion(dictionary);

        switch (schemaVersion) {
            case CCNxTlvDictionary_SchemaVersion_V1:
                impl = &CCNxManifestFacadeV1_Interface;
                break;
            default:
                trapUnexpectedState("Unknown SchemaVersion encountered in ccnxInterestImplementation_GetImplementation()");
                break;
        }

        if (impl == NULL) {
            // The cast to (CCNxTlvDictionary *) is to break the const.
            ccnxTlvDictionary_SetMessageInterface((CCNxTlvDictionary *) dictionary, (CCNxMessageInterface *) impl);
        }
    }

    return impl;
}
