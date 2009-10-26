SET(BASELINE ${OTB_DATA_ROOT}/Baseline/OTB/Images)
SET(BASELINE_FILES ${OTB_DATA_ROOT}/Baseline/OTB/Files)
SET(INPUTDATA ${OTB_DATA_ROOT}/Input)
SET(TEMP ${Monteverdi_BINARY_DIR}/Testing/Temporary)

# Large Input 
IF(OTB_DATA_USE_LARGEINPUT)
  SET(LARGEINPUT ${OTB_DATA_LARGEINPUT_ROOT} )
ENDIF(OTB_DATA_USE_LARGEINPUT)

SET(Modules_TESTS ${CXX_TEST_PATH}/otbModulesTests)


#-------------- Speckle Filtering Tests ---------------------------------
ADD_TEST(moTvSpeckleFilteringApplicationTest ${Modules_TESTS} otbSpeckleFilteringApplication
${INPUTDATA}/GomaBestScale_w15_50.raw.hdr
${TEMP}/speckleFilteringOuptut.tif
)

ADD_TEST(moTvSpeckleFilteringModuleTest  ${Modules_TESTS} otbSpeckleFilteringModuleTest
${INPUTDATA}/GomaBestScale_w15_50.raw.hdr
${TEMP}/speckleFilteringModuleOuptut.tif
)

#------------ Reader Module Test ---------------------------------
ADD_TEST(moTvReaderModuleTest ${Modules_TESTS}  otbReaderModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerModuleOuptut.tif
)

