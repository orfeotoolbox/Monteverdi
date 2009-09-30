# First, compile the module

SET(MODULE_SRCS
otbExampleModule.cxx
)

ADD_LIBRARY(otbExampleModule ${MODULE_SRCS})
TARGET_LINK_LIBRARIES(otbExampleModule OTBMonteverdiCommon)
INSTALL_TARGETS(/bin otbExampleModule)


# Then, compile the application

ADD_EXECUTABLE(otbExampleApplication otbExampleApplication.cxx)
TARGET_LINK_LIBRARIES(otbExampleApplication otbMonteverdi otbExampleModule)
INSTALL_TARGETS(/bin otbExampleApplication)