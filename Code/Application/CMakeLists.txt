IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${Monteverdi_BINARY_DIR})

ADD_EXECUTABLE(Monteverdi  Monteverdi.cxx otbMonteverdiModel.cxx)
TARGET_LINK_LIBRARIES(Monteverdi OTBGui OTBMonteverdiCommon otbReaderModule otbSpeckleFilteringModule)
INSTALL_TARGETS(/bin Monteverdi)

