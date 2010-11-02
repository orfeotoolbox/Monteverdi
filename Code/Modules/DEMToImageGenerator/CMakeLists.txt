# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)


SET(MODULE_SRCS
otbDEMToImageGeneratorModule.cxx
otbDEMToImageGeneratorModel.cxx
otbDEMToImageGeneratorController.cxx
otbDEMToImageGeneratorView.cxx
)

SET(DEM_TO_IMAGE_GENERATOR_FL_SRCS
otbDEMToImageGeneratorGUI.fl
)


# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target
FLTK_WRAP_UI(otbDEMToImageGeneratorModule ${DEM_TO_IMAGE_GENERATOR_FL_SRCS})


# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${MODULE_SRCS}
	    ${otbDEMToImageGeneratorModule_FLTK_UI_SRCS}
	    PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(otbDEMToImageGeneratorModule ${MODULE_SRCS} ${otbDEMToImageGeneratorModule_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbDEMToImageGeneratorModule OTBMonteverdiCommon OTBIO OTBVisualization OTBGui flu OTBCommon)
ADD_DEPENDENCIES( otbDEMToImageGeneratorModule OTBMonteverdiCommon)	    


ENDIF(OTB_USE_VISU_GUI)
