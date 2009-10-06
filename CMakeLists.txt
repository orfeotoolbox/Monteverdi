PROJECT(Monteverdi)

cmake_minimum_required(VERSION 2.6)

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# OTB option to access large file (> 4Go).
ADD_DEFINITIONS(
        -D_LARGEFILE_SOURCE
        -D_FILE_OFFSET_BITS=64
        -D_LARGEFILE64_SOURCE
        -D_LARGE_FILES
        )

#Prevents conflicts with visual min/max function
#Add the OSSIMSTATIC flag to prevent from including dclspec(dllimport) from ossim
IF(WIN32)
ADD_DEFINITIONS(-DNOMINMAX -DOSSIM_STATIC)
ENDIF(WIN32)

# ensure WIN32 definition
IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

# Link to the Orfeo ToolBox
FIND_PACKAGE(OTB)
IF(OTB_FOUND)
  INCLUDE(${OTB_USE_FILE})
ELSE(OTB_FOUND)
  MESSAGE(FATAL_ERROR
          "OTB not found. Please set OTB_DIR")
ENDIF(OTB_FOUND)

#-----------------------------------------------------------------------------
# Configure files with settings for use by the build.
CONFIGURE_FILE(${Monteverdi_SOURCE_DIR}/ConfigureMonteverdi.h.in
               ${Monteverdi_BINARY_DIR}/ConfigureMonteverdi.h)


#-----------------------------------------------------------------------------
# Configure the default OTB_DATA_ROOT for the location of OTB Data.
FIND_PATH(OTB_DATA_ROOT OTBData.readme $ENV{OTB_DATA_ROOT})
MARK_AS_ADVANCED(OTB_DATA_ROOT)

OPTION(OTB_DATA_USE_LARGEINPUT "Use Large inputs images test." OFF)
MARK_AS_ADVANCED(OTB_DATA_USE_LARGEINPUT)
IF(OTB_DATA_USE_LARGEINPUT)
  FIND_PATH(OTB_DATA_LARGEINPUT_ROOT OTBData.readme $ENV{OTB_DATA_LARGEINPUT_ROOT})
  MARK_AS_ADVANCED(OTB_DATA_LARGEINPUT_ROOT)
ENDIF(OTB_DATA_USE_LARGEINPUT)

# Activate testing
ENABLE_TESTING()

# Set the Buildname for the dashboard
SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")

# Include directories
INCLUDE_DIRECTORIES(
${Monteverdi_BINARY_DIR}/
${Monteverdi_SOURCE_DIR}/Code/Common
${Monteverdi_BINARY_DIR}/Code/Common
${Monteverdi_SOURCE_DIR}/Utilities/FLU/
${Monteverdi_SOURCE_DIR}/Utilities/FLU/FLU
${Monteverdi_SOURCE_DIR}/Utilities/FLU/src
${Monteverdi_SOURCE_DIR}/Code/Application
${Monteverdi_BINARY_DIR}/Code/Application
)

MESSAGE(STATUS "Automatic includes modules:")
FILE(GLOB moduleList ${Monteverdi_SOURCE_DIR}/Code/Modules/*/)
FOREACH(moduleDir ${moduleList})
  IF(IS_DIRECTORY ${moduleDir})
    FILE(RELATIVE_PATH moduleName ${Monteverdi_SOURCE_DIR}/Code/Modules ${moduleDir})  
    MESSAGE(STATUS  - ${moduleName})
    INCLUDE_DIRECTORIES(${Monteverdi_SOURCE_DIR}/Code/Modules/${moduleName})
    INCLUDE_DIRECTORIES(${Monteverdi_BINARY_DIR}/Code/Modules/${moduleName})
  ENDIF(IS_DIRECTORY ${moduleDir})
ENDFOREACH(moduleDir)


# Code location
ADD_SUBDIRECTORY(Code)
ADD_SUBDIRECTORY(Prototype1)
ADD_SUBDIRECTORY(Utilities)
ADD_SUBDIRECTORY(StarterKit)

#-----------------------------------------------------------------------------
# Output directories.
SET (LIBRARY_OUTPUT_PATH ${Monteverdi_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${Monteverdi_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

# Enable / Disable testing
INCLUDE(${CMAKE_ROOT}/Modules/Dart.cmake)
MARK_AS_ADVANCED(TCL_TCLSH DART_ROOT)
IF(BUILD_TESTING)
  ENABLE_TESTING()
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)


