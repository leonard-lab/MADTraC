##################################################
#
# CMakeLists.txt - Root cmake file for MADTraC
#
# History:
#   * Original file - Dan Swain, 6/10/10
#   
##################################################

# require cmake version 2.8 since it has a module
#  to find HDF5.  2.8 is available on mac via
#  macports/darwinports
cmake_minimum_required (VERSION 2.8)

# name of the project
project(MADTraC)

# versioning
set(MT_VER_MAJOR 0)
set(MT_VER_MINOR 7)
set(MT_VER_PATCH 0)
set(MT_VERSION "${MT_VER_MAJOR}.${MT_VER_MINOR}.${MT_VER_PATCH}")
math(EXPR MT_VERSION_NUMBER "${MT_VER_MAJOR}*10000 + ${MT_VER_MINOR}*100 + ${MT_VER_PATCH}")

# determine if this is OS X
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(OS_X ON)
endif()

# Compiler flags we want to use for every build:
if(NOT WIN32)
  #  -Wall - see all warnings
  #  -O3   - highest optimization (potentially large files)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -fPIC")
else(NOT WIN32)
  # warnings are very aggressive in VS, so going with W1
  set(CMAKE_C_FFLAGS "${CMAKE_C_FLAGS} /W1")
  set(CMAKE_CXX_FFLAGS "${CMAKE_C_FLAGS} /W1")  
endif(NOT WIN32)

##################################################
#  OpenGL Support
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
  include_directories(${OPENGL_INCLUDE_DIR})
  set(GL_INCLUDE_DIR ${OPENGL_INCLUDE_DIR})
  set(GL_LIBS ${OPENGL_LIBRARIES})
endif(OPENGL_FOUND)

##################################################
#  HDF5 Support
option(WITH_CLF "Build with Couzin Lab File support" OFF)

# Initial guesses on where CLF might be
if(MSVC)    # - windows
  set(CLF_ROOT_GUESS "c:/src/CLF/build")  
else(MSVC)   
  set(CLF_ROOT_GUESS "~/src/CLF/build")
endif(MSVC)
set(CLF_ROOT ${CLF_ROOT_GUESS} CACHE PATH "Path to CLF build.")

