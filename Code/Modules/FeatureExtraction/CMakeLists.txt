# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)


IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)


SET(MODEL_SRCS
Model/otbFeatureExtractionModel.cxx
)

SET(CONTROLLER_SRCS
Controller/otbFeatureExtractionController.cxx
)

SET(VIEW_FL_SRCS
View/otbFeatureExtractionViewGroup.fl
)

SET(VIEW_SRCS
View/otbFeatureExtractionViewGUI.cxx
)

SET(MODULE_SRCS
otbFeatureExtractionModule.cxx
)

INCLUDE_DIRECTORIES(
  ${Monteverdi_SOURCE_DIR}/Code/Modules/FeatureExtraction/View
  ${Monteverdi_SOURCE_DIR}/Code/Modules/FeatureExtraction/Model
  ${Monteverdi_SOURCE_DIR}/Code/Modules/FeatureExtraction/Controller
  ${Monteverdi_SOURCE_DIR}/Code/Modules/FeatureExtraction
)

FLTK_WRAP_UI(otbMVCFeatureExtractionModule ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${MODULE_SRCS} 
            ${MODEL_SRCS} 
            ${CONTROLLER_SRCS} 
            ${VIEW_SRCS} 
            ${otbMVCFeatureExtraction_FLTK_UI_SRCS}
            otbFeatureExtractionGUITest.cxx
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(otbMVCFeatureExtractionModule ${MODULE_SRCS} ${VIEW_SRCS} ${otbMVCFeatureExtraction_FLTK_UI_SRCS} ${MODEL_SRCS} ${CONTROLLER_SRCS})
# TARGET_LINK_LIBRARIES(otbMVCFeatureExtraction OTBIO OTBBasicFilters OTBVisualization OTBFeatureExtraction OTBApplicationsCommon)
TARGET_LINK_LIBRARIES(otbMVCFeatureExtractionModule OTBMonteverdiCommon OTBIO OTBBasicFilters OTBFeatureExtraction OTBGui)
# INSTALL_TARGETS(/bin otbMVCFeatureExtraction)
# ADD_DEPENDENCIES(otbMVCFeatureExtraction OTBApplicationsCommon)                                            
# ADD_EXECUTABLE(otbFeatureExtractionApplication otbFeatureExtractionGUITest.cxx )
# TARGET_LINK_LIBRARIES(otbFeatureExtractionApplication otbMVCFeatureExtraction)
# INSTALL_TARGETS(/bin otbFeatureExtractionApplication)

ENDIF(OTB_USE_VISU_GUI)
