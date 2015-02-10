SET(Common_TESTS ${CXX_TEST_PATH}/otbCommonTests)

ADD_TEST(coTuDataObjectWrapperTest  ${Common_TESTS} otbDataObjectWrapperTest)
ADD_TEST(coTuDataDescriptorTest ${Common_TESTS} otbDataDescriptorTest)
ADD_TEST(coTuInputDataDescriptorTest ${Common_TESTS}  otbInputDataDescriptorTest)
ADD_TEST(coTuOutputDataDescriptorTest  ${Common_TESTS}  otbOutputDataDescriptorTest)
ADD_TEST(coTuTypeManagerTest ${Common_TESTS} otbTypeManagerTest)
ADD_TEST(coTuModuleTest ${Common_TESTS} otbModuleTest)

SET(Common_SRCS
otbDataObjectWrapperTest.cxx
otbDataDescriptorTest.cxx
otbInputDataDescriptorTest.cxx
otbOutputDataDescriptorTest.cxx
otbTypeManagerTest.cxx
otbModuleTest.cxx
)

ADD_EXECUTABLE(otbCommonTests otbCommonTests.cxx ${Common_SRCS})
TARGET_LINK_LIBRARIES(otbCommonTests OTBMonteverdiCommon ${OTB_LIBRARIES})
