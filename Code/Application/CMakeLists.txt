IF(WIN32)
  ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${Monteverdi_BINARY_DIR})

ADD_EXECUTABLE(Monteverdi  Monteverdi.cxx otbMonteverdiModel.cxx)
TARGET_LINK_LIBRARIES(Monteverdi OTBGui OTBMonteverdiCommon otbReaderModule otbSpeckleFilteringModule)
INSTALL_TARGETS(/bin Monteverdi)

ADD_EXECUTABLE(SplashScreen SplashScreen.cxx)
TARGET_LINK_LIBRARIES(SplashScreen OTBIO OTBVisualization flu)
INSTALL_TARGETS(/bin SplashScreen)



# ADD_EXECUTABLE(Flu_Tree_Browser Flu_Tree_Browser.cpp )
# TARGET_LINK_LIBRARIES(Flu_Tree_Browser OTBIO OTBVisualization ${FLU_DIR})
# INSTALL_TARGETS(/bin Flu_Tree_Browser)