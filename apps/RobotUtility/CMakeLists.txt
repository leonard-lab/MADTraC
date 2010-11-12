######################################################################
#
# CMakeLists.txt - cmake file for RobotUtility
#
# History:
#   * Original file - Dan Swain, 7/2/10
#   * Simplified - DTS, 7/14/10
#   * Modified from RobotUtility - DTS, 11/12/10
#
######################################################################

cmake_minimum_required(VERSION 2.8)

project(RobotUtility)

# MADTRaC information
set(MT_ROOT "../../build" CACHE PATH "MADTraC root directory")
# sets variables related to MT
include(${MT_ROOT}/cmake/MT_Config.cmake)

# set the name of the built target
set(APP_NAME RobotUtility)

# set this to a list of source and header files to be compiled
set(APP_SRC
  src/RobotUtility.cpp
  src/RobotUtility.h)
  
include_directories(src)

add_executable(${APP_NAME} WIN32 MACOSX_BUNDLE ${APP_SRC})

MT_robot_app(${APP_NAME})
