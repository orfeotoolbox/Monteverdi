
# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)


IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

SET(OR_SRCS 
otbOrthorectificationAppli.cxx 
otbOrthorectification.cxx 
otbOrthorectificationModule.cxx
)

SET(VIEW_FL_SRCS
otbOrthorectificationGUI.fl
)
 
# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target
FLTK_WRAP_UI(otbOrthorectificationApplication ${VIEW_FL_SRCS})


# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${OR_SRCS} 
            ${otbOrthorectificationApplication_FLTK_UI_SRCS} 
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(otbOrthorectificationModule ${OR_SRCS} ${otbOrthorectificationApplication_FLTK_UI_SRCS})
#TARGET_LINK_LIBRARIES(otbOrthorectificationModule  OTBIO OTBProjections OTBGui OTBMonteverdiCommon)
TARGET_LINK_LIBRARIES(otbOrthorectificationModule  OTBProjections OTBGui OTBMonteverdiCommon)
ADD_DEPENDENCIES(otbOrthorectificationModule OTBMonteverdiCommon)


ENDIF(OTB_USE_VISU_GUI)
