##################################################
#
# CMakeLists.txt - Root cmake file for PlaySwarm
#
# History:
#   * Original file - Dan Swain, 6/11/10
#   
##################################################

# require cmake version 2.8 since it has a module
#  to find HDF5.  2.8 is available on mac via
#  macports/darwinports
cmake_minimum_required (VERSION 2.8)

# name of the project
project(PlaySwarm)

# determine if this is OS X
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(OS_X ON)
endif()

if(NOT MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
else(NOT MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W1 /MT")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1 /MT")
endif(NOT MSVC)

set(PLAYSWARM_SRC src/PlaySwarm.cpp src/PlaySwarm.h src/Occlusion.cpp src/Occlusion.h)

if(OS_X)
  set(PLAYSWARM_EXTRA_FILES
    resources/PlaySwarm.icns
    resources/clf.icns
    ~/src/MADTraC/OpenCV.framework)
endif(OS_X)    

include_directories(src)

set(MT_ROOT "" CACHE PATH "MADTraC root directory")
include(${MT_ROOT}/cmake/MT_Config.cmake)

#if(NOT MSVC)
#  set(CMAKE_EXE_LINKER_FLAGS
#  "${CMAKE_EXE_LINKER_FLAGS} ${MT_LIBS_DIR}/libMT_Core.a ${MT_LIBS_DIR}/libMT_GUI.a")
#endif(NOT MSVC)

add_executable(PlaySwarm WIN32 MACOSX_BUNDLE
  ${PLAYSWARM_SRC}
  ${PLAYSWARM_EXTRA_FILES})

if(MT_HAVE_CLF)
  add_definitions(-D_HDF5USEDLL_)
#  target_link_libraries(PlaySwarm ${MT_HDF5_LIB} ${MT_HDF5_LIB} ${MT_CLF_LIB})
#  if(MSVC)
#    target_link_libraries(PlaySwarm ${MT_HDF5_EXTRA_LIBS})
#  endif(MSVC)
endif()

#if(MT_HAVE_OPENCV)
#  if(MT_OPENCV_FRAMEWORK)
#    target_link_libraries(PlaySwarm ${MT_OPENCV_FRAMEWORK})
#  else()
#    target_link_libraries(PlaySwarm ${MT_OPENCV_LIBS})
#  endif()
#endif(MT_HAVE_OPENCV)  

#if(MSVC)
#  target_link_libraries(PlaySwarm
#    ${MT_ROOT}/lib/Release/MT_Core.lib
#    ${MT_ROOT}/lib/Release/MT_GUI.lib)
#endif(MSVC)

message("Libs are ${MT_GUI_LIBS} and ${MT_GUI_EXTRA_LIBS}")
target_link_libraries(PlaySwarm ${MT_GUI_LIBS} ${MT_GUI_EXTRA_LIBS})

if(OS_X)
  SET_SOURCE_FILES_PROPERTIES(
    resources/PlaySwarm.icns
    resources/clf.icns
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
    )
  SET_TARGET_PROPERTIES(PlaySwarm
    PROPERTIES
    MACOSX_BUNDLE_ICON_FILE PlaySwarm.icns
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist.in)
  if(MT_HAVE_OPENCV AND MT_OPENCV_FRAMEWORK)
  add_custom_command(TARGET PlaySwarm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MT_OPENCV_FRAMEWORK} ${CMAKE_BINARY_DIR}/PlaySwarm.app/Contents/Frameworks/OpenCV.framework)
  endif()
endif(OS_X)
