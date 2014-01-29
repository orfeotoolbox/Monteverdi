# Sources of non-templated classes.

file(GLOB OTBGuiFLTK_SRCS "*.cxx" )

add_library(OTBGuiFLTK ${OTBGuiFLTK_SRCS})
target_link_libraries(OTBGuiFLTK OTBCommon ${MVD_VISU_GUI_LIBRARIES})
if(OTB_LIBRARY_PROPERTIES)
  set_target_properties(OTBGuiFLTK PROPERTIES ${MVD_LIBRARY_PROPERTIES})
endif()


