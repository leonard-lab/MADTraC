/* build_apps.h - This file contains doxygen code to
 * generate a page in the MT documentation about building applications
 * that link against MADTraC.
 *
 * This file is not intended to ever be compiled.
 */


/** @page build_apps Building MADTraC Applications
 *
 * This page contains information on building applications that link
 * against the MADTraC libraries.  It is not intended to tell you how
 * to create such an application (see @ref MT_App_Overview, @ref
 * tracker_tutorial), rather it will give an overview of the actual
 * build process.
 *
 * By far the best way to build a MADTraC application is to first
 * generate project files using CMake and MT_Config.cmake, which gets
 * automatically generated during the library build so that it
 * reflects the proper build settings.
 *
 * In most cases, you can probably simply copy CMakeLists.txt from the
 * samples/SimpleBWTracker directory and edit it as needed.  If you
 * are building on OS X, you'll also want to copy Info.plist.in.
 *
 * If you use the SimpleBWTracker CMakeLists.txt file you'll need to
 *   - Set the "project" name appropriately.  The actual value isn't
 * too important.  Typically this is the same as APP_NAME.
 *   - Set APP_NAME to the name of the application you want to build.
 * This will be the name of the end-result application (i.e. the .exe
 * file or .app bundle). 
 *   - Set APP_SRC to a (space-separated) list of source files for
 * your application.  Note that you will want to list .h as well as
 * .cpp and .c files.  CMake will use this list to generate
 * dependencies - so that when you modify a file it knows what needs
 * to be recompiled.
 *   - If necessary, change @c MT_tracking_app to @c MT_GUI_app or @c
 * MT_robot_app, depending upon what type of project you are building.
 *
 * When running CMake, you will need to specify the location of your
 * MADTraC build directory as MT_ROOT.  On OS X or linux, this can be
 * done by adding @c -DMT_ROOT=~/src/MADTraC/build to the cmake
 * command (modify the path as necessary).  On Windows, simply
 * configure the application once and then change MT_ROOT from the
 * GUI.  You can also set MT_ROOT as an environment variable - this
 * will be used as the initial "guess".
 *
 * Note that if you use the supplied @c MT_GUI_app, @c
 * MT_tracking_app, or @c MT_robot_app macro (as in the example file),
 * on OS X cmake will automatically configure the build as a proper OS
 * X .app bundle.  This includes setting up the Info.plist file and
 * copying over the OpenCV framework if necessary.
 *
 * @section build_apps_ex_posix App Build Example on OS X or linux
 *
 * Suppose that we have unzipped (or checked out) the MADTraC source
 * to "~/src/MADTraC/" and built into "~/src/MADTraC/build/" a la
 * @code
 * cd ~/src/MADTraC
 * mkdir build;  cd build;
 * cmake ..
 * make
 * @endcode
 *
 * Then, to build SimpleBWTracker, do
 * @code
 * cd ~/src/MADTraC/samples/SimpleBWTracker
 * mkdir builde; cd build;
 * cmake .. -DMT_ROOT=~/src/MADTraC/build
 * make
 * @endcode
 *
 * Alternatively, if we want to generate an XCode project, we could do
 * @code
 * cmake -G "Xcode" .. -DMT_ROOT=~/src/MADTraC/build
 * @endcode
 *
 * @section build_apps_ex_windows App Build Example on Windows
 *
 * Supposing that we have unzipped (or checked out) the MADTraC source
 * to "c:/src/MADTraC/" and built into "c:/src/MADTraC/build/" (the
 * build directory in CMake), then built the MADTraC project in Visual
 * Studio.  To build the SimpleBWTracker app we can simply open the
 * CMake GUI back up, point the source directory to
 * "c:/src/MADTraC/samples/SimpleBWTracker/" and the build directory
 * to "c:/src/MADTraC/samples/SimpleBWTracker/build/".
 *
 * Then, when we click configure the first time we'll get errors about
 * not being able to find MT_Config.cmake, but MT_ROOT will show up as
 * a build option.  Set MT_ROOT to "c:/src/MADTraC/build/" and hit
 * configure again until the generate button becomes available, then
 * click generate to create the Visual Studio project.
 *
 * 
 */
