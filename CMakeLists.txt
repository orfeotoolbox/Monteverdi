PROJECT(Monteverdi)

cmake_minimum_required(VERSION 2.6)

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# OTB option to access large files (> 4Go).
ADD_DEFINITIONS(
        -D_LARGEFILE_SOURCE
        -D_FILE_OFFSET_BITS=64
        -D_LARGEFILE64_SOURCE
        -D_LARGE_FILES
        )
#-----------------------------------------------------------------------------
# Montevrdi version number.  An even minor number corresponds to releases.
SET(Monteverdi_VERSION_MAJOR "1")
SET(Monteverdi_VERSION_MINOR "22")
SET(Monteverdi_VERSION_PATCH "0")

# Version string should not include patch level.  The major.minor is
# enough to distinguish available features of the toolkit.
SET(Monteverdi_VERSION_STRING "${Monteverdi_VERSION_MAJOR}.${Monteverdi_VERSION_MINOR}.${Monteverdi_VERSION_PATCH}")

SET(Monteverdi_LIBRARY_PROPERTIES
    VERSION   "${Monteverdi_VERSION_MAJOR}.${Monteverdi_VERSION_MINOR}.${Monteverdi_VERSION_PATCH}"
    SOVERSION "${Monteverdi_VERSION_MAJOR}.${Monteverdi_VERSION_MINOR}")


SET(Monteverdi_FLOATING_TYPE "float" CACHE STRING "The type of data in which input images are read (double/float)" )

#Prevents conflicts with visual min/max function
#Add the OSSIMSTATIC flag to prevent from including dclspec(dllimport) from ossim
if(WIN32)
  if(NOT BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DNOMINMAX -DOSSIM_STATIC)
  endif()
endif()

if(MSVC)
    if (${CMAKE_VERSION} VERSION_GREATER "2.8.10.2")
        if("${CMAKE_EXE_LINKER_FLAGS}" MATCHES "/STACK:[0-9]+")
            message(STATUS "The size of the stack is already defined, so we don't modify it.")
        else()
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:10000000" CACHE STRING "Flags used by the linker" FORCE)
            message(STATUS "Increase the size of the stack to 10Mbytes.")
        endif()
    endif()
endif()

# ensure WIN32 definition
IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)
# On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security
# warnings
IF(WIN32)
    IF(NOT CYGWIN)
      IF(NOT MINGW)
        IF(NOT ITK_ENABLE_VISUAL_STUDIO_DEPRECATED_C_WARNINGS)
          ADD_DEFINITIONS(
            -D_CRT_FAR_MAPPINGS_NO_DEPRECATE
            -D_CRT_IS_WCTYPE_NO_DEPRECATE
            -D_CRT_MANAGED_FP_NO_DEPRECATE
            -D_CRT_NONSTDC_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE_GLOBALS
            -D_CRT_SETERRORMODE_BEEP_SLEEP_NO_DEPRECATE
            -D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
            -D_CRT_VCCLRIT_NO_DEPRECATE
            -D_SCL_SECURE_NO_DEPRECATE
            )
        ENDIF(NOT ITK_ENABLE_VISUAL_STUDIO_DEPRECATED_C_WARNINGS)

        IF (MSVC)
          ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
          ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_WARNING)
        ENDIF(MSVC)

      ENDIF(NOT MINGW)
    ENDIF(NOT CYGWIN)
ENDIF(WIN32)

# Include the patched boost file directory
# Make it the first one in the list of includes
INCLUDE_DIRECTORIES(${Monteverdi_SOURCE_DIR}/Utilities)

# Link to the Orfeo ToolBox
FIND_PACKAGE(OTB PATHS /usr/lib/otb )
IF(OTB_FOUND)
  INCLUDE(${OTB_USE_FILE})
ELSE(OTB_FOUND)
  MESSAGE(FATAL_ERROR
          "OTB not found. Please set OTB_DIR")
ENDIF(OTB_FOUND)

IF(OTB_VERSION_MAJOR LESS "4")
  MESSAGE(FATAL_ERROR
          "Monteverdi is only compatible with OTB version greater than 4.0.0")
ENDIF()

SET(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/CMake
    ${CMAKE_MODULE_PATH}
    "${OTB_SOURCE_DIR}/CMake" )

# Import FLTK
include(ImportFLTK)


