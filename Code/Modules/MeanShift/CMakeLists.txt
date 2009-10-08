# Only if OTB is generated with Visu GUI option
IF(OTB_USE_VISU_GUI)


# First, compile the module

SET(MODULE_SRCS
otbMeanShiftModule.cxx
otbMeanShiftModuleController.cxx
otbMeanShiftModuleModel.cxx
otbMeanShiftModuleView.cxx
)

SET(VIEW_FL_SRCS
otbMeanShiftModuleViewGroup.fl
)


# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target

FLTK_WRAP_UI(otbMeanShiftModule ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointer
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${MODULE_SRCS}
            ${otbMeanShiftApplication_FLTK_UI_SRCS}
	    PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)



ADD_LIBRARY(otbMeanShiftModule ${MODULE_SRCS}  ${otbMeanShiftModule_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbMeanShiftModule OTBMonteverdiCommon OTBIO OTBBasicFilters OTBGui)
ADD_DEPENDENCIES(otbMeanShiftModule OTBMonteverdiCommon)

INSTALL(TARGETS otbMeanShiftModule
        RUNTIME DESTINATION /bin
        LIBRARY DESTINATION /bin
        ARCHIVE DESTINATION /bin )

# Then, compile the application

ADD_EXECUTABLE(otbMeanShiftApplication otbMeanShiftApplication.cxx)
TARGET_LINK_LIBRARIES(otbMeanShiftApplication otbMonteverdi otbMeanShiftModule)
INSTALL(TARGETS otbMeanShiftApplication
        RUNTIME DESTINATION /bin
        LIBRARY DESTINATION /bin
        ARCHIVE DESTINATION /bin )
        
ENDIF(OTB_USE_VISU_GUI)