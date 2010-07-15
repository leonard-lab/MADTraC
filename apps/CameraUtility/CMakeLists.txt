######################################################################
#
# CMakeLists.txt - cmake file for CameraUtility
#
# History:
#   * Original file - Dan Swain, 7/2/10
#   * Simplified - DTS, 7/14/10
#
######################################################################

cmake_minimum_required(VERSION 2.8)

project(CameraUtility)

include_directories(src)

add_executable(CameraUtility WIN32 MACOSX_BUNDLE
  src/CameraUtility.cpp
  src/CameraUtility.h)

set(MT_ROOT $ENV{MT_ROOT} CACHE PATH "MADTraC root directory")

include(${MT_ROOT}/cmake/MT_Config.cmake)

MT_tracking_app(CameraUtility)
