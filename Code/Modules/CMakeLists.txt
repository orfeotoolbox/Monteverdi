#Init list
SET(LIST_MODULE_LIBRARIES "")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

FILE(GLOB moduleList ${Monteverdi_SOURCE_DIR}/Code/Modules/*)

#For insource compilation, the CMakeFiles directory is created by cmake inside
#the module directory. Of course, this is not a real module and should be
#removed from the module list
LIST(REMOVE_ITEM moduleList ${Monteverdi_SOURCE_DIR}/Code/Modules/CMakeFiles)
if(NOT OTBSVMLearning_LOADED)
LIST(REMOVE_ITEM moduleList ${Monteverdi_SOURCE_DIR}/Code/Modules/ChangeDetection)
LIST(REMOVE_ITEM moduleList ${Monteverdi_SOURCE_DIR}/Code/Modules/Classification)

LIST(REMOVE_ITEM moduleList ${Monteverdi_SOURCE_DIR}/Code/Modules/ObjectLabeling)
LIST(REMOVE_ITEM moduleList ${Monteverdi_SOURCE_DIR}/Code/Modules/SupervisedClassification)
endif()

if (NOT OTBGKSVM_LOADED)
LIST(REMOVE_ITEM moduleList ${Monteverdi_SOURCE_DIR}/Code/Modules/ObjectCounting)
endif()

FOREACH(moduleDir ${moduleList})
  IF(IS_DIRECTORY ${moduleDir})
    FILE(RELATIVE_PATH moduleName ${Monteverdi_SOURCE_DIR}/Code/Modules ${moduleDir})
    SET(LIB_NAME "otb${moduleName}Module")

    SET(LINK_LIBS ${OTB_LIBRARIES} OTBVisuFLTK OTBVisuLegacyFLTK OTBGuiFLTK OTBMonteverdiCommon flu)
    SET(DEPS_LIBS OTBMonteverdiCommon)

    # Special cases with modules using FeatureExtractionBase
    IF(${LIB_NAME} MATCHES "otbRadiometricIndicesExtractionModule")
      SET(LINK_LIBS ${LINK_LIBS} OTBMonteverdiFeatureExtraction)
      SET(DEPS_LIBS OTBMonteverdiFeatureExtraction)
    ENDIF(${LIB_NAME} MATCHES "otbRadiometricIndicesExtractionModule")
    IF(${LIB_NAME} MATCHES "otbTextureExtractionModule")
      SET(LINK_LIBS ${LINK_LIBS} OTBMonteverdiFeatureExtraction)
      SET(DEPS_LIBS OTBMonteverdiFeatureExtraction)
    ENDIF(${LIB_NAME} MATCHES "otbTextureExtractionModule")
    IF(${LIB_NAME} MATCHES "otbMorphologyExtractionModule")
      SET(LINK_LIBS ${LINK_LIBS} OTBMonteverdiFeatureExtraction)
      SET(DEPS_LIBS OTBMonteverdiFeatureExtraction)
    ENDIF(${LIB_NAME} MATCHES "otbMorphologyExtractionModule")
    IF(${LIB_NAME} MATCHES "otbSmoothingModule")
      SET(LINK_LIBS ${LINK_LIBS} OTBMonteverdiFeatureExtraction)
      SET(DEPS_LIBS OTBMonteverdiFeatureExtraction)
    ENDIF(${LIB_NAME} MATCHES "otbSmoothingModule")
    IF(${LIB_NAME} MATCHES "otbEdgeExtractionModule")
      SET(LINK_LIBS ${LINK_LIBS} OTBMonteverdiFeatureExtraction)
      SET(DEPS_LIBS OTBMonteverdiFeatureExtraction)
    ENDIF(${LIB_NAME} MATCHES "otbEdgeExtractionModule")

    # If Module == Tile Map Import Module
    IF(${LIB_NAME} MATCHES "otbTileMapImportModule")
      # Remove modules using curl
      IF(OTBCurlAdapters_LOADED)
        FILE(GLOB SC_SRCS  ${moduleDir}/*.cxx )
        FILE(GLOB SC_GUI_SRCS_TEMP  ${moduleDir}/*.fl )
        SET(SC_GUI_SRCS "")
        IF(SC_GUI_SRCS_TEMP)
          FOREACH(file_gui_src ${SC_GUI_SRCS_TEMP})
            GET_FILENAME_COMPONENT(name "${file_gui_src}" NAME)
            SET (SC_GUI_SRCS
              ${SC_GUI_SRCS}
              "${moduleName}/${name}"
              )
          ENDFOREACH(file_gui_src)
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
        ENDIF(SC_GUI_SRCS_TEMP)

        ADD_LIBRARY(${LIB_NAME} ${SC_SRCS} )
        TARGET_LINK_LIBRARIES(${LIB_NAME} ${LINK_LIBS})
        IF(Monteverdi_LIBRARY_PROPERTIES)
          SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES ${Monteverdi_LIBRARY_PROPERTIES})
        ENDIF()
        ADD_DEPENDENCIES(${LIB_NAME} ${DEPS_LIBS})

        SET(LIST_MODULE_LIBRARIES "${LIST_MODULE_LIBRARIES};${LIB_NAME}" )

        IF(NOT OTB_INSTALL_NO_LIBRARIES)
          INSTALL(TARGETS ${LIB_NAME}
            RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
            LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
            ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT Development)
        ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)
      ENDIF()
    else()
      # If Module == Rasterization Module
      IF(${LIB_NAME} MATCHES "otbRasterizationModule")
        # Remove modules using mapnik
        IF(OTBMapnik_LOADED)
          FILE(GLOB SC_SRCS  ${moduleDir}/*.cxx )
          FILE(GLOB SC_GUI_SRCS_TEMP  ${moduleDir}/*.fl )
          SET(SC_GUI_SRCS "")
          IF(SC_GUI_SRCS_TEMP)
            FOREACH(file_gui_src ${SC_GUI_SRCS_TEMP})
              GET_FILENAME_COMPONENT(name "${file_gui_src}" NAME)
              SET (SC_GUI_SRCS
                ${SC_GUI_SRCS}
                "${moduleName}/${name}"
                )
            ENDFOREACH(file_gui_src)
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
        ENDIF(SC_GUI_SRCS_TEMP)

        ADD_LIBRARY(${LIB_NAME} ${SC_SRCS} )
        TARGET_LINK_LIBRARIES(${LIB_NAME} ${LINK_LIBS})
        IF(Monteverdi_LIBRARY_PROPERTIES)
          SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES ${Monteverdi_LIBRARY_PROPERTIES})
        ENDIF()
        ADD_DEPENDENCIES(${LIB_NAME} ${DEPS_LIBS})

        SET(LIST_MODULE_LIBRARIES "${LIST_MODULE_LIBRARIES};${LIB_NAME}" )

        IF(NOT OTB_INSTALL_NO_LIBRARIES)
          INSTALL(TARGETS ${LIB_NAME}
            RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
            LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
            ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT Development)
        ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)
      ENDIF()
    else() #${LIB_NAME} MATCHES "otbRasterizationModule"
        FILE(GLOB SC_SRCS  ${moduleDir}/*.cxx )
        FILE(GLOB SC_GUI_SRCS_TEMP  ${moduleDir}/*.fl )
        SET(SC_GUI_SRCS "")
        IF(SC_GUI_SRCS_TEMP)
          FOREACH(file_gui_src ${SC_GUI_SRCS_TEMP})
            GET_FILENAME_COMPONENT(name "${file_gui_src}" NAME)
            SET (SC_GUI_SRCS
              ${SC_GUI_SRCS}
              "${moduleName}/${name}"
              )
          ENDFOREACH(file_gui_src)
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
        ENDIF(SC_GUI_SRCS_TEMP)

        ADD_LIBRARY(${LIB_NAME} ${SC_SRCS} )
        TARGET_LINK_LIBRARIES(${LIB_NAME} ${LINK_LIBS} ${OTB_LIBRARIES})
        IF(Monteverdi_LIBRARY_PROPERTIES)
          SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES ${Monteverdi_LIBRARY_PROPERTIES})
        ENDIF()
        ADD_DEPENDENCIES(${LIB_NAME} ${DEPS_LIBS})

        # OTBGeospatialAnalysis is not generated if OTB_USE_PQXX
        # is set to OFF
        IF(OTB_USE_PQXX)
          TARGET_LINK_LIBRARIES(${LIB_NAME} ${OTB_LIBRARIES})
          IF(Monteverdi_LIBRARY_PROPERTIES)
            SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES ${Monteverdi_LIBRARY_PROPERTIES})
          ENDIF()
        ENDIF(OTB_USE_PQXX)

        SET(LIST_MODULE_LIBRARIES "${LIST_MODULE_LIBRARIES};${LIB_NAME}" )

        IF(NOT OTB_INSTALL_NO_LIBRARIES)
          INSTALL(TARGETS ${LIB_NAME}
            RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
            LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
            ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR} COMPONENT Development)
        ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)

      ENDIF(${LIB_NAME} MATCHES "otbRasterizationModule")
    ENDIF(${LIB_NAME} MATCHES "otbTileMapImportModule")

    # Create Executable
    #    OPTION(USE_${APPLI_NAME} "Build optional application Code/Module/${moduleName}." ON)
    #    IF(USE_${APPLI_NAME})
    #      ADD_EXECUTABLE(${APPLI_NAME}  ...... )
    #      TARGET_LINK_LIBRARIES(${APPLI_NAME} ${LIB_NAME})
    #      IF(NOT OTB_INSTALL_NO_LIBRARIES)
    #        INSTALL(TARGETS ${APPLI_NAME}
    #          RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
    #          LIBRARY DESTINATION ${Monteverdi_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
    #          ARCHIVE DESTINATION ${Monteverdi_INSTALL_BIN_DIR} COMPONENT Development)
    #      ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)
    #    ENDIF(USE_${APPLI_NAME})

  ENDIF(IS_DIRECTORY ${moduleDir})
ENDFOREACH(moduleDir)

# Set the value to the Cache
SET(LIST_MODULE_LIBRARIES ${LIST_MODULE_LIBRARIES} CACHE STRING "List of alls modules libraries" FORCE)
