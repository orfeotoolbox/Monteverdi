# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)


SET(MODEL_SRCS
otbWriterModel.cxx
)

SET(CONTROLLER_SRCS
otbWriterController.cxx
)

SET(MODULE_SRCS
otbWriterMVCModule.cxx
)

SET(VIEW_FL_SRCS
otbWriterMVCModuleGUI.fl
)

SET(VIEW_SRCS
otbWriterView.cxx
)
# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target

INCLUDE_DIRECTORIES(
  ${Monteverdi_SOURCE_DIR}/Code/Modules/WriterMVC
)

FLTK_WRAP_UI(otbWriterMVCModule ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${MODULE_SRCS}
            ${MODEL_SRCS} 
            ${CONTROLLER_SRCS} 
            ${VIEW_SRCS} 
            ${otbWriterMVCApplication_FLTK_UI_SRCS}
	    PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(otbWriterMVCModule ${MODULE_SRCS} ${VIEW_SRCS} ${MODEL_SRCS} ${CONTROLLER_SRCS} ${otbWriterModule_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbWriterMVCModule OTBMonteverdiCommon OTBIO OTBBasicFilters OTBGui OTBVisualization)

ENDIF(OTB_USE_VISU_GUI)