#------------- Ortho Rectification Tests ---------------------------
IF(OTB_DATA_USE_LARGEINPUT)
ADD_TEST(moTvOrthorectificationModuleTest ${Modules_TESTS}  otbOrthorectificationModuleTest
${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_PAN/02APR01105228-P1BS-000000128955_01_P001.TIF   
${TEMP}/OrthorectificationModuleOuptut.tif
)
ADD_TEST(moTvOrthorectificationSingleImageModuleTest ${Modules_TESTS}  otbOrthorectificationSingleImageModuleTest
${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_PAN/02APR01105228-P1BS-000000128955_01_P001.TIF   
${TEMP}/OrthorectificationSingleImageModuleOuptut.tif
)
ENDIF(OTB_DATA_USE_LARGEINPUT)

#------------- Feature Extraction Tests ---------------------------
ADD_TEST(moTvFeatureExtractionModuleTest  ${Modules_TESTS} otbFeatureExtractionModuleTest
${INPUTDATA}/GomaBestScale_w15_50.raw.hdr   
${TEMP}/FeatureExtractionModuleOuptut.hdr
)

ADD_TEST(moTvFeatureExtractionModuleApplication  ${Modules_TESTS} otbFeatureExtractionModuleApplication
${INPUTDATA}/qb_RoadExtract.img
${TEMP}/featModelTest.tif
)

# ADD_EXECUTABLE(otbFeatureExtractionModuleInterne otbFeatureExtractionApplication_test_interne.cxx)
# TARGET_LINK_LIBRARIES(otbFeatureExtractionModuleInterne otbFeatureExtractionModule)

#------------- Supervised Classification Tests ---------------------------
ADD_TEST(moTvSupervisedClassificationModuleTest ${Modules_TESTS}  otbSupervisedClassificationModuleTest
${INPUTDATA}/poupees.tif
${TEMP}/SupervisedClassificationModuleOuptut.tif
${INPUTDATA}/poupees.svm    
)
#------------- Writer Module Tests ---------------------------
ADD_TEST(moTvWriterModuleTest  ${Modules_TESTS} otbWriterModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerModuleOuptut.tif
)

ADD_TEST(moTvReaderWriterModuleTest ${Modules_TESTS} otbReaderWriterModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerwriterModuleOuptut.tif
)
#------------------  Viewer Tests ---------------------------
ADD_TEST(moTvViewerModuleTest ${Modules_TESTS} otbViewerModuleTest
${INPUTDATA}/poupees.tif
0
)

IF(OTB_DATA_USE_LARGEINPUT)
ADD_TEST(moTvViewerModuleTestWithSHP ${Modules_TESTS} otbViewerModuleTest
${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_MUL/02APR01105228-M1BS-000000128955_01_P001.TIF
0
${LARGEINPUT}/VECTOR/MidiPyrenees/waterways.shp
${LARGEINPUT}/VECTOR/MidiPyrenees/buildings.shp
)
ENDIF(OTB_DATA_USE_LARGEINPUT)



#------------- Writer MVC Module Tests ---------------------------
ADD_TEST(moTvWriterMVCModuleTest  ${Modules_TESTS} otbWriterMVCModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerModuleOuptut.tif
)

#ADD_TEST(WriterMVCModuleTest2  ${Modules_TESTS} otbWriterMVCModuleTest2
#${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
#${TEMP}/readerModuleOuptut.tif
#)

#------------- PanSharpening Module Tests ---------------------------

ADD_TEST(moTvPanSharpeningModuleTest ${Modules_TESTS} otbPanSharpeningModuleTest
    ${INPUTDATA}/QB_Toulouse_Ortho_PAN.tif
    ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
    ${TEMP}/panSharpeningModuleOutput.tif
)

#------------- Projection Module Tests ---------------------------
IF(OTB_DATA_USE_LARGEINPUT)
ADD_TEST(moTuProjectionModule ${Modules_TESTS} otbProjectionModuleTest
${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_MUL/02APR01105228-M1BS-000000128955_01_P001.TIF
0
${TEMP}/projectionModuleOutput.tif
)
ENDIF(OTB_DATA_USE_LARGEINPUT)

#------------- Extract ROI Module Tests ---------------------------
ADD_TEST(moTvExtractROIModuleTest  ${Modules_TESTS} otbExtractROIModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles.tif
${TEMP}/extractROIImageModuleOuptut.tif
15 25
42 74
)

ADD_TEST(moTvExtractROIVectorImageModuleTest  ${Modules_TESTS} otbExtractROIModuleTest
${INPUTDATA}/SPOT5_EXTRACTS/Arcachon/Arcachon_extrait_3852_3319_546_542.tif
${TEMP}/extractROIVectorImageModuleOuptut.tif
50 60
78 98
)

#------------- Point Extraction Module Tests ---------------------------

ADD_TEST(moTvHomologousPointExtractionModuleTest ${Modules_TESTS} otbHomologousPointExtractionModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub_warped_translation.tif
${TEMP}/moTvHomologousPointExtractionModule.tif	
158 246 162 242
110 153 114 149
192 94  195 90
0
)

#------------- Concatenate Module Tests ---------------------------

ADD_TEST(moTvConcatenateImagesModuleTest ${Modules_TESTS} otbConcatenateModuleTest
${INPUTDATA}/Seg1InputForRCC8Graph.tif
${INPUTDATA}/Seg2InputForRCC8Graph.tif
${INPUTDATA}/Seg3InputForRCC8Graph.tif
${INPUTDATA}/Seg4InputForRCC8Graph.tif
${TEMP}/moTvConcatenateImageModule.tif 
)


#------------- Algebra Module test
ADD_TEST(moTvAlgebraModuleTest ${Modules_TESTS} otbAlgebraModuleTest
${INPUTDATA}/prison_toulouse.tif
${INPUTDATA}/prison_toulouse.tif
${TEMP}/moTvAlgebraImageModule.tif
0
)	      


#------------- Interactive Change Detection Module test
ADD_TEST(moTuChangeDetectionModuleTest ${Modules_TESTS} 
         otbChangeDetectionModuleTest
	 ${INPUTDATA}/GomaAvant.png
	 ${INPUTDATA}/GomaApres.png
	 ${INPUTDATA}/icdSVMModel.svm
	 ${TEMP}/moTvChangeDetectionModule.tif
	 0
)
#------------- GCP to Sensor Model Module test -------------
ADD_TEST(gcpTvGCPToSensorModelModuleTest ${Modules_TESTS} otbGCPToSensorModelModuleTest
${INPUTDATA}/SPOT5_EXTRACTS/Arcachon/Arcachon_extrait_3852_3319_546_542.tif
${TEMP}/gcpTvGCPToSensorModelModuleTest.tif
)

#------------- Threshold  Module test New -------------
ADD_TEST(moTuThresholdModuleTest ${Modules_TESTS} otbThresholdModuleTestNew
)

#------------- Threshold  Module test -------------
IF(OTB_DATA_USE_LARGEINPUT)
ADD_TEST(moTvThresholdModuleTest ${Modules_TESTS} 
	 otbThresholdModuleTest
	 ${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_MUL/02APR01105228-M1BS-000000128955_01_P001.TIF
	 0
)
ENDIF(OTB_DATA_USE_LARGEINPUT)

SET(Modules_SRCS
otbSpeckleFilteringApplication.cxx
otbSpeckleFilteringModuleTest.cxx
otbReaderModuleTest.cxx 
otbWriterModuleTest.cxx
otbOrthorectificationModuleTest.cxx
otbOrthorectificationSingleImageModuleTest.cxx
otbFeatureExtractionModuleTest.cxx
otbFeatureExtractionApplication.cxx
otbSupervisedClassificationModuleTest.cxx
otbReaderWriterModuleTest.cxx
otbViewerModuleTest.cxx
otbWriterMVCModuleTest.cxx
otbWriterMVCModuleTest2.cxx
otbPanSharpeningModuleTest.cxx
otbProjectionModuleTest.cxx
otbExtractROIModuleTest.cxx
otbExtractROIVectorImageModuleTest.cxx
otbHomologousPointExtractionModuleTest.cxx
otbConcatenateModuleTest.cxx
otbAlgebraModuleTest.cxx
otbChangeDetectionModuleTest.cxx
otbGCPToSensorModelModuleTest.cxx	
otbThresholdModuleTestNew.cxx	
otbThresholdModuleTest.cxx
)

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${Modules_SRCS}
            otbModulesTests.cxx
            otbMeanShiftApplication.cxx
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_EXECUTABLE(otbModulesTests otbModulesTests.cxx ${Modules_SRCS})

TARGET_LINK_LIBRARIES(otbModulesTests ${LIST_MODULE_LIBRARIES} OTBIO OTBCommon OTBTesting)


# CPack installation: only Monteverdi application
#IF(NOT OTB_INSTALL_NO_LIBRARIES)
#  INSTALL(TARGETS otbHomologousPointExtractionApplication
#    RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
#    LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
#    ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT Development)
#ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)

ADD_EXECUTABLE(otbMeanShiftApplication otbMeanShiftApplication.cxx)
TARGET_LINK_LIBRARIES(otbMeanShiftApplication otbMonteverdi otbMeanShiftModule otbReaderModule)
# CPack installation: only Monteverdi application
#IF(NOT OTB_INSTALL_NO_LIBRARIES)
#  INSTALL(TARGETS otbMeanShiftApplication
#    RUNTIME DESTINATION ${Monteverdi_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
#    LIBRARY DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
#    ARCHIVE DESTINATION ${Monteverdi_INSTALL_LIB_DIR_CM24} COMPONENT Development)
#ENDIF(NOT OTB_INSTALL_NO_LIBRARIES)

#ADD_EXECUTABLE(otbOrthorectificationApplication ${OR_SRCS} ${otbOrthorectificationApplication_FLTK_UI_SRCS})
#TARGET_LINK_LIBRARIES(otbOrthorectificationApplication  otbOrthorectificationModule)

