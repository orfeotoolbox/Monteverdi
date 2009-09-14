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

# Activate testing
ENABLE_TESTING()

# Set the Buildname for the dashboard
SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")

SUBDIRS(Code Testing Prototype1)