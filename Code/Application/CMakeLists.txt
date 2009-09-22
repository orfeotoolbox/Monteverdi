# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${Monteverdi_BINARY_DIR})


SET(APPLICATION_SRCS
otbMonteverdiModel.cxx
otbMonteverdiViewGUI.cxx
otbMonteverdiController.cxx
)

SET(VIEW_FL_SRCS
otbMonteverdiViewGroup.fl
)


# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target

FLTK_WRAP_UI(Monteverdi ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${APPLICATION_SRCS}
            ${Monteverdi_FLTK_UI_SRCS}
            Monteverdi.cxx
        PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(otbMonteverdi ${APPLICATION_SRCS} ${Monteverdi_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbMonteverdi OTBGui OTBMonteverdiCommon otbReaderModule otbSpeckleFilteringModule flu)
INSTALL_TARGETS(/bin otbMonteverdi)

ADD_EXECUTABLE(Monteverdi Monteverdi.cxx )
TARGET_LINK_LIBRARIES(Monteverdi otbMonteverdi)
INSTALL_TARGETS(/bin Monteverdi)


ADD_EXECUTABLE(SplashScreen SplashScreen.cxx)
TARGET_LINK_LIBRARIES(SplashScreen OTBIO OTBVisualization flu)
INSTALL_TARGETS(/bin SplashScreen)

ENDIF(OTB_USE_VISU_GUI)