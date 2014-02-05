set(MODEL_SRCS
Model/otbViewerModel.cxx
)

set(CONTROLLER_SRCS
Controller/otbViewerController.cxx
)

set(VIEW_FL_SRCS
View/otbViewerViewGroup.fl
)

set(VIEW_SRCS
View/otbViewerViewGUI.cxx
)


include_directories(
  ${Monteverdi_SOURCE_DIR}/Code/Viewer/View
  ${Monteverdi_SOURCE_DIR}/Code/Viewer/Model
  ${Monteverdi_SOURCE_DIR}/Code/Viewer/Controller
  ${Monteverdi_BINARY_DIR}/Code/Viewer
)

# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target

fltk_wrap_ui(otbViewer ${VIEW_FL_SRCS})

# Suppress warning Fl_RGB_Image deferencing pointeur
if(CMAKE_COMPILER_IS_GNUCXX)
      set_source_files_properties(
            ${MODEL_SRCS}
            ${CONTROLLER_SRCS}
            ${VIEW_SRCS}
            ${otbViewer_FLTK_UI_SRCS}
            otbViewer.cxx
	    PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
endif()

add_executable(otbViewer
                   otbViewer.cxx
                   ${VIEW_SRCS} ${CONTROLLER_SRCS} ${MODEL_SRCS}
                   ${otbViewer_FLTK_UI_SRCS})
target_link_libraries(otbViewer OTBIO OTBCommon OTBVisuFLTK)

install(TARGETS otbViewer
        RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
        LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
        ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT Development)


