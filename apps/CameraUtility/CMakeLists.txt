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

# MADTRaC information
set(MT_ROOT "../../build" CACHE PATH "MADTraC root directory")
# sets variables related to MT
include(${MT_ROOT}/cmake/MT_Config.cmake)

# set the name of the built target
set(APP_NAME CameraUtility)

# set this to a list of source and header files to be compiled
set(APP_SRC
  src/CameraUtility.cpp
  src/CameraUtility.h)
  
include_directories(src)

add_executable(${APP_NAME} WIN32 MACOSX_BUNDLE ${APP_SRC})

MT_tracking_app(${APP_NAME})
