##################################################
#
# MT/MT_GUI/CMakeLists.txt - cmake file for MT_GUI
#
# History:
#   * Original file - Dan Swain, 6/10/10
#   
##################################################

# require cmake version 2.8 since it has a module
#  to find HDF5.  2.8 is available on mac via
#  macports/darwinports
cmake_minimum_required (VERSION 2.8)

#######################################################
# Target: MT_GUI.a
#######################################################
# sources
set(base_srcs
  ./base/MT_AppBase.cpp            ./base/MT_AppBase.h
  ./base/MT_ControlFrameBase.cpp   ./base/MT_ControlFrameBase.h
  ./base/MT_FrameBase.cpp          ./base/MT_FrameBase.h)      
set(dialogs_srcs
  ./dialogs/MT_DataDialogs.cpp          ./dialogs/MT_DataDialogs.h
  ./dialogs/MT_DialogManagement.cpp     ./dialogs/MT_DialogManagement.h
  ./dialogs/MT_MovieExporterDialog.cpp  ./dialogs/MT_MovieExporterDialog.h
  ./dialogs/MT_NumericInputDialog.cpp   ./dialogs/MT_NumericInputDialog.h
  ./dialogs/MT_ParameterDialog.cpp      ./dialogs/MT_ParameterDialog.h)   
set(server_srcs
  ./server/MT_Server.cpp   ./server/MT_Server.h)
set(support_srcs
  ./support/wxSupport.cpp       ./support/wxSupport.h
  ./support/wxXMLSupport.cpp    ./support/wxXMLSupport.h)

include_directories(${WX_INCLUDE})
add_definitions(${WX_DEFS})

# tell cmake this is a static library built from the
#   given sources
add_library(MT_GUI STATIC ${base_srcs} ${dialogs_srcs} ${server_srcs} ${support_srcs})

# copies the header files to the "include" directory
if(OS_X)
  get_directory_property(defs DIRECTORY . COMPILE_DEFINITIONS)
  add_custom_command(TARGET MT_GUI POST_BUILD
     COMMAND echo ARGS "\"${defs}\">defs.txt"
     COMMAND ${CMAKE_SOURCE_DIR}/misc/build_scripts/osx_cmake_target_script.sh ARGS "MT_GUI" ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
  )
endif(OS_X)

if(HAVE_OPENCV_FRAMEWORK)
  target_link_libraries(MT_GUI ${OPENCV_FRAMEWORK})
endif()

if(MSVC)
  string(REPLACE "/" "\\" ARG2 ${CMAKE_SOURCE_DIR})
  string(REPLACE "/" "\\" ARG3 ${CMAKE_BINARY_DIR})
  add_custom_command(TARGET MT_GUI POST_BUILD
    COMMAND ${CMAKE_SOURCE_DIR}/misc/build_scripts/MSVC_cmake_post_build.bat ARGS "MT_GUI" ${ARG2} ${ARG3})
endif(MSVC)    

