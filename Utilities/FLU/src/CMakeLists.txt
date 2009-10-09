IF(CYGWIN)
  ADD_DEFINITIONS(-DCYGWIN)
ENDIF(CYGWIN)

SET(CPPFILES
Flu_Button.cpp             Flu_Combo_Tree.cpp            Flu_Float_Input.cpp  Flu_Link_Button.cpp     Flu_Separator.cpp     Flu_Wrap_Group.cpp
Flu_Chat_Buffer.cpp        Flu_DND.cpp                   Flu_GL_Canvas.cpp    Flu_Output.cpp          Flu_Simple_Group.cpp
Flu_Choice_Group.cpp       Flu_Dual_Progress_Meter.cpp   Flu_GL_Window.cpp    flu_pixmaps.cpp         FluSimpleString.cpp
Flu_Collapsable_Group.cpp  Flu_Dual_Slider.cpp           Flu_Helpers.cpp      Flu_Progress.cpp        Flu_Spinner.cpp
Flu_Combo_Box.cpp          Flu_File_Chooser.cpp          Flu_Int_Input.cpp    Flu_Progress_Meter.cpp  Flu_Toggle_Group.cpp
Flu_Combo_List.cpp         flu_file_chooser_pixmaps.cpp  Flu_Label.cpp        Flu_Return_Button.cpp   Flu_Tree_Browser.cpp)

# Suppress deprecated warning
IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(${CPPFILES} PROPERTIES COMPILE_FLAGS -w)
ELSE(CMAKE_COMPILER_IS_GNUCXX)
  IF(WIN32)
    IF (CMAKE_CXX_COMPILER MATCHES "^cl$")
        SET_SOURCE_FILES_PROPERTIES( ${CPPFILES} PROPERTIES COMPILE_FLAGS "/W0" )
    ENDIF (CMAKE_CXX_COMPILER MATCHES "^cl$")
  ENDIF(WIN32)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


ADD_LIBRARY(flu ${CPPFILES})
TARGET_LINK_LIBRARIES(flu OTBGui)