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
TARGET_LINK_LIBRARIES(OTBMonteverdiCommon ITKCommon )

INSTALL_TARGETS(/bin OTBMonteverdiCommon)