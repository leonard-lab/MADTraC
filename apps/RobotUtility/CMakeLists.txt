##################################################
#
# CMakeLists.txt - Root cmake file for RobotUtility
#
# History:
#   * Original file - Dan Swain, 7/6/10
#   
##################################################

# require cmake version 2.8 since it has a module
#  to find HDF5.  2.8 is available on mac via
#  macports/darwinports
cmake_minimum_required (VERSION 2.8)

# name of the project - this has to come first
project(RobotUtility)

set(APP_NAME RobotUtility)
# source files
set(APP_SOURCE_FILES src/RobotUtility.cpp src/RobotUtility.h)

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

if(OS_X)
  set(EXTRA_FILES
    ${MT_OPENCV_FRAMEWORK})
endif(OS_X)    

# include directories for this project
include_directories(src)

add_executable(${APP_NAME} WIN32 MACOSX_BUNDLE
  ${APP_SOURCE_FILES}
  ${EXTRA_FILES})

# MADTRaC information
set(MT_ROOT $ENV{MT_ROOT} CACHE PATH "MADTraC root directory")
include(${MT_ROOT}/cmake/MT_Config.cmake)

# libraries to link - inherited from MT
target_link_libraries(${APP_NAME} ${MT_ROBOT_LIBS} ${MT_ROBOT_EXTRA_LIBS})

# Special bits to build an OS X application bundle
if(OS_X)
  # handle the Info.plist
  SET_TARGET_PROPERTIES(${APP_NAME}
    PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist.in)
  # copy OpenCV.framework into the bundle
  if(MT_HAVE_OPENCV AND MT_OPENCV_FRAMEWORK)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory ${MT_OPENCV_FRAMEWORK} ${CMAKE_BINARY_DIR}/${APP_NAME}.app/Contents/Frameworks/OpenCV.framework)
  endif()
endif(OS_X)
