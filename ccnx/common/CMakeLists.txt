# Define a few configuration variables that we want accessible in the software

set(VALIDATION_HDRS 
	validation/ccnxValidation_CRC32C.h 
	validation/ccnxValidation_EcSecp256K1.h 
	validation/ccnxValidation_HmacSha256.h 
	validation/ccnxValidation_RsaSha256.h
	)

set(INTERNAL_HDRS 
	internal/ccnx_ChunkingFacadeV1.h 
	internal/ccnx_ContentObjectFacadeV1.h 
	internal/ccnx_ContentObjectInterface.h 
	internal/ccnx_InterestFacadeV1.h 
	internal/ccnx_InterestDefault.h 
	internal/ccnx_InterestInterface.h 
	internal/ccnx_InterestReturnFacadeV1.h 
	internal/ccnx_InterestReturnInterface.h 
	internal/ccnx_InterestPayloadIdMethod.h 
	internal/ccnx_MessageInterface.h 
	internal/ccnx_TlvDictionary.h 
	internal/ccnx_ValidationFacadeV1.h 
	internal/ccnx_WireFormatFacadeV1.h 
	internal/ccnx_WireFormatMessageInterface.h
	)

set(CODEC_HDRS 
	codec/ccnxCodec_EncodingBuffer.h 
	codec/ccnxCodec_Error.h 
	codec/ccnxCodec_ErrorCodes.h 
	codec/ccnxCodec_NetworkBuffer.h 
	codec/ccnxCodec_TlvEncoder.h 
	codec/ccnxCodec_TlvDecoder.h 
	codec/ccnxCodec_TlvUtilities.h 
	codec/ccnxCodec_TlvPacket.h
	)

set(TESTDATA_HDRS 
	codec/testdata/tlv_Schema.h 
	codec/testdata/testdata_common.h 
	)

set(SCHEMAV1_TESTDATA_HDRS 
	codec/schema_v1/testdata/v1_content_nameA_crc32c.h 
	codec/schema_v1/testdata/v1_content_nameA_keyid1_rsasha256.h 
	codec/schema_v1/testdata/v1_content_no_payload.h 
	codec/schema_v1/testdata/v1_ContentObjectSchema.h 
	codec/schema_v1/testdata/v1_content_zero_payload.h 
	codec/schema_v1/testdata/v1_cpi_add_route_crc32c.h 
	codec/schema_v1/testdata/v1_cpi_add_route.h 
	codec/schema_v1/testdata/v1_CPISchema.h 
	codec/schema_v1/testdata/v1_interest_all_fields.h 
	codec/schema_v1/testdata/v1_interest_bad_message_length.h 
	codec/schema_v1/testdata/v1_interest_bad_validation_alg.h 
	codec/schema_v1/testdata/v1_interest_nameA_badcrc32c.h 
	codec/schema_v1/testdata/v1_interest_nameA_crc32c.h 
	codec/schema_v1/testdata/v1_interest_nameA.h 
	codec/schema_v1/testdata/v1_InterestSchema.h 
	codec/schema_v1/testdata/v1_interest_validation_alg_overrun.h 
	codec/schema_v1/testdata/v1_testrig_truthSet.h 
	codec/schema_v1/testdata/v1_testrig_truthTable.h 
	)

set(SCHEMAV1_HDRS 
	codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.h 
	codec/schema_v1/ccnxCodecSchemaV1_CryptoSuite.h 
	codec/schema_v1/ccnxCodecSchemaV1_FixedHeaderDecoder.h 
	codec/schema_v1/ccnxCodecSchemaV1_FixedHeaderEncoder.h 
	codec/schema_v1/ccnxCodecSchemaV1_FixedHeader.h 
	codec/schema_v1/ccnxCodecSchemaV1_LinkCodec.h 
	codec/schema_v1/ccnxCodecSchemaV1_MessageDecoder.h 
	codec/schema_v1/ccnxCodecSchemaV1_MessageEncoder.h 
	codec/schema_v1/ccnxCodecSchemaV1_NameCodec.h 
	codec/schema_v1/ccnxCodecSchemaV1_NameSegmentCodec.h 
	codec/schema_v1/ccnxCodecSchemaV1_OptionalHeadersDecoder.h 
	codec/schema_v1/ccnxCodecSchemaV1_OptionalHeadersEncoder.h 
	codec/schema_v1/ccnxCodecSchemaV1_PacketDecoder.h 
	codec/schema_v1/ccnxCodecSchemaV1_PacketEncoder.h 
	codec/schema_v1/ccnxCodecSchemaV1_Types.h 
	codec/schema_v1/ccnxCodecSchemaV1_ValidationDecoder.h 
	codec/schema_v1/ccnxCodecSchemaV1_ValidationEncoder.h 
	)

set(COMMON_HDRS 
	libccnxCommon_About.h 
    ccnx_ContentObject.h 
	ccnx_Interest.h 
	ccnx_InterestReturn.h 
	ccnx_InterestPayloadId.h 
	ccnx_KeyLocator.h 
	ccnx_KeystoreUtilities.h 
	ccnx_Link.h 
	ccnx_Manifest.h 
	ccnx_ManifestSection.h 
	ccnx_Name.h 
	ccnx_NameSegment.h 
	ccnx_NameSegmentNumber.h 
	ccnx_NameLabel.h 
	ccnx_PayloadType.h 
	ccnx_TimeStamp.h  
	ccnx_WireFormatMessage.h 
	)
  
