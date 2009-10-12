# Only if OTB is generated with Visu GUI option
IF(OTB_USE_VISU_GUI)


# First, compile the module

SET(MODULE_SRCS
otbHomologousPointExtractionModule.cxx
otbHomologousPointExtractionModuleController.cxx
otbHomologousPointExtractionModuleModel.cxx
otbHomologousPointExtractionModuleView.cxx
)

SET(VIEW_FL_SRCS
otbHomologousPointExtractionViewGroup.fl
)


# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target

FLTK_WRAP_UI(otbHomologousPointExtractionModule ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointer
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${MODULE_SRCS}
            ${otbHomologousPointExtractionApplication_FLTK_UI_SRCS}
	    PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)



ADD_LIBRARY(otbHomologousPointExtractionModule ${MODULE_SRCS}  ${otbHomologousPointExtractionModule_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbHomologousPointExtractionModule OTBMonteverdiCommon OTBIO OTBBasicFilters OTBGui OTBVisualization)
ADD_DEPENDENCIES(otbHomologousPointExtractionModule OTBMonteverdiCommon)

IF(NOT OTB_INSTALL_NO_LIBRARIES)
  INSTALL(TARGETS otbHomologousPointExtractionModule
    RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT Development)
ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)


# Then, compile the application

       
ENDIF(OTB_USE_VISU_GUI)