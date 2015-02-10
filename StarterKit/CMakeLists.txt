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
TARGET_LINK_LIBRARIES(otbExampleModule OTBMonteverdiCommon ${OTB_LIBRARIES})
IF(Monteverdi_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(otbExampleModule PROPERTIES ${Monteverdi_LIBRARY_PROPERTIES})
ENDIF()

# Then, compile the application
ADD_EXECUTABLE(otbExampleApplication otbExampleApplication.cxx)
TARGET_LINK_LIBRARIES(otbExampleApplication otbMonteverdi otbExampleModule otbReaderModule)
