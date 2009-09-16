# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

SET(MODEL_SRCS
otbSpeckleFilteringModel.cxx
)

SET(CONTROLLER_SRCS
otbSpeckleFilteringController.cxx
)

SET(VIEW_FL_SRCS
otbSpeckleFilteringViewGUI.fl
)

SET(VIEW_SRCS
otbSpeckleFilteringView.cxx
)


# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target

FLTK_WRAP_UI(otbSpeckleFilteringApplication ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${MODEL_SRCS}
            ${CONTROLLER_SRCS}
            ${VIEW_SRCS}
            ${otbSpeckleFilteringApplication_FLTK_UI_SRCS}
            otbSpeckleFiltering.cxx
	    PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)



ADD_EXECUTABLE(otbSpeckleFilteringApplication  otbSpeckleFilteringApplication.cxx ${VIEW_SRCS} ${otbSpeckleFilteringApplication_FLTK_UI_SRCS} ${MODEL_SRCS} ${CONTROLLER_SRCS})
TARGET_LINK_LIBRARIES(otbSpeckleFilteringApplication OTBIO OTBVisualization OTBBasicFilters )


ENDIF(OTB_USE_VISU_GUI)







