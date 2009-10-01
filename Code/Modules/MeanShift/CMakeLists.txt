# First, compile the module

SET(MODULE_SRCS
otbMeanShiftModule.cxx
otbMeanShiftModuleController.cxx
otbMeanShiftModuleModel.cxx
otbMeanShiftModuleView.cxx
)

ADD_LIBRARY(otbMeanShiftModule ${MODULE_SRCS})
TARGET_LINK_LIBRARIES(otbMeanShiftModule OTBMonteverdiCommon)
INSTALL_TARGETS(/bin otbMeanShiftModule)


# Then, compile the application

ADD_EXECUTABLE(otbMeanShiftApplication otbMeanShiftApplication.cxx)
TARGET_LINK_LIBRARIES(otbMeanShiftApplication otbMonteverdi otbMeanShiftModule)
INSTALL_TARGETS(/bin otbMeanShiftApplication)