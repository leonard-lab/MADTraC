/* PlaySwarm_As_Tutorial.h - This file contains doxygen code to
 * generate a page in the MT documentation that uses PlaySwarm as a
 * "tutorial" on how to build an MT app based on MT_FrameBase.
 *
 * This file is not intended to ever be compiled.
 */

/** @defgroup PlaySwarm PlaySwarm
 *
 * @brief Source code of the PlaySwarm app.  See @ref playswarm_tutorial
 *
 */

/** @page playswarm_tutorial Tutorial - PlaySwarm
 *
 * This page uses PlaySwarm as a tutorial for how a visualization /
 * simulation app can be developed using MT_FrameBase.  It covers
 * basic MT app initialization and operation, interfacing the
 * preferences and command-line interface systems, automatic
 * persistence of variables (including paths), automatic dialog
 * generation using MT_DataGroup, popup menus, keyboard and mouse
 * event handling, file opening, etc.
 *
 * You'll want to refer to the full source:
 * apps/PlaySwarm/src/PlaySwarm.h and PlaySwarm.cpp
 *
 * @section structure Basic application structure
 *
 * A basic MT application has two main classes: an application class
 * and a frame ("window") class.  In this case, the app class is @c
 * PlaySwarmApp and the frame class is @c PlaySwarmFrame.
 *
 * @subsection PlaySwarmApp PlaySwarmApp
 *
 * The main job of the app class is to create and pass control off to
 * the frame class.  The code is therefore usually pretty simple.  The
 * app must be derived from MT_AppBase and have at least public method,
 * an override of MT_AppBase::createMainFrame.  For PlaySwarm, this
 * is accomplished in PlaySwarm.h via
 * @dontinclude PlaySwarm.h
 * @skip PlaySwarmApp
 * @until };
 * Note that the sole parameter of the PlaySwarmFrame constructor is
 * NULL.  This indicates that PlaySwarmFrame is the "main" frame
 * because it does not have a parent.
 *
 * The only other piece of app-level code is the line
 * @code
 * IMPLEMENT_APP(PlaySwarmApp)
 * @endcode
 * at the top of PlaySwarm.cpp.  This tells the wx system that
 * PlaySwarmApp is our application class.  See @ref intmain for an
 * explanation of @c IMPLEMENT_APP
 *
 * @subsection PlaySwarmFrame PlaySwarmFrame
 *
 * The most important class of PlaySwarm is PlaySwarmFrame.  This
 * class represents the main "window" (windows are called frames in
 * wx), which contains all of the application-level data and event
 * handling.  In theory, the main frame only needs to be a derivative
 * of MT_FrameWithInit, but this wouldn't include any GUI elements
 * that we probably want.  So in this case, we derive from
 * MT_FrameBase, which includes most of the built-in functionality of
 * MT without including any tracking- or robot-specific functions.
 * The class is therefore declared as
 * @code
 * class PlaySwarmFrame : public MT_FrameBase
 * @endcode
 *
 * The constructor looks somewhat menacing, but it's actually fairly
 * straightforward. 
 * @dontinclude PlaySwarm.h
 * @skip PlaySwarmFrame(
 * @until );
 * The structure is mainly due to the structure of the <a
 * href="http://docs.wxwidgets.org/stable/wx_wxframe.html#wxframector">wxFrame</a>
 * constructor.  Usually the default parameters are just fine and we
 * want to pass NULL as the @c parent, to indicate that this is a
 * top-level window.
 *
 * The implementation of the constructor looks like
 * @dontinclude PlaySwarm.cpp
 * @skip PlaySwarmFrame::PlaySwarmFrame
 * @until }
 *
 * Mostly this just passes the input parameters on to the base class
 * (MT_FrameBase) constructor.  If you're unfamiliar with the pitfalls
 * of c++ constructors and don't want to take my word for it, you
 * might want to take a look at the <a
 * href="http://www.parashift.com/c++-faq-lite/">c++ FAQ lite</a>,
 * particularly the sections on <a
 * href="http://www.parashift.com/c++-faq-lite/ctors.html">constructors</a>
 * and <i>especially</i> <a
 * href="http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.5">"When
 * my base class's constructor calls a virtual function on its this
 * object, why doesn't my derived class's override of that virtual
 * function get invoked?"</a>, or, why it's a bad idea to write much
 * code in a constructor unless you're <b>real</b> sure that no one
 * will inherit from your class.
 *
 * We use the initializer list to
 * initialize a few variable values.  Because we're
 * sure that we won't be deriving any classes from PlaySwarmFrame,
 * it's OK to put a couple things in the constructor code.
 * @dontinclude PlaySwarm.cpp
 * @skip ifdef MT_HAVE_CLF
 * @until endif
 * Looks for the MT_HAVE_CLF flag (which should have been defined
 * during MT compile) to determine if we have access to CLF (Couzin
 * Lab Format) functions.  If it does, it disables some annoying warnings.
 *
 * @line m_sDescriptionText
 * Sets the text description of this app, which is used by various
 * help facilities built into MT, for example calling the application
 * with the "-h" or "--help" command line argument.
 *
 * @section Application Initialization
 *
 * After the @c PlaySwarmFrame constructor gets called, MT begins a
 * specifically-ordered set of initialization steps.  For more of the
 * gory details, see @ref mt_init_long.
 *
 * As far as PlaySwarm is concerned, there's really only three functions
 * that are of importance here. The first and most
 * universally applicable is initUserData, which is an override of
 * MT_FrameBase::initUserData.  This is the place to initialize the
 * state of the program beyond what can be done in the frame
 * constructor's initialization list (see above).  This is also a good
 * introduction to MT's data persistence interface.
 * @dontinclude PlaySwarm.cpp
 * @skip PlaySwarmFrame::initUserData
 * @until AddChoice
 * The first block of code is where we specialize
 * MT_FrameBase::m_pPreferences, which is a special instance of an
 * MT_DataGroup that gets saved to and loaded from the settings XML
 * file automatically.  A "Preferences" menu item and corresponding
 * dialog are also automatically created.  Here we add two switches,
 * which point to the boolean variables @c m_bTails and @c m_bOcclusions and
 * get dialog labels "Show Tails" and "Show Occlusions",
 * respectively.  Similarly, there are two @c double variables, @c
 * m_dObjectLength and @c m_dObjectWidth.  The final addition,
 * @c m_GlyphChoice is special.  This is an instance of an MT_Choice
 * class, which is a list of named choices with corresponding integer
 * values, and gets displayed as a drop-box in the dialog.  It gets
 * declared in the class definition as
 * @code
 * MT_Choice m_GlyphChoice
 * @endcode
 * and defined in the constructor initializer list as
 * @code
 * m_GlyphChoice(glyph_names, 3, 1)
 * @endcode
 * which could be read as &ldquo;@c m_GlyphChoice is an MT_Choice created
 * with a name list @c glyph_names which has 3 entries and the number
 * 1 (of 0, 1, 2 - so the second one) choice is selected by
 * default&rdquo;.  The list of names is defined at the top of the
 * file as a static variable at file scope:
 * @code
 * const char* glyph_names[] = {"Fish", "Arrow", "Ellipse"};
 * @endcode
 * So "Arrow" is selected by default.  @c m_GlyphChoice is used later
 * in the OpenGL drawing routine where it's integer value, obtained
 * via MT_Choice::GetIntValue, is mapped to a predefined OpenGL
 * drawing subroutine.
 *
 * After initializing a couple parameters, the next thing to get set
 * up is the path where PlaySwarm should look for data files 
 * @skip m_dGrabRadius
 * @until m_PathGroup
 * Any path in m_PathGroup, which is similar to an MT_DataGroup but
 * specialized for storing paths, gets saved to and read from the
 * settings XML file automatically.  Note that there are no spaces in
 * the name.  The name is stored only in the XML file as a key, and
 * XML keys cannot have spaces, so a path stored in a path group
 * cannot have spaces in the name.  The easiest work-around is just to
 * use underscores.  In MT_DataGroup this is done automatically, but
 * the name with spaces is kept because it is displayed in dialogs.
 * 
 * @section GUI Setup
 *
 * One of the main goals of MT is to make it so you don't have to
 * worry about the GUI.  On the other hand, it strives to be as
 * flexible as possible without being cumbersome.  
 */
