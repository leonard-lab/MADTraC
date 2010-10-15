/* SimpleBWTracker_Tutorial.h - This file contains doxygen code to
 * generate a page in the MT documentation based on the SimpleBWTracker
 * as a tutorial.
 *
 * This file is not intended to ever be compiled.
 */

/** @defgroup SimpleBWTracker SimpleBWTracker
 *
 * @brief Source code of the SimpleBWTracker app.  See @ref tracker_tutorial
 *
 */

/** @page MT_App_Overview MT Application Overview
 *
 * This page aims to put the overall structure of an MT 
 * application into context.  We examine the structure of a generic c 
 * program and map the basic components (initialization, program loop,
 * etc) into their MT equivalents.
 *
 * Outline:
 *  - @ref cpp_app_templates
 *  - @ref MT_app_specifics
 *      - @ref MT_app_specifics_app
 *      - @ref MT_app_specifics_frame
 *          - @ref MT_app_specifics_frame_FrameBase
 *          - @ref MT_app_specifics_frame_TrackerFrameBase
 *
 * @section cpp_app_templates C++ Application Templates
 * 
 * There's a hint of lecturing on the
 * differences between c and c++ here, so I preface this with a note
 * of humility: I've been programming since I was old enough to read
 * and I didn't <i>really</i> understand a lot of this until
 * recently, myself.
 *
 * If you're used to c programming, you probably think of a
 * generic program with a main loop as behaving something like this:
 * @code
 * int main(int argc, char** argv)
 * {
 *     // do initialization, load data, allocation, etc
 *     ...
 *
 *     // main program loop
 *     while(do_main_loop)
 *     {
 *         ...
 *     }
 *
 *     // exit routine - save data, deallocation, etc
 *     ...
 * }
 * @endcode
 *
 * With object-oriented programming, things might get a little less
 * obvious.  Suppose that we wanted to create a &ldquo;template&rdquo;
 * application class, so that we could re-use code.  Our application
 * might look something like this:
 * @code
 * class App
 * {
 *     // private, protected member variables and functions
 *  public:
 *     // constructor
 *     App();
 *     // destructor
 *     ~App();
 *
 *     // main loop function
 *     virtual void doMainLoop();
 * }
 *
 * // implementation of App::App, App::~App, and App::doMainLoop
 * ...
 *
 * int main(int argc, char** argv)
 * {
 *     // create app object
 *     App* theApp = new App();
 *
 *     // do main loop
 *     theApp->doMainLoop();
 *
 *     // clean up
 *     delete theApp;
 * }
 * @endcode
 *
 * This means that the initialization, allocation, etc move to the
 * App constructor, which gets called implicitly when we create the
 * theApp object.  Likewise, the exit routine, deallocation, etc move
 * to the destructor.
 *
 * Now, suppose that we hone our App class to perfection.  However, we
 * want it to remain flexible, so we leave some of the functionality
 * to virtual functions.  We want the initialization to be flexible as
 * well, so we create an App::doInit virtual function as well.  An
 * implemented app that derives from App might then look like
 * @code
 * class App
 * {
 *     // private, protected member variables and functions
 *  public:
 *     // constructor
 *     App(int argc, char** argv);
 *     // virtual destructor
 *     virtual ~App();
 *     // virtual initialization function
 *     virtual void doInit();
 *     // main loop function
 *     virtual void doMainLoop();
 * }
 *
 * // implementation of App::App, App::~App, App::doInit, and App::doMainLoop
 * ...
 *
 * // implemented app derived from App class 
 * class myApp : public App
 * {
 *     // private, protected member variables and function
 * public:
 *     // constructor, destructor
 *     myApp(int argc, char** argv);
 *     ~myApp();
 *
 *     // overridden virtual functions
 *     void doInit();
 *     void doMainLoop();
 * };
 *
 * // implementation of myApp::myApp, etc
 * ...
 *
 * int main(int argc, char** argv)
 * {
 *     myApp* theApp = new myApp(argc, argv);
 *     theApp->doInit();
 *     theApp->doMainLoop();
 *     delete theApp;
 * }
 * @endcode
 *
 * Note that we've moved the command line handling to the App class
 * and made the destructor virtual.  This ensures that the code inside
 * the @c int @c main block is completely generic (more on this later)
 * and that
 * both the derived and base destructor get called when the object is
 * deleted.  You might also wonder why we have to call @c
 * theApp->doInit manually rather than including it in the constructor
 * of App, a la
 * @code
 * // NOTE: This is only a demonstration of the WRONG way to do this
 * App::App()
 * {
 *      this->doInit();
 * }
 * @endcode
 * The reason is subtle, but basically amounts to the fact that
 * constructors are called in "bottom up" order.  In our example,
 * App() would be called beore myApp(); in fact the object would not
 * even know that it is an instance of myApp() (i.e. it is an
 * incomplete object) when App() is called.  Therefore the call to @c
 * this->doInit() will ONLY call App::doInit and not myApp::doInit.
 * For a more technical discussion, see the C++ FAQ Lite section
 * &ldquo;<a
 * href="http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.7">Should
 * you use the this pointer in the constructor?</a>&ldquo;.  While
 * you're at it, take a look at &ldquo;<a
 * href="http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.6">Should
 * my constructors use &ldquo;initialization lists&rdquo; or
 * &ldquo;assignment&rdquo;?</a> to see why your constructors should
 * always look like this
 * @code
 * // Right way to initialize member variables
 * Foo::Foo(int inherited_init_value, double derived_init_value)
 *  : Foo_Base(inherited_init_value),
 *    m_dSomeDoubleVar(derived_init_value)
 * {}
 * @endcode
 * rather than
 * @code
 * // WRONG way to initialize member variables
 * Foo::Foo(int inherited_init_value, double derived_init_value)
 *  : Foo_Base(inherited_init_value)
 * {
 *    m_dSomeDoubleVar = derived_init_value;
 * }
 * @endcode
 * It's not strictly illegal or even necessarily wrong to do it the
 * second way, but the first way is a VERY good habit to get
 * into. (end digression)
 *
 * Back to our App example.  Because we've moved command line argument
 * handling to the App class, the @c int @c main function is now
 * completely generic.  That is, any application we propose to
 * implement will include exactly the same @c int @c main function.
 * Therefore there's no need to repeat it every time we make a new
 * derived application.  In fact, it could be bad to even <i>allow</i>
 * this, since whoever is doing the derivation may forget to include a
 * line or otherwise muck things up.
 *
 * To make the application class safely generic, let's consider moving
 * the whole thing to an external header file, say App.h, and creating
 * a preprocessor macro called @c IMPLEMENT_APP that essentially
 * implements the @c int @c main function correctly.
 * @code
 * // App.h (and possibly App.cpp)
 * 
 * // App class definition, mostly the same as above
 * class App:
 * {
 *     ...
 * };
 *
 * // int main implementation macro
 * #define IMPLEMENT_APP(app_class_name)                     \
 *     int main(int argc, char** argv)                       \
 *     {                                                     \
 *         app_class_name* theApp;                           \
 *         theApp = new app_class_name(argc, argv);          \
 *         theApp->doInit();                                 \
 *         theApp->doMainLoop();                             \
 *         delete theApp;                                    \
 *     }                                                     \
 * @endcode 
 *
 * Then our implementation would look something like
 * @code
 * // content of implementation file
 *
 * // include the App class definition and IMPLEMENT_APP macro
 * #include "App.h"
 *
 * // mApp definition and implementation, same as before
 * class myApp : public App
 * {
 *     ...
 * };
 *
 * // inserts int main macro
 * IMPLEMENT_APP(myApp)
 * @endcode
 *
 * @section MT_app_specifics Specifics to MT apps.
 *
 * @subsection MT_app_specifics_app The app template
 *
 * MT implements its App template as MT_AppBase, and inherits the
 * wxWidgets macro <a
 * href="http://docs.wxwidgets.org/stable/wx_rttimacros.html#implementapp">IMPLEMENT_APP</a>.
 * Therefore the application header will usually contain code looking like
 * @code
 * // in the header
 * class myApp
 * : public MT_AppBase
 * {
 * public:
 *     MT_FrameWithInit* createMainFrame()
 *     {
 *         return new myFrame(NULL);
 *     };
 * };
 * @endcode
 * and the source will include
 * @code
 * // in the source
 * IMPLEMENT_APP(myApp)
 * @endcode
 *
 * Note that we really don't even use the app class implementation to
 * do much.  In fact, we shift most of the load to the frame
 * (&ldquo;frame&rdquo; is wx-ese for what most people would call a
 * &ldquo;window&rdquo;) class.  See the next section.
 *
 * @subsection MT_app_specifics_frame The frame templates 
 *
 * The MT_FrameWithInit class derives
 * from wxFrame and simply adds a virtual initialization function (see
 * above) MT_FrameWithInit::doMasterInitialization as well as
 * MT_FrameWithInit::handleOpenWithFile.
 *
 * For normal tracking, etc, applications, you won't want to use
 * MT_FrameWithInit directly as it does not include any of the other
 * mechanisms you probably want to inherit (OpenGL drawing, File I/O,
 * etc) and does not have a main loop.  Instead, you probably want to
 * inherit from MT_FrameBase, MT_TrackerFrameBase, or
 * MT_RobotFrameBase - each of which adds progressively more
 * functionality (and corresponding size and dependencies).
 *
 * @subsubsection MT_app_specifics_frame_FrameBase MT_FrameBase
 *
 * The simplest frame base app is MT_FrameBase.  The main
 * initialization function is MT_FrameBase::doMasterInitialization,
 * which is a non-virtual function that is not intended to be
 * reimplemented.  Instead, doMasterInitialization calls various other
 * initialization functions at the proper times.  Directly or
 * indirectly, the following virtual functions are called:
 *  - MT_FrameBase::createTimer
 *  - MT_FrameBase::makeFileMenu, MT_FrameBase::makeEditMenu,
 * MT_FrameBase::makeViewMenu, MT_FrameBase::createUserMenus,
 * MT_FrameBase::makeHelpMenu 
 *  - MT_FrameBase::createControlDialog
 *  - MT_FrameBase::createUserDialogs
 *  - MT_FrameBase::initUserData
 *  - MT_FrameBase::readUserXML
 *  - MT_FrameBase::handleCommandLineArguments
 *  
 * The main initialization function for the user will be
 * MT_FrameBase::initUserData.  The other virtual functions are for
 * more advanced purposes.
 *
 * The main loop of MT_FrameBase is driven by its timer, which is
 * created by MT_FrameBase::createTimer (usually the base result is
 * fine here).  Ever MT_DEFAULT_FRAME_PERIOD (currently 25)
 * milliseconds, MT_FrameBase::doTimedEvents is called, which in turn
 * calls MT_FrameBase::doUserTimedEvents.  If the application is
 * not paused (if MT_FrameBase::m_bPaused is false),
 * MT_FrameBase::doStep is called.  This in turn calls
 * MT_FrameBase::doUserStep, updates any dialogs, and makes sure that
 * the OpenGL drawing gets done.  This architecture allows us to
 * separate the events that should happen EVERY time step
 * (e.g. refreshing the viewport, other UI or time-specific events)
 * from events that should only happen when the application is
 * supposed to be &ldquo;running&rdquo; (e.g. advance a simulation
 * time step, get a new movie frame).
 *
 * The drawing routine is special.  This gets called whenever the
 * drawing area is invalidated, a decision that gets made
 * automatically by the operating system but can also be forced by the
 * event loop.  Because of this, it's not wise to include any state
 * updates (e.g. simulation time steps, etc) in the drawing routine -
 * it should only draw the <i>current</i> state.  In MT, this happens
 * automatically, but is guaranteed to happen at least as often as
 * MT_FrameBase::doUserStep.  The overridable function is
 * MT_FrameBase::doUserGLDrawing.  The corresponding initialization
 * function is MT_FrameBase::doUserGLInitialization - though there's
 * usually no need to override this.
 *
 * Because destructors occur in reverse order (the most derived class
 * has its destructor called first, working backwards to the base
 * class), it's safe to deallocate memory in the frame destructor.
 * Note, though, that this is not necessarily what you want to happen
 * when a user &ldquo;quits&rdquo; the application.  This is handled
 * by MT_FrameBase::doQuit, which stops the timer, writes XML (calls
 * MT_FrameBase::writeUserXML), and calls MT_FrameBase::doUserQuit.
 *
 * <b>Summary of MT_FrameBase</b> virtual functions of interest
 *  - MT_FrameBase::initUserData
 *  - MT_FrameBase::doUserTimedEvents
 *  - MT_FrameBase::doUserStep
 *  - MT_FrameBase::doUserGLDrawing
 *  - MT_FrameBase::doUserQuit
 *
 * @subsubsection MT_app_specifics_frame_TrackerFrameBase MT_TrackerFrameBase
 *
 * MT_TrackerFrameBase inherits from MT_FrameBase and adds basic
 * functionality for a tracking application.  NOTE, though, that when
 * inheriting from MT_TrackerFrameBase, the following functions need
 * to pass through to the MT_TrackerFrameBase in addition to the
 * derived functionality, since MT_TrackerFrameBase hijacks the
 * corresponding MT_FrameBase virtual functions for its own purposes:
 *  - MT_TrackerFrameBase::initUserData
 *  - MT_TrackerFrameBase::doUserStep
 *
 * When an MT_TrackerFrameBase is created, it does not automatically
 * create a tracker.  This is desireable because the video source (avi
 * or camera) needs to be initialized first, but that won't be
 * initialized until the user requests it from the GUI.  Instead, the
 * MT_TrackerFrameBase::initTracker function is provided.  You can
 * simply override this function to set
 * MT_TrackerFrameBase::m_pTracker to a pointer to an instance of a
 * class derived from MT_TrackerBase.  It's simpler than it sounds:
 * @code
 * void myTrackerFrame::initTracker()
 * {
 *     // you probably want a properly-cast pointer to your
 *     //   derived tracker class.  m_pCurrentFrame always points
 *     //   to the most recently grabbed frame
 *     m_pDerivedTracker = new DerivedTracker(m_pCurrentFrame);
 *     m_pTracker = (MT_TrackerBase *) m_pDerivedTracker;
 * }
 * @endcode
 *
 * In addition to the frame events, we want to make sure that the
 * tracker gets initialized correctly, receives tracking events, and
 * deallocates memory properly.  The main functions of interest are
 *  - MT_TrackerBase::createFrames
 *  - MT_TrackerBase::doInit
 *  - MT_TrackerBase::doTracking
 *  - MT_TrackerBase::doGLDrawing
 *  - MT_TrackerBase::releaseFrames
 * 
 */

