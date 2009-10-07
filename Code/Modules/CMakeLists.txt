
FILE(GLOB moduleList ${Monteverdi_SOURCE_DIR}/Code/Modules/*)
FOREACH(moduleDir ${moduleList})
  IF(IS_DIRECTORY ${moduleDir})
    FILE(RELATIVE_PATH moduleName ${Monteverdi_SOURCE_DIR}/Code/Modules ${moduleDir})
    ADD_SUBDIRECTORY(${moduleName})
  ENDIF(IS_DIRECTORY ${moduleDir})
ENDFOREACH(moduleDir)
