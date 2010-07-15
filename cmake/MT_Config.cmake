######################################################################
# MT_Config.cmake - CMake include file for projects built using
#    MADTraC.  Aims to simplify creating CMakeLists.txt files for
#    the target projects.
#
# History: Created Dan Swain 6/11/10

######################################################################
# MT_Exports gets configured by CMake to reflect the build environment,
#  i.e. with flags to let us know what's available and paths configured
#  to the local machine
include(${MT_ROOT}/cmake/MT_Exports.cmake)

# MT-specific Configuragion
if(NOT IS_DIRECTORY ${MT_INCLUDE})
  message(SEND_ERROR "Couldn't find MADTraC includes")
endif(NOT IS_DIRECTORY ${MT_INCLUDE})
if(NOT IS_DIRECTORY ${MT_LIBS_DIR})
  message(SEND_ERROR "Couldn't find MADTraC libraries directory")
endif(NOT IS_DIRECTORY ${MT_LIBS_DIR})

######################################################################
# Include directories
include_directories(${MT_INCLUDE})
include_directories(${MT_WX_INCLUDE})
include_directories(${MT_GL_INCLUDE_DIR})
if(MT_HAVE_CLF)
  include_directories(${MT_CLF_INCLUDE} ${MT_HDF5_INCLUDE})
endif(MT_HAVE_CLF)  
if(MT_HAVE_ARTK)
  include_directories(${MT_ARTK_INCLUDE})
endif(MT_HAVE_ARTK)
if(MT_HAVE_OPENCV)
  include_directories(${MT_OPENCV_INC})
endif(MT_HAVE_OPENCV)
if(MT_HAVE_AVT)
  include_directories(${MT_AVT_INC})
endif(MT_HAVE_AVT)  

######################################################################
# Preprocessor definitions
if(MT_HAVE_CLF)
  add_definitions(-DMT_HAVE_CLF)
else(MT_HAVE_CLF)
  add_definitions(-DMT_NO_CLF)
endif(MT_HAVE_CLF)
if(MT_HAVE_ARTK)
  add_definitions(-DMT_HAVE_ARTK)
endif(MT_HAVE_ARTK)
if(MT_HAVE_OPENCV_FRAMEWORK)
  add_definitions(-DMT_HAVE_OPENCV_FRAMEWORK)
endif(MT_HAVE_OPENCV_FRAMEWORK)
if(MT_HAVE_OPENCV)
  add_definitions(-DMT_HAVE_OPENCV)
endif(MT_HAVE_OPENCV)
if(MT_HAVE_AVT)
  add_definitions(-DMT_HAVE_AVT)
endif(MT_HAVE_AVT)  
if(MT_USE_WX_JOYSTICK)
  add_definitions(-DMT_GAMEPAD_USE_WX)
endif(MT_USE_WX_JOYSTICK)

######################################################################
# Library setup

##### MT Libraries
if(NOT MSVC)
  set(MT_CORE_LIBS ${MT_LIBS_DIR}/libMT_Core.a)
  set(MT_GUI_LIBS ${MT_CORE_LIBS} ${MT_LIBS_DIR}/libMT_GUI.a)
  set(MT_TRACKING_LIBS ${MT_GUI_LIBS} ${MT_LIBS_DIR}/libMT_Tracking.a)
  set(MT_ROBOT_LIBS ${MT_TRACKING_LIBS} ${MT_LIBS_DIR}/libMT_Robot.a)
else(NOT MSVC)
  set(MT_CORE_LIBS ${MT_LIBS_DIR}/Release/MT_Core.lib)
  set(MT_GUI_LIBS ${MT_CORE_LIBS} ${MT_LIBS_DIR}/Release/MT_GUI.lib)
  set(MT_TRACKING_LIBS ${MT_GUI_LIBS} ${MT_LIBS_DIR}/Release/MT_Tracking.lib)
  set(MT_ROBOT_LIBS ${MT_TRACKING_LIBS} ${MT_LIBS_DIR}/Release/MT_Robot.lib)
endif(NOT MSVC)  

##### MT_Core extra libraries
if(MT_HAVE_OPENCV)
  set(MT_CORE_EXTRA_LIBS "${MT_CORE_EXTRA_LIBS};${MT_OPENCV_LIBS}")
endif(MT_HAVE_OPENCV)
set(MT_CORE_EXTRA_LIBS "${MT_CORE_EXTRA_LIBS};${MT_GL_LIBS}")

