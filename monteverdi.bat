:::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Monteverdi launcher to set up proper GDAL environment
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Get the directory of the current script 
@set CURRENT_SCRIPT_DIR=%~dp0

:: Set GDAL_DATA env. variable
@set GDAL_DATA=%CURRENT_SCRIPT_DIR%..\share\gdal

:: Set current dir to TEMP dir because Monteverdi generates temporary files and need write access
@cd %TEMP%

:: Start Monteverdi
@"%CURRENT_SCRIPT_DIR%monteverdi.exe"
