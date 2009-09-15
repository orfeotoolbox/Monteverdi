PROJECT(Monteverdi)

cmake_minimum_required(VERSION 2.6)

# Link to the Orfeo ToolBox
FIND_PACKAGE(OTB)
IF(OTB_FOUND)
  INCLUDE(${OTB_USE_FILE})
ELSE(OTB_FOUND)
  MESSAGE(FATAL_ERROR
          "OTB not found. Please set OTB_DIR")
ENDIF(OTB_FOUND)

# ensure WIN32 definition
IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

# Include directories
INCLUDE_DIRECTORIES(
${Monteverdi_SOURCE_DIR}/Code/Common
)

# Code location
SUBDIRS(Code Prototype1)

# Enable / Disable testing
INCLUDE(${CMAKE_ROOT}/Modules/Dart.cmake)
MARK_AS_ADVANCED(TCL_TCLSH DART_ROOT)
IF(BUILD_TESTING)
  ENABLE_TESTING()
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)


