#Init list
SET(LIST_MODULE_LIBRARIES "")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

FILE(GLOB moduleList ${Monteverdi_SOURCE_DIR}/Code/Modules/*)
FOREACH(moduleDir ${moduleList})
  IF(IS_DIRECTORY ${moduleDir})
    SET(LIB_NAME "otb${moduleName}Module")
#    SET(APPLI_NAME "otb${moduleName}Application")


    FILE(RELATIVE_PATH moduleName ${Monteverdi_SOURCE_DIR}/Code/Modules ${moduleDir})
    FILE(GLOB SC_SRCS  ${moduleDir}/*.cxx )
    FILE(GLOB SC_GUI_SRCS_TEMP  ${moduleDir}/*.fl )
    SET(SC_GUI_SRCS "")
    FOREACH(file_gui_src ${SC_GUI_SRCS_TEMP})
      GET_FILENAME_COMPONENT(name "${file_gui_src}" NAME)
      SET (SC_GUI_SRCS 
          ${SC_GUI_SRCS}
          "${moduleName}/${name}"
          )
    ENDFOREACH(file_gui_src)
    IF(SC_GUI_SRCS)
      FLTK_WRAP_UI(${LIB_NAME} ${SC_GUI_SRCS}) 
      # Suppress warning Fl_RGB_Image deferencing pointeur
      IF(CMAKE_COMPILER_IS_GNUCXX)
        SET_SOURCE_FILES_PROPERTIES(
            ${SC_SRCS} 
            ${${LIB_NAME}_FLTK_UI_SRCS}
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
      ENDIF(CMAKE_COMPILER_IS_GNUCXX)
      # Add GUI cxx file
      SET(SC_SRCS ${SC_SRCS} ${${LIB_NAME}_FLTK_UI_SRCS} )
    ENDIF(SC_GUI_SRCS)

    ADD_LIBRARY(${LIB_NAME} ${SC_SRCS} )
    TARGET_LINK_LIBRARIES(${LIB_NAME} OTBLearning OTBIO OTBVisualization OTBVisu OTBGui OTBMonteverdiCommon)
    ADD_DEPENDENCIES(${LIB_NAME} OTBMonteverdiCommon)

    SET(LIST_MODULE_LIBRARIES "${LIST_MODULE_LIBRARIES};${LIB_NAME}" )

    IF(NOT OTB_INSTALL_NO_LIBRARIES)
      INSTALL(TARGETS ${LIB_NAME}
        RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
        LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
        ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT Development)
    ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)

    # Create Executable
#    OPTION(USE_${APPLI_NAME} "Build optional application Code/Module/${moduleName}." ON)
#    IF(USE_${APPLI_NAME})
#      ADD_EXECUTABLE(${APPLI_NAME}  ...... )
#      TARGET_LINK_LIBRARIES(${APPLI_NAME} ${LIB_NAME})
#      IF(NOT OTB_INSTALL_NO_LIBRARIES)
#        INSTALL(TARGETS ${APPLI_NAME}
#          RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
#          LIBRARY DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
#          ARCHIVE DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT Development)
#      ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)
#    ENDIF(USE_${APPLI_NAME})


  ENDIF(IS_DIRECTORY ${moduleDir})
ENDFOREACH(moduleDir)

SET(LIST_MODULE_LIBRARIES ${LIST_MODULE_LIBRARIES} CACHE STRING "List of alls modules libraries" FORCE)
