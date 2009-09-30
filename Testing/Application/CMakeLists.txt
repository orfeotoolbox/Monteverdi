SET(BASELINE ${OTB_DATA_ROOT}/Baseline/OTB/Images)
SET(BASELINE_FILES ${OTB_DATA_ROOT}/Baseline/OTB/Files)
SET(INPUTDATA ${OTB_DATA_ROOT}/Input)
SET(TEMP ${Monteverdi_BINARY_DIR}/Testing/Temporary)

# Large Input 
IF(OTB_DATA_USE_LARGEINPUT)
  SET(LARGEINPUT ${OTB_DATA_LARGEINPUT_ROOT} )
ENDIF(OTB_DATA_USE_LARGEINPUT)



#-------------- Monteverdi Application Tests ---------------------------------
ADD_EXECUTABLE(otbMonteverdiApplicationNew otbMonteverdiApplicationNew.cxx)
TARGET_LINK_LIBRARIES(otbMonteverdiApplicationNew otbMonteverdi)
ADD_TEST(apTuMonteverdiApplication otbMonteverdiApplicationNew)

ADD_EXECUTABLE(otbMonteverdiApplicationTest otbMonteverdiApplicationTest.cxx)
TARGET_LINK_LIBRARIES(otbMonteverdiApplicationTest otbMonteverdi)
ADD_TEST(apTvMonteverdiApplication otbMonteverdiApplicationTest)


