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
 *   - @ref bi_quick_OSX_SnowLeopard  
 *   - @ref bi_quick_OSX_Leopard
 *   - @ref bi_quick_WIN
 *   - @ref bi_quick_linux
 *
 * @section bi_quick Build Instructions
 *
 * @subsection bi_quick_OSX_SnowLeopard Mac OS X 10.6 Snow Leopard
 *
 * @par Introduction
 *
 * These instructions should work on Mac OS X 10.6 to build 64-bit
 * libraries and binaries.  They assume a "normal" set up - i.e. you
 * have not changed any default paths, etc.
 *
 * It's also possible to build 32-bit libraries and binaries on Snow
 * Leopard.  In the future, this may be a simple option in the CMake
 * configuration.  For now, however, it's suggested to stick with the
 * 64-bit build unless you have the know-how to hack the build
 * configurations.  
 * 
 * @par Prerequisites
 * 
 * The main prerequisites can be installed most easily with <a
 * href="http://www.macports.org">MacPorts</a>.  Note that you will
 * need the <a
 * href="http://developer.apple.com/technologies/tools/">Apple
 * Developer Tools</a> (i.e. XCode) installed.
 *
 * CMake 2.8.3 is also required.  It is available via MacPorts, but
 * the <a
 * href="http://www.cmake.org/cmake/resources/software.html">cmake
 * binary package</a> for OS X is recommended.
 *
 * If you have previously installed MacPorts, make sure it is updated
 * by running <tt>sudo port selfupdate</tt> and <tt>sudo port upgrade
 * outdated</tt>.  This could take a while, but it's the best way to
 * avoid problems later.
 * 
 * To install cmake (if necessary), OpenCV, and wxWidgets using
 * MacPorts, simply execute the following commands from a terminal.
 * 
 * @code
 * sudo port install cmake  #(if not already installed)
 * sudo port install opencv
 * sudo port install wxWidgets-devel
 * @endcode
 *
 * @note Note that we have installed the wxWidgets-devel port instead of
 * wxWidgets.  Currently the wxWidgets port is not a 64-bit build.
 * It's OK if you've previously installed wxWidgets - the patches
 * below will make sure that the right version gets used.
 *
 * The following patches are important to make sure that the right
 * version of wx is used.  First, we need to set the system up to use
 * the correct wx-config binary.
 *
 * @code
 * sudo mv /usr/bin/wx-config /usr/bin/old-wx-config
 * sudo ln -s /opt/local/lib/wx-devel/bin/wx-config /usr/bin/wx-config
 * @endcode
 *
 * Finally, we need to fix a bug in the current libraries so that they
 * have the correct embedded paths.
 *
 * @note This applies to the wxWidgets-devel @@2.9.1, Revision 1 port.
 * To check which version you have installed, you can use <tt>port
 * info wxWidgets-devel</tt>.  This is a known bug, so future versions
 * should have this problem fixed and hence this step might not be
 * necessary.  Similarly, this bug may not be present in older ports.
 *
 * @code
 * sudo /usr/bin/install_name_tool -id /opt/local/lib/wx-devel/libwx_osx_cocoau-2.9.1.0.0.dylib /opt/local/lib/wx-devel/libwx_osx_cocoau-2.9.1.0.0.dylib
 * sudo /usr/bin/install_name_tool -id /opt/local/lib/wx-devel/libwx_osx_cocoau_gl-2.9.1.0.0.dylib /opt/local/lib/wx-devel/libwx_osx_cocoau_gl-2.9.1.0.0.dylib
 * sudo install_name_tool -change /opt/local/var/macports/build/_opt_local_var_macports_sources_rsync.macports.org_release_ports_graphics_wxWidgets-devel/work/wxWidgets-2.9.1/build/lib/libwx_osx_cocoau-2.9.1.0.0.dylib /opt/local/lib/wx-devel/libwx_osx_cocoau-2.9.1.0.0.dylib /opt/local/lib/wx-devel/libwx_osx_cocoau_gl-2.9.1.0.0.dylib
 * @endcode
 *
 * As with Leopard (see below and @ref cmake_config), it is possible
 * to build MADTraC using OpenCV and wxWidgets built from source as
 * well as with support from other optional libraries.  However, this
 * is not well tested on Snow Leopard and should be undertaken with a
 * large dose of patience.
 *
 * @par Build
 *
 * With cmake, OpenCV, and wxWidgets-devel installed, go to the directory
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
 *
 * @subsection bi_quick_OSX_Leopard Mac OS X 10.5 Leopard
 * 
 * @par Introduction
 *
 * These instructions are tested on Mac OS X 10.5 to build 32-bit
 * libraries and binaries.  
 * 
 * @par Prerequisites
 * 
 * The main prerequisites can be installed most easily with <a
 * href="http://www.macports.org">MacPorts</a>.  Note that you will
 * need the <a
 * href="http://developer.apple.com/technologies/tools/">Apple
 * Developer Tools</a> (i.e. XCode) installed.
 * 
 * CMake 2.8.3 is also required.  It is available via MacPorts, but
 * the <a
 * href="http://www.cmake.org/cmake/resources/software.html">cmake
 * binary package</a> for OS X is recommended.
 *
 * If you have previously installed MacPorts, make sure it is updated
 * by running <tt>sudo port selfupdate</tt> and <tt>sudo port upgrade
 * outdated</tt>.  This could take a while, but it's the best way to
 * avoid problems later.
 * 
 * To install cmake (if necessary), OpenCV, and wxWidgets using
 * MacPorts, simply execute the following commands from a terminal.
 * 
 * @code
 * sudo port install cmake  (if not already installed)
 * sudo port install opencv
 * sudo port install wxWidgets
 * @endcode
 *
 * It's also possible to build MADTraC if you've installed OpenCV
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
