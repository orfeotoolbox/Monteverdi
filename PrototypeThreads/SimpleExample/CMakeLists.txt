
SET(MY_SRCS
otbAsynchronousProcessBase.cxx
otbAsynchronousProcess.cxx
otbAsynchronousModel.cxx
)


ADD_EXECUTABLE(otbSimpleExample otbSimpleExampleApplication.cxx ${MY_SRCS})
TARGET_LINK_LIBRARIES(otbSimpleExample OTBCommon OTBIO)
INSTALL_TARGETS(/bin otbSimpleExample)

ADD_EXECUTABLE(otbCrazySimpleExample otbCrazySimpleExampleApplication.cxx ${MY_SRCS})
TARGET_LINK_LIBRARIES(otbCrazySimpleExample OTBCommon OTBIO)
INSTALL_TARGETS(/bin otbCrazySimpleExample)
