# Sources of non-templated classes.

file(GLOB OTBVisuLegacyFLTK_SRCS "*.cxx" )

add_library(OTBVisuLegacyFLTK ${OTBVisuLegacyFLTK_SRCS})
target_link_libraries(OTBVisuLegacyFLTK OTBGuiFLTK OTBCommon ${MVD_VISU_GUI_LIBRARIES})
if(MVD_LIBRARY_PROPERTIES)
  set_target_properties(OTBVisuLegacyFLTK PROPERTIES ${MVD_LIBRARY_PROPERTIES})
endif()

install(TARGETS OTBVisuLegacyFLTK
        RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
        LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
        ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT Development)

file(GLOB __files1 "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB __files2 "${CMAKE_CURRENT_SOURCE_DIR}/*.txx")
install(FILES ${__files1} ${__files2}
        DESTINATION ${Monteverdi_INSTALL_INCLUDE_DIR}/VisuLegacyFLTK
        COMPONENT Development)
