# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)

SET(MODULE_SRCS
otbCachingModule.cxx
)

SET(VIEW_FL_SRCS
otbCachingModuleGUI.fl
)


# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target

FLTK_WRAP_UI(otbCachingModule ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${MODULE_SRCS}
            ${otbCachingApplication_FLTK_UI_SRCS}
	    PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(otbCachingModule ${MODULE_SRCS} ${otbCachingModule_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbCachingModule OTBMonteverdiCommon OTBIO OTBBasicFilters OTBGui flu)

ENDIF(OTB_USE_VISU_GUI)







