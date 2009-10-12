# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)


SET(SC_SRCS 
otbSupervisedClassificationAppli.cxx
otbSupervisedClassificationModule.cxx
)

SET(SC_GUI_SRCS 
otbSupervisedClassificationAppliGUI.fl
)

FLTK_WRAP_UI(otbSupervisedClassificationModule otbSupervisedClassificationAppliGUI.fl)

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${SC_SRCS} 
            ${otbSupervisedClassificationApplication_FLTK_UI_SRCS}
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(otbSupervisedClassificationModule ${SC_SRCS} ${otbSupervisedClassificationModule_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbSupervisedClassificationModule OTBLearning OTBIO OTBVisu OTBGui OTBMonteverdiCommon)
ADD_DEPENDENCIES(otbSupervisedClassificationModule OTBMonteverdiCommon)

ENDIF(OTB_USE_VISU_GUI)
