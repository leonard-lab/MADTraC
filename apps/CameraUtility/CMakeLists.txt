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

set(APP_NAME "CameraUtility")

# Special bits to build an OS X application bundle
if(OS_X)
    # handle the Info.plist
    SET_TARGET_PROPERTIES(${APP_NAME}
        PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist.in)
    # copy OpenCV.framework into the bundle
    if(MT_HAVE_OPENCV AND MT_OPENCV_FRAMEWORK)
        if(${CMAKE_GENERATOR} MATCHES "Xcode")
            set(FRAMEWORK_DEST "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
        else()
            set(FRAMEWORK_DEST "${CMAKE_BINARY_DIR}")      
        endif()
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${MT_OPENCV_FRAMEWORK} ${FRAMEWORK_DEST}/${APP_NAME}.app/Contents/Frameworks/OpenCV.framework)
    endif()
endif(OS_X)
