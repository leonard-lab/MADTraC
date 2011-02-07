######################################################################
#
# CMakeLists.txt - cmake file for BackgroundCreator
#
# NOTE: This is a pseudo-console app - i.e. it is intended to be run
# from the command line, but it does in fact pop up a window that is
# kept off-screen.  This is therefore a bad example to learn from -
# CameraUtility or PlaySwarm are better for that.
#
# History:
#   * Original file - Dan Swain, 7/2/10
#   * Simplified - DTS, 7/14/10
#   * Modified from CameraUtility - DTS, 9/15/10
#
######################################################################

cmake_minimum_required(VERSION 2.8)

project(BackgroundCreator)

set(MT_ROOT "../../build" CACHE PATH "MADTraC root directory")
include(${MT_ROOT}/cmake/MT_Config.cmake)

set(APP_NAME BackgroundCreator)
set(APP_SRC src/BackgroundCreator.cpp)

# for MT_wxTest.h
include_directories(${MT_ROOT}/../test/src/)
include_directories(src)

add_executable(${APP_NAME} WIN32 MACOSX_BUNDLE ${APP_SRC})

add_definitions(${MT_WX_DEFS})
# target_link_libraries(BackgroundCreator
#   ${MT_TRACKING_LIBS} ${MT_TRACKING_EXTRA_LIBS})
# MT_osx_app_copy_step(BackgroundCreator)
MT_tracking_app(${APP_NAME})