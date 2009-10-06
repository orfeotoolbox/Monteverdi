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

SET(VIEW_FL_SRCS
otbWriterViewGroup.fl
)

SET(VIEW_SRCS
otbWriterViewGUI.cxx
)

SET(MODULE_SRCS
otbWriterMVCModule.cxx
)

INCLUDE_DIRECTORIES(
#   ${Monteverdi_SOURCE_DIR}/Code/Modules/Writer/View
#   ${Monteverdi_SOURCE_DIR}/Code/Modules/Writer/Model
#   ${Monteverdi_SOURCE_DIR}/Code/Modules/Writer/Controller
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
            ${otbWriterMVC_FLTK_UI_SRCS}
            otbWriterGUITest.cxx
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(otbWriterMVCModule ${MODULE_SRCS} ${VIEW_SRCS} ${otbWriterMVCModule_FLTK_UI_SRCS} ${MODEL_SRCS} ${CONTROLLER_SRCS})
# TARGET_LINK_LIBRARIES(otbMVCWriter OTBIO OTBBasicFilters OTBVisualization OTBWriter OTBApplicationsCommon)
TARGET_LINK_LIBRARIES(otbWriterMVCModule OTBMonteverdiCommon OTBIO OTBBasicFilters OTBGui OTBVisualization)
# INSTALL(TARGETS /bin otbMVCWriter)
# ADD_DEPENDENCIES(otbMVCWriter OTBApplicationsCommon)                                            
# ADD_EXECUTABLE(otbWriterApplication otbWriterGUITest.cxx )
# TARGET_LINK_LIBRARIES(otbWriterApplication otbMVCWriter)
# INSTALL(TARGETS /bin otbWriterApplication)

ENDIF(OTB_USE_VISU_GUI)
