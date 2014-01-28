message(STATUS "Importing FLTK...")

set(MVD_GLU_INCLUDE_PATH "")

#-------------------------------
# OpenGL Library
#-------------------------------
find_package(OpenGL)

if( NOT OPENGL_FOUND )
        message(FATAL_ERROR "Unable to find OpenGL on your system.\nCannot build Monteverdi without OpenGL library.\nInstall OpenGL on your system.")
endif()
if( NOT OPENGL_GLU_FOUND )
        message(FATAL_ERROR "Unable to find GLU library on your system.\nCannot build Monteverdi module without GLU library.\nInstall GLU library on your system.")
endif()

if(OPENGL_INCLUDE_DIR)
        include_directories(${OPENGL_INCLUDE_DIR})
endif()

#-------------------------------
# FLTK Library
#-------------------------------

find_package(FLTK)
if(NOT FLTK_FOUND)
    message(FATAL_ERROR
          "Cannot build Monteverdi project without FLTK. Please set FLTK_DIR.")
endif()

# Only if we used a FLTK 1.3.0 because in FLTK 1.1.>=9 is already done
if(FLTK_USE_FILE)
    include_directories(${FLTK_INCLUDE_DIR})

    # UseFLTK is buggy on windows, so we do not import it
    # The definition of FLTK_EXE_LINKER_FLAGS in FLTKConfig.cmake
    # is not valid and we end up with
    # CMAKE_EXE_LINKER_FLAGS = " /STACK:10000000 /machine:X86 ; /STACK:10000000 /machine:X86"
    # The ";" in the middle creates link error :
    # impossible d'ouvrir le fichier en entrΘe ';.obj'

    # include(${FLTK_USE_FILE})
endif()

set(MVD_VISU_GUI_LIBRARIES "${FLTK_LIBRARIES}")
if(APPLE)
  set(MVD_VISU_GUI_LIBRARIES "${MVD_VISU_GUI_LIBRARIES};-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
endif()

message(STATUS "  Using FLTK external version ${FLTK_EXTERNAL_VERSION}")
message(STATUS "  FLTK includes : ${FLTK_INCLUDE_DIR}")
message(STATUS "  FLTK libraries: ${FLTK_LIBRARIES}")
message(STATUS "  FLUID executable = ${FLTK_FLUID_EXECUTABLE}")

