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
  ./base/MT_FrameBase.cpp          ./base/MT_FrameBase.h
  ./base/MT_CameraSlaveFrame.cpp   ./base/MT_CameraSlaveFrame.h)      
set(dialogs_srcs
  ./dialogs/MT_DataDialogs.cpp          ./dialogs/MT_DataDialogs.h
  ./dialogs/MT_DialogManagement.cpp     ./dialogs/MT_DialogManagement.h
  ./dialogs/MT_MovieExporterDialog.cpp  ./dialogs/MT_MovieExporterDialog.h
  ./dialogs/MT_NumericInputDialog.cpp   ./dialogs/MT_NumericInputDialog.h
  ./dialogs/MT_ParameterDialog.cpp      ./dialogs/MT_ParameterDialog.h
  ./dialogs/MT_XDFDialogs.cpp           ./dialogs/MT_XDFDialogs.h)   
set(server_srcs
  ./server/MT_Server.cpp   ./server/MT_Server.h
  ./server/MT_Client.cpp   ./server/MT_Client.h
  ./server/MT_ServerModules.cpp
  ./server/MT_ServerModules.h)
set(support_srcs
  ./support/wxSupport.cpp       ./support/wxSupport.h
  ./support/wxXMLSupport.cpp    ./support/wxXMLSupport.h
  ./support/Sequence.cpp        ./support/Sequence.h)

include_directories(${WX_INCLUDE})
add_definitions(${WX_DEFS})

set(module_name "MT_GUI")
set(module_srcs ${base_srcs} ${dialogs_srcs} ${server_srcs} ${support_srcs})

# tell cmake this is a static library built from the
#   given sources
if(BUILD_SHARED)
  add_library(${module_name} SHARED ${module_srcs})
else(BUILD_SHARED)  
  add_library(${module_name} STATIC ${module_srcs})
endif(BUILD_SHARED)

target_link_libraries(${module_name} ${OPENGL_LIBRARIES})
target_link_libraries(${module_name} ${WX_LIB})
if(WITH_CLF)
  target_link_libraries(${module_name} ${CLF_LIB} ${HDF5_LIBRARIES})
endif(WITH_CLF)

# copies the header files to the "include" directory
# if(NOT MSVC)
#   get_directory_property(defs DIRECTORY . COMPILE_DEFINITIONS)
#   add_custom_command(TARGET MT_GUI POST_BUILD
#      COMMAND echo ARGS "\"${defs}\">defs.txt"
#      COMMAND ${CMAKE_SOURCE_DIR}/misc/build_scripts/posix_cmake_target_script.sh ARGS "MT_GUI" ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
#   )
# endif(NOT MSVC)

# if(MSVC)
#   string(REPLACE "/" "\\" ARG2 ${CMAKE_SOURCE_DIR})
#   string(REPLACE "/" "\\" ARG3 ${CMAKE_BINARY_DIR})
#   add_custom_command(TARGET MT_GUI POST_BUILD
#     COMMAND ${CMAKE_SOURCE_DIR}/misc/build_scripts/MSVC_cmake_post_build.bat ARGS "MT_GUI" ${ARG2} ${ARG3})
# endif(MSVC)    

if(HAVE_OPENCV_FRAMEWORK)
  target_link_libraries(${module_name} ${OPENCV_FRAMEWORK})
endif()

# file(MAKE_DIRECTORY "lib" "include")
# set(MT_GUI_SOURCES ${base_srcs} ${dialogs_srcs} ${server_srcs} ${support_srcs})
# include(${CMAKE_SOURCE_DIR}/cmake/MT_BuildFunctions.cmake)
# MT_MAKE_MODULE_HEADER(. "MT_GUI" "${MT_GUI_SOURCES}")
# MT_COPY_MODULE_HEADERS("${MT_GUI_SOURCES}" "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/include/MT/MT_GUI")

include(${CMAKE_SOURCE_DIR}/cmake/MT_BuildFunctions.cmake)
MT_MAKE_MODULE_HEADER(. "${module_name}" "${module_srcs}")
MT_COPY_MODULE_HEADERS("${module_srcs}" "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/include/MT/${module_name}")
