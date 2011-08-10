SET(CPACK_NSIS_MODIFY_PATH ON PARENT_SCOPE)

# Install DLL needed by Monteverdi
# This part is "OSGeo specific" because it makes assumption about the naming of the DLLs,
# and the fact that they can all be found in the GDAL bin directory
# It supposes that GDAL version is 1.8

# Find path to GDAL DLL
FIND_PATH(CPACK_OTB_DLL_PATH gdal18.dll)
MARK_AS_ADVANCED(CPACK_OTB_DLL_PATH)
MESSAGE("Found OSGeo DLL path : ${CPACK_OTB_DLL_PATH}")

INSTALL(FILES
${CPACK_OTB_DLL_PATH}/adrg.dll
${CPACK_OTB_DLL_PATH}/comerr32.dll
${CPACK_OTB_DLL_PATH}/dted.dll
${CPACK_OTB_DLL_PATH}/gdal18.dll
${CPACK_OTB_DLL_PATH}/geos_c.dll
${CPACK_OTB_DLL_PATH}/geotiff.dll
${CPACK_OTB_DLL_PATH}/gssapi32.dll
${CPACK_OTB_DLL_PATH}/hdf5dll.dll
${CPACK_OTB_DLL_PATH}/hdf_fw.dll
${CPACK_OTB_DLL_PATH}/jpeg12_osgeo.dll
${CPACK_OTB_DLL_PATH}/jpeg_osgeo.dll
${CPACK_OTB_DLL_PATH}/k5sprt32.dll
${CPACK_OTB_DLL_PATH}/krb5_32.dll
${CPACK_OTB_DLL_PATH}/libcurl.dll
${CPACK_OTB_DLL_PATH}/libeay32.dll
${CPACK_OTB_DLL_PATH}/libexpat.dll
${CPACK_OTB_DLL_PATH}/libintl-8.dll
${CPACK_OTB_DLL_PATH}/libiconv-2.dll
${CPACK_OTB_DLL_PATH}/libmysql.dll
${CPACK_OTB_DLL_PATH}/libpng-3.dll
${CPACK_OTB_DLL_PATH}/libpng12-0.dll
${CPACK_OTB_DLL_PATH}/libpq.dll
${CPACK_OTB_DLL_PATH}/libtiff.dll
${CPACK_OTB_DLL_PATH}/mfhdf_fw.dll
${CPACK_OTB_DLL_PATH}/msvcp60.dll
${CPACK_OTB_DLL_PATH}/msvcp70.dll
${CPACK_OTB_DLL_PATH}/msvcp71.dll
${CPACK_OTB_DLL_PATH}/msvcr71.dll
${CPACK_OTB_DLL_PATH}/msvcrt.dll
${CPACK_OTB_DLL_PATH}/netcdf.dll
${CPACK_OTB_DLL_PATH}/ogdi_32b1.dll
${CPACK_OTB_DLL_PATH}/png_osgeo.dll
${CPACK_OTB_DLL_PATH}/proj.dll
${CPACK_OTB_DLL_PATH}/python25.dll
${CPACK_OTB_DLL_PATH}/remote.dll
${CPACK_OTB_DLL_PATH}/rpf.dll
${CPACK_OTB_DLL_PATH}/skeleton.dll
${CPACK_OTB_DLL_PATH}/sqlite3.dll
${CPACK_OTB_DLL_PATH}/ssleay32.dll
${CPACK_OTB_DLL_PATH}/szlibdll.dll
${CPACK_OTB_DLL_PATH}/vrf.dll
${CPACK_OTB_DLL_PATH}/xerces-c_3_1.dll
${CPACK_OTB_DLL_PATH}/xerces-depdom_2_7.dll
${CPACK_OTB_DLL_PATH}/zlib1.dll
${CPACK_OTB_DLL_PATH}/zlib_osgeo.dll
${CPACK_OTB_DLL_PATH}/mapnik.dll
${CPACK_OTB_DLL_PATH}/icuuc42.dll
${CPACK_OTB_DLL_PATH}/icudt42.dll
${CPACK_OTB_DLL_PATH}/icuin42.dll
${CPACK_OTB_DLL_PATH}/libxml2.dll
${CPACK_OTB_DLL_PATH}/iconv.dll
DESTINATION bin
COMPONENT Runtime)


# Need to include csv files provided with OSGeo that contains some needed EPSG definitions
# For GDAL 1.6, those files are located in OSGEO_ROOT/apps/gdal-16/share/gdal
# This path is available from the GDAL_DATA env var (from an OSGeo shell with gdal16.bat executed)
# We use a FIND_PATH because typically $ENV{GDAL_DATA} contains backslashes which issue a warning
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