FIND_PACKAGE(Mercurial QUIET)
IF(Mercurial_FOUND)
  Mercurial_WC_IDENTIFY(${PROJECT_SOURCE_DIR} Monteverdi)
  MESSAGE(STATUS "Mercurial version is ${Mercurial_VERSION_HG}")
  MESSAGE(STATUS "Repository revision is ${Monteverdi_WC_REVISION}")
  SET(Monteverdi_WC_REVISION ${Monteverdi_WC_REVISION} CACHE STRING "Repository version" FORCE)
  MARK_AS_ADVANCED(Monteverdi_WC_REVISION)
  IF(Monteverdi_WC_STATUS)
    MESSAGE(STATUS "Local file modifications:")
    MESSAGE(STATUS ${Monteverdi_WC_STATUS})
  ELSE(Monteverdi_WC_STATUS)
    MESSAGE(STATUS "No files modified locally")
  ENDIF(Monteverdi_WC_STATUS)
  STRING(REGEX REPLACE "\n" ";" Monteverdi_WC_STATUS "${Monteverdi_WC_STATUS}")
  SET(Monteverdi_WC_STATUS ${Monteverdi_WC_STATUS} CACHE STRING "Repository status" FORCE)
  MARK_AS_ADVANCED(Monteverdi_WC_STATUS)
ENDIF(Mercurial_FOUND)


#-----------------------------------------------------------------------------
# Configure files with settings for use by the build.
CONFIGURE_FILE(${Monteverdi_SOURCE_DIR}/ConfigureMonteverdi.h.in
               ${Monteverdi_BINARY_DIR}/ConfigureMonteverdi.h)


#-----------------------------------------------------------------------------
#Build shared libraries for monteverdi
#By default, import the option from the OTB configuration
SET(BUILD_SHARED_LIBS ${OTB_BUILD_SHARED_LIBS})


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

# Set the Buildname for the dashboard
SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")

# Include directories
SET(Monteverdi_INCLUDE_DIRS
${Monteverdi_BINARY_DIR}/
${Monteverdi_SOURCE_DIR}/Code/Common
${Monteverdi_BINARY_DIR}/Code/Common
${Monteverdi_SOURCE_DIR}/Code/Common/FeatureExtractionBase
${Monteverdi_BINARY_DIR}/Code/Common/FeatureExtractionBase
${Monteverdi_SOURCE_DIR}/Utilities/FLU/
${Monteverdi_SOURCE_DIR}/Utilities/FLU/FLU
${Monteverdi_SOURCE_DIR}/Utilities/FLU/src
${Monteverdi_SOURCE_DIR}/Code/Application
${Monteverdi_BINARY_DIR}/Code/Application
${Monteverdi_SOURCE_DIR}/Code/VisuFLTK
${Monteverdi_BINARY_DIR}/Code/VisuFLTK
${Monteverdi_SOURCE_DIR}/Code/GuiFLTK
${Monteverdi_SOURCE_DIR}/Code/VisuLegacyFLTK
${OTB_INCLUDE_DIRS}
)

#-----------------------------------------------------------------------------
# Installation vars.
# Monteverdi_INSTALL_BIN_DIR          - binary dir (executables)
# Monteverdi_INSTALL_LIB_DIR          - library dir (libs)
# Monteverdi_INSTALL_DATA_DIR         - share dir (say, examples, data, etc)
# Monteverdi_INSTALL_INCLUDE_DIR      - include dir (headers)
# Monteverdi_INSTALL_PACKAGE_DIR      - package/export configuration files
# Monteverdi_INSTALL_NO_DEVELOPMENT   - do not install development files
# Monteverdi_INSTALL_NO_RUNTIME       - do not install runtime files
# Monteverdi_INSTALL_NO_DOCUMENTATION - do not install documentation files

IF(NOT Monteverdi_INSTALL_BIN_DIR)
  SET(Monteverdi_INSTALL_BIN_DIR "bin")
ENDIF(NOT Monteverdi_INSTALL_BIN_DIR)

IF(NOT Monteverdi_INSTALL_LIB_DIR)
  SET(Monteverdi_INSTALL_LIB_DIR "lib/otb")
ENDIF(NOT Monteverdi_INSTALL_LIB_DIR)

IF(NOT Monteverdi_INSTALL_DATA_DIR)
  SET(Monteverdi_INSTALL_DATA_DIR "share/otb")
ENDIF(NOT Monteverdi_INSTALL_DATA_DIR)

IF(NOT Monteverdi_INSTALL_INCLUDE_DIR)
  SET(Monteverdi_INSTALL_INCLUDE_DIR "include/otb")
ENDIF(NOT Monteverdi_INSTALL_INCLUDE_DIR)