# configuration of CLF support
if(WITH_CLF)
  if(MSVC)
    set(HDF5_ROOT "C:/src/hdf5" CACHE PATH "HDF5 Path")
    if((IS_DIRECTORY ${HDF5_ROOT})
      AND (IS_DIRECTORY ${HDF5_ROOT}/include)
      AND (IS_DIRECTORY ${HDF5_ROOT}/dll)
      AND (EXISTS ${HDF5_ROOT}/dll/hdf5dll.lib))
      set(HDF5_INCLUDE_DIR ${HDF5_ROOT}/include)
      set(HDF5_LIBRARY_DIRS ${HDF5_ROOT}/dll)
      set(HDF5_LIBRARIES ${HDF5_ROOT}/dll/hdf5dll.lib)
      set(HDF5_ZLIB "c:/src/zlib/build/Release/zlib.lib" CACHE PATH "Path to zlib.lib")
      set(HDF5_SZIP "c:/src/szip/dll/szip.lib" CACHE PATH "Path to szip.lib")
      if(NOT EXISTS ${HDF5_ZLIB})
        message(SEND_ERROR "Set HDF5_ZLIB to the path to zlib.lib")
      endif()
      if(NOT EXISTS ${HDF5_SZIP})
        message(SEND_ERROR "Set HDF5_SZIP to the path to szip.lib")
      endif()
      #set(HDF5_EXTRA_LIBS ${HDF5_ZLIB} ${HDF5_SZIP})
    else()
      message(SEND_ERROR "Set HDF5_ROOT to a directory containing include and dll directories")
    endif()
  else(MSVC)
    # flag that we want to link statically against HDF5 libraries
    #   - this makes binaries slightly larger but makes binaries
    #     much more portable
    set(HDF5_USE_STATIC_LIBRARIES 1)
    # finds HDF5
    find_package(HDF5 REQUIRED)
  endif(MSVC)
  
  # make sure the specified CLF_ROOT exists and has the necessary
  #  contents
  if(MSVC)
    if((IS_DIRECTORY ${CLF_ROOT})
        AND (EXISTS ${CLF_ROOT}/include/CLFpp.h)
        AND (EXISTS ${CLF_ROOT}/include/CLF.h)
	AND (EXISTS ${CLF_ROOT}/lib/Release/CLFpp.lib))
      set(CLF_LIB ${CLF_ROOT}/lib/Release/CLFpp.lib)
      set(CLF_INCLUDE ${CLF_ROOT}/include)
      add_definitions(-DMT_HAVE_CLF)
      include_directories(${CLF_INCLUDE} ${HDF5_INCLUDE_DIR})
    else()
      message(SEND_ERROR "CLF directory not found.  Either set CLF_ROOT to a directory containing lib/Release/CLFpp.lib, include/CLF.h, and include/CLFpp.h, or turn off WITH_CLF.")
    endif()
  else(MSVC)
  if((IS_DIRECTORY ${CLF_ROOT})
      AND (EXISTS ${CLF_ROOT}/lib/libCLFpp.a)
      AND (EXISTS ${CLF_ROOT}/include/CLFpp.h)
      AND (EXISTS ${CLF_ROOT}/include/CLF.h))
    set(CLF_LIB ${CLF_ROOT}/lib/libCLFpp.a)
    set(CLF_INCLUDE ${CLF_ROOT}/include)
    add_definitions(-DMT_HAVE_CLF)
    include_directories(${CLF_INCLUDE} ${HDF5_INCLUDE_DIR})
  else()
    # if the files couldn't be found, flag an error
    message(SEND_ERROR "CLF directory not found.  Either set CLF_ROOT to a directory containing lib/libCLFpp.a, include/CLF.h, and include/CLFpp.h, or turn off WITH_CLF.")
  endif()
  endif(MSVC)
else(WITH_CLF)
  # remove the CLF directory from the GUI
  unset(CLF_ROOT CACHE)
  add_definitions(-DMT_NO_CLF)
endif(WITH_CLF)

##################################################
#  OpenCV Support
option(WITH_OPENCV "Build with OpenCV support (HIGHLY recommended)" ON)

unset(OPENCV_INC CACHE)
unset(OPENCV_LIBS_DIR CACHE)
unset(OPENCV_LIBS_TMP CACHE)
unset(OPENCV_LIBS CACHE)

if(WITH_OPENCV)
  if(OS_X)
    set(OPENCV_FRAMEWORK "${CMAKE_SOURCE_DIR}/OpenCV.framework" CACHE PATH "Path to OpenCV framework")
    if(IS_DIRECTORY ${OPENCV_FRAMEWORK})
      set(HAVE_OPENCV_FRAMEWORK ON)
      set(OPENCV_INC ${OPENCV_FRAMEWORK})
      set(OPENCV_LIBS_DIR "")
      set(OPENCV_LIBS ${OPENCV_FRAMEWORK})
      add_definitions(-DMT_HAVE_OPENCV_FRAMEWORK)
    endif(IS_DIRECTORY ${OPENCV_FRAMEWORK})
  endif(OS_X)
  
  if(NOT MSVC)
    if(NOT HAVE_OPENCV_FRAMEWORK)
      set(CV_NOT_FOUND ON)
      
      find_package(PkgConfig)
      if(PKG_CONFIG_FOUND)
        pkg_check_modules(CV opencv)
        if(CV_FOUND)
          set(CV_NOT_FOUND OFF)
          set(OPENCV_INC ${CV_INCLUDE_DIRS})
          set(OPENCV_LIBS_DIR ${CV_LIBRARY_DIRS})
          set(OPENCV_LIBS ${CV_LIBRARIES})
