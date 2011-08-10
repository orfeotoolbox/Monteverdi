SET(PIXMAPS_DIR ${CMAKE_INSTALL_PREFIX}/share/pixmaps)
SET(DESKTOP_FILES_DIR ${CMAKE_INSTALL_PREFIX}/share/applications)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/Data/Icons/monteverdi.xpm ${CMAKE_SOURCE_DIR}/Data/Icons/monteverdi.png
        DESTINATION ${PIXMAPS_DIR}
        COMPONENT Resources)
        
INSTALL(FILES ${CMAKE_SOURCE_DIR}/Data/monteverdi.desktop
        DESTINATION ${DESKTOP_FILES_DIR}
        COMPONENT Resources)
