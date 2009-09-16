SET(BASELINE ${OTB_DATA_ROOT}/Baseline/OTB/Images)
SET(BASELINE_FILES ${OTB_DATA_ROOT}/Baseline/OTB/Files)
SET(INPUTDATA ${OTB_DATA_ROOT}/Input)
SET(TEMP ${Monteverdi_BINARY_DIR}/Testing/Temporary)

ADD_EXECUTABLE(otbSpeckleFilteringApplication otbSpeckleFilteringApplication.cxx)
TARGET_LINK_LIBRARIES(otbSpeckleFilteringApplication otbSpeckleFilteringModule)

ADD_TEST(SpeckleFilteringApplicationTest otbSpeckleFilteringApplication
${INPUTDATA}/GomaBestScale_w15_50.raw.hdr
${TEMP}/speckleFilteringOuptut.tif
)

ADD_EXECUTABLE(otbSpeckleFilteringModuleTest otbSpeckleFilteringModuleTest.cxx)
TARGET_LINK_LIBRARIES(otbSpeckleFilteringModuleTest otbSpeckleFilteringModule)

ADD_TEST(SpeckleFilteringModuleTest otbSpeckleFilteringModuleTest
${INPUTDATA}/GomaBestScale_w15_50.raw.hdr
${TEMP}/speckleFilteringModuleOuptut.tif
)

ADD_EXECUTABLE(otbReaderModuleTest otbReaderModuleTest.cxx)
TARGET_LINK_LIBRARIES(otbReaderModuleTest otbReaderModule)

ADD_TEST(ReaderModuleTest otbReaderModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerModuleOuptut.tif
)