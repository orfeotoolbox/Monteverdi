# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)


IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)


SET(MODEL_SRCS
Model/otbObjectCountingModel.cxx
)

SET(CONTROLLER_SRCS
Controller/otbObjectCountingController.cxx
)

SET(VIEW_FL_SRCS
View/otbObjectCountingViewGroup.fl
)

SET(VIEW_SRCS
View/otbObjectCountingViewGUI.cxx
)

INCLUDE_DIRECTORIES(
  ${OTB-Applications_SOURCE_DIR}/ObjectCountingApplication/View
  ${OTB-Applications_SOURCE_DIR}/ObjectCountingApplication/Model
  ${OTB-Applications_SOURCE_DIR}/ObjectCountingApplication/Controller
  ${OTB-Applications_BINARY_DIR}/ObjectCountingApplication
)

FLTK_WRAP_UI(otbMVCObjectCounting ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${VIEW_SRCS} 
            ${CONTROLLER_SRCS}
            ${MODEL_SRCS}
            ${otbMVCObjectCounting_FLTK_UI_SRCS}
            otbObjectCountingApplication.cxx 
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(otbMVCObjectCounting ${VIEW_SRCS} ${otbMVCObjectCounting_FLTK_UI_SRCS} ${MODEL_SRCS} ${CONTROLLER_SRCS})
TARGET_LINK_LIBRARIES(otbMVCObjectCounting OTBIO OTBBasicFilters OTBApplicationsCommon OTBVisu OTBLearning)
INSTALL_TARGETS(/bin otbMVCObjectCounting)
ADD_DEPENDENCIES(otbMVCObjectCounting OTBApplicationsCommon)

ADD_EXECUTABLE(otbObjectCountingApplication otbObjectCountingApplication.cxx )
TARGET_LINK_LIBRARIES(otbObjectCountingApplication otbMVCObjectCounting)
INSTALL_TARGETS(/bin otbObjectCountingApplication)



ENDIF(OTB_USE_VISU_GUI)