#          set(OPENCV_LIBS)
#          foreach(l IN LISTS OPENCV_LIBS_TMP)
#            list(APPEND OPENCV_LIBS ${OPENCV_LIBS_DIR}/lib${l}.dylib)
#          endforeach()
        endif(CV_FOUND)
      endif(PKG_CONFIG_FOUND)

      if(CV_NOT_FOUND)
        set(OPENCV_INC /usr/local/include)
        set(OPENCV_LIBS_DIR /usr/local/lib)
        set(OPENCV_LIBS_TMP cxcore cv highgui cvaux ml)
        foreach(l IN LISTS OPENCV_LIBS_TMP)
          list(APPEND OPENCV_LIBS ${OPENCV_LIBS_DIR}/lib${l}.la)
        endforeach()
        message(WARNING "Made a guess at OpenCV configuration.  You should check the values of OPENCV_INC and OPENCV_LIBS")
      endif(CV_NOT_FOUND)  
      
    endif(NOT HAVE_OPENCV_FRAMEWORK)

  else(NOT MSVC)
    set(CV_NOT_FOUND ON)

    set(OpenCV_DIR "c:/src/OpenCV-2.1.0/build" CACHE PATH "Path to OpenCV build directory - should include OpenCVConfig.cmake.")
    
    find_package(OpenCV REQUIRED)
    if(OpenCV_FOUND)
      set(CV_NOT_FOUND OFF)
      set(OPENCV_INC ${OpenCV_INCLUDE_DIRS})
      set(OPENCV_LIBS_DIR ${OpenCV_LIB_DIR})
      set(OPENCV_LIBS_TMP ${OpenCV_LIBS})
      # filter out extra information in the list, matches on e.g. cv210.lib,
      #   cv200.lib, cv.lib, but not cv210d.lib and not release, debug, etc.
      foreach(l IN LISTS OPENCV_LIBS_TMP)
        if(("${l}" MATCHES "^cv*") OR
            ("${l}" MATCHES "^cvaux*") OR
            ("${1}" MATCHES "^cxcore*") OR
            ("${l}" MATCHES "^highgui*") OR
            ("${l}" MATCHES "^ml*") OR
            ("${l}" MATCHES "^opencv_ffmpeg*") )
          if(NOT "${l}" MATCHES "d$")
            list(APPEND OPENCV_LIBS ${OPENCV_LIBS_DIR}/Release/${l}.lib)
          endif()
        endif()
      endforeach()
    endif(OpenCV_FOUND)

    if(CV_NOT_FOUND)
      set(OPENCV_ROOT "C:/src/OpenCV/" CACHE PATH "OpenCV Root Directory")
      if(NOT IS_DIRECTORY ${OPENCV_ROOT})
        message(SEND_ERROR "Set OPENCV_ROOT to the OpenCV root directory")
      endif()
      set(OPENCV_INC ${OPENCV_ROOT}/cv/include)
      set(OPENCV_LIBS_DIR ${OPENCV_ROOT}/lib)
      if(EXISTS ${OPENCV_LIBS_DIR}/cxcore.lib)
        set(OPENCV_LIBS
        ${OPENCV_LIBS_DIR}/cxcore.lib
        ${OPENCV_LIBS_DIR}/highgui.lib
        ${OPENCV_LIBS_DIR}/cv.lib
        ${OPENCV_LIBS_DIR}/cvaux.lib)        
      else()
        if(EXISTS ${OPENCV_LIB_DIR}/cxcore200.lib)
          set(OPENCV_LIBS
            ${OPENCV_LIBS_DIR}/cxcore200.lib
            ${OPENCV_LIBS_DIR}/highgui200.lib
            ${OPENCV_LIBS_DIR}/cv200.lib
            ${OPENCV_LIBS_DIR}/cvaux200.lib)        
        else()
          message("You will need to manually set up OpenCV link directories in your project.")
        endif()
      endif()
    endif(CV_NOT_FOUND)
  endif(NOT MSVC)  

  set(OPENCV_INC ${OPENCV_INC} CACHE PATH "OpenCV include directory")
  set(OPENCV_LIBS_DIR ${OPENCV_LIBS_DIR} CACHE PATH
    "OpenCV libraries directory (not needed if OPENCV_LIBS include full paths)")
  set(OPENCV_LIBS ${OPENCV_LIBS} CACHE PATH "OpenCV libraries")
  include_directories(${OPENCV_INC})
  
