SET(TESTSYSTEM_TESTS ${CXX_TEST_PATH}/otbTestSystemTests)

#Test if the current working copy corresponds to the nightly revision number
ADD_TEST(tsTvIsNightlyRevision ${TESTSYSTEM_TESTS}
         otbIsNightlyRevision
         ${Monteverdi_WC_REVISION}
         http://hg.orfeo-toolbox.org/nightly/monteverdiNightlyNumber )

# -------       CXX source files -----------------------------------
SET(TESTSYSTEM_SRCS
otbTestSystemTests.cxx
otbIsNightlyRevision.cxx
)

ADD_EXECUTABLE(otbTestSystemTests ${TESTSYSTEM_SRCS})
TARGET_LINK_LIBRARIES(otbTestSystemTests OTBTesting OTBCommon OTBIO)
