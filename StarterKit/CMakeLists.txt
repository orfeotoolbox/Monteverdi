# Only if OTB is generate with Visu GUI option
IF(OTB_USE_VISU_GUI)

# First, compile the module

SET(MODULE_SRCS
otbExampleModule.cxx
)

ADD_LIBRARY(otbExampleModule ${MODULE_SRCS})
TARGET_LINK_LIBRARIES(otbExampleModule OTBMonteverdiCommon OTBIO)

INSTALL(TARGETS otbExampleModule
        RUNTIME DESTINATION /bin
        LIBRARY DESTINATION /bin
        ARCHIVE DESTINATION /bin )

# Then, compile the application

ADD_EXECUTABLE(otbExampleApplication otbExampleApplication.cxx)
TARGET_LINK_LIBRARIES(otbExampleApplication otbMonteverdi otbExampleModule otbReaderModule)
INSTALL(TARGETS otbExampleApplication
        RUNTIME DESTINATION /bin
        LIBRARY DESTINATION /bin
        ARCHIVE DESTINATION /bin )

ENDIF(OTB_USE_VISU_GUI)
