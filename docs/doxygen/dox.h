/* dox.h - This file contains extra information for the
 * doxygen documentation generation system. */

/** @mainpage MADTraC Framework Source Documentation
 *
 * - @ref mp_intro
 * - @ref mp_get
 * - @ref mp_build
 * - @ref mp_doc_overview
 *
 * @section mp_intro Introduction
 *
 * MADTraC contains:
 *  - A GUI framework suitable for any time-driven application with an
 * OpenGL visualization; for example simulation or data playback.
 *  - A framework for video tracking applications, with support for
 * visualization, AVI and camera input, standardized data output, etc.
 *  - A set of routines for working with serial input/output for use
 * with robots and servo controllers.
 *  - A set of utility functions, constants, and classes.
 *  - Camera and robot utility applications.
 *  - The <a
 * href="http://www.princeton.edu/~dswain/apps/">PlaySwarm</a> trajectory data visualization application.
 * 
 * @section mp_get Getting MADTraC
 *
 * There are three methods to get MADTraC:
 *    - Download the nightly <a
 * href="http://poincare.princeton.edu/MADTraC/latest.tar.gz">tarball</a>
 * or <a href="http://poincare.princeton.edu/MADTraC/latest.zip">zip</a>.
 *    - Anonymous SVN checkout
 * @code
 * svn checkout http://poincare.princeton.edu/MADTraC/svn/MADTraC/trunk MADTraC
 * @endcode
 *    - Secure checkout (with write privileges) can be done with a valid
 * username and password from
 * \code
 * svn checkout svn+ssh://username@poincare.princeton.edu/svn/repos/MADTraC/trunk MADTraC
 * \endcode
 *
 * @section mp_build Prerequisites and Building
 *
 * The MT_Core library can be built without dependency on any external
 * libraries, but many of the useful features of MADTraC require at least
 *  - The <a href="http://opencv.willowgarage.com">OpenCV</a> computer
 * vision libraries.
 *  - The <a href="http://wxwidgets.org">wxWidgets</a> GUI framework.
 *
 * MADTraC uses <a
 * href="http://www.cmake.org/cmake/resources/software.html">CMake</a>
 * to manage configuration across multiple platforms and with many
 * build options.  You can run cmake on the MADTraC directory and
 * generate build files appropriate to your system - makefiles, XCode
 * Projects, Visual Studio Solutions, etc.
 *
 * Note that building an actual application requires first building
 * the MADTraC libraries and then building your application that links
 * against those libraries.  A CMake configuration file is
 * included that can be used to configure target applications to use
 * MADTraC.
 *
 * For more detailed information, see
 *   - @ref build_info
 *   - @ref cmake_config
 *   - @ref build_apps
 * 
 * @section mp_doc_overview Documentation Overview
 * 
 * Documentation of the code is here and is split among the four MT modules:
 *  - @ref MT_Core "MT Core" - Non-GUI data structures and support modules.
 *  - @ref MT_GUI "MT GUI" - Base classes for a simple time-based GUI
 * application with an OpenGL visualization window, command line
 * parsing, parameter persistence, screen still and video output.
 *  - @ref MT_Tracking "MT Tracking" - Base classes for a generic
 * video tracking application.
 *  - @ref MT_Robot "MT Robot" - Base classes for a generic
 * application involving video tracking and serial I/O.
 *
 * Related documentation:
 *  - @ref MT_App_Overview - Overview of MT Application structure
 *  - @ref tracker_tutorial - Simple tracker tutorial (work in progress)
 *
 */

