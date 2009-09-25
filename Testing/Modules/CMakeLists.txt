SET(BASELINE ${OTB_DATA_ROOT}/Baseline/OTB/Images)
SET(BASELINE_FILES ${OTB_DATA_ROOT}/Baseline/OTB/Files)
SET(INPUTDATA ${OTB_DATA_ROOT}/Input)
SET(TEMP ${Monteverdi_BINARY_DIR}/Testing/Temporary)

# Large Input 
IF(OTB_DATA_USE_LARGEINPUT)
  SET(LARGEINPUT ${OTB_DATA_LARGEINPUT_ROOT} )
ENDIF(OTB_DATA_USE_LARGEINPUT)



#-------------- Speckle Filtering Tests ---------------------------------
ADD_EXECUTABLE(otbSpeckleFilteringApplication otbSpeckleFilteringApplication.cxx)
TARGET_LINK_LIBRARIES(otbSpeckleFilteringApplication otbSpeckleFilteringModule)

ADD_TEST(moTvSpeckleFilteringApplicationTest otbSpeckleFilteringApplication
${INPUTDATA}/GomaBestScale_w15_50.raw.hdr
${TEMP}/speckleFilteringOuptut.tif
)



ADD_EXECUTABLE(otbSpeckleFilteringModuleTest otbSpeckleFilteringModuleTest.cxx)
TARGET_LINK_LIBRARIES(otbSpeckleFilteringModuleTest otbSpeckleFilteringModule)

ADD_TEST(moTvSpeckleFilteringModuleTest otbSpeckleFilteringModuleTest
${INPUTDATA}/GomaBestScale_w15_50.raw.hdr
${TEMP}/speckleFilteringModuleOuptut.tif
)

#------------ Reader Module Test ---------------------------------
ADD_EXECUTABLE(otbReaderModuleTest otbReaderModuleTest.cxx)
TARGET_LINK_LIBRARIES(otbReaderModuleTest otbReaderModule)


ADD_TEST(moTvReaderModuleTest otbReaderModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerModuleOuptut.tif
)

#------------- Ortho Rectification Tests ---------------------------

ADD_EXECUTABLE(otbOrthorectificationModuleTest otbOrthorectificationModuleTest.cxx)
TARGET_LINK_LIBRARIES(otbOrthorectificationModuleTest otbOrthorectificationModule)

IF(OTB_DATA_USE_LARGEINPUT)
ADD_TEST(moTvOrthorectificationModuleTest otbOrthorectificationModuleTest
${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_PAN/02APR01105228-P1BS-000000128955_01_P001.TIF 	
${TEMP}/OrthorectificationModuleOuptut.tif
)
ENDIF(OTB_DATA_USE_LARGEINPUT)

#------------- Feature Extraction Tests ---------------------------

ADD_EXECUTABLE(otbFeatureExtractionModuleTest otbFeatureExtractionModuleTest.cxx)
TARGET_LINK_LIBRARIES(otbFeatureExtractionModuleTest otbFeatureExtractionModule)

ADD_TEST(moTvFeatureExtractionModuleTest otbFeatureExtractionModuleTest
${INPUTDATA}/GomaBestScale_w15_50.raw.hdr	
${TEMP}/FeatureExtractionModuleOuptut.hdr
)

ADD_EXECUTABLE(otbFeatureExtractionModuleApplication otbFeatureExtractionApplication.cxx)
TARGET_LINK_LIBRARIES(otbFeatureExtractionModuleApplication otbFeatureExtractionModule)

ADD_TEST(moTvFeatureExtractionModuleApplication otbFeatureExtractionModuleApplication
${INPUTDATA}/qb_RoadExtract.img
${TEMP}/featModelTest.tif
)

# ADD_EXECUTABLE(otbFeatureExtractionModuleInterne otbFeatureExtractionApplication_test_interne.cxx)
# TARGET_LINK_LIBRARIES(otbFeatureExtractionModuleInterne otbFeatureExtractionModule)

#------------- Supervised Classification Tests ---------------------------

ADD_EXECUTABLE(otbSupervisedClassificationModuleTest otbSupervisedClassificationModuleTest.cxx)
TARGET_LINK_LIBRARIES(otbSupervisedClassificationModuleTest otbSupervisedClassificationModule)

ADD_TEST(moTvSupervisedClassificationModuleTest otbSupervisedClassificationModuleTest
${INPUTDATA}/poupees.tif
${TEMP}/SupervisedClassificationModuleOuptut.tif
${INPUTDATA}/poupees.svm	
)
#------------- Writer Module Tests ---------------------------
ADD_EXECUTABLE(otbWriterModuleTest otbWriterModuleTest.cxx)
TARGET_LINK_LIBRARIES(otbWriterModuleTest otbWriterModule)

ADD_TEST(WriterModuleTest otbWriterModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerModuleOuptut.tif
)

ADD_EXECUTABLE(otbReaderWriterModuleTest otbReaderWriterModuleTest.cxx)
TARGET_LINK_LIBRARIES(otbReaderWriterModuleTest otbReaderModule otbWriterModule)

ADD_TEST(ReaderWriterModuleTest otbReaderWriterModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerwriterModuleOuptut.tif
)
#------------------  Viewer Tests ---------------------------
ADD_EXECUTABLE(otbViewerModuleTest otbViewerModuleTest.cxx)
TARGET_LINK_LIBRARIES(otbViewerModuleTest otbViewerModule)

ADD_TEST(moTvViewerModuleTest otbViewerModuleTest
${INPUTDATA}/poupees.tif
)

