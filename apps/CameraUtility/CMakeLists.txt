######################################################################
#
# CMakeLists.txt - cmake file for CameraUtility
#
# History:
#   * Original file - Dan Swain, 7/2/10
#
######################################################################

cmake_minimum_required(VERSION 2.8)

project(CameraUtility)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(OS_X ON)
endif()

set(CAMERA_UTILITY_SRC src/CameraUtility.cpp src/CameraUtility.h)

include_directories(src)

add_executable(CameraUtility WIN32 MACOSX_BUNDLE
  ${CAMERA_UTILITY_SRC})

set(MT_ROOT $ENV{MT_ROOT} CACHE PATH "MADTraC root directory")
include(${MT_ROOT}/cmake/MT_Config.cmake)

target_link_libraries(CameraUtility ${MT_TRACKING_LIBS} ${MT_TRACKING_EXTRA_LIBS})