/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * Copyright 2013-2015 Palo Alto Research Center (PARC), a Xerox company.  All Rights Reserved.
 * The content of this file, whole or in part, is subject to licensing terms.
 * If distributing this software, include this License Header Notice in each
 * file and provide the accompanying LICENSE file.
 */
/**
 * @file ccnxCodecSchemaV1_ManifestEncoder.h
 * @brief Encode a V1 Manifest.
 *
 * @author Christopher Wood, Computing Science Laboratory, PARC
 * @copyright 2015 Palo Alto Research Center (PARC), A Xerox Company. All Rights Reserved.
 */

#ifndef ccnx_common_ccnxCodecSchemaV1_ManifestEncoder_h
#define ccnx_common_ccnxCodecSchemaV1_ManifestEncoder_h

#include <ccnx/common/internal/ccnx_TlvDictionary.h>
#include <ccnx/common/codec/ccnxCodec_TlvEncoder.h>

ssize_t ccnxCodecSchemaV1ManifestEncoder_Encode(CCNxCodecTlvEncoder *encoder, CCNxTlvDictionary *packetDictionary);

#endif // ccnx_common_ccnxCodecSchemaV1_ManifestEncoder_h
