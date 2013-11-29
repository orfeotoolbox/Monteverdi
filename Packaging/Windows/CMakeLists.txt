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

# The following DLL handling is very specific
# both to OSGeo4W flavour (32 bit or 64 bit), OSGeo4W package state
# and the compiler used to make the Monteverdi2 installer (VC2010)
# Don't expect too much of it without VC2010.

# For some reason, fixup_bundle won't package the msvc runtime dll
# I guess it's because cpack already handles installing the runtime,
# but here we build with a particular version of Visual, but
# some OSGeo4W dependencies are built upon other runtimes
set(MVD2_ADDITIONNAL_DLLS
        ${OSGEO4W_ROOT}/bin/msvcp60.dll
        ${OSGEO4W_ROOT}/bin/msvcp70.dll
        ${OSGEO4W_ROOT}/bin/msvcp71.dll
        ${OSGEO4W_ROOT}/bin/msvcr71.dll
        ${OSGEO4W_ROOT}/bin/msvcrt.dll)
        
# Since an upgrade in OSGeo4W-32bit of libpq (around 2013/10)
# libpq depends on two additional libs that cmake
# surprisingly miss during package generation
# I really don't understand why.
# Let's add them manually
set(MVD2_ADDITIONNAL_DLLS
        ${MVD2_ADDITIONNAL_DLLS}
        ${OSGEO4W_ROOT}/bin/libgcc_s_dw2-1.dll
        ${OSGEO4W_ROOT}/bin/libiconv-2.dll)

foreach(dll ${MVD2_ADDITIONNAL_DLLS})
  if (EXISTS ${dll})
     install( FILES ${dll}
              DESTINATION bin
              COMPONENT Runtime )
  endif()
endforeach()

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