else(WITH_OPENCV)
  unset(OPENCV_FRAMEWORK CACHE)
  unset(OPENCV_ROOT CACHE)
  add_definitions(-DMT_NO_OPENCV)
endif(WITH_OPENCV)

##################################################
#  wxWidgets Support
option(AUTO_FIND_WX "Automatically find wxWidgets on the system.  Otherwise look in WX_ROOT." ON)

if(NOT AUTO_FIND_WX)
  unset(wxWidgets_CONFIG_EXECUTABLE CACHE)
  unset(wxWidgets_wxrc_EXECUTABLE CACHE)
  if(OS_X)
    # NOTE This assumes you built wx with
    #   ./configure --enable-opengl --enable-monolithic
    # and the resulting files are in ~/src/wxLib
    set(WX_ROOT "~/src/wxLib" CACHE PATH "Path to wxWidgets library.")
    if((IS_DIRECTORY ${WX_ROOT})
        AND (EXISTS ${WX_ROOT}/lib/libwx_mac-2.8.a)
        AND (IS_DIRECTORY ${WX_ROOT}/include))
      set(WX_LIB ${WX_ROOT}/lib/libwx_mac-2.8.a ${WX_ROOT}/lib/libwx_mac_gl-2.8.a ${WX_ROOT}/lib/libwxjpeg-2.8.a ${WX_ROOT}/lib/libwxpng-2.8.a ${WX_ROOT}/lib/libwxtiff-2.8.a)
      set(FMWK_PATH "${CMAKE_OSX_SYSROOT}/System/Library/Frameworks")
      set(AGL_FMWK "${FMWK_PATH}/AGL.framework")
      set(QT_FMWK "${FMWK_PATH}/QuickTime.framework")
      set(AppKit_FMWK "${FMWK_PATH}/AppKit.framework")
      set(Carbon_FMWK "${FMWK_PATH}/Carbon.framework")
      set(IOKit_FMWK "${FMWK_PATH}/IOKit.framework")
      # set(LIB_iconv /opt/local/lib/libiconv.a)
      # set(LIB_z /opt/local/lib/libz.a)
      set(LIB_iconv /usr/lib/libiconv.dylib)
      set(LIB_z /usr/lib/libz.dylib)
      set(WX_EXTRA_LIBS "${LIB_iconv};${LIB_z};${AGL_FMWK};${QT_FMWK};${AppKit_FMWK};${Carbon_FMWK};${IOKit_FMWK}")
      set(WX_INCLUDE ${WX_ROOT}/include ${WX_ROOT}/lib/wx/include/mac-ansi-release-static-2.8 ${WX_ROOT}/contrib/include)
      set(WX_DEFS -D__WX__ -DNO_GCC_PRAGMA -D__WXMAC__ -DwxUSE_BASE=1 -D
        _FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXMAC_XCODE__=1)
    else()
      message(SEND_ERROR "wxWidgets was not found.  Point WX_ROOT at a directory that includes lib/libwx_mac_static.a and include.")
    endif()
  endif(OS_X)
