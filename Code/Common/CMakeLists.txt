FILE(GLOB OTBMonteverdiCommon_SRCS "*.cxx" )


SET(VIEW_FL_SRCS
otbMsgReporterGUI.fl
)

FLTK_WRAP_UI(OTBMonteverdiCommon ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${VIEW_FL_SRCS}
            ${OTBMonteverdiCommon_FLTK_UI_SRCS}
            #Monteverdi.cxx
        PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET(OTBMonteverdiCommon_SRCS ${OTBMonteverdiCommon_SRCS} ${OTBMonteverdiCommon_FLTK_UI_SRCS})

ADD_LIBRARY(OTBMonteverdiCommon ${OTBMonteverdiCommon_SRCS} )
TARGET_LINK_LIBRARIES(OTBMonteverdiCommon OTBIO OTBCommon OTBGui flu)


IF(NOT Monteverdi_INSTALL_NO_LIBRARIES)
  INSTALL(TARGETS OTBMonteverdiCommon
    RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT Development)
ENDIF(NOT Monteverdi_INSTALL_NO_LIBRARIES)

