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
otbInputChoiceDescriptor.cxx
otbInputViewGUI.cxx
)

SET(VIEW_FL_SRCS
otbMonteverdiViewGroup.fl
otbInputViewGroup.fl
)


# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target

FLTK_WRAP_UI(otbMonteverdi ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${APPLICATION_SRCS}
            ${Monteverdi_FLTK_UI_SRCS}
            Monteverdi.cxx
        PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(otbMonteverdi ${APPLICATION_SRCS} ${otbMonteverdi_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbMonteverdi OTBGui OTBMonteverdiCommon 
                      otbReaderModule otbSpeckleFilteringModule otbFeatureExtractionModule otbSupervisedClassificationModule 
                      otbOrthorectificationModule otbWriterModule otbWriterMVCModule otbViewerModule flu)
INSTALL_TARGETS(/bin otbMonteverdi)


ADD_EXECUTABLE(Monteverdi Monteverdi.cxx
)
TARGET_LINK_LIBRARIES(Monteverdi otbMonteverdi)
INSTALL_TARGETS(/bin Monteverdi)


ENDIF(OTB_USE_VISU_GUI)