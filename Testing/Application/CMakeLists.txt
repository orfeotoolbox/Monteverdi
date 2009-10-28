SET(BASELINE ${OTB_DATA_ROOT}/Baseline/OTB/Images)
SET(BASELINE_FILES ${OTB_DATA_ROOT}/Baseline/OTB/Files)
SET(INPUTDATA ${OTB_DATA_ROOT}/Input)
SET(TEMP ${Monteverdi_BINARY_DIR}/Testing/Temporary)

# Large Input 
IF(OTB_DATA_USE_LARGEINPUT)
  SET(LARGEINPUT ${OTB_DATA_LARGEINPUT_ROOT} )
ENDIF(OTB_DATA_USE_LARGEINPUT)

SET(Application_TESTS ${CXX_TEST_PATH}/otbApplicationTests)

#-------------- Monteverdi Application Tests ---------------------------------
ADD_TEST(apTuMonteverdiApplication ${Application_TESTS} otbMonteverdiApplicationNew)
ADD_TEST(apTvMonteverdiApplication ${Application_TESTS} otbMonteverdiApplicationTest)


#-------------- Monteverdi Application Read and vizualise an image ---------------------------------
ADD_TEST(apTvMonteverdiReaderViewer ${Application_TESTS} otbMonteverdiApplicationReaderViewer
${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
0
)
# ADD_TEST(apTvMonteverdiGraphviz ${Application_TESTS} otbMonteverdiApplicationGraphviz
# ${INPUTDATA}/ROI_IKO_PAN_LesHalles_sub.tif
# 0
# ${TEMP}/graphviz.dot
# )

SET(Application_SRCS
otbMonteverdiApplicationNew.cxx
otbMonteverdiApplicationTest.cxx
otbMonteverdiApplicationReaderViewer.cxx
# otbMonteverdiApplicationGraphviz.cxx
)

# Suppress warning Fl_RGB_Image deferencing pointeur
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(
            ${Application_SRCS}
            otbApplicationTests.cxx
            PROPERTIES COMPILE_FLAGS -fno-strict-aliasing)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_EXECUTABLE(otbApplicationTests otbApplicationTests.cxx ${Application_SRCS})
TARGET_LINK_LIBRARIES(otbApplicationTests otbMonteverdi OTBIO OTBTesting ${LIST_MODULE_LIBRARIES} )