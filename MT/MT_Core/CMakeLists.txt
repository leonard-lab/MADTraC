##################################################
#
# MT/MT_Core/CMakeLists.txt - cmake file for MT_Core
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
# Target: MT_Core.a
#######################################################
# sources
set(3rdparty_srcs
  ./3rdparty/tinyxml/tinystr.cpp        ./3rdparty/tinyxml/tinystr.h
  ./3rdparty/tinyxml/tinyxml.cpp        ./3rdparty/tinyxml/tinyxml.h       
  ./3rdparty/tinyxml/tinyxmlerror.cpp
  ./3rdparty/tinyxml/tinyxmlparser.cpp)
set(fileio_srcs
  ./fileio/CLFSupport.cpp           ./fileio/CLFSupport.h
  ./fileio/ExperimentDataFile.cpp   ./fileio/ExperimentDataFile.h
  ./fileio/MovieExporter.cpp        ./fileio/MovieExporter.h
  ./fileio/ParticleFile.cpp         ./fileio/ParticleFile.h
  ./fileio/XMLSupport.cpp           ./fileio/XMLSupport.h)
set(gl_srcs
  ./gl/glImageSupport.cpp   ./gl/glImageSupport.h
  ./gl/glSupport.cpp        ./gl/glSupport.h)
set(primitives_srcs
  ./primitives/agent.cpp          ./primitives/agent.h       
  ./primitives/BoundingBox.cpp    ./primitives/BoundingBox.h 
  ./primitives/BufferAgent.cpp    ./primitives/BufferAgent.h 
  ./primitives/Choice.cpp         ./primitives/Choice.h      
  ./primitives/Color.cpp          ./primitives/Color.h       
  ./primitives/DataGroup.cpp      ./primitives/DataGroup.h   
  ./primitives/Matrix.cpp         ./primitives/Matrix.h      
  ./primitives/R3.cpp             ./primitives/R3.h          
  ./primitives/Rectangle.cpp      ./primitives/Rectangle.h   
  ./primitives/ringbuffer.cpp     ./primitives/ringbuffer.h) 
set(support_srcs
  ./support/mathsupport.cpp    ./support/mathsupport.h
  ./support/stringsupport.cpp  ./support/stringsupport.h
  ./support/filesupport.cpp    ./support/filesupport.h
  ./support/kbsupport.cpp      ./support/kbsupport.h
  ./support/OpenCVmath.cpp     ./support/OpenCVmath.h
  ./support/UKF.cpp            ./support/UKF.h)

set(module_name "MT_Core")
set(module_srcs ${3rdparty_srcs} ${fileio_srcs} ${gl_srcs} ${primitives_srcs} ${support_srcs})

# tell cmake this is a static library built from the
#   given sources
if(BUILD_SHARED)
  add_library(${module_name} SHARED ${module_srcs})
  if(OS_X)
    add_custom_command(TARGET ${module_name}
      POST_BUILD
      COMMAND install_name_tool -id @executable_path/../Libraries/lib${module_name}.dylib ${CMAKE_BINARY_DIR}/lib/lib${module_name}.dylib)
  endif(OS_X)
else(BUILD_SHARED)
  add_library(${module_name} STATIC ${module_srcs})
endif(BUILD_SHARED)

target_link_libraries(${module_name} ${OPENGL_LIBRARIES})
if(WITH_CLF)
  target_link_libraries(${module_name} ${CLF_LIB} ${HDF5_LIBRARIES})
endif(WITH_CLF)  

# copies the header files to the "include" directory
# if(NOT MSVC)
#   get_directory_property(defs DIRECTORY . COMPILE_DEFINITIONS)
#   add_custom_command(TARGET MT_Core POST_BUILD
#      COMMAND echo ARGS "\"${defs}\">defs.txt"
#      COMMAND ${CMAKE_SOURCE_DIR}/misc/build_scripts/posix_cmake_target_script.sh ARGS "MT_Core" ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
#   )
# endif(NOT MSVC)

# if(MSVC)
#   string(REPLACE "/" "\\" ARG2 ${CMAKE_SOURCE_DIR})
#   string(REPLACE "/" "\\" ARG3 ${CMAKE_BINARY_DIR})
#   add_custom_command(TARGET MT_Core POST_BUILD
#     COMMAND ${CMAKE_SOURCE_DIR}/misc/build_scripts/MSVC_cmake_post_build.bat ARGS "MT_Core" ${ARG2} ${ARG3})
# endif(MSVC)    

if(HAVE_OPENCV_FRAMEWORK)
  target_link_libraries(${module_name} ${OPENCV_FRAMEWORK})
endif()

include(${CMAKE_SOURCE_DIR}/cmake/MT_BuildFunctions.cmake)
MT_MAKE_MODULE_HEADER(. "${module_name}" "${module_srcs}")
MT_COPY_MODULE_HEADERS("${module_name}" "${module_srcs}" "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/include/MT/${module_name}")
