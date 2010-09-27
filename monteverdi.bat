:: Monteverdi launcher to set up proper GDAL environment


:: Get the directory of the current script 
@set CURRENT_SCRIPT_DIR=%~dp0


:: Set GDAL_DATA env. variable
@set GDAL_DATA=%CURRENT_SCRIPT_DIR%..\share\gdal


:: Start Monteverdi
@"%CURRENT_SCRIPT_DIR%monteverdi.exe"