/* @page codeflow Flow of the Code
 *
 * This page describes how the MADTraC code flows.  That is, what
 * functions are typically executed and in what order as a normal
 * application runs?
 *
 * Contents:
 *   - @ref entry
 *     - @ref intmain
 *     - @ref mt_init_tutorial
 *     - @ref mt_init_long
 *   - @ref loop
 *    
 * @section entry Entry Points
 * @subsection intmain Where's int main?
 * One question often asked by coders that are new to C++ application
 * programming is &ldquo; where's @c int @c main ?&rdquo;  This is an
 * understandable question.  Snarky message board commenters might
 * give you one of these answers.
 *   - RTFM, see @c IMPLEMENT_APP
 *   - Don't worry about it.  You don't need to know.
 *   
 * The key to understanding what really happens is to see that your
 * application is really an instance of a class, say @c myApp, and
 * driven mainly by an event loop.
 * Regardless of what your application does, the program will
 * inevitably start something like this:
 * @code
 * int main(int argc, char** argv)
 * {
 *     myApp* theApp = new myApp(argc, argv);
 *     theApp->DoEventLoop();
 *     return theApp->ErrorStatus();
 * }
 * @endcode
 * with the intention that @c theApp::DoEventLoop() runs until the user quits
 * the program.  The details are slightly different, but this is
 * essentially what the wxWidgets @c IMPLEMENT_APP macro does.  The
 * practical details are in the next section.
 *
 * @subsection mt_init_tutorial MADTraC App Initialization - Tutorial
 *
 *   - Derive your app class
 *      @code class myApp : public MT_AppBase @endcode
 *   - Derive your main frame (window) class
 *      @code
 *      class myFrame : public MT_FrameBase
 *      // or MT_TrackingFrameBase or MT_RobotFrameBase
 *      @endcode
 *   - Somewhere in your .cpp file, put
 *      @code
 *      IMPLEMENT_APP(myApp)
 *
 *      MT_FrameWithInit* myFrame::createMainFrame()
 *      {
 *        return new myFrame(NULL);
 *      }
 *      @endcode
 *   - The things you might normally do in @c int @c main, you
 * probably want to do in your frame class's derivative of
 *      MT_FrameBase::initUserData.  Note that "program-level"
 *      variables probably want to become member variables of
 *      myFrame.  This is also where you want to modify
 *      MT_FrameBase::m_pPreferences,
 *      MT_FrameBase::m_CmdLineParser, and m_PathGroup.  It might look
 *      something like this: 
 *      @code
 *      void myFrame::initUserData()
 *      {
 *          // add a couple preferences
 *          m_pPreferences->addBool("A switch", &m_bSwitch1);
 *          m_pPreferences->addDouble("A parameter", &m_dParam1);
 *           
 *          // initialize some data
 *          m_dData1 = 0;
 *          m_vdVectorData.resize(DEFAULT_NUM_POINTS);
 *           
 *          // add a remembered path
 *          m_PathGroup.AddPath("Data_Directory", &m_sDataPath);
 *           
 *          // add a command line switch
 *          m_CmdLineParser.AddSwitch("w",
 *                                    "no-warnings",
 *                                    "Disable Warnings.");
        }
 *      @endcode
 *      
 *   - If you need to handle command line arguments, write a
 *      derivative of MT_FrameBase::handleCommandLineArgumepnts.  For
 *      custom XML, see MT_FrameBase::readUserXML and
 *      MT_FrameBase::writeUserXML.
 *   - If your app handles "Open with" file associations on a Mac, see
 *      MT_FrameBasee::handleOpenWithFile.  Note you'll also need to
 *      edit the app's Info.plist file as described <a
 *      href="http://wiki.wxwidgets.org/WxMac-specific_topics#The_file_extension_way">here</a>.
 *      Note that if you're using cmake, you'll want to edit
 *      Info.plist.in instead.
 *
 * @subsection mt_init_long MADTraC app initialization - Details
 * 
 * From a practical standpoint (i.e. here are the functions you care
 * about), this is what happens when an MT app initializes.  It
 * assumes that you derive an app class called @c myApp from
 * MT_AppBase and a frame class @c myFrame from MT_FrameBase (or
 * MT_TrackingFrameBase or MT_RobotFrameBase).
 *   -# You must include the line
 *        @code
 *        IMPLEMENT_APP(myApp)
 *        @endcode
 *        to tell wxWidgest that @c myApp is your app class.
 *   -# The @c myApp() initializer gets called.  As a matter of
 * practice, you probably don't want to actually *do* anything here.
 *   -# @c myApp::createMainFrame() gets called.  This should only do
 * the following.
 *        @code
 *        return new myFrame(NULL);   // NULL means no parent
 *        @endcode
 *   -# MT_FrameBase::doMasterInitialization() is called.  This is not
 * a virtual function.  It does important initializations of
 * MT_FrameBase in the correct order.
 *   -# MT_FrameBase::createTimer() is called.  This is a virtual
 * function but unless you need a specialized timer, you don't need to
 * write your own function.  It needs to return a derivative of
 * MT_TimerBase.
 *   -# MT_FrameBase::createMenus() is called.  This is not a
 * virtual function.  It creates the basic menu structure by calling
 * the following functions (which ARE virtual) in this order.
 *     -# MT_FrameBase::makeFileMenu 
 *     -# MT_FrameBase::makeEditMenu 
 *     -# MT_FrameBase::makeViewMenu 
 *     -# MT_FrameBase::createUserMenus
 *     -# MT_FrameBase::makeHelpMenu
 *     .
 *  MT_TrackingFrameBase and MT_RobotFrameBase hijack the
 * createUserMenus function to add specialized menus, so make sure
 * that if you override it, you call the appropriate base method.
 *   -# The OpenGL canvas is created and sized accordingly.
 *   -# MT_FrameBase::createControlDialog is called.
 *   -# MT_FrameBase::createUserDialogs is called.
 *   -# MT_FrameBase::initUserData is called.  You should override
 * this function to do your main initialization work.  This should
 * include your own modifications to MT_FrameBase::m_CmdLineParser and
 * MT_FrameBase::m_pPreferences. 
 *   -# MT_FrameBase::readXML is called, which reads
 * MT_FrameBase::m_pPreferences from the file and in turn calls
 * MT_FrameBase::readUserXML, where you can set up any custom XML
 * reading code.
 *   -# MT_FrameBase::handleCommandLineArguments is called.
 *   -# On a Mac, if the application was initialized by
 * double-clicking on an associated file or by dragging an associated
 * file onto the app's icon, MT_FrameBase::handleOpenWithFile is called.
 *  
 * 
 *
 * @section loop Main Program Loop
 *
 */