else(NOT AUTO_FIND_WX)
  unset(WX_ROOT CACHE)
  if(NOT MSVC)
    set(wxWidgets_USE_STATIC ON)
    set(wxWidgets_USE_DEBUG OFF)
    set(wxWidgets_USE_UNICODE OFF)
  endif(NOT MSVC)

  if(MSVC)
    find_path(WX_ROOT
      NAMES include/wx/wx.h
      PATHS
        $ENV{wxWidgets_ROOT_DIR}
        $ENV{WXWIN}
        c:/
        d:/
        c:/src
        d:/src
        $ENV{ProgramFiles}
      PATH_SUFFIXES
        wxWidgets-2.8.10
        wxWidgets-2.8.11
        wxWidgets-2.9.0
        wxWidgets
    )
    set(WX_ROOT "${WX_ROOT}" CACHE PATH "wxWidgets root path")
    set(ENV{WXWIN} "${WX_ROOT}")
  endif(MSVC)
  
  find_package(wxWidgets COMPONENTS core base gl net adv)

  if(wxWidgets_FOUND)
    set(WX_INCLUDE ${wxWidgets_INCLUDE_DIRS})
    set(WX_LIB ${wxWidgets_LIBRARIES})
    set(WX_DEFS_TMP ${wxWidgets_DEFINITIONS})
    foreach(d IN LISTS WX_DEFS_TMP)
      list(APPEND WX_DEFS -D${d})
    endforeach()
  endif(wxWidgets_FOUND)
endif(NOT AUTO_FIND_WX)

set(WX_INCLUDE ${WX_INCLUDE} CACHE PATH "wxWidgets include directory")
set(WX_LIB ${WX_LIB} CACHE PATH "wxWidgets libraries")
set(WX_DEFS ${WX_DEFS} CACHE STRING "wxWidgets preprocessor definitions")

##################################################
# ARToolKit Support
if(OS_X)
  option(WITH_ARTK "Build with ARToolKit support" OFF)
else(OS_X)
  option(WITH_ARTK "Build with ARToolKit support" OFF)
endif(OS_X)

if(WITH_ARTK)
  if(OS_X)
    set(ARTK_ROOT "~/src/ARlib" CACHE PATH "Path to ARToolKit library")
    if((IS_DIRECTORY ${ARTK_ROOT})
        AND (EXISTS ${ARTK_ROOT}/lib/libAR.a)
        AND (EXISTS ${ARTK_ROOT}/lib/libARvideo.a)
        AND (IS_DIRECTORY ${ARTK_ROOT}/include))
      set(ARTK_LIB ${ARTK_ROOT}/lib/libAR.a ${ARTK_ROOT}/lib/libARvideo.a)
      set(ARTK_INCLUDE ${ARTK_ROOT}/include)
      add_definitions(-DMT_HAVE_ARTK)
    else()
      message(SEND_ERROR "ARToolKit was not found.  Point WX_ROOT at a directory that includes lib/libAR.a, lib/libARvideo.a, and include.")
      unset(ARTK_LIB CACHE)
      unset(ARTK_INCLUDE CACHE)
    endif()
  else(OS_X)
    set(ARTK_ROOT "" CACHE PATH "Path to ARToolKit library")
    set(ARTK_LIB "" CACHE PATH "ARToolKit libraries")
    set(ARTK_INCLUDE "" CACHE PATH "ARToolKit Includes")
    add_definitions(-DMT_HAVE_ARTK)
    if((NOT IS_DIRECTORY ${ARTK_INCLUDE})
        OR (NOT EXISTS ${ARTK_LIB}))
      message(WARNING "You need to specify ARTK_LIB and ARTK_INCLUDE manually.")
    endif()
  endif(OS_X)
endif(WITH_ARTK)

##################################################
# AVT Support
if(MSVC)
  option(WITH_AVT "Build with Allied Vision Tech camera support (needs AVTFirePackage)" OFF)

  if(WITH_AVT)
    find_path(AVT_ROOT
      NAMES FireGrab/Lib/FGCamera.h
      PATHS
        "c:/Program Files/"
        "c:/Program Files (x86)/"
      PATH_SUFFIXES
        "intek/FirePackage64"
        "intek/FirePackage64 2v34/"
    )
    set(AVT_ROOT "${AVT_ROOT}" CACHE PATH "AVT API Root")
    if(IS_DIRECTORY "${AVT_ROOT}/FireGrab/Lib32")
      set(AVT_LIB "${AVT_ROOT}/FireGrab/Lib32/FGCamera.lib")
    else()
      set(AVT_LIB "${AVT_ROOT}/FireGrab/Lib/FGCamera.lib")
    endif()
    set(AVT_LIB "${AVT_LIB}" CACHE STRING "AVT Library")
    set(AVT_INC "${AVT_ROOT}/FireGrab/Lib/" CACHE PATH "AVT Include Directory")
    add_definitions(-DMT_HAVE_AVT)
    include_directories("${AVT_INC}")
      
  endif(WITH_AVT)
  