IF(NOT Monteverdi_INSTALL_CMAKE_DIR)
  SET(Monteverdi_INSTALL_CMAKE_DIR ${Monteverdi_INSTALL_LIB_DIR}
    CACHE INTERNAL "")
ENDIF(NOT Monteverdi_INSTALL_CMAKE_DIR)

IF(NOT Monteverdi_INSTALL_NO_DEVELOPMENT)
  SET(Monteverdi_INSTALL_NO_DEVELOPMENT 0)
ENDIF(NOT Monteverdi_INSTALL_NO_DEVELOPMENT)

IF(NOT Monteverdi_INSTALL_NO_RUNTIME)
  SET(Monteverdi_INSTALL_NO_RUNTIME 0)
ENDIF(NOT Monteverdi_INSTALL_NO_RUNTIME)

IF(NOT Monteverdi_INSTALL_NO_DOCUMENTATION)
  SET(Monteverdi_INSTALL_NO_DOCUMENTATION 0)
ENDIF(NOT Monteverdi_INSTALL_NO_DOCUMENTATION)

SET(Monteverdi_INSTALL_NO_LIBRARIES)
IF(Monteverdi_BUILD_SHARED_LIBS)
  IF(Monteverdi_INSTALL_NO_RUNTIME AND Monteverdi_INSTALL_NO_DEVELOPMENT)
    SET(Monteverdi_INSTALL_NO_LIBRARIES 1)
  ENDIF(Monteverdi_INSTALL_NO_RUNTIME AND Monteverdi_INSTALL_NO_DEVELOPMENT)
ELSE(Monteverdi_BUILD_SHARED_LIBS)
  IF(Monteverdi_INSTALL_NO_DEVELOPMENT)
    SET(Monteverdi_INSTALL_NO_LIBRARIES 1)
  ENDIF(Monteverdi_INSTALL_NO_DEVELOPMENT)
ENDIF(Monteverdi_BUILD_SHARED_LIBS)

# Make sure the user does not try to install on top of the build tree.
IF(WIN32 OR APPLE OR UNIX)
  STRING(TOLOWER "${CMAKE_INSTALL_PREFIX}" _PREFIX)
  STRING(TOLOWER "${Monteverdi_BINARY_DIR}" _BUILD)
ELSE(WIN32 OR APPLE OR UNIX)
  SET(_PREFIX "${CMAKE_INSTALL_PREFIX}")
  SET(_BUILD "${Monteverdi_BINARY_DIR}")
ENDIF(WIN32 OR APPLE OR UNIX)
IF("${_PREFIX}" STREQUAL "${_BUILD}")
  MESSAGE(FATAL_ERROR
    "The current CMAKE_INSTALL_PREFIX points at the build tree:\n"
    "  ${CMAKE_INSTALL_PREFIX}\n"
    "This is not supported."
    )
ENDIF("${_PREFIX}" STREQUAL "${_BUILD}")




