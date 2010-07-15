/* build_info.h - This file contains doxygen code to
 * generate a page in the MT documentation about building MADTraC.
 *
 * This file is not intended to ever be compiled.
 */


/** @page build_info Build Information
 *
 * This page contains information on building MADTraC for various systems.
 *
 * @section bi_quick Quick Build Instructions
 *
 * @subsection bi_quick_OSX Mac OS X
 * 
 * These instructions are tested on Mac OS X 10.5 and should work on
 * 10.6, but are not yet tested.
 * 
 * The main prerequisites can be installed most easily with <a
 * href="http://www.macports.org">MacPorts</a>.  Open up a terminal
 * and go to wherever you have put the MADTraC code
 * (e.g. "~/src/MADTraC").
 * @code
 * sudo port install cmake
 * sudo port install opencv
 * sudo port install wxWidgets
 * mkdir build
 * cd build
 * ccmake ..
 * @endcode
 * Use the cmake "GUI" (text-based) to configure MADTraC how you
 * want it.  The defaults should be OK.  Hit 'c' to configure until
 * there are no more entries with an asterisk, then hit 'g' to
 * generate makefiles.  Then
 * @code
 * make
 * @endcode
 * Alternatively, you can generate an XCode project with
 * @code
 * ccmake -G "Xcode" ..
 * @endcode
 * This can make things complicated later, though.  It will be OK to
 * use XCode for target applications, though.
 *
 * If installing opencv or wxwidgets fails, you may need to do
 * @code
 * sudo port selfupdate
 * @endcode
 *
 * @todo The main CMakeLists should be configured to allow XCode
 * builds of the libraries without breaking app building.
 * 
 * 
 */

/*
 * The MADTraC build system is now <a
 * href="http://www.cmake.org/cmake/resources/software.html">CMake</a>-based.
 * For most 
 * applications, MADTraC requires you to first download and build <a
 * href="http://wxwidgets.org">wxWidgets</a> and <a
 * href="http://opencv.willowgarage.com/">OpenCV</a>.  Then,
 *   - On Windows,
 *       - Unzip the source somewhere, say c:/src/MADTraC
 *       - Open the CMake GUI application
 *       - Point the source directory to c:/src/MADTraC and the build
 * directory to c:/src/MADTraC/build
 *       - Click configure.  Choose Visual Studio 9 2008 as the generator.
 *       - Configure the build
 *       - Click configure until there are no more red entries (could take a couple tries).
 *       - Click generate.
 *       - Quit the CMake GUI
 *       - Open c:/src/MADTraC/build/MADTraC.sln (e.g. in Visual
 * Studio 2008 Express)
 *       - Select the "Release" configuration
 *       - Build Solution
 *   - On OS X or linux,
 *       - Unzip the source somewhere, say ~/src/MADTraC
 *       - cd ~/src/MADTraC
 *       - mkdir build
 *       - cd build
 *       - ccmake ..  (on OS X you can also do 'ccmake -G "Xcode"
 * ..' to make an xcode project.  This is somewhat experimental.)
 *       - Hit 'c' to configure, configure the build options, 'c'
 * again until there are no more asterisked entries, then 'g' to
 * generate and 'q' to quit.
 *       - make
 *
 * To build an application, take a look at
 * samples/SimpleBWTracker.CMakeLists.  This can be modified pretty
 * easily for another application.
 */