if(MT_HAVE_CLF)
  set(MT_CORE_EXTRA_LIBS "${MT_CORE_EXTRA_LIBS};${MT_HDF5_LIB};${MT_CLF_LIB}")
  if(MSVC)
    add_definitions(-D_HDF5USEDLL_)
    set(MT_CORE_EXTRA_LIBS "${MT_CORE_EXTRA_LIBS};${MT_HDF5_EXTRA_LIBS}")
  endif(MSVC)
endif(MT_HAVE_CLF)

#### MT_GUI extra libraries
set(MT_GUI_EXTRA_LIBS "${MT_CORE_EXTRA_LIBS}")
set(MT_GUI_EXTRA_LIBS "${MT_GUI_EXTRA_LIBS};${MT_WX_LIB};${MT_WX_EXTRA_LIBS}")

#### MT_Tracking extra libraries
set(MT_TRACKING_EXTRA_LIBS "${MT_GUI_EXTRA_LIBS}")

if(MT_HAVE_ARTK)
  set(MT_TRACKING_EXTRA_LIBS "${MT_TRACKING_EXTRA_LIBS};${MT_ARTK_LIB}")
endif(MT_HAVE_ARTK)

if(MT_HAVE_AVT)
  set(MT_TRACKING_EXTRA_LIBS "${MT_TRACKING_EXTRA_LIBS};${MT_AVT_LIB}")
endif(MT_HAVE_AVT)  

#### MT_Robot extra libraries
set(MT_ROBOT_EXTRA_LIBS "${MT_TRACKING_EXTRA_LIBS}")

######################################################################
# Helper functions

#### Building OpenCV framework into a project if necessary
# 1. Checks if Info.plist.in exists in the source path,
#      if so uses it to generate the application's Info.plist
# 2. If MT was built with an OpenCV framework, copies the framework
#      to the proper path (which depends upon the generator and
#      build configuration
function(MT_osx_app_copy_step APP_NAME)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    
    # handle the Info.plist, if Info.plist.in exists
    if(EXISTS "${CMAKE_SOUCE_DIR}/Info.plist.in")
      SET_TARGET_PROPERTIES(${APP_NAME}
        PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist.in)
    endif(EXISTS "${CMAKE_SOUCE_DIR}/Info.plist.in")
    
    # copy OpenCV.framework into the bundle if necessary
    if(MT_HAVE_OPENCV AND MT_OPENCV_FRAMEWORK)

      # the final location is different if we're building with XCode
      if(${CMAKE_GENERATOR} MATCHES "Xcode")
        set(FRAMEWORK_DEST "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
      else(${CMAKE_GENERATOR} MATCHES "Xcode")
        set(FRAMEWORK_DEST "${CMAKE_BINARY_DIR}")      
      endif(${CMAKE_GENERATOR} MATCHES "Xcode")
      
      add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${MT_OPENCV_FRAMEWORK} ${FRAMEWORK_DEST}/${APP_NAME}.app/Contents/Frameworks/OpenCV.framework)
      
    endif(MT_HAVE_OPENCV AND MT_OPENCV_FRAMEWORK)
    
  endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
endfunction(MT_osx_app_copy_step)


#### Linking functions
function(MT_link_GUI_app APP_NAME)
  target_link_libraries(${APP_NAME} ${MT_GUI_LIBS} ${MT_GUI_EXTRA_LIBS})
endfunction(MT_link_GUI_app)  

function(MT_link_tracking_app APP_NAME)
  target_link_libraries(${APP_NAME} ${MT_TRACKING_LIBS} ${MT_TRACKING_EXTRA_LIBS})
endfunction(MT_link_tracking_app)  

function(MT_link_robot_app APP_NAME)
  target_link_libraries(${APP_NAME} ${MT_ROBOT_LIBS} ${MT_ROBOT_EXTRA_LIBS})
endfunction(MT_link_robot_app)

#### Blanket functions
function(MT_GUI_app APP_NAME)
  MT_link_GUI_app(${APP_NAME})
  MT_osx_app_copy_step(${APP_NAME})
endfunction(MT_GUI_app)  

function(MT_tracking_app APP_NAME)
  MT_link_tracking_app(${APP_NAME})
  MT_osx_app_copy_step(${APP_NAME})
endfunction(MT_tracking_app)  

function(MT_robot_app APP_NAME)
  MT_link_robot_app(${APP_NAME})
  MT_osx_app_copy_step(${APP_NAME})
endfunction(MT_robot_app)  