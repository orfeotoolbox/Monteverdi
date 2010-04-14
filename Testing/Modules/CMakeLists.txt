SET(BASELINE ${OTB_DATA_ROOT}/Baseline/OTB/Images)
SET(BASELINE_FILES ${OTB_DATA_ROOT}/Baseline/OTB/Files)
SET(INPUTDATA ${OTB_DATA_ROOT}/Input)
SET(TEMP ${Monteverdi_BINARY_DIR}/Testing/Temporary)

# Large Input 
IF(OTB_DATA_USE_LARGEINPUT)
  SET(LARGEINPUT ${OTB_DATA_LARGEINPUT_ROOT} )
ENDIF(OTB_DATA_USE_LARGEINPUT)

SET(Modules_TESTS1 ${CXX_TEST_PATH}/otbModulesTests1)
SET(Modules_TESTS2 ${CXX_TEST_PATH}/otbModulesTests2)
SET(Modules_TESTS3 ${CXX_TEST_PATH}/otbModulesTests3)
SET(Modules_TESTS4 ${CXX_TEST_PATH}/otbModulesTests4)



#----------------------------------------------------------------------------------------------
#------------------------------------ otbModulesTests1 ----------------------------------------
#----------------------------------------------------------------------------------------------

#-------------- Speckle Filtering Tests ---------------------------------
ADD_TEST(moTvSpeckleFilteringApplicationTest ${Modules_TESTS1} otbSpeckleFilteringApplication
${INPUTDATA}/GomaBestScale_w15_50.raw.hdr
${TEMP}/speckleFilteringOuptut.tif
)

ADD_TEST(moTvSpeckleFilteringModuleTest  ${Modules_TESTS1} otbSpeckleFilteringModuleTest
${INPUTDATA}/GomaBestScale_w15_50.raw.hdr
${TEMP}/speckleFilteringModuleOuptut.tif
)

#------------ Reader Module Test ---------------------------------
ADD_TEST(moTvReaderModuleTest ${Modules_TESTS1}  otbReaderModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerModuleOuptut.tif
)

