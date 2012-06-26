SET(CPACK_NSIS_MODIFY_PATH ON PARENT_SCOPE)


FILE(TO_CMAKE_PATH "$ENV{OSGEO4W_ROOT}" OSGEO4W_ROOT)
IF(NOT OSGEO4W_ROOT)
  MESSAGE(FATAL_ERROR "Cannot generate installer without OSGeo4W environment : OSGEO4W_ROOT")
ENDIF(NOT OSGEO4W_ROOT)

FILE(TO_CMAKE_PATH "$ENV{GDAL_DATA}" GDAL_DATA)
IF(NOT GDAL_DATA)
  MESSAGE(FATAL_ERROR "Cannot generate installer without OSGeo4W environment : GDAL_DATA")
ENDIF(NOT GDAL_DATA)

# Gather all dlls from which Monteverdi depends, and put them aside the executable
# For this to work, the necessary DLL must be in PATH
INSTALL(SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/BundleFixing.cmake
        COMPONENT Runtime)

# For some reason, fixup_bundle won't package the msvc runtime dll
# I guess it's because cpack already handles installing the runtime,
# but here we build with a particular version of Visual, but
# some OSGeo4W dependencies are built upon other runtimes
INSTALL(FILES ${OSGEO4W_ROOT}/bin/msvcp60.dll
              ${OSGEO4W_ROOT}/bin/msvcp70.dll
              ${OSGEO4W_ROOT}/bin/msvcp71.dll
              ${OSGEO4W_ROOT}/bin/msvcr71.dll
              ${OSGEO4W_ROOT}/bin/msvcrt.dll
        DESTINATION bin
        COMPONENT Runtime)

# Need to include csv files provided with OSGeo that contains some needed EPSG definitions
FIND_PATH(GDAL_DATA epsg.wkt $ENV{GDAL_DATA})

INSTALL(DIRECTORY ${GDAL_DATA}
        DESTINATION share
        COMPONENT Resources)

INSTALL(FILES ${Monteverdi_SOURCE_DIR}/Packaging/Windows/monteverdi.bat
        DESTINATION bin
        COMPONENT Runtime)

# Override the monteverdi.exe
# with the monteverdi.bat shortcut
SET(CPACK_NSIS_MENU_LINKS
    "bin/monteverdi.bat" "Monteverdi application" PARENT_SCOPE)
