FILE(GLOB OTBMonteverdiCommon_SRCS "*.cxx" )

SET(OTBMonteverdiCommon_SRCS ${OTBMonteverdiCommon_SRCS})

ADD_LIBRARY(OTBMonteverdiCommon ${OTBMonteverdiCommon_SRCS} )
TARGET_LINK_LIBRARIES(OTBMonteverdiCommon OTBIO OTBCommon OTBGuiFLTK OTBVisuFLTK flu)


IF(NOT Monteverdi_INSTALL_NO_LIBRARIES)
  INSTALL(TARGETS OTBMonteverdiCommon
    RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT Development)
ENDIF(NOT Monteverdi_INSTALL_NO_LIBRARIES)

ADD_SUBDIRECTORY(FeatureExtractionBase)
