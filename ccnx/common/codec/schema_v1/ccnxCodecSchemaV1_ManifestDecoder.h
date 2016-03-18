/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * Copyright 2013-2015 Palo Alto Research Center (PARC), a Xerox company.  All Rights Reserved.
 * The content of this file, whole or in part, is subject to licensing terms.
 * If distributing this software, include this License Header Notice in each
 * file and provide the accompanying LICENSE file.
 */
/**
 * @file ccnxCodecSchemaV1_ManifestDecoder.h
 * @brief Decode the body of a CCNx Manifest.
 *
 * @author Christopher A. Wood, Computing Science Laboratory, PARC
 * @copyright 2015 Palo Alto Research Center (PARC), A Xerox Company. All Rights Reserved.
 */

#ifndef TransportRTA_ccnxCodecSchemaV1_ManifestDecoder_h
#define TransportRTA_ccnxCodecSchemaV1_ManifestDecoder_h

#include <stdbool.h>

#include <ccnx/common/internal/ccnx_TlvDictionary.h>
#include <ccnx/common/codec/ccnxCodec_TlvDecoder.h>

/**
 * Decode a V1 Manifest.
 *
 * The decoder should point to byte 0 of a Manifest (message) TLV.
 * The results are put in the provided dictionary.
 * It is an error if the message does not extend to the end of
 * the decoder.
 *
 * @param [in] decoder The decoder to parse
 * @param [in] manifestDictionary The results go directly in to the provided dictionary.
 *
 * @return true Fully parsed interest, no errors
 * @return false Error decoding, decoder is left pointing to the first byte of the error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxCodecSchemaV1ManifestDecoder_Decode(CCNxCodecTlvDecoder *decoder, CCNxTlvDictionary *manifestDictionary);

#endif // TransportRTA_ccnxCodecSchemaV1_ManifestDecoder_h
