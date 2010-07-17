/* cmake_config.h - This file contains doxygen code to
 * generate a page in the MT documentation about the cmake options
 * available with CMake.
 *
 * This file is not intended to ever be compiled.
 */


/** @page cmake_config CMake Configuration Options
 *
 * Note: Entries marked as (auto) are automatically configured by
 * cmake if the corresponding packages are found, but can be manually
 * configured afterwards.
 *
 *  - @ref cmake_config_modules
 *      - @ref BUILD_GUI
 *      - @ref BUILD_TRACKING
 *      - @ref BUILD_ROBOT
 *  - @ref cmake_config_options
 *      - @ref BUILD_SHARED
 *      - @ref MT_DEBUG_CONSOLE
 *      - @ref MT_USE_WX_JOYSTICK
 *  - @ref cmake_config_libraries
 *      - @ref WITH_ARTK
 *      - @ref ARTK_ROOT
 *      - @ref WITH_OPENCV
 *      - @ref OPENCV_FRAMEWORK
 *      - @ref OPENCV_INC
 *      - @ref OPENCV_LIBS
 *      - @ref OPENCV_LIBS_DIR
 *      - @ref WITH_CLF
 *      - @ref CLF_ROOT
 *      - @ref AUTO_FIND_WX
 *      - @ref WX_DEFS
 *      - @ref WX_INCLUDE
 *      - @ref WX_LIB
 *
 * @section cmake_config_modules Optional Modules
 *
 * @par BUILD_GUI @anchor BUILD_GUI
 * Build the MT_GUI library.  Default ON, Recommended ON.
 *
 * @par BUILD_TRACKING @anchor BUILD_TRACKING (requires BUILD_GUI)
 * Build the MT_TRACKING library.  Default ON, Recommended ON.
 * 
 * @par BUILD_ROBOT @anchor BUILD_ROBOT (requires BUILD_ROBOT)
 * Build the MT_ROBOT library.  Default ON.
 *
 * @section cmake_config_options Build Options
 *
 * @par BUILD_SHARED @anchor BUILD_SHARED
 * Build as shared libraries.  This is highly experimental.  Leave it
 * OFF unless you are working on development of MADTraC.  Default OFF,
 * Recommended OFF.
 *
 * @par MT_DEBUG_CONSOLE @anchor MT_DEBUG_CONSOLE (Windows Only)
 * When ON, Windows apps will pop up a console window to which stdout
 * and stderr are redirected.  I.e. printf results will appear here as
 * will fprintf with a file descriptor of "stdout" or "stderr".  This
 * is very helpful when debugging applications, but should probably be
 * disabled in "release" applications.  Default OFF.
 *
 * @par MT_USE_WX_JOYSTICK @anchor MT_USE_WX_JOYSTICK (OS X Only)
 * Use the wxWidgets joystick driver rather than the MT driver.  The
 * MT driver seems to almost always work, whereas the wx driver is
 * iffy.  Default OFF, Recommended OFF.
 *
 * @section cmake_config_libraries External Libraries
 *
 * @par WITH_ARTK @anchor WITH_ARTK
 * Build with support for <a
 * href="http://www.hitl.washington.edu/artoolkit/">ARToolkit</a>.
 * ARTK provides a slightly better camera interface on some operating
 * systems (particularly OS X, but also works on Windows and linux).
 * If you enable this, make sure to set up @ref ARTK_ROOT.  This is
 * not particularly well tested so far, especially on systems other
 * than OS X. Default OFF. 
 *
 * @par ARTK_ROOT @anchor ARTK_ROOT
 * Root directory including ARTK includes and libraries.  Must include
 * at least two subdirectories: an include directory containing an AR
 * subdirectory, and a lib subdirectory containing the AR and ARvideo
 * libraries.  Only matters if @ref WITH_ARTK is ON.  Not well tested
 * out of OS X.
 *
 * @par WITH_OPENCV @anchor WITH_OPENCV
 * Build with support for OpenCV.  Only MT_Core and MT_GUI can be
 * built without OpenCV support, and in this case there will be no
 * support for displaying images, creating screen captures, or
 * exporting movies.  Default ON, Recommended ON (highly).
 *
 * @par OPENCV_FRAMEWORK @anchor OPENCV_FRAMEWORK (OS X <= 10.5 Only)
 * Path to OpenCV.framework (see <a
 * href="http://opencv.willowgarage.com/wiki/PrivateFramework">OpenCV
 * Private Framework</a> documentation).  On OS X Leopard (10.5) and
 * older, this is a simpler way to use OpenCV.  There doesn't yet
 * appear to be a compiled framework that works on Snow Leopard
 * (10.6).
 *
 * @par 
 * If the directory pointed to by this variable exists, then
 * it is used to generate the proper OpenCV
 * includes and linking, and MT_HAVE_OPENCV_FRAMEWORK is defined as a
 * preprocessor macro.  The framework will also be packaged in target
 * applications when using the example CMake files, which is
 * convenient because it does not require the deployment machine to
 * have OpenCV already installed.
 *
 * @par 
 * If this directory does not exist,
 * cmake will attempt to find OpenCV automatically.  This will work if
 * you installed OpenCV using MacPorts or from source with the proper
 * installation steps.  To use the framework, download it and paste it
 * into the MADTraC root directory, or otherwise set the value of this
 * variable to its location.  Default \<MADTraC Root\>/OpenCV.framework.
 *
 * @par OPENCV_INC @anchor OPENCV_INC (auto)
 * Location of the OpenCV include files. 
 *   - When using OpenCV.framework, this should be set to the same
 * value as @ref OPENCV_FRAMEWORK
 *   - When OpenCV was installed with MacPorts, the value will most
 * likely be /opt/local/include/opencv
 *   - On Windows, the value should be something like \<OpenCV Dir\>include/opencv
 *
 * @par OPENCV_LIBS @anchor OPENCV_LIBS (auto)
 * Names of the OpenCV libraries. 
 *   - When using OpenCV.framework, this should be set to the same
 * value as @ref OPENCV_FRAMEWORK
 *   - When OpenCV was installed with MacPorts, the value will most
 * likely be cv;highgui;cvaux;ml;cxcore
 *   - On Windows, the value should be something like
 * cv210.lib;cxcore210.lib;highgui210.lib;ml210.lib (210 refers to the
 * version - check the names of the .lib files that OpenCV generates).
 *
 * @par OPENCV_LIBS_DIR @anchor OPENCV_LIBS_DIR (auto)
 * Path to the folder containing the OpenCV library files.
 *   - When using OpenCV.framework, this should be blank
 *   - When OpenCV was installed with MacPorts, the value will most
 * likely be /opt/local/lib
 *   - On Windows, the value should be something like \<OpenCV Dir\>lib/Release
 *
 * @par WITH_CLF @anchor WITH_CLF
 * Build with <a href="http://www.princeton.edu/~sleblanc/clf/">Couzin
 * Lab File Format</a> (CLF) support.  This is experimental.  If
 * enabled, you will need to configure @ref CLF_ROOT support
 * manually.  CLF also requires HDF5, which should be detected
 * automatically by cmake, but may require manual configuration.
 * Default OFF, Recommended OFF. 
 *
 * @par CLF_ROOT @anchor CLF_ROOT (requires WITH_CLF)
 * Path to the CLF build directory (assuming it was built with
 * CMake).  Default is ~/src/CLF/build on OS X / linux and
 * c:/src/CLF/build on Windows.
 *
 * @par AUTO_FIND_WX @anchor AUTO_FIND_WX
 * If ON (the default), cmake attemps to automatically find
 * wxWidgets.  This will work if you've installed wxWidgets through
 * MacPorts or properly compiled from source and performed the
 * required installation steps.  Note that cmake will use the
 * environment variable WXWIN as a hint on where to find wxWidgets.
 * Only set this to OFF if you have a good reason.  
 * Default is ON, Recommended ON. 
 *
 * @par WX_DEFS @anchor WX_DEFS (auto)
 * Preprocessor macros required by wxWidgets.
 *
 * @par WX_INCLUDE @anchor WX_INCLUDE (auto)
 * Include directories for wxWidgets.
 *
 * @par WX_LIB @anchor WX_LIB (auto)
 * Libraries required for linking wxWidgets applications.
 * 
 */
