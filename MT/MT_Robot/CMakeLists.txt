##################################################
#
# MT/MT_Robot/CMakeLists.txt - cmake file for MT_Robot
#
# History:
#   * Original file - Dan Swain, 6/10/10
#   
##################################################

# require cmake version 2.8 since it has a module
#  to find HDF5.  2.8 is available on mac via
#  macports/darwinports
cmake_minimum_required (VERSION 2.8)

#######################################################
# Target: MT_GUI.a
#######################################################
# sources
if(WIN32)
  set(WinCOM_srcs
    ./3rdparty/Win32Com/Serial.cpp    ./3rdparty/Win32Com/Serial.h     
    ./3rdparty/Win32Com/SerialEx.cpp  ./3rdparty/Win32Com/SerialEx.h
    ./3rdparty/Win32Com/SerialWnd.cpp ./3rdparty/Win32Com/SerialWnd.h) 
else(WIN32)
  set(WinCOM_srcs)
endif(WIN32)

set(base_srcs
  ./base/MT_RobotFrameBase.cpp  ./base/MT_RobotFrameBase.h)
set(dialogs_srcs
  ./dialogs/MT_JoyStickFrame.cpp      ./dialogs/MT_JoyStickFrame.h
  ./dialogs/MT_RobotCommandDialog.cpp ./dialogs/MT_RobotCommandDialog.h
  ./dialogs/MT_RobotConnectDialog.cpp ./dialogs/MT_RobotConnectDialog.h) 
set(io_srcs
  ./io/ComIO.cpp    ./io/ComIO.h
  ./io/gamepad.cpp  ./io/gamepad.h
  ./io/COMSequence.cpp  ./io/COMSequence.h) 
set(robot_srcs
  ./robot/RobotBase.cpp          ./robot/RobotBase.h
  ./robot/AllRobotContainer.cpp  ./robot/AllRobotContainer.h
  ./robot/GamePadController.cpp  ./robot/GamePadController.h
  ./robot/MiaBotPro.cpp          ./robot/MiaBotPro.h   
  ./robot/SteeredRobot.cpp       ./robot/SteeredRobot.h)

include_directories(${WX_INCLUDE})
add_definitions(${WX_DEFS})

set(module_name "MT_Robot")
set(module_srcs ${WinCOM_srcs} ${base_srcs} ${dialogs_srcs} ${io_srcs} ${robot_srcs})

if(WITH_ARTK)
  include_directories(${ARTK_INCLUDE})
  add_definitions(-DMT_HAVE_ARTOOLKIT)
endif(WITH_ARTK)

# tell cmake this is a static library built from the
#   given sources
if(BUILD_SHARED)
  add_library(${module_name} SHARED ${module_srcs})
  target_link_libraries(${module_name} MT_Core)
  target_link_libraries(${module_name} MT_GUI)
  target_link_libraries(${module_name} MT_Tracking)
  target_link_libraries(${module_name} ${WX_EXTRA_LIBS})
else(BUILD_SHARED)  
  add_library(${module_name} STATIC ${module_srcs})
endif(BUILD_SHARED)


# copies the header files to the "include" directory
# if(NOT MSVC)
#   get_directory_property(defs DIRECTORY . COMPILE_DEFINITIONS)
#   add_custom_command(TARGET MT_Robot POST_BUILD
#     COMMAND echo ARGS "\"${defs}\">defs.txt"
#     COMMAND ${CMAKE_SOURCE_DIR}/misc/build_scripts/posix_cmake_target_script.sh ARGS "MT_Robot" ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
#     )
# endif(NOT MSVC)

# if(MSVC)
#   string(REPLACE "/" "\\" ARG2 ${CMAKE_SOURCE_DIR})
#   string(REPLACE "/" "\\" ARG3 ${CMAKE_BINARY_DIR})
#   add_custom_command(TARGET MT_Robot POST_BUILD
#     COMMAND ${CMAKE_SOURCE_DIR}/misc/build_scripts/MSVC_cmake_post_build.bat ARGS "MT_Robot" ${ARG2} ${ARG3})
# endif(MSVC)

if(HAVE_OPENCV_FRAMEWORK)
  target_link_libraries(${module_name} ${OPENCV_FRAMEWORK})
endif()

include(${CMAKE_SOURCE_DIR}/cmake/MT_BuildFunctions.cmake)
MT_MAKE_MODULE_HEADER(. "${module_name}" "${module_srcs}")
MT_COPY_MODULE_HEADERS("${module_srcs}" "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/include/MT/${module_name}")