set(VALIDATION_SRCS  
	validation/ccnxValidation_CRC32C.c 
	validation/ccnxValidation_EcSecp256K1.c 
	validation/ccnxValidation_HmacSha256.c 
	validation/ccnxValidation_RsaSha256.c
	)

set(INTERNAL_SRCS  
	internal/ccnx_ChunkingFacadeV1.c 
	internal/ccnx_ContentObjectFacadeV1.c 
	internal/ccnx_ContentObjectInterface.c 
	internal/ccnx_InterestFacadeV1.c 
	internal/ccnx_InterestDefault.c 
	internal/ccnx_InterestInterface.c 
	internal/ccnx_InterestReturnFacadeV1.c 
	internal/ccnx_InterestReturnInterface.c 
	internal/ccnx_TlvDictionary.c 
	internal/ccnx_ValidationFacadeV1.c 
	internal/ccnx_WireFormatFacadeV1.c 
	internal/ccnx_WireFormatMessageInterface.c
	)


set(CODEC_V1_SRCS 
	codec/schema_v1/ccnxCodecSchemaV1_CryptoSuite.c 
	codec/schema_v1/ccnxCodecSchemaV1_FixedHeaderDecoder.c 
	codec/schema_v1/ccnxCodecSchemaV1_FixedHeaderEncoder.c 
	codec/schema_v1/ccnxCodecSchemaV1_LinkCodec.c 
	codec/schema_v1/ccnxCodecSchemaV1_MessageDecoder.c 
	codec/schema_v1/ccnxCodecSchemaV1_MessageEncoder.c 
	codec/schema_v1/ccnxCodecSchemaV1_NameCodec.c 
	codec/schema_v1/ccnxCodecSchemaV1_NameSegmentCodec.c 
	codec/schema_v1/ccnxCodecSchemaV1_OptionalHeadersDecoder.c 
	codec/schema_v1/ccnxCodecSchemaV1_OptionalHeadersEncoder.c 
	codec/schema_v1/ccnxCodecSchemaV1_PacketDecoder.c 
	codec/schema_v1/ccnxCodecSchemaV1_PacketEncoder.c 
	codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.c 
	codec/schema_v1/ccnxCodecSchemaV1_ValidationDecoder.c 
	codec/schema_v1/ccnxCodecSchemaV1_ValidationEncoder.c
	)

set(CODEC_SRCS  
	codec/ccnxCodec_EncodingBuffer.c 
	codec/ccnxCodec_Error.c 
	codec/ccnxCodec_NetworkBuffer.c 
	codec/ccnxCodec_TlvEncoder.c 
	codec/ccnxCodec_TlvDecoder.c 
	codec/ccnxCodec_TlvUtilities.c 
	codec/ccnxCodec_TlvPacket.c 
	${CODEC_V1_SRCS}
	)

set(CORE_SRCS   
	libccnxCommon_About.c 
    ccnx_ContentObject.c 
	ccnx_Interest.c 
	ccnx_InterestReturn.c 
	ccnx_InterestPayloadId.c 
	ccnx_KeyLocator.c 
	ccnx_KeystoreUtilities.c 
	ccnx_Link.c 
	ccnx_Manifest.c 
	ccnx_ManifestSection.c 
	ccnx_Name.c 
	ccnx_NameSegment.c 
	ccnx_NameSegmentNumber.c 
	ccnx_NameLabel.c 
	ccnx_TimeStamp.c 
	ccnx_WireFormatMessage.c
	)

set(ALL_SRCS  
	${INTERNAL_SRCS} 
	${VALIDATION_SRCS} 
	${CORE_SRCS} 
	${CODEC_SRCS} 
	)

set(PUBLIC_HDRS 
	${CODEC_PRIVATE_HDRS}
)

configure_file("config.h.in" "config.h" @ONLY)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -undefined dynamic_lookup")
	message( "-- Set \"-undefined dynamic_lookup\" for shared libraries")
endif()

add_library(ccnx_common         STATIC ${ALL_SRCS})
add_library(ccnx_common.shared  SHARED ${ALL_SRCS})
target_link_libraries(ccnx_common.shared ${LIBPARC_LIBRARIES})
set_target_properties(ccnx_common.shared PROPERTIES 
  C_STANDARD 99
  SOVERSION 1 
  VERSION 1.0
  OUTPUT_NAME ccnx_common )

set(libccnx_commonLibraries
  ccnx_common
  ccnx_common.shared
  )

foreach(lib ${libccnx_commonLibraries})
  install(TARGETS ${lib} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
  set_property(TARGET ${lib} PROPERTY C_STANDARD 99)
  set_property(TARGET ${lib} PROPERTY INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
endforeach()

install(FILES ${COMMON_HDRS}             DESTINATION include/ccnx/common )
install(FILES ${INTERNAL_HDRS}           DESTINATION include/ccnx/common/internal )
install(FILES ${VALIDATION_HDRS}         DESTINATION include/ccnx/common/validation )
install(FILES ${CODEC_HDRS}              DESTINATION include/ccnx/common/codec )
install(FILES ${TESTDATA_HDRS}           DESTINATION include/ccnx/common/codec/testdata )
install(FILES ${SCHEMAV1_HDRS}           DESTINATION include/ccnx/common/codec/schema_v1 )
install(FILES ${SCHEMAV1_TESTDATA_HDRS}  DESTINATION include/ccnx/common/codec/schema_v1/testdata )
	
add_subdirectory(test)
add_subdirectory(validation/test)
add_subdirectory(codec/test)
add_subdirectory(internal/test)
add_subdirectory(codec/schema_v1/test)

