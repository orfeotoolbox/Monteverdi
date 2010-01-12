# First, compile the module
SET(MODULE_SRCS
otbExampleModule.cxx
)

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${MODULE_SRCS}
            otbExampleApplication.cxx
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(otbExampleModule ${MODULE_SRCS})
TARGET_LINK_LIBRARIES(otbExampleModule OTBMonteverdiCommon OTBIO)

IF(NOT OTB_INSTALL_NO_LIBRARIES)
  INSTALL(TARGETS otbExampleModule
    RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT Development)
ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)


# Then, compile the application
ADD_EXECUTABLE(otbExampleApplication otbExampleApplication.cxx)
TARGET_LINK_LIBRARIES(otbExampleApplication otbMonteverdi otbExampleModule otbReaderModule)

# CPack installation: only Monteverdi application
#IF(NOT OTB_INSTALL_NO_LIBRARIES)
#  INSTALL(TARGETS otbExampleApplication
#    RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
#    LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
#    ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT Development)
#ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)

