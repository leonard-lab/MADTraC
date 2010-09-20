##################################################
#
# CMakeLists.txt - CMake file for MTTest
#
# History:
#   * Original file - Dan Swain, 7/2/10
#   
##################################################

# require cmake version 2.8 since it has a module
#  to find HDF5.  2.8 is available on mac via
#  macports/darwinports
cmake_minimum_required (VERSION 2.8)

# name of the project - this has to come first
project(MTTest)

# determine if this is OS X
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(OS_X ON)
endif()

set(MT_ROOT ${CMAKE_BINARY_DIR}/../../build/
  CACHE PATH "MADTraC root directory")
include(${MT_ROOT}/cmake/MT_Config.cmake)

# some build configurations will require that OpenCV.framework be
# loaded from @executable_path/../Frameworks/ (as if it were in an
# app bundle).  If this is the case, we can fake it out by creating
# a Frameworks directory here and symlinking OpenCV.framework to it
#   This should only matter if we're building on OS X using the
#   OpenCV private bundle.
function(ensure_OpenCV APP_NAME)
  if(IS_DIRECTORY ${MT_OPENCV_FRAMEWORK})
    if(NOT IS_DIRECTORY ${CMAKE_BINARY_DIR}/Frameworks)
      add_custom_command(TARGET ${APP_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Frameworks)
    endif(NOT IS_DIRECTORY ${CMAKE_BINARY_DIR}/Frameworks)
    if(NOT IS_DIRECTORY ${CMAKE_BINARY_DIR}/Frameworks/OpenCV.framework)
      add_custom_command(TARGET ${APP_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${MT_OPENCV_FRAMEWORK} ${CMAKE_BINARY_DIR}/Frameworks/OpenCV.framework)
    endif(NOT IS_DIRECTORY ${CMAKE_BINARY_DIR}/Frameworks/OpenCV.framework)
  endif(IS_DIRECTORY ${MT_OPENCV_FRAMEWORK})
endfunction(ensure_OpenCV)


enable_testing()
include_directories(src)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/tests)

######################################################################
# MT_Core/primitives

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# BoundingBox
set(CURRENT_TEST test_BoundingBox)
add_executable(${CURRENT_TEST} src/MT_Core/primitives/test_BoundingBox.cpp)
target_link_libraries(${CURRENT_TEST} ${MT_CORE_LIBS})
add_test(NAME BoundingBox COMMAND ${CURRENT_TEST})
list(APPEND MT_CORE_TESTS ${CURRENT_TEST})

######################################################################
# MT_Core/support tests

# stringsupport
set(CURRENT_TEST test_stringsupport)
add_executable(${CURRENT_TEST} src/MT_Core/support/test_stringsupport.cpp)
target_link_libraries(${CURRENT_TEST} ${MT_CORE_LIBS})
add_test(NAME StringSupport COMMAND ${CURRENT_TEST})
list(APPEND MT_CORE_TESTS ${CURRENT_TEST})

# filesupport
set(CURRENT_TEST test_filesupport)
add_executable(${CURRENT_TEST} src/MT_Core/support/test_filesupport.cpp)
target_link_libraries(${CURRENT_TEST} ${MT_CORE_LIBS})
add_test(NAME FileSupport COMMAND ${CURRENT_TEST})
list(APPEND MT_CORE_TESTS ${CURRENT_TEST})

# filesupport
set(CURRENT_TEST test_ExperimentDataFile)
add_executable(${CURRENT_TEST} src/MT_Core/fileio/test_ExperimentDataFile.cpp)
target_link_libraries(${CURRENT_TEST} ${MT_CORE_LIBS})
add_test(NAME XDF COMMAND ${CURRENT_TEST})
list(APPEND MT_CORE_TESTS ${CURRENT_TEST})

# OpenCVmath
set(CURRENT_TEST test_OpenCVmath)
add_executable(${CURRENT_TEST} src/MT_Core/support/test_OpenCVmath.cpp)
target_link_libraries(${CURRENT_TEST} ${MT_CORE_LIBS} ${MT_CORE_EXTRA_LIBS})
add_test(NAME OpenCVmath COMMAND ${CURRENT_TEST})
list(APPEND MT_CORE_TESTS ${CURRENT_TEST})

# UKF
set(CURRENT_TEST test_UKF)
add_executable(${CURRENT_TEST} src/MT_Core/support/test_UKF.cpp)
target_link_libraries(${CURRENT_TEST} ${MT_CORE_LIBS} ${MT_CORE_EXTRA_LIBS})
add_test(NAME UKF COMMAND ${CURRENT_TEST})
list(APPEND MT_CORE_TESTS ${CURRENT_TEST})


######################################################################
# MT_GUI/support

add_definitions(${MT_WX_DEFS})

# wxSupport
set(CURRENT_TEST test_wxSupport)
add_executable(${CURRENT_TEST} src/MT_GUI/support/test_wxSupport.cpp)
target_link_libraries(${CURRENT_TEST}
  ${MT_GUI_LIBS}
  ${MT_WX_LIB}
  ${MT_WX_EXTRA_LIBS}
  ${MT_GL_LIBS})
add_test(NAME wxSupport COMMAND ${CURRENT_TEST})
list(APPEND MT_GUI_TESTS ${CURRENT_TEST})

# Sequence
set(CURRENT_TEST test_Sequence)
add_executable(${CURRENT_TEST} src/MT_GUI/support/test_Sequence.cpp)
target_link_libraries(${CURRENT_TEST}
  ${MT_GUI_LIBS}
  ${MT_WX_LIB}
  ${MT_WX_EXTRA_LIBS}
  ${MT_GL_LIBS})
add_test(NAME Sequence COMMAND ${CURRENT_TEST})
list(APPEND MT_GUI_TESTS ${CURRENT_TEST})

# Server
set(CURRENT_TEST test_Server)
add_executable(${CURRENT_TEST} WIN32 src/MT_GUI/server/test_Server.cpp)
target_link_libraries(${CURRENT_TEST}
  ${MT_GUI_LIBS}
  ${MT_WX_LIB}
  ${MT_WX_EXTRA_LIBS}
  ${MT_GL_LIBS})
set(CURRENT_TEST test_Client)
add_executable(${CURRENT_TEST} WIN32 src/MT_GUI/server/test_Client.cpp)
target_link_libraries(${CURRENT_TEST}
  ${MT_GUI_LIBS}
  ${MT_WX_LIB}
  ${MT_WX_EXTRA_LIBS}
  ${MT_GL_LIBS})

######################################################################
# MT_Robot/io tests
set(CURRENT_TEST test_COMSequence)
add_executable(${CURRENT_TEST} src/MT_Robot/io/test_COMSequence.cpp)
target_link_libraries(${CURRENT_TEST}
  ${MT_GUI_LIBS}
  ${MT_ROBOT_LIBS}
  ${MT_ROBOT_EXTRA_LIBS}
  ${MT_WX_LIB}
  ${MT_WX_EXTRA_LIBS}
  ${MT_GL_LIBS})
add_test(NAME COMSequence COMMAND ${CURRENT_TEST})
ensure_OpenCV(${CURRENT_TEST})
list(APPEND MT_ROBOT_TESTS ${CURRENT_TEST})

include(${MT_ROOT}/cmake/MT_Config.cmake)

add_custom_target(MT_Core_tests
  DEPENDS
  MT_CORE_TESTS)

add_custom_target(MT_GUI_tests
  DEPENDS
  MT_GUI_TESTS)

add_custom_target(MT_Robot_tests
  DEPENDS
  MT_ROBOT_TESTS)
