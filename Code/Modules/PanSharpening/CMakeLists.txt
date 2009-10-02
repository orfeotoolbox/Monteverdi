# First, compile the module

SET(MODULE_SRCS
otbPanSharpeningModule.cxx
)

ADD_LIBRARY(otbPanSharpeningModule ${MODULE_SRCS})
TARGET_LINK_LIBRARIES(otbPanSharpeningModule OTBMonteverdiCommon)
INSTALL(TARGETS otbPanSharpeningModule
        RUNTIME DESTINATION /bin
        LIBRARY DESTINATION /bin
        ARCHIVE DESTINATION /bin )