/** @defgroup MT_Core MT Core
 *
 * @brief Non-GUI data structures and help routines.
 *
 */

/** @defgroup MT_GUI MT GUI
 *
 * @brief Base classes for GUI interface
 *
 * */

/** @defgroup MT_Tracking MT Tracking
 *
 * @brief Base classes and support for tracking applications
 *
 * */

/** @defgroup MT_Robot MT Robot
 *
 * @brief Base classes and support for robot applications
 *
 * */

/** @defgroup third_party Third Party
 *
 * @brief Externally included libraries and references.
 *
 * */

/*************************************************************/
/* Definitions of data types included only for documentation
 * purposes - that is, these are not meant to compile, just as input
 * to Doxygen for reference purposes.  This way we don't have to mark
 * up any of the 3rd party files.  */

/** @addtogroup third_party
 * @{
 */

/** @struct IplImage OpenCV
 *
 * This is the main image structure used in MT - mainly because it is
 * the main image structure used in OpenCV.  The definition is
 * included here for reference purposes. */
typedef struct IplImage {
    int  nSize;                    /**< sizeof(IplImage) */
    int  ID;                       /**< version (=0)*/
    int  nChannels;                /**< Most of OpenCV functions support 1,2,3 or 4 channels */
    int  alphaChannel;             /**< ignored by OpenCV */
    int  depth;                    /**< pixel depth in bits: IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16S,
                                        IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F are supported */
    char colorModel[4];            /**< ignored by OpenCV */
    char channelSeq[4];            /**< ignored by OpenCV */
    int  dataOrder;                /**< 0 - interleaved color channels, 1 - separate color channels.
                                        cvCreateImage can only create interleaved images */
    int  origin;                   /**< 0 - top-left origin,
                                        1 - bottom-left origin (Windows bitmaps style) */
    int  align;                    /**< Alignment of image rows (4 or 8).
                                       OpenCV ignores it and uses widthStep instead */
    int  width;                    /**< image width in pixels */
    int  height;                   /**< image height in pixels */
    struct _IplROI *roi;           /**< image ROI. if NULL, the whole image is selected */
    struct _IplImage *maskROI;     /**< must be NULL */
    void  *imageId;                /**< must be NULL */
    struct _IplTileInfo *tileInfo; /**< must be null */
    int  imageSize;                /**< image data size in bytes
                                        (==image->height*image->widthStep
                                        in case of interleaved data)*/
    char *imageData;               /**< pointer to aligned image data */
    int  widthStep;                /**< size of aligned image row in bytes */
    int  BorderMode[4];            /**< ignored by OpenCV */
    int  BorderConst[4];           /**< ignored by OpenCV */
    char *imageDataOrigin;         /**< pointer to very origin of image data
                                        (not necessarily aligned) -
                                        needed for correct deallocation */
}
IplImage;

/* @} */
