# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)


SET(MODULE_SRCS
otbWriterModule.cxx
)

SET(VIEW_FL_SRCS
otbWriterModuleGUI.fl
)


# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target

FLTK_WRAP_UI(otbWriterModule ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${MODULE_SRCS}
            ${otbWriterApplication_FLTK_UI_SRCS}
	    PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(otbWriterModule ${MODULE_SRCS} ${otbWriterModule_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbWriterModule OTBMonteverdiCommon OTBIO OTBBasicFilters OTBGui flu)
ADD_DEPENDENCIES(otbWriterModule OTBMonteverdiCommon)

ENDIF(OTB_USE_VISU_GUI)







