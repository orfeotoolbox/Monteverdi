# Sources of non-templated classes.

file(GLOB OTBVisuLegacyFLTK_SRCS "*.cxx" )

add_library(OTBVisuLegacyFLTK ${OTBVisuLegacyFLTK_SRCS})
target_link_libraries(OTBVisuLegacyFLTK OTBGuiFLTK OTBCommon ${MVD_VISU_GUI_LIBRARIES})
if(OTB_LIBRARY_PROPERTIES)
  set_target_properties(OTBVisuLegacyFLTK PROPERTIES ${MVD_LIBRARY_PROPERTIES})
endif()


