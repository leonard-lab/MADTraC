/* build_info.h - This file contains doxygen code to
 * generate a page in the MT documentation about building MADTraC.
 *
 * This file is not intended to ever be compiled.
 */


/** @page build_info Build Information
 *
 * This page contains information on building MADTraC for various
 * systems.  These instructions are for the libraries themselves.  See
 * @ref build_apps for information on building apps that use the
 * MADTraC libraries.
 *
 * See @ref cmake_config for a list of CMake build options for MADTraC.
 *
 * @ref bi_quick
 *   - @ref bi_quick_OSX
 *   - @ref bi_quick_WIN
 *   - @ref bi_quick_linux
 *
 * @section bi_quick Build Instructions
 *
 * @subsection bi_quick_OSX Mac OS X
 * 
 * @par Supported Systems
 *
 * These instructions are tested on Mac OS X 10.5 and should work on
 * 10.6, but are not yet tested.  You of course need the <a
 * href="http://developer.apple.com/technologies/tools/">Apple
 * Developer Tools</a> (i.e. XCode) installed.
 * 
 * @par Prerequisites
 * 
 * The main prerequisites can be installed most easily with <a
 * href="http://www.macports.org">MacPorts</a>.
 *
 * If you have previously installed MacPorts on your system, you may
 * want to make sure that it is up-to-date by running <tt>sudo port
 * selfupdate</tt>.  Similarly, if the version of cmake you have is
 * not at least 2.8.1 (run <tt>cmake --version</tt>), update macports
 * and then run <tt>sudo port upgrade cmake</tt>.
 * 
 * To install cmake, OpenCV, and wxWidgets using MacPorts, simply
 * execute the following commands from a terminal.
 * @code
 * sudo port install cmake
 * sudo port install opencv
 * sudo port install wxWidgets
 * @endcode
 *
 * It's also possible to build MADTraC if you've installed MacPorts
 * and wxWidgets from source, you will just need to make sure that the
 * build configuration has the proper definitions, include
 * directories, and libraries.  See @ref cmake_config.
 *
 * MADTraC can also be configured to build against some other external
 * libraries, including ARToolKit, CLF, and HDF5.  See @ref cmake_config
 *
 * @par Build
 *
 * With cmake, OpenCV, and wxWidgets installed, go to the directory
 * where you unzipped or checked out (e.g. "~/src/MADTraC/") the
 * MADTraC source and 
 * @code
 * mkdir build
 * cd build
 * ccmake ..
 * @endcode
 * Use the cmake "GUI" (text-based) to configure MADTraC how you
 * want it.  The defaults should be OK.  Hit 'c' to configure until
 * there are no more entries with an asterisk, then hit 'g' to
 * generate makefiles.  Then 'q' to quit and at the terminal do
 * @code
 * make
 * @endcode
 *
 * @subsection bi_quick_WIN Windows (Visual Studio 2008)
 *
 * @note Detailed instructions are included in the VS2008Notes.txt
 * file in the MADTraC root directory.
 *
 * @par Supported Systems
 *
 * MADTraC has been built at various times on Windows XP, Vista, and
 * 7.  It can be built as a 64-bit library, but this can be tricky
 * (mostly because building OpenCV as a 64-bit library for Windows 7
 * is still quite buggy).  It's recommended that you just build it as
 * a 32-bit library.  This means that OpenCV and wxWidgets need to be
 * built as 32-bit libraries as well.  This will happen
 * automatically if you are using an Express version of Visual Studio,
 * as they do not support 64-bit builds.
 * 
 * MADTraC building has been tested with Visual Studio 2008 Express,
 * which can still be downloaded by going <a
 * href="http://www.microsoft.com/express/Downloads/">here</a> and
 * clicking the "Visual Studio 2008 Express" tab.  It should also work
 * with Visual Studio 2008 (non-Express).  It has also worked with
 * Visual Studio 2010 Express, but only on a system that <i>also</i>
 * has Visual Studio 2008 (for reasons not discussed here).
 *
 * If you have problems finding OpenGL includes, you may need to
 * install the Windows <a
 * href="http://www.microsoft.com/downloads/details.aspx?FamilyID=c17ba869-9671-4330-a63e-1fd44e0e2505&displaylang=en">SDK</a>.
 *
 * @par Prerequisites
 *
 * Download and install <a
 * href="http://www.cmake.org/cmake/resources/software.html">CMake</a>.
 *
 * Download and install <a
 * href="http://sourceforge.net/projects/opencvlibrary/">OpenCV</a>: 
 *    - OpenCV uses a CMake build system.
 *    - Detailed instructions can be found <a
 * href="http://opencv.willowgarage.com/wiki/InstallGuide">here</a>,
 * about 1/3 of the way down the page under "Building OpenCV from
 * source using CMake".
 *    - You'll need to add the folder containing the built DLLs
 * (e.g. \<OpenCV Dir\>/build/bin/Release and Debug) to your PATH system
 * variable - this is step 6.a in the above link.
 * 
 * Download and install <a
 * href="http://www.wxWidgets.org">wxWidgets</a>: 
 *    - Download wxMSW from <a
 * href="http://wxwidgets.org/downloads/">here</a>.  Either the 2.8.11
 * or 2.9.0 release should work.
 *    - Open up \<wxWidgets Dir\>/build/msw/wx.vcxproj (or .sln) in
 * Visual Studio.
 *    - <b>IMPORTANT</b> Edit \<wxWidgets Dir\>/include/wx/msw/setup0.h
 * AND \<wxWidgets Dir\>/include/wx/msw/setup.h so that the line reading
 * @code
 * #define wxUSE_GLCANVAS       0
 * @endcode
 * becomes
 * @code
 * #define wxUSE_GLCANVAS       1
 * @endcode
 * wxWidgets, by default, does not build with OpenGL support.  Making
 * this change BEFORE you build wx will ensure that OpenGL support
 * is configured and built.
 *    - Build at least the "Release" and (recommended) "Debug" configurations.
 *    - To smooth cmake finding your installation of wxWidgets, add a
 * system environment variable (Start Menu -> Right Click on
 * "Computer" -> Properties -> Advanced System Settings) called
 * "WXWIN" and set it to e.g. "c:\Program Files\wxWidgets-2.8.11" (or
 * wherever you installed wx).
 *
 * @par Build
 *
 *   - Open up CMake and point the "Source Directory" to the location
 * where you unzipped (or checked out) MADTraC -
 * e.g. "c:/src/MADTraC".
 *   - Set the "Build Directory" to the same
 * directory but with "/build" added to it -
 * e.g. "c:/src/MADTraC/build".
 *   - Click "Configure".  Set the generator to Visual Studio 9 2008.
 *   - Configure MADTraC.  The following options may be of interest.
 *       - MT_DEBUG_CONSOLE - Check this if you want a console window
 * to pop up in your applications where the output of "printf" will
 * appear.  Handy for development.
 *       - BUILD_ROBOT - You can safely uncheck this if you're not
 * interested in using Joystick and/or COM port functionality.
 *       - WITH_AVT - Check this if you have Allied Vision Technologies
 * (e.g. Guppy) cameras.
 *       - See @ref cmake_config for a more complete description of options.
 *   - Click "Configure" again.  You may need to do this a couple
 * times until there are no more options highlighted in red.
 *   - Click "Generate" to create the Visual Studio Solution
 *   - Open up the newly created MADTraC.sln Visual Studio solution.
 * Click build.  If the Debug configuration doesn't work, try building
 * the Release configuration.
 *
 * @subsection bi_quick_linux Linux
 *
 * Building
 * MADTraC on linux is essentially the same as it is on OS X
 * (@ref bi_quick_OSX), except you'll need to replace "port" with your
 * favorite package manager (apt-get on Ubuntu).  HOWEVER, you need to
 * make sure that OpenCV installs with support for ffmpeg for video
 * I/O support.  This can 
 * be quite tricky (especially for 64-bit systems) and may involve
 * compiling ffmpeg and x264 from source, making sure to use
 * @c ./configure with the @c --enable-shared option.
 *
 * MADTraC will build as long as you have wxWidgets and OpenCV, but
 * you won't be able to open any cameras or AVI files unless OpenCV
 * was built with ffmpeg support.  If this happens, sort out the
 * OpenCV problems and rebuild MADTraC.  You can tell if OpenCV is
 * going to build with ffmpeg by looking at the output of cmake at the
 * command line.  Even if you use ccmake to configure with the GUI,
 * you can run cmake again from the command line to see the output
 * without trashing the existing configuration.
 *
 * There's hope.  I've (DTS) gotten it to work on Ubuntu 10.04
 * 64-bit, with a bit of google searching and digging through forums.
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
