#ADD_EXECUTABLE(otbUnsignedShortRelabeling otbUnsignedShortRelabeling.cxx)
#TARGET_LINK_LIBRARIES(otbUnsignedShortRelabeling OTBIO OTBCommon)

#ADD_EXECUTABLE(otbRgbRelabeling otbRgbRelabeling.cxx)
#TARGET_LINK_LIBRARIES(otbRgbRelabeling OTBIO OTBCommon)

#ADD_EXECUTABLE(otbSVMClassification otbSVMClassification.cxx)
#TARGET_LINK_LIBRARIES(otbSVMClassification OTBIO OTBCommon  OTBLearning)

#ADD_EXECUTABLE(otbSOMClassification otbSOMClassification.cxx)
#TARGET_LINK_LIBRARIES(otbSOMClassification OTBIO OTBCommon OTBLearning)

#ADD_EXECUTABLE(otbKMeansClassification otbKMeansClassification.cxx)
#TARGET_LINK_LIBRARIES(otbKMeansClassification OTBIO OTBCommon OTBLearning)


# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)


SET(SC_SRCS 
otbSupervisedClassificationAppli.cxx
otbSupervisedClassificationApplication.cxx
otbSupervisedClassificationModule.cxx
)

SET(SC_GUI_SRCS 
otbSupervisedClassificationAppliGUI.fl
)

FLTK_WRAP_UI(otbSupervisedClassificationApplication otbSupervisedClassificationAppliGUI.fl)

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${SC_SRCS} 
            ${otbSupervisedClassificationApplication_FLTK_UI_SRCS}
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(otbSupervisedClassificationModule ${SC_SRCS} ${otbSupervisedClassificationApplication_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbSupervisedClassificationModule OTBLearning OTBIO OTBVisu OTBGui OTBMonteverdiCommon)

ADD_EXECUTABLE(otbSupervisedClassificationApplication ${SC_SRCS} ${otbSupervisedClassificationApplication_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbSupervisedClassificationApplication otbSupervisedClassificationModule OTBVisualization)

ENDIF(OTB_USE_VISU_GUI)
