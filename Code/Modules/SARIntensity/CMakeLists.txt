# First, compile the module

SET(MODULE_SRCS
otbSarIntensityModule.cxx
)

ADD_LIBRARY(otbSarIntensityModule ${MODULE_SRCS})
TARGET_LINK_LIBRARIES(otbSarIntensityModule OTBMonteverdiCommon)
ADD_DEPENDENCIES(otbSarIntensityModule OTBMonteverdiCommon)

INSTALL(TARGETS otbSarIntensityModule
        RUNTIME DESTINATION /bin
        LIBRARY DESTINATION /bin
        ARCHIVE DESTINATION /bin )