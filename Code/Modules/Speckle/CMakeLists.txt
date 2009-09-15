# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

SET(MODEL_SRCS
Model/otbSpeckleFilteringModel.cxx
)

SET(CONTROLLER_SRCS
Controller/otbSpeckleFilteringController.cxx
)

SET(VIEW_FL_SRCS
View/otbSpeckleFilteringViewGroup.fl
)

SET(VIEW_SRCS
View/otbSpeckleFilteringViewGUI.cxx
)


INCLUDE_DIRECTORIES(
View
#  ${OTB-Applications_SOURCE_DIR}/ViewerManager/Model
#  ${OTB-Applications_SOURCE_DIR}/ViewerManager/Controller
#  ${OTB-Applications_BINARY_DIR}/ViewerManager
#  ${OTB-Applications_SOURCE_DIR}/Common
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
            ${otbMVCSpeckleFiltering_FLTK_UI_SRCS}
            otbSpeckleFiltering.cxx
	    PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)



ADD_EXECUTABLE(otbSpeckleFilteringApplication  otbSpeckleFilteringApplication.cxx ${VIEW_SRCS} ${otbMVCSpeckleFiltering_FLTK_UI_SRCS} ${MODEL_SRCS} ${CONTROLLER_SRCS})
TARGET_LINK_LIBRARIES(otbSpeckleFilteringApplication OTBIO OTBVisualization OTBLearning OTBFeatureExtraction)


ENDIF(OTB_USE_VISU_GUI)










#Removed 

#ADD_LIBRARY(otbMVCSpeckleFiltering   ${VIEW_SRCS}   ${CONTROLLER_SRCS} ${MODEL_SRCS} ${otbMVCSpeckleFiltering_FLTK_UI_SRCS} ) 
#TARGET_LINK_LIBRARIES(otbMVCSpeckleFiltering OTBIO  OTBVisualization  OTBApplicationsCommon )
#INSTALL_TARGETS(/bin otbMVCSpeckleFiltering)
#ADD_DEPENDENCIES(otbMVCSpeckleFiltering)

#ADD_EXECUTABLE(otbSpeckleFiltering otbSpeckleFiltering.cxx )
#TARGET_LINK_LIBRARIES(otbSpeckleFiltering otbMVCSpeckleFiltering)
#INSTALL_TARGETS(/bin otbSpeckleFiltering)



#INCLUDE_DIRECTORIES(
#  ${OTB-Applications_SOURCE_DIR}/ViewerManager/View
#  ${OTB-Applications_SOURCE_DIR}/ViewerManager/Model
#  ${OTB-Applications_SOURCE_DIR}/ViewerManager/Controller
#  ${OTB-Applications_BINARY_DIR}/ViewerManager
#  ${OTB-Applications_SOURCE_DIR}/Common
#)