endif(MSVC)

if(MSVC)
  option(MT_DEBUG_CONSOLE "Enable debug console for printf output" OFF)
  if(MT_DEBUG_CONSOLE)
    add_definitions(-DMT_WIN32_DEBUG_CONSOLE)
  endif(MT_DEBUG_CONSOLE)
endif(MSVC)  

if(OS_X)
  option(MT_USE_WX_JOYSTICK "Use WX Joystick API" OFF)
  if(MT_USE_WX_JOYSTICK)
    add_definitions(-DMT_GAMEPAD_USE_WX)
  endif(MT_USE_WX_JOYSTICK)
endif(OS_X)

include_directories(.)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(BUILD_SHARED OFF CACHE BOOL "Build as shared libs (experimental)")

if(OS_X)
  set(CMAKE_INSTALL_NAME_DIR "@executable_path/../Frameworks/")
endif(OS_X)  

set(BUILD_GUI ON CACHE BOOL "Build MT_GUI")
if(BUILD_GUI)
  set(BUILD_TRACKING ON CACHE BOOL "Build MT_Tracking")
  if(BUILD_TRACKING)
    set(BUILD_ROBOT ON CACHE BOOL "Build MT_Robot")
  else(BUILD_TRACKING)
    unset(BUILD_ROBOT)
  endif(BUILD_TRACKING)
else(BUILD_GUI)
  unset(BUILD_TRACKING CACHE)
endif(BUILD_GUI)

add_subdirectory(MT/MT_Core)
if(BUILD_GUI)
  add_subdirectory(MT/MT_GUI)
  add_dependencies(MT_GUI MT_Core)
  if(BUILD_TRACKING)
    add_subdirectory(MT/MT_Tracking)
    add_dependencies(MT_Tracking MT_GUI)
    if(BUILD_ROBOT)
      add_subdirectory(MT/MT_Robot)
      add_dependencies(MT_Robot MT_Tracking)
    endif(BUILD_ROBOT)
  endif(BUILD_TRACKING)
endif(BUILD_GUI)

# if(OS_X)
#   set(BUILD_FOR_TIGER OFF CACHE BOOL "Build for OS X 10.4")
#   if(BUILD_FOR_TIGER)
#     set(CMAKE_OSX_DEPLOYMENT_TARGET "10.4")
#     set(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.4u.sdk")
#     set(WITH_OPENCV OFF)
#     set(WITH_ARTK OFF)
#     set(WITH_CLF OFF)
#   else(BUILD_FOR_TIGER)
#     set(CMAKE_OSX_DEPLOYMENT_TARGET "")
#     set(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.5.sdk")
#   endif(BUILD_FOR_TIGER)
# endif(OS_X)  
  

if(OS_X)
  add_custom_target(MT ALL
    COMMAND
      ${CMAKE_COMMAND} -E
        copy ${CMAKE_SOURCE_DIR}/MT/MT.h ${CMAKE_BINARY_DIR}/MT/MT.h
    DEPENDS MT_Core
    )
  if(BUILD_GUI)
    add_dependencies(MT MT_GUI)
  endif()
  if(BUILD_TRACKING)
    add_dependencies(MT MT_Tracking)
  endif()
  if(BUILD_ROBOT)
    add_dependencies(MT MT_Robot)
  endif()
endif(OS_X)

set(MT_INCLUDE_INSTALL_DIR ${CMAKE_BINARY_DIR}/include)
set(MT_LIBS_INSTALL_DIR ${CMAKE_BINARY_DIR}/lib)
configure_file(cmake/MT_Config.cmake cmake/MT_Config.cmake COPYONLY)
configure_file(cmake/MT_Exports.cmake.in cmake/MT_Exports.cmake)