/** @page tracker_tutorial Tutorial - SimpleBWTracker
 *
 * <b>Please Note</b> - This tutorial is a work in progress.
 *
 * This page uses the SimpleBWTracker app as a tutorial for building
 * tracking applications with MADTraC.  The full source is
 * samples/SimpleBWTracker/src/SimpleBWTracker.h and SimpleBWTracker.cpp
 *
 * @section sbwt_overview Overview
 *
 * This is meant to be a very practical tutorial.  For more
 * information on the overall structure of an MT app, see @ref 
 * MT_App_Overview. 
 * 
 * @section sbwt_tracker The Tracker Class
 *
 * @subsection sbwt_tracker_init Initialization
 *
 * @subsubsection sbwt_tracker_init_ctor Constructor
 * 
 * The constructor of our tracker is fairly straightforward.
 *
 * @dontinclude SimpleBWTracker.cpp
 * @skip ProtoFrame
 * @until }
 *
 * First, we pass the prototype frame on to the MT_TrackerBase
 * constructor.  We set several member variables to their default
 * values (which are defined at the top of the file).  After the first
 * run of the application, the values should be automatically loaded
 * from the XML file, but they should be set to safe values for the
 * first run. The body of the function simply calls doInit, so let's
 * take a look at that function next. 
 *
 * @subsubsection sbwt_tracker_init_doInit doInit
 * 
 * The doInit function takes a prototype frame so that it can
 * determine appropriate image sizes.  For safety, we initialize the
 * pointers to our frames to NULL.  We also grab the number of frames
 * per channel (i.e. 1 for GrayScale, 3 for RBG Color). 
 *
 * @skip void
 * @until m_pThreshFrame
 *
 * At this point we pass the call to MT_TrackerBase::doInit, which
 * initializes the underlying variables and calls
 * MT_TrackerBase::doTrain to set the
 * background image (see documentation of MT_TrackerBase::doInit,
 * MT_TrackerBase::doTrain).  doTrain also calls createFrames, which
 * is a virtual function that we'll discuss below.
 *
 * @skip doInit
 * @until doInit
 *
 * Next we set up MT_TrackerBase::m_pTrackerFrameGroup.  This sets up
 * the list of frames that are available in the "View" menu of the
 * application.  The set up is fairly simple: we create the object by
 * calling the default MT_TrackerFrameGroup constructor and add frames
 * by calling MT_TrackerFrameGroup::pushFrame.
 *
 * @skip m_pTrackerFrameGroup
 * @until m_pThreshFrame
 *
 * Next we'll set up two MT_DataGroup objects.  These will be used to
 * make parameters available to the GUI and automatically saved to
 * XML.  The first data group is for our blob parameters.
 *
 * @skip parameter
 * @until MaxBlobArea
 *
 * &ldquo;Blob Tracking Parameters&rdquo; will be the name displayed
 * in the Tracker menu in the GUI and used in the XML to key the
 * group.  The second group is for drawing parameters.
 *
 * @skip second
 * @until AddColor
 *
 * By adding these data group sto MT_TrackerBase::m_vDataGroups, they
 * will automatically be added to the GUI in the Tracker menu and
 * saved in the XML settings file.
 *
 * @skip stuff
 * @until dg_draw
 *
 * Similarly, we can add a "report" to the GUI so that we can keep an
 * eye on the numerical values of the results in real time.  
 *
 * @skip reports
 * @until }
 *
 * This is
 * all we do in the doInit function here, but in general you may want
 * to use it to initialize member data, etc.
 *
 * @subsubsection sbwt_tracker_init_createFrames createFrames
 *
 * We need to allocate memory for the frames (images) we will use in
 * our tracker.  In this case, we need a background frame, potentially
 * an ROI / mask frame, a grayscale copy of the current frame, a
 * background difference frame, and a difference threshold frame. 
 * MT_TrackerBase has built-in background (see
 * MT_TrackerBase::doTrain, MT_TrackerBase::setBackgroundImage) and
 * ROI (MT_TrackerBase::setROIImage) frame managers; these get
 * allocated and deallocated automatically.  The remainder of the
 * frames we need to allocate ourselves.  This can be done fairly
 * easily.  After we call MT_TrackerBase::doInit, the correct image
 * width and height are available from MT_TrackerBase::FrameWidth and
 * MT_TrackerBase::FrameHeight, respectively.
 *
 * @skip createFrames
 * @until endof
 *
 * Note that we check if the pointers are Null or not and then release
 * the frames if they were previously allocated.  This allows us to
 * resize the frames if a new background is specified.
 *
 * @subsubsection sbwt_tracker_init_releaseFrames releaseFrames
 *
 * MT_TrackerBase::releaseFrames is not an initialization function,
 * but it is the opposite of createFrames - i.e. it de-allocates the
 * frames when we want to close the application.  Again, we don't need
 * to worry about the background or ROI frames.  The code is very
 * similar to createFrames.
 *
 * @skip releaseFrames
 * @until endof
 * 
 * @section sbwt_app_class The App Class
 *
 * Deriving a class from MT_AppBase is how we tell the MADTraC
 * library how the operating system should enter our applications
 * (similar to @c int @c main).  This only needs to have a single
 * public method, derived from 
 * 
 *
 */
