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
  set(MT_CORE_LIBS ${MT_LIBS_DIR}/MT_Core.lib)
  set(MT_GUI_LIBS ${MT_CORE_LIBS} ${MT_LIBS_DIR}/MT_GUI.lib)
  set(MT_TRACKING_LIBS ${MT_GUI_LIBS} ${MT_LIBS_DIR}/MT_Tracking.lib)
  set(MT_ROBOT_LIBS ${MT_TRACKING_LIBS} ${MT_LIBS_DIR}/MT_Robot.lib)
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

#### MT_Tracking extra libraries
set(MT_ROBOT_EXTRA_LIBS "${MT_TRACKING_EXTRA_LIBS}")