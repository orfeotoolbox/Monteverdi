set(GUI_TESTS ${CXX_TEST_PATH}/otbGuiTests)

SET(INPUTDATA ${OTB_DATA_ROOT}/Input)
SET(TEMP ${Monteverdi_BINARY_DIR}/Testing/Temporary)

# -------            otb::System  -------------
add_test(guTuFltkFilterWatcher ${GUI_TESTS}
        otbFltkFilterWatcher
        ${INPUTDATA}/couleurs.tif
        )


# -------------  otb::FltkWriterWatcher ----------------------------

add_test(coTvFltkWriterWatcher ${GUI_TESTS}
     otbFltkWriterWatcher
        ${INPUTDATA}/couleurs.tif
        ${TEMP}/coTvFltkWriterWatcherOutput.tif
        20)



# -------       Fichiers sources CXX -----------------------------------
set(Gui_SRCS
otbGuiTests.cxx
otbFltkFilterWatcher.cxx
otbFltkWriterWatcher.cxx
)

# To suppress "ld: cycle in dylib re-exports with /usr/X11R6/lib/libGL.dylib" error on APPLE configuration
if(APPLE)
      foreach(c "" "_DEBUG" "_RELEASE" "_MINSIZEREL" "_RELWITHDEBINFO")
        set(CMAKE_EXE_LINKER_FLAGS${c} "${CMAKE_EXE_LINKER_FLAGS${c}} -Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
      endforeach()
endif()

ADD_EXECUTABLE(otbGuiTests otbGuiTests.cxx ${Gui_SRCS})

TARGET_LINK_LIBRARIES(otbGuiTests OTBIO OTBGui OTBTesting)
