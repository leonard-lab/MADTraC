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

# MADTRaC information
if($ENV{MT_ROOT})
  set(MT_ROOT $ENV{MT_ROOT})
else($ENV{MT_ROOT})  
  set(MT_ROOT ${CMAKE_BINARY_DIR}/../../build/)
endif($ENV{MT_ROOT})

set(MT_ROOT ${MT_ROOT} CACHE PATH "MADTraC root directory")
include(${MT_ROOT}/cmake/MT_Config.cmake)

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

include(${MT_ROOT}/cmake/MT_Config.cmake)

add_custom_target(MT_Core_tests
  DEPENDS
  MT_CORE_TESTS)

add_custom_target(MT_GUI_tests
  DEPENDS
  MT_GUI_TESTS)