#------------- Ortho Rectification Tests ---------------------------
IF(OTB_DATA_USE_LARGEINPUT)
ADD_TEST(moTvOrthorectificationModuleTest ${Modules_TESTS1}  otbOrthorectificationModuleTest
${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_PAN/02APR01105228-P1BS-000000128955_01_P001.TIF   
${TEMP}/OrthorectificationModuleOuptut.tif
)
ADD_TEST(moTvOrthorectificationSingleImageModuleTest ${Modules_TESTS1}  otbOrthorectificationSingleImageModuleTest
${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_PAN/02APR01105228-P1BS-000000128955_01_P001.TIF   
${TEMP}/OrthorectificationSingleImageModuleOuptut.tif
)
ENDIF(OTB_DATA_USE_LARGEINPUT)

#------------- Feature Extraction Tests ---------------------------
ADD_TEST(moTvFeatureExtractionModuleTest  ${Modules_TESTS1} otbFeatureExtractionModuleTest
${INPUTDATA}/GomaBestScale_w15_50.raw.hdr   
${TEMP}/FeatureExtractionModuleOuptut.hdr
)

ADD_TEST(moTvFeatureExtractionModuleApplication  ${Modules_TESTS1} otbFeatureExtractionModuleApplication
${INPUTDATA}/qb_RoadExtract.img
${TEMP}/featModelTest.tif
)

# ADD_EXECUTABLE(otbFeatureExtractionModuleInterne otbFeatureExtractionApplication_test_interne.cxx)
# TARGET_LINK_LIBRARIES(otbFeatureExtractionModuleInterne otbFeatureExtractionModule)

#------------- Supervised Classification Tests ---------------------------
ADD_TEST(moTvSupervisedClassificationModuleTest ${Modules_TESTS1}  otbSupervisedClassificationModuleTest
${INPUTDATA}/poupees.tif
${TEMP}/SupervisedClassificationModuleOuptut.tif
${INPUTDATA}/poupees.svm    
)
#------------- Writer Module Tests ---------------------------
ADD_TEST(moTvWriterModuleTest  ${Modules_TESTS1} otbWriterModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerModuleOuptut.tif
)

ADD_TEST(moTvReaderWriterModuleTest ${Modules_TESTS1} otbReaderWriterModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerwriterModuleOuptut.tif
)



#----------------------------------------------------------------------------------------------
#------------------------------------ otbModulesTests2 ----------------------------------------
#----------------------------------------------------------------------------------------------


#------------------  Viewer Tests ---------------------------
ADD_TEST(moTvViewerModuleTest ${Modules_TESTS2} otbViewerModuleTest
${INPUTDATA}/poupees.tif
0
)

IF(OTB_DATA_USE_LARGEINPUT)
ADD_TEST(moTvViewerModuleTestWithSHP ${Modules_TESTS2} otbViewerModuleTest
${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_MUL/02APR01105228-M1BS-000000128955_01_P001.TIF
0
${LARGEINPUT}/VECTOR/MidiPyrenees/waterways.shp
${LARGEINPUT}/VECTOR/MidiPyrenees/buildings.shp
)
ENDIF(OTB_DATA_USE_LARGEINPUT)



#------------- Writer MVC Module Tests ---------------------------
ADD_TEST(moTvWriterMVCModuleTest  ${Modules_TESTS2} otbWriterMVCModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${TEMP}/readerModuleOuptut.tif
)

#ADD_TEST(WriterMVCModuleTest2  ${Modules_TESTS} otbWriterMVCModuleTest2
#${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
#${TEMP}/readerModuleOuptut.tif
#)

#------------- PanSharpening Module Tests ---------------------------

ADD_TEST(moTvPanSharpeningModuleTest ${Modules_TESTS2} otbPanSharpeningModuleTest
    ${INPUTDATA}/QB_Toulouse_Ortho_PAN.tif
    ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
    ${TEMP}/panSharpeningModuleOutput.tif
)

#------------- Projection Module Tests ---------------------------
IF(OTB_DATA_USE_LARGEINPUT)
ADD_TEST(moTuProjectionModule ${Modules_TESTS2} otbProjectionModuleTest
${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_MUL/02APR01105228-M1BS-000000128955_01_P001.TIF
0
${TEMP}/projectionModuleOutput.tif
)
ENDIF(OTB_DATA_USE_LARGEINPUT)

#------------- Extract ROI Module Tests ---------------------------
ADD_TEST(moTvExtractROIModuleTest  ${Modules_TESTS2} otbExtractROIModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles.tif
${TEMP}/extractROIImageModuleOuptut.tif
15 25
42 74
)

ADD_TEST(moTvExtractROIVectorImageModuleTest  ${Modules_TESTS2} otbExtractROIVectorImageModuleTest
${INPUTDATA}/SPOT5_EXTRACTS/Arcachon/Arcachon_extrait_3852_3319_546_542.tif
${TEMP}/extractROIVectorImageModuleOuptut.tif
50 60
78 98
)

#------------- Point Extraction Module Tests ---------------------------

ADD_TEST(moTvHomologousPointExtractionModuleTest ${Modules_TESTS2} otbHomologousPointExtractionModuleTest
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub_warped_translation.tif
${TEMP}/moTvHomologousPointExtractionModule.tif	
158 246 162 242
110 153 114 149
192 94  195 90
0
)

#------------- Concatenate Module Tests ---------------------------

ADD_TEST(moTvConcatenateImagesModuleTest ${Modules_TESTS2} otbConcatenateModuleTest
${INPUTDATA}/Seg1InputForRCC8Graph.tif
${INPUTDATA}/Seg2InputForRCC8Graph.tif
${INPUTDATA}/Seg3InputForRCC8Graph.tif
${INPUTDATA}/Seg4InputForRCC8Graph.tif
${TEMP}/moTvConcatenateImageModule.tif 
)


#------------- Algebra Module test
ADD_TEST(moTvAlgebraModuleTest ${Modules_TESTS2} otbAlgebraModuleTest
${INPUTDATA}/prison_toulouse.tif
${INPUTDATA}/prison_toulouse.tif
${TEMP}/moTvAlgebraImageModule.tif
0
)	      

#----------------------------------------------------------------------------------------------
#------------------------------------ otbModulesTests3 ----------------------------------------
#----------------------------------------------------------------------------------------------

#------------- Interactive Change Detection Module test
ADD_TEST(moTuChangeDetectionModuleTest ${Modules_TESTS3} 
         otbChangeDetectionModuleTest
	 ${INPUTDATA}/GomaAvant.png
	 ${INPUTDATA}/GomaApres.png
	 ${INPUTDATA}/icdSVMModel.svm
	 ${TEMP}/moTvChangeDetectionModule.tif
	 0
)
#------------- GCP to Sensor Model Module test -------------
IF(OTB_DATA_USE_LARGEINPUT)
ADD_TEST(gcpTvGCPToSensorModelModuleTest ${Modules_TESTS3} otbGCPToSensorModelModuleTest
${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_MUL/02APR01105228-M1BS-000000128955_01_P001.TIF
${INPUTDATA}/DEM/srtm_directory/
${TEMP}/gcpTvGCPToSMModuleTest.xml
)
ENDIF(OTB_DATA_USE_LARGEINPUT)

#------------- Threshold  Module test New -------------
ADD_TEST(moTuThresholdModuleTest ${Modules_TESTS3} otbThresholdModuleTestNew
)

#------------- Threshold  Module test -------------
ADD_TEST(moTvThresholdModuleTest ${Modules_TESTS3} 
	 otbThresholdModuleTest
	 ${INPUTDATA}/prison_toulouse.tif
	 0
	 ${TEMP}/moTvThresholdModule.tif
)

#------------- OpticalCalibration Module -------------
ADD_TEST(moTvOpticalCalibrationModuleTest ${Modules_TESTS3} otbOpticalCalibrationModuleTest
${INPUTDATA}/SPOT5_EXTRACTS/Arcachon/Arcachon_extrait_3852_3319_546_542.tif
1
${TEMP}/moTvOpticalCalibrationModuleTest.tif
)
ADD_TEST(moTvOpticalCalibrationModuleNoMDTest ${Modules_TESTS3} otbOpticalCalibrationModuleTest
${INPUTDATA}/poupees.tif
1
)
IF(OTB_DATA_USE_LARGEINPUT)
ADD_TEST(moTvOpticalCalibrationModuleQBTest ${Modules_TESTS3} otbOpticalCalibrationModuleTest
${LARGEINPUT}/QUICKBIRD/TOULOUSE/000000128955_01_P001_MUL/02APR01105228-M1BS-000000128955_01_P001.TIF
0
)
ENDIF(OTB_DATA_USE_LARGEINPUT)


#------------- SarCalibration Module -------------
ADD_TEST(moTvSarCalibrationModuleNoMDTest ${Modules_TESTS3} otbSarCalibrationModuleTest
${INPUTDATA}/ExtractIMAGE_HH_SRA_strip_012.tif
0 # scalar image
)

IF(OTB_DATA_USE_LARGEINPUT)
ADD_TEST(moTvSarCalibrationModuleCplxTest ${Modules_TESTS3} otbSarCalibrationModuleTest
${LARGEINPUT}/TERRASARX/TORONTO/TSX1_SAR__SSC/IMAGEDATA/IMAGE_HH_SRA_spot_074.cos
1 #complex
)
ENDIF(OTB_DATA_USE_LARGEINPUT)

#----------------------------------------------------------------------------------------------
#------------------------------------ otbModulesTests4 ----------------------------------------
#----------------------------------------------------------------------------------------------

#------------- Tile Map Import Module Tests ---------------------------
IF(OTB_USE_CURL)
  ADD_TEST(moTvTileMapImportModuleTest ${Modules_TESTS4} otbTileMapImportModuleTest
  ${TEMP}/tileMapImportModuleTestOutput.tif
  Toulouse
  500
  500
  15
  )
ENDIF(OTB_USE_CURL)



SET(Modules_SRCS1
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
)


SET(Modules_SRCS2
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
)

SET(Modules_SRCS3
otbChangeDetectionModuleTest.cxx
otbGCPToSensorModelModuleTest.cxx	
otbThresholdModuleTestNew.cxx	
otbThresholdModuleTest.cxx
otbOpticalCalibrationModuleTest.cxx
otbSarCalibrationModuleTest.cxx
)

IF(OTB_USE_CURL)
SET(Modules_SRCS4
  otbTileMapImportModuleTest.cxx
)
ENDIF(OTB_USE_CURL)


# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${Modules_SRCS}
            otbModulesTests.cxx
            otbMeanShiftApplication.cxx
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_EXECUTABLE(otbModulesTests1 otbModulesTests1.cxx ${Modules_SRCS1})
ADD_EXECUTABLE(otbModulesTests2 otbModulesTests2.cxx ${Modules_SRCS2})
ADD_EXECUTABLE(otbModulesTests3 otbModulesTests3.cxx ${Modules_SRCS3})
IF(OTB_USE_CURL)
ADD_EXECUTABLE(otbModulesTests4 otbModulesTests4.cxx ${Modules_SRCS4})
ENDIF(OTB_USE_CURL)

TARGET_LINK_LIBRARIES(otbModulesTests1 ${LIST_MODULE_LIBRARIES} OTBIO OTBCommon OTBTesting)
TARGET_LINK_LIBRARIES(otbModulesTests2 ${LIST_MODULE_LIBRARIES} OTBIO OTBCommon OTBTesting)
TARGET_LINK_LIBRARIES(otbModulesTests3 ${LIST_MODULE_LIBRARIES} OTBIO OTBCommon OTBTesting)
IF(OTB_USE_CURL)
TARGET_LINK_LIBRARIES(otbModulesTests4 ${LIST_MODULE_LIBRARIES} OTBIO OTBProjections OTBCommon OTBTesting)
ENDIF(OTB_USE_CURL)

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