MESSAGE(STATUS "Automatic includes modules:")
FILE(GLOB moduleList ${Monteverdi_SOURCE_DIR}/Code/Modules/*/)
FOREACH(moduleDir ${moduleList})
  IF(IS_DIRECTORY ${moduleDir})
    FILE(RELATIVE_PATH moduleName ${Monteverdi_SOURCE_DIR}/Code/Modules ${moduleDir})
    MESSAGE(STATUS  - ${moduleName})
    SET(Monteverdi_INCLUDE_DIRS ${Monteverdi_INCLUDE_DIRS} ${Monteverdi_SOURCE_DIR}/Code/Modules/${moduleName})
    SET(Monteverdi_INCLUDE_DIRS ${Monteverdi_INCLUDE_DIRS} ${Monteverdi_BINARY_DIR}/Code/Modules/${moduleName})
  ENDIF(IS_DIRECTORY ${moduleDir})
ENDFOREACH(moduleDir)

SET(Monteverdi_INCLUDE_DIRS ${Monteverdi_INCLUDE_DIRS} ${Monteverdi_BINARY_DIR}/Code/Modules)
SET(Monteverdi_INCLUDE_DIRS ${Monteverdi_INCLUDE_DIRS} ${Monteverdi_SOURCE_DIR}/Code/Application)
SET(Monteverdi_INCLUDE_DIRS ${Monteverdi_INCLUDE_DIRS} ${Monteverdi_BINARY_DIR}/Code/Application)

SET(LIST_MODULE_LIBRARIES "" CACHE STRING "List of alls modules libraries" FORCE)
MARK_AS_ADVANCED(LIST_MODULE_LIBRARIES)

INCLUDE_DIRECTORIES(
${Monteverdi_INCLUDE_DIRS}
)

# Code location
ADD_SUBDIRECTORY(Code)
ADD_SUBDIRECTORY(Utilities)
# For an unknow reason this part failed to build on windows
#ADD_SUBDIRECTORY(StarterKit)

# Include the Packaging subdir after every other dir
# since all other install directives must be executed before fixupbundle
ADD_SUBDIRECTORY(Packaging)

#-----------------------------------------------------------------------------
# Output directories.
SET (LIBRARY_OUTPUT_PATH ${Monteverdi_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${Monteverdi_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

# Enable / Disable testing
OPTION(BUILD_TESTING  "Build testing." OFF)

INCLUDE(${CMAKE_ROOT}/Modules/Dart.cmake)
CONFIGURE_FILE(CMake/CTestCustom.cmake.in CTestCustom.cmake)
MARK_AS_ADVANCED(TCL_TCLSH DART_ROOT)
IF(BUILD_TESTING)
  ENABLE_TESTING()
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)

#-----------------------------------------------------------------------------
# Generate MonteverdiConfig.cmake



export(TARGETS OTBGuiFLTK OTBVisuFLTK
       FILE "${PROJECT_BINARY_DIR}/MonteverdiTargets.cmake")
export(PACKAGE Monteverdi)

file(RELATIVE_PATH REL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/${Monteverdi_INSTALL_CMAKE_DIR}"
   "${CMAKE_INSTALL_PREFIX}/${Monteverdi_INSTALL_INCLUDE_DIR}")


# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/Code/GuiFLTK" "${PROJECT_BINARY_DIR}/Code/GuiFLTK"
"${PROJECT_SOURCE_DIR}/Code/VisuFLTK" "${PROJECT_BINARY_DIR}/Code/VisuFLTK"
)

configure_file(MonteverdiConfig.cmake.in
  "${PROJECT_BINARY_DIR}/MonteverdiConfig.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${Monteverdi_CMAKE_DIR}/${REL_INCLUDE_DIR}/GuiFLTK" "\${Monteverdi_CMAKE_DIR}/${REL_INCLUDE_DIR}/VisuFLTK")
configure_file(MonteverdiConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MonteverdiConfig.cmake" @ONLY)
# ... for both
configure_file(MonteverdiConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/MonteverdiConfigVersion.cmake" @ONLY)

# Install the MonteverdiConfig.cmake and MonteverdiConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MonteverdiConfig.cmake"
  "${PROJECT_BINARY_DIR}/MonteverdiConfigVersion.cmake"
  DESTINATION "${Monteverdi_INSTALL_CMAKE_DIR}" COMPONENT Development)

# Install the export set for use with the install-tree
install(EXPORT MonteverdiTargets DESTINATION
  "${Monteverdi_INSTALL_CMAKE_DIR}" COMPONENT Development)






# Save library dependencies.
#SET(Monteverdi_LIBRARY_DEPENDS_FILE ${Monteverdi_BINARY_DIR}/MonteverdiLibraryDepends.cmake)
#EXPORT_LIBRARY_DEPENDENCIES(${Monteverdi_LIBRARY_DEPENDS_FILE})

# Library directory.
#SET(Monteverdi_LIBRARY_DIRS ${OTB_LIBRARY_DIRS} ${LIBRARY_OUTPUT_PATH})

#SET(Monteverdi_USE_FILE ${Monteverdi_BINARY_DIR}/UseMonteverdi.cmake)

#CONFIGURE_FILE(${Monteverdi_SOURCE_DIR}/MonteverdiConfig.cmake.in
#               ${Monteverdi_BINARY_DIR}/MonteverdiConfig.cmake @ONLY IMMEDIATE)
#CONFIGURE_FILE(${Monteverdi_SOURCE_DIR}/UseMonteverdi.cmake.in
#               ${Monteverdi_USE_FILE} @ONLY IMMEDIATE)

#install(FILES ${Monteverdi_BINARY_DIR}/MonteverdiConfig.cmake ${Monteverdi_USE_FILE}
#        DESTINATION ${Monteverdi_INSTALL_PACKAGE_DIR}
#        COMPONENT Development)

#-----------------------------------------------------------------------------
# Setup valgrind option (default hard coded value is ctest is invalid)
SET(VALGRIND_COMMAND_OPTIONS  "--show-reachable=yes --workaround-gcc296-bugs=yes --num-callers=50")
