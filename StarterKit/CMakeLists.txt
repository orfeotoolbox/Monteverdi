# First, compile the module

SET(MODULE_SRCS
otbExampleModule.cxx
)

ADD_LIBRARY(otbExampleModule ${MODULE_SRCS})
TARGET_LINK_LIBRARIES(otbExampleModule OTBMonteverdiCommon)
INSTALL(TARGETS otbExampleModule
        RUNTIME DESTINATION /bin
        LIBRARY DESTINATION /bin
        ARCHIVE DESTINATION /bin )

# Then, compile the application

ADD_EXECUTABLE(otbExampleApplication otbExampleApplication.cxx)
TARGET_LINK_LIBRARIES(otbExampleApplication otbMonteverdi otbExampleModule)
INSTALL(TARGETS otbExampleApplication
        RUNTIME DESTINATION /bin
        LIBRARY DESTINATION /bin
        ARCHIVE DESTINATION /bin )
