FILE(GLOB OTBMonteverdiCommon_SRCS "*.cxx" )

ADD_LIBRARY(OTBMonteverdiCommon ${OTBMonteverdiCommon_SRCS})
TARGET_LINK_LIBRARIES(OTBMonteverdiCommon ITKCommon)

