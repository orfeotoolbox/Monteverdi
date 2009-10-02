
SET(MY_SRCS
otbAsynchronousProcessBase.cxx
otbAsynchronousModel.cxx
otbAsynchronousProcess.cxx
)


ADD_EXECUTABLE(otbSimpleExample otbSimpleExampleApplication.cxx ${MY_SRCS})
TARGET_LINK_LIBRARIES(otbSimpleExample OTBCommon OTBIO)
INSTALL(TARGETS /bin otbSimpleExample)

ADD_EXECUTABLE(otbCrazySimpleExample otbCrazySimpleExampleApplication.cxx ${MY_SRCS})
TARGET_LINK_LIBRARIES(otbCrazySimpleExample OTBCommon OTBIO)
INSTALL(TARGETS /bin otbCrazySimpleExample)
