# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)


SET(MODULE_SRCS
otbViewerModule.cxx
)

SET(VIEW_FL_SRCS
otbViewerModuleGroup.fl
)


# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target
FLTK_WRAP_UI(otbViewerModule ${VIEW_FL_SRCS})


# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${MODULE_SRCS}
	    ${otbViewerModule_FLTK_UI_SRCS}
	    PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(otbViewerModule ${MODULE_SRCS} ${otbReaderModule_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbViewerModule OTBMonteverdiCommon OTBIO OTBBasicFilters OTBVisualization OTBGui)
				    
ENDIF(OTB_USE_VISU_GUI)







