IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})


SET(APPLICATION_SRCS
otbMonteverdiModel.cxx
otbMonteverdiViewGUI.cxx
otbMonteverdiController.cxx
otbInputViewComponent.cxx
otbInputViewGUI.cxx
otbMonteverdiPixmaps.cxx
otbFluTreeBrowser.cxx
)

SET(VIEW_FL_SRCS
otbMonteverdiViewGroup.fl
otbInputViewGroup.fl
)


# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target

FLTK_WRAP_UI(otbMonteverdi ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${APPLICATION_SRCS}
            ${otbMonteverdi_FLTK_UI_SRCS}
            Monteverdi.cxx
        PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(otbMonteverdi ${APPLICATION_SRCS} ${otbMonteverdi_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(otbMonteverdi OTBGui OTBMonteverdiCommon flu otbCachingModule)
ADD_DEPENDENCIES(otbMonteverdi OTBMonteverdiCommon otbCachingModule)

IF(NOT OTB_INSTALL_NO_LIBRARIES)
  INSTALL(TARGETS otbMonteverdi
    RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT Development)
ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)

#Create Monteverdi application
ADD_EXECUTABLE(Monteverdi Monteverdi.cxx)
TARGET_LINK_LIBRARIES(Monteverdi otbMonteverdi ${LIST_MODULE_LIBRARIES} )
IF(NOT OTB_INSTALL_NO_LIBRARIES)
  INSTALL(TARGETS Monteverdi
    RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
    LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT Development)
ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)
