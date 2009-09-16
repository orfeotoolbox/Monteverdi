
# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)


IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)


SET(OR_SRCS 
otbOrthoRectifAppli.cxx 
otbOrthoRectif.cxx )

# Include directories
INCLUDE_DIRECTORIES(
${Monteverdi_SOURCE_DIR}/Code/Modules/OrthoRectif
${Monteverdi_BINARY_DIR}/Code/Modules/OrthoRectif	
)


SET(VIEW_FL_SRCS
otbOrthoRectifGUI.fl
)


 
# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target
FLTK_WRAP_UI(otbOrthoRectifApplication ${VIEW_FL_SRCS})


# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${OR_SRCS} 
            ${otbOrthoRectifApplication_FLTK_UI_SRCS} 
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(otbOrthoRectifApplication ${OR_SRCS} ${otbOrthoRectifApplication_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbOrthoRectifApplication   OTBIO OTBProjections OTBVisu ) #OTBGui

ADD_EXECUTABLE(otbOrthoRectifApplicationGUI ${OR_SRCS} ${otbOrthoRectifApplication_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbOrthoRectifApplicationGUI  otbOrthoRectifApplication) #OTBGui



ENDIF(OTB_USE_VISU_GUI)
