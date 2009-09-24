FILE(GLOB OTBMonteverdiCommon_SRCS "*.cxx" )


SET(VIEW_FL_SRCS
otbMsgReporterGUI.fl
)

FLTK_WRAP_UI(MonteverdiCommon ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${VIEW_FL_SRCS}
            ${MonteverdiCommon_FLTK_UI_SRCS}
            #Monteverdi.cxx
        PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(OTBMonteverdiCommon ${OTBMonteverdiCommon_SRCS} ${MonteverdiCommon_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(OTBMonteverdiCommon ITKCommon)