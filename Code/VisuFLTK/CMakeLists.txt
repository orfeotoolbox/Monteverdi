# Sources of non-templated classes.

file(GLOB OTBVisuFLTK_SRCS "*.cxx" )

fltk_wrap_ui(OTBVisuFLTK
               otbImageWidgetPackedManager.fl
               otbImageWidgetSplittedManager.fl
               otbMsgReporterGUI.fl)

add_library(OTBVisuFLTK ${OTBVisuFLTK_SRCS} ${OTBVisuFLTK_FLTK_UI_SRCS})
target_link_libraries(OTBVisuFLTK OTBGuiFLTK ${OTB_LIBRARIES} ${MVD_VISU_GUI_LIBRARIES})
if(Monteverdi_LIBRARY_PROPERTIES)
  set_target_properties(OTBVisuFLTK PROPERTIES ${Monteverdi_LIBRARY_PROPERTIES})
endif()

install(TARGETS OTBVisuFLTK
        EXPORT MonteverdiTargets
        RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
        LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
        ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT Development)

file(GLOB __files1 "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB __files2 "${CMAKE_CURRENT_SOURCE_DIR}/*.txx")
file(GLOB __files3 "${CMAKE_CURRENT_SOURCE_DIR}/*.fl")
install(FILES ${__files1} ${__files2}  ${__files3}
   ${CMAKE_CURRENT_BINARY_DIR}/otbImageWidgetPackedManager.h
   ${CMAKE_CURRENT_BINARY_DIR}/otbImageWidgetSplittedManager.h
   ${CMAKE_CURRENT_BINARY_DIR}/otbMsgReporterGUI.h
   DESTINATION ${Monteverdi_INSTALL_INCLUDE_DIR}/VisuFLTK
   COMPONENT Development)
