#ifndef MT_FrameBase_H
#define MT_FrameBase_H

/** @addtogroup MT_GUI
 * @{ */

/** @file
 *  MT_FrameBase.h
 *
 *  Defines several frames basic to the operation of 
 *  almost all programs derived from the MT framework.
 *
 *  - MT_GLCanvasBase - OpenGL drawing canvas.
 *  - MT_FrameBase - Base class from which the "Main" window
 *                      for most programs is derived.
 *  - MT_TimerBase - Simple timer to drive the operation of
 *                      MT_FrameBase.
 *  - MT_CheatSheetFrame - Super-simple "Help" dialog.
 *
 *  Created by Daniel Swain on 12/7/09.
 *      - combination of previous classes.
 *
 */


/* --------------------- Headers ----------------------- */

/* Standard WX include block */
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
/* End standard WX include block */

/* wx-specific includes */
#include <wx/glcanvas.h>
#include <wx/cmdline.h>

/* Headers specific to the user interface */

#include "MT/MT_GUI/base/MT_AppBase.h"
#include "MT/MT_GUI/base/MT_ControlFrameBase.h"
#include "MT/MT_GUI/dialogs/MT_DialogManagement.h"
#include "MT/MT_GUI/dialogs/MT_ParameterDialog.h"
#include "MT/MT_GUI/support/wxSupport.h"
#include "MT/MT_GUI/support/wxXMLSupport.h"

/* Headers for functionality */

#include "MT/MT_Core/gl/glSupport.h"
#include "MT/MT_Core/primitives/DataGroup.h"
#include "MT/MT_Core/primitives/Rectangle.h"


/* set up for OpenCV / IPLImage support */

/** @def MT_USE_IMAGES
 * Define MT_USE_IMAGES if you plan to use IPLImages,
 * i.e. to paint a background image on the frame.  Or,
 * conversely, comment this out if you want to build
 * without OpenCV. */
#ifndef MT_NO_OPENCV
#define MT_USE_IMAGES
#endif

#include "MT/MT_Core/gl/glImageSupport.h"
#include "MT/MT_Core/fileio/MovieExporter.h"

/* constants & definitions */

/* IDs for event handlers */
enum
{
    MT_ID_CANVAS = wxID_HIGHEST + 1,
    MT_ID_MENU_FILE_SAVESCREEN,
    MT_ID_MENU_FILE_CREATEMOVIE,
    MT_ID_MENU_VIEW_ZOOM,
    MT_ID_MENU_VIEW_UNZOOM,
    MT_ID_MENU_VIEW_AUTOZOOM,
    MT_ID_MENU_HELP_CHEATSHEET,

    MT_ID_HIGHEST_  /* leave this as the last ID! */
};

/* Doxygen note: hideninitializer makes this display 
 * as 
 * define MT_ID_HIGHEST 
 * and not
 * define MT_ID_HIGHEST MT_ID_HIGHEST_ 
 * which would be confusing */
/** @def MT_ID_HIGHEST
 * @hideinitializer
 * Highest ID claimed by the MT base classes.  If
 * you need to define IDs for your derived class,
 * define them like this:
 * @code
 * enum
 * {
 *   ID_MY_BUTTON = MT_ID_HIGHEST + 1,
 *   ID_MY_MENU_ITEM,
 *   etc...
 * };
 * @endcode
 *
 * @note This <em>shouldn't</em> be necessary in classes
 * that are not derived from MT_FrameBase.
 */
#define MT_ID_HIGHEST MT_ID_HIGHEST_

/** @var const bool MT_DO_BASE_MOUSE
    Return this from the various mouse functions if you want the
    base mouse functionality in MT_GLCanvasBase::onMouse
    @see MT_GLCanvasBase::onMouse
    @see MT_GLCanvasBase::doMouseCallback
    @see MT_FrameBase::doMouseCallback */
const bool MT_DO_BASE_MOUSE = true;

/** @var const bool MT_SKIP_BASE_MOUSE
    Return this from the various mouse functions if you want to
    skip the base mouse functionality.
    @see MT_GLCanvasBase::onMouse
    @see MT_GLCanvasBase::doMouseCallback
    @see MT_FrameBase::doMouseCallback */
const bool MT_SKIP_BASE_MOUSE = false;

/** @var const int MT_MIN_ZOOM_RECT
    Minimum width or height of a rectangle drawn by the user
    before a zoom is considered to be requested, as a fraction
    of the current viewport's height. */
const double MT_MIN_ZOOM_RECT = 0.02;

/** @var const bool MT_DO_BASE_KEY
 * Pass this from the various keyboard functions if you want 
 the base keyboard functionality.
 @see MT_GLCanvasBase::onKeyboard
 @see MT_GLCanvasBase::doKeyboardCallback
 @see MT_FrameBase::doKeyboardCallback */
const bool MT_DO_BASE_KEY = true;

/** @var const bool MT_SKIP_BASE_KEY
 * Pass this from the various keyboard functions if you want to
 skip the base keyboard functionality.
 @see MT_GLCanvasBase::onKeyboard
 @see MT_GLCanvasBase::doKeyboardCallback
 @see MT_FrameBase::doKeyboardCallback */
const bool MT_SKIP_BASE_KEY = false;

/** @var const int MT_DEFAULT_FRAME_PERIOD
 * Default period in msec between successive timer
 * calls and therefore (roughly) the default time
 * step size.  Note that there is no guarantee
 * that this will be the precise time interval.  In fact
 * it could be quite a bit longer depending upon
 * processing time. */
const int MT_DEFAULT_FRAME_PERIOD = 25; /* 25 msec <-> 40 frames per sec */

/** @var const MT_Color MT_DEFAULT_BG_COLOR
 * Default background color of an MT_GLCanvasBase. 
 * Currently white. */
const MT_Color MT_DEFAULT_BG_COLOR = MT_Color(1,1,1,0);

/** @var const double MT_Z_EXTENTS
 * Default + and - depth of the OpenGL Z-buffer, i.e.
 * the extents to which anything can be drawn in the
 * z-axis (in-and-out of the screen).  Since we are
 * drawing primarily in 2D here this should be fine. */
const double MT_Z_EXTENTS = 0.1;

/** @var const wxString MT_FILTER_IMAGE_FILES
 * File dialog filter used when opening or saving image files. 
 * Be sure not to modify this to include any formats that OpenCV
 * is not comfortable with. */
const wxString MT_FILTER_IMAGE_FILES(_T("Image files (*.bmp; *.jpg; *.png)|*.bmp;*.jpg;*.png"));

/* file type filters used in dialogs to determine which files can be selected.
   this list is probably not exhaustive, but contains most of the commonly
   used types. */
const wxString MT_FILTER_MOVIE_FILES(_T("Movie files (*.avi; *.mov; *.dv)|*.avi;*.mov;*.dv"));

const bool MT_SOFT_RESET = false;
const bool MT_HARD_RESET = true;

/* forward class definitions - both of these classes are
 * defined below in this file.  */
class MT_TimerBase;
class MT_FrameBase;
class MT_ControlFrameBase;

/** @class MT_GLCanvasBase
 *
 * @brief Base class for OpenGL drawing and view management.
 *
 * This class is derived from wxGLCanvas and provides the context
 * on which all of our OpenGL drawing is done, including drawing
 * an image if MT_USE_IMAGES and MT_HAVE_OPENCV are defined.
 *
 * The main use of this class is as the "drawing area" on the 
 * main window of most MT applications.
 *
 * The most important thing to understand about the GL canvas
 * is that it defines two different coordinate systems.  The first
 * is the "Window" coordinate system, in which the top left
 * corner of the window is at (0,0) and the bottom right corner
 * is at (x,y) = (width, height) in pixels.  The second
 * coordinate system is the "Viewport" or "Drawing" coordinate
 * system.  This can be defined arbitrarily and corresponds
 * to the units that OpenGL will use for drawing objects.  
 * The default "Viewport" coordinate system defines the 
 * bottom left corner of the window as (0,0) and the top
 * right corner as (width, height) in pixels but with
 * double precision.
 *
 * There is some base functionality built in here:
 *  - Zoom management - 
 *     - If you press "z" and then draw a rectangle
 *        with the mouse, the "viewport" coordinate system 
 *        will zoom to that rectangle.
 *     - If you press "Z" the view will revert to the original.
 *  - Image display -
 *     - If MT_USE_IMAGES and MT_HAVE_OPENCV are defined,
 *        an image can be displayed on the canvas.
 *
 * I've tried to make it so that you don't have to worry about
 * this class.  Any functions that might need to get called here
 * should have an equivalent in MT_FrameBase.  If you find
 * yourself needing to call any functions here, check with me
 * to see if there's a safer way to do what you want to do or
 * maybe it's a design flaw that I can fix so you don't have to.
 *
 * I've made several of the methods here virtual, but I don't
 * intend for users to typically need to override them.  It's
 * done this way from a moral standpoint - i.e. if we were to
 * derive a class from this class these are the functions you
 * would probably want to have as virtual.
 *
 * The methods documented here are the ones that are most 
 * pertinent to understanding how the MT framework *works*,
 * hence the documentation is not intended to provide much
 * insight into how you should call these functions (because
 * you shouldn't need to).
 *
 * Note this frame is designed to be useful in two different
 * situations.  The "normal" situation is to be embedded
 * in an MT_FrameBase, in which case the various callbacks
 * are automatically passed on to the parent.  It can also
 * be used by itself (see, for example MT_CreateBackgroundDialog)
 * in which case only the base functionality callbacks are
 * used.
 *
 */
class MT_GLCanvasBase : public wxGLCanvas
{
    friend class MT_FrameBase;

private:

    GLuint m_itexID;
    IplImage* m_pCurrentImage;

    bool m_bZooming;
    bool m_bAutoZoom;
    bool m_bIsZoomed;
    bool m_bPanning;
    bool m_bGLInitialized;
    bool m_bNeedDraw;

    MT_Color m_BackgroundColor;

    /* these are static and const because they will be the same
     * for every instance of this class and every time they get
     * called.  However, to avoid the "static initialization
     * order fiasco" (see the c++ FAQ) we declare them as static
     * member functions! */
    static const wxCursor& CursorZoom();
    static const wxCursor& CursorPan();

    /** This function can be thought of as the "master" OpenGL drawing
     * function.  It handles initialization and wx-specific code for
     * drawing.  It does something like this:
     *
     * - wx-specific set up and safety checks
     *
     * - Calls glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
     *   - this is the typical start of any OpenGL drawing code,
     *      i.e. you can skip it in your own code, but it probably
     *      won't hurt anything if you don't
     *
     * - Calls MT_GLCanvasBase::doGLInitialization() if necessary 
     *     (i.e. only the first time)
     *
     * - Calls MT_GLCanvasBase::doGLDrawing 
     *
     * - Swaps out the double buffer ("does" the animation)
     *
     * - If an MT_FrameBase is available, its doUserGLInitialization and
     * doUserGLDrawing functions are called automatically.
     *
     * @see MT_GLCanvasBase::doGLInitialization
     * @see MT_GLCanvasBase::doGLDrawing
     */
    void onPaint(wxPaintEvent& event);
    void onSize(wxSizeEvent& event);
    void onEraseBackground(wxEraseEvent& event);
    /** This method is linked to any key press while the
     * canvas has focus.  It has some built-in functionality
     * that can be skipped if you return MT_SKIP_BASE_KEY
     * from a doKeyboardCallback.
     *
     * Base functionality:
     *  - Pressing 'z' enables mouse zoom mode.
     *  - Pressing 'Z' reverts to the original view.
     *  - Pressing 'a' enables auto zoom mode.
     *
     * Calls MT_GLCanvasBase::doKeyboardCallback.
     *
     * @see MT_GLCanvasBase::doKeyboardCallback */
    void onKeyboard(wxKeyEvent& event);
    /** This method is linked to any mouse event on the canvas.
     * It gets called whenever you move or click the mouse while
     * on the canvas.  It has some built-in functionality that
     * can be skipped if you return MT_SKIP_BASE_MOUSE from
     * a doMouseCallback.
     *
     * It does something like this:
     *
     * - Calculates the viewport coordinates of the mouse
     * 
     * - Calls MT_GLCanvasBase::doMouseCallback (providing the
     *      viewport coordinates)
     *
     * - If MT_GLCanvasBase::doMouseCallback returns MT_SKIP_BASE_MOUSE,
     *   the function exits
     *
     * - While the left mouse button is being pressed, a rectangle is
     * constructed that will be used if a zoom is requested. 
     *
     * @see MT_GLCanvasBase::doMouseCallback */
    void onMouse(wxMouseEvent& event);

    MT_FrameBase* m_pMTParent;
    void setMT_FrameBaseParent(MT_FrameBase* parent){m_pMTParent = parent;};

    MT_Rectangle m_CurrentMouseRect;
    MT_PointHistory m_MouseDragHistory;
    MT_PointHistory m_MouseDragHistoryWinCoords;
    bool m_bMouseDragging;
    bool m_bFlagMouseUp;

    MT_Rectangle m_ViewportOrg;
    MT_Rectangle m_CurrentViewport;
    MT_Rectangle m_ObjectLimits;
    MT_Rectangle m_PanningReference;
    MT_RectangleHistory m_ViewportHistory;

    void setZoomOn(){m_bZooming = m_bZooming;};
    void setAutoZoomOn(){m_bAutoZoom = true;};
    void toggleAutoZoom();
    void toggleZoomMode();
    void togglePanMode();
    void resetZoom();

    void zoomTo(const MT_Rectangle& zoom_viewport);
    void lockCurrentViewportAsOriginal(){m_ViewportOrg = m_CurrentViewport;};

    void convertWin2ViewXY(long win_x, 
                           long win_y, 
                           double* view_x, 
                           double* view_y) const ;
    void convertWin2ViewDXDY(long win_dx, 
                             long win_dy, 
                             double* view_dx, 
                             double* view_dy) const ;

protected:
    void paintImage();
    void setBackgroundColor(const MT_Color& color){m_BackgroundColor = color;};
    void tellObjectLimits(const MT_Rectangle& object_limits, double exp_factor = 0);

public:
    void setMTParent(MT_FrameBase* parent){m_pMTParent = parent;};

    MT_GLCanvasBase(wxWindow* parent, 
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize);

    void doDraw(){m_bNeedDraw = true;};

    void setImage(IplImage* image){m_pCurrentImage = image; doDraw();};
	const IplImage* getImage() const {return m_pCurrentImage;};

    void setViewport(const MT_Rectangle& requested = MT_BlankRectangle, bool force = false);

    virtual ~MT_GLCanvasBase();

    /** Does OpenGL initialization.  If a MT_FrameBase is
     * available, calls MT_FrameBase::doUserGLInitialization 
     * @see MT_GLCanvasBase::onPaint
     * @see MT_FrameBase::doUserGLInitialization */
    virtual void doGLInitialization();

    /** Handles base drawing functionality.  Handles 
     * zooming if necessary.  If a MT_FrameBase is
     * available, calls MT_FrameBase::doUserGLDrawing
     * @see MT_GLCanvasBase::onPaint
     * @see MT_FrameBase::doUserGLDrawing */
    virtual void doGLDrawing();

    virtual void doSizeCallback(wxSizeEvent& event);

    /** If a MT_FrameBase is available calls 
     * MT_FrameBase::doKeyboardCallback and returns the
     * result.  Otherwise returns MT_DO_BASE_KEY.
     * @see MT_GLCanvasBase::onKeyboard
     * @see MT_FrameBase::doKeyboardCallback */
    virtual bool doKeyboardCallback(wxKeyEvent& event);
    /** If a MT_FrameBase is available calls 
     * MT_FrameBase::doMouseCallback and returns the
     * result.  Otherwise returns MT_DO_BASE_KEY.
     *
     * @param event - wx mouse event information
     *          (e.g. button states)
     * @param vx - x-coordinate of mouse in viewport coordinates
     * @param vy - y-coordinate of mouse in viewport coordinates
     *
     * @see MT_GLCanvasBase::onKeyboard
     * @see MT_FrameBase::doKeyboardCallback */
    virtual bool doMouseCallback(wxMouseEvent& event, double vx, double vy);

    virtual void doAutoZoom();

    DECLARE_EVENT_TABLE()

        };

/** @class MT_FrameBase
 *
 * @brief Base class for an application's "main" window.
 *
 * MT_FrameBase is the base class from which we derive an application's
 * "main" window.  It's derived from wxFrame (via MT_FrameWithInit).
 *
 * You should be able to get away with deriving most of your application's
 * GUI code by deriving/interfacing this class.  
 *
 * The protected methods and members of this class should be seen as
 * utilities your application can use but not override.  They provide
 * access to the base functionality.
 *
 * The public methods of this class are the virtual functions you
 * may want to override in your derived application.
 *
 * The private members and methods of this class are inaccessible from
 * derived classes.  They are hidden so that you don't have to worry
 * about them and can't break them.  
 *
 * If you find yourself in a position where you need to modify any
 * of the contents of this class, please check with me first.
 * 
 */
class MT_FrameBase : public MT_FrameWithInit
{
    friend class MT_TimerBase;
    friend class MT_ControlFrameBase;

private:
    MT_ControlFrameBase* m_pControlFrame;

    wxString m_sScreenSaveDirectory;
    wxString m_sScreenSavePath;

    wxString m_sMovieExportDirectory;

    MT_DialogGroup m_DialogGroup;

    IplImage* m_pCurrentFrame;
    IplImage* m_pCurrentScreen;
    
    MT_MovieExporter m_MovieExporter;

    bool m_bDoTimedEvents;
    bool m_bPaused;
    bool m_bDrawStatusBar;
    bool m_bQuitOnReset;
    bool m_bMakingMovie;
    int m_iFramePeriod_msec;
    bool m_bDoQuitWasCalled;

    void doMasterInitialization();
    void createMenus();
    void doTimedEvents();

    void writeXML();
    void readXML();


    void onChildMove(wxCommandEvent& event);

    void onSize(wxSizeEvent& event);
    void onClose(wxCloseEvent& event);

protected:
    wxSize m_ClientSize;
    MT_TimerBase* m_pTimer;
    MT_GLCanvasBase* m_pCanvas;

    /** Command line parser.  The base frame adds the "-h"/"--help" options, which will
     * respond with an automatically-generated help message.  You should add to the
     * parser using the wxCmdLineParser::AddXXX() functions in your derivative of
     * MT_FrameBase::initUserData and then handle the results in your derivative of
     * MT_FrameBase::handleCommandLineArguments.  For usage, see the wxWidgets
     * documentation for <a href="http://docs.wxwidgets.org/stable/wx_wxcmdlineparser.html#wxcmdlineparser">wxCmdLineParser</a>.
     * 
     * @see MT_FrameBase::initUserData
     * @see MT_FrameBase::handleCommandLineArguments */
    wxCmdLineParser m_CmdLineParser;

    /** Returns true if the frame has a control frame.  Can be used
     * to check if the control frame's GUI needs to be updated. */
    bool haveControlFrame() const {return m_pControlFrame != NULL;};

    /** MT_XMLFile where application settings will be stored. On OS X this
     * will be ~/Library/Application Support/<your app's name>/<your app's name>.xml
     * On Windows it will be <your app's name>.xml stored in the same directory
     * as the .exe file.  Not sure on linux yet...
     *
     * You may or may not need to actually access this variable.
     *
     * @see MT_GetXMLPathForApp
     * @see MT_FrameBase::readUserXML
     * @see MT_FrameBase::writeUserXML */
    MT_XMLFile m_XMLSettingsFile;

    /** The group of paths that will be automatically stored to the settings XML
     * file.  The only path that is included here by default is the directory
     * in which screen shots are stored.  To use, add something like this
     * @code
     * m_PathGroup.AddPath("readable_name", &m_sPathToAdd);
     * @endcode
     * to your initUserData() function, where "readable_name" is a
     * human-readable key for the path to use when storing it in the XML file 
     * and m_sPathToAdd is a wxString storing the path as a string.  
     * "readable_string" should not have any spaces (use underscores instead) 
     * and should be unique.  The only one claimed by MT_FrameBase is
     * "Screen_Save_Directory".
     *
     * As long as you do this in initUserData, the path should automatically
     * be stored and read from the settings file whenever your application
     * is run.
     * 
     * @see MT_PathGroup
     * */ 
    MT_PathGroup m_PathGroup;
    /** The MT_DataGroup storing preference data for the application that is
     * automatically written/read from the XML file and displayed in the
     * application's "Preferences" dialog.  Add something like this
     * @code
     * m_pPreferences->AddBool("My Flag", &m_bFlag);
     * m_pPreferences->AddDouble("My Double", &m_dDoubleParam);
     * @endcode
     * in your initUserData function.  For more information on what kinds
     * of data can be added, see MT_DataGroup.  
     * 
     * @see MT_DataGroup */
    MT_DataGroup* m_pPreferences;
    /** Description text to be displayed on the command line when the 
     * "-h" or "--help" options are passed and in the CheatSheet window.  
     * This can be set either in initUserData or in the ctor of your 
     * derived class. */
    wxString m_sDescriptionText;

    /* pass-through functions to MT_GLCanvasBase */

    /** Sets the background color of the window.  Stored as a preference. */
    void setBackgroundColor(const MT_Color& color){m_pCanvas->setBackgroundColor(color);};
    /** Sets the drawing "Viewport" that determines the coordinate 
     * transform between window and drawing coordinates.  Automatically 
     * adjusts new_viewport to have the proper aspect ratio.  
     * 
     * If MT_BlankRectangle (MT_Rectangle(0,0,0,0)) is supplied, uses the size of the window.
     *
     * @see MT_GLCanvasBase 
     * @see MT_Rectangle */
    void setViewport(const MT_Rectangle& new_viewport, bool force = false)
    {m_pCanvas->setViewport(new_viewport, force);};
    /** Enables mouse-zooming mode.  Changes the mouse cursor to a 
     * magnifying glass; subsequently drawing a rectangle will cause 
     * the drawing to zoom to that rectangle. 
     *
     * @see MT_FrameBase::resetZoom
     * @see MT_FrameBase::setAutoZoomOn
     * @see MT_FrameBase::zoomTo 
     * @see MT_FrameBase::lockCurrentViewportAsOriginal */
    void setZoomOn(){m_pCanvas->setZoomOn();};
    /** Reset the viewport to the last one locked in as the original by
     * a call to MT_FrameBase::lockCurrentViewportAsOriginal 
     *
     * @see MT_FrameBase::setZoomOn
     * @see MT_FrameBase::setAutoZoomOn
     * @see MT_FrameBase::zoomTo 
     * @see MT_FrameBase::lockCurrentViewportAsOriginal */
    void resetZoom(){m_pCanvas->resetZoom();};
    /** Turn on the (experimental) auto-zoom mode that follows objects.  
     * The zoom is determined by a call to MT_FrameBase::tellObjectLimits 
     *
     * @see MT_FrameBase::tellObjectLimits
     * @see MT_FrameBase::setZoomOn
     * @see MT_FrameBase::resetZoom
     * @see MT_FrameBase::zoomTo 
     * @see MT_FrameBase::lockCurrentViewportAsOriginal */
    void setAutoZoomOn(){m_pCanvas->setAutoZoomOn();};
    /** Tell the auto-zoom system what the current object limits are.
     * @param object_limits A viewport rectangle corresponding to the 
     *    rectangle you want used to determine the auto-zoom.  It's 
     *    handy to generate this using a BoundingBox object.
     * @param exp_factor The fraction you want the object limits 
     *    rectangle to be expanded by.  For example, a value of 0.1 
     *    would expand the viewport by 10%.  The default value 0 
     *    corresponds to no expansion.
     *
     * @see MT_FrameBase::setAutoZoomOn */
    void tellObjectLimits(const MT_Rectangle& object_limits, double exp_factor = 0)
    {
        m_pCanvas->tellObjectLimits(object_limits, exp_factor);
    };
    /** Zoom the current drawing to the specified viewport.  Is essentially the
     * same as a call to setViewport except that it flags to the system that
     * the current view is a zoomed view. 
     *
     * @see MT_FrameBase::tellObjectLimits
     * @see MT_FrameBase::setZoomOn
     * @see MT_FrameBase::resetZoom
     * @see MT_FrameBase::lockCurrentViewportAsOriginal */
    void zoomTo(const MT_Rectangle& zoom_view){m_pCanvas->zoomTo(zoom_view);};
    /** Lock in the current viewport as the "original" one - i.e. the one 
     * reverted to when resetZoom is called (happens when user presses 'Z').
     *
     * @see MT_FrameBase::tellObjectLimits
     * @see MT_FrameBase::setZoomOn
     * @see MT_FrameBase::resetZoom
     * @see MT_FrameBase::zoomTo 
     * @see MT_FrameBase::lockCurrentViewportAsOriginal */
    void lockCurrentViewportAsOriginal()
    {
        m_pCanvas->lockCurrentViewportAsOriginal();
    }
    /** Convert a coordinate in window coordinates (i.e. with 0,0 at the 
     * top left corner and win_width,win_height at the bottom right) 
     * to proper drawing coordinates. */
    void convertWin2ViewXY(long win_x, long win_y, double* view_x, double* view_y) const 
    {
        m_pCanvas->convertWin2ViewXY(win_x, win_y, view_x, view_y);
    };
    /** Query the minimum x value of the original viewport (the one 
     * locked in by calls to lockCurrentViewportAsOriginal). 
     *
     * @see lockCurrentViewportAsOriginal */
    double getXMin() const {return m_pCanvas->m_ViewportOrg.xmin;};
    /** Query the maximum x value of the original viewport (the one 
     * locked in by calls to lockCurrentViewportAsOriginal). 
     *
     * @see lockCurrentViewportAsOriginal */
    double getXMax() const {return m_pCanvas->m_ViewportOrg.xmax;};
    /** Query the minimum y value of the original viewport (the one 
     * locked in by calls to lockCurrentViewportAsOriginal). 
     *
     * @see lockCurrentViewportAsOriginal */
    double getYMin() const {return m_pCanvas->m_ViewportOrg.ymin;};
    /** Query the maximum y value of the original viewport (the one 
     * locked in by calls to lockCurrentViewportAsOriginal). 
     *
     * @see lockCurrentViewportAsOriginal */
    double getYMax() const {return m_pCanvas->m_ViewportOrg.ymax;};

    /** Quits the application.  Does so gracefully - saves all 
     * settings to XML and does whatever garbage collection is 
     * necessary.   Calls writeUserXML and doUserQuit.  
     *
     * @par force_quit Pass false if you don't want the window to 
     *                  actually close.  You almost always want to
     *                  pass true, which is the default, i.e.
     *                  doQuit() is equivalent to doQuit(true)
     *
     * @see MT_FrameBase::writeUserXML 
     * @see MT_FrameBase::doUserQuit */
    void doQuit(bool force_quit = true);

    /** Register a dialog so that it will be automatically updated 
     * on each call to MT_FrameBasee::doStep (every time step when the
     * application is not paused).  The dialog's Update() method
     * will be called.  
     * 
     * @param dialog Pointer to the dialog (derived from MT_DialogWithUpdate)
     *                    to be updated. 
     * @param key A unique key to identify the dialog.  MT_FrameBase 
     *                    does not claim any keys.  
     *
     * @see MT_DialogWithUpdate */
    void registerUpdatableDialog(MT_DialogWithUpdate* dialog,
                                 const string& key);

    /** Register a dialog so that it's state (position and size) will
     * be automatically stored to and recalled from the XML settings file.
     * The dialog's title is used as a key, so be careful not to
     * register multiple dialogs with the same title. If the dialog
     * is found in the XML file, it will be automatically moved to
     * it's last stored position.  */
    void registerDialogForXML(wxWindow* dialog);

    /** Toggle the application's pause state.  When the application is
     * paused events propagated by derivatives of MT_FrameBase::doUserStep
     * are skipped.  Other events continue to occur.  By default, this
     * is called when the user presses 'p'. */
    void togglePause();

    /** Set the pause state manually.
     *
     * @see MT_FrameBase::togglePause */
    void setPause(bool setting = true)
        {m_bPaused = setting; onPauseToggled(m_bPaused);};

    /** Return the application's pause state. 
     *
     * @see MT_FrameBase togglePause */
    bool getPaused(){return m_bPaused;};

    /** Displays the cheat sheet window, which contains, by default,
     * the contents of the usage and cheat sheet text files stored in
     * the application's resources.  
     * 
     * @see getUsageTextFromFile 
     * @see getCheatSheetTextFromFile */
    void displayCheatSheet();
    /** Extracts usage (what you see when you pass the "-h" or "--help" 
     * command line options) from the usage text file.  
     *
     * How to set up your project to have these files:
     *  - Run the application once with the "-h" option and paste the output
     *     into a file called <Your app's name>Usage.txt (note you'll
     *     need to do this again any time you add command line options)
     *  - On OS X, add the text file to the "Copy Bundle Resources" build
     *     step of your application.
     *  - On Windows, place the text file in the same directory as the .exe
     *     file.
     *
     * @see MT_FrameBase::displayCheatSheet
     * @see MT_FrameBase::getCheatSheetTextFromFile() */
    wxString getUsageTextFromFile();
    /** Extracts help text from the cheat sheet text file.  
     *
     * How to set up your project to have these files:
     *  - Create a text file called <Your app's name>CheatSheet.txt and type
     *     whatever help text you want to be displayed in the help window.
     *  - On OS X, add the text file to the "Copy Bundle Resources" build
     *     step of your application.
     *  - On Windows, place the text file in the same directory as the .exe
     *     file.
     *
     * @see MT_FrameBase::displayCheatSheet
     * @see MT_FrameBase::getUsageTextFromFile() */
    wxString getCheatSheetTextFromFile();

    /** Set the status bar text of the control frame (if it exists).
     *
     * @see MT_ControlFrameBase */
    void setControlFrameStatusText(const wxString& text);

    /** Function to save the current canvas to a file.  If filename
     * is NULL then a dialog is displayed asking for the user to
     * specify a file. */
    void saveScreen(char* filename = NULL);

    /** Create the timer that drives the frame's events.  This method is
     * provided as virtual in case you want to derive your own timer from
     * MT_TimerBase.  You shouldn't need to, though. */
    virtual MT_TimerBase* createTimer();
    /** Set's the timer's period in milliseconds and starts the timer. 
     * If the period supplied is zero or is not supplied, then the
     * value of MT_DEFAULT_FRAME_PERIOD is used. */
    void setTimer(int period_msec = 0);
    /** Stop the frame's timer. */
    void stopTimer();

    /** Sets the size of the window so that the client area (the area
     * inside the window, i.e. without the border, titlebar, menubar,
     * etc) has the requested width and height in pixels.  If the
     * requested size is larger than the screen, it will be halved 
     * until it will fit.  Also sets the viewport so that the bottom
     * right corner is at (client_width, client_height), where the 
     * requested values are used (not the sized-to-fit values).
     *
     * For example, if the screen is 1280x1024 and a 1920x1080 
     * client size is requested, the window will be sized to 960x540
     * and the viewport will be set as MT_Rectangle(0, 1920, 0, 1080). 
     *
     * @see MT_FrameBase::setViewport */
    void setSizeByClient(int client_width, int client_height, int pad_height = 0);

    /** Queries the paused state of the application. */
    bool getPaused() const {return m_bPaused;};
    /** Pauses the application.
     *
     * @see MT_FrameBase::togglePause */
    void doPause(){m_bPaused = true;};

	void stopTimedEvents(){m_bDoTimedEvents = false;};

    void ensureDraw(){if(m_pCanvas){m_pCanvas->doDraw();}};

    void drawStatusBar(double frac);

    void setupMovie(const wxString& filename = wxEmptyString);

    virtual void onPauseToggled(bool paused_state){};
    void doStep();

	virtual void fillPopupMenu(wxMenu* menu){};
	bool m_bAmSlave;
	double m_dLastMouseRightX;
	double m_dLastMouseRightY;

public:
    /** Basic constructor.  Doesn't do much.  Most initialization
     * occurs in MT_FrameBase::doMasterInitialization, which calls
     * the virtual method MT_FrameBase::initUserData.
     *
     * @param parent The parent object. When creating from 
     *                 MT_AppBase::createMainFrame, pass NULL.
     * @param id ID for the object.  Passing wxID_ANY should work unless
     *                 there is a specific reason to use otherwise.
     * @param title Window title string.
     * @param pos Initial position of the window.  Under normal 
     *                 circumstances this will get overridden by
     *                 the setting in the XML settings file.
     * @param size Initial window size.  Under normal circumstances
     *                 this will get overridden by the setting in the
     *                 XML settings file.
     * @param style The window style, some combination of the flags defined
     *                 in the wxWidgets <a href="http://docs.wxwidgets.org/stable/wx_wxframe.html#wxframe">documentation</a>.
     *                 The default value is MT_FIXED_SIZE_FRAME, for a
     *                 non-resizable window.
     * */
    MT_FrameBase(wxFrame* parent,
                 wxWindowID id = wxID_ANY,
                 const wxString& title = wxT("Window"),
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxSize(640,480),
                 long style = MT_FIXED_SIZE_FRAME);

    /** Function to set the image displayed on the canvas. */
    virtual void setImage(IplImage* image){m_pCanvas->setImage(image);};

    virtual ~MT_FrameBase();

    /** Function that gets called just before the application quits. */
    virtual void doUserQuit(){};

    /** Function to handle the parsed command line.  At this point,
     * m_CmdLineParser has been parsed and the results can be interpreted.
     * Note the original values of argc and argv are passed to the
     * method though it should not be necessary to access them if the
     * m_CmdLineParser has been used properly.
     *
     * @see m_CmdLineParser */
    virtual void handleCommandLineArguments(int argc, wxChar** argv){};
    /** Function to handle when the application is started by dragging
     * a file onto its icon, by double clicking a file associated with
     * the application, or by choosing "Open With".  I think this is
     * an OS X only function. */
    virtual void handleOpenWithFile(const wxString& filename){};

    /** Keyboard callback.  The following keys are defined by default:
     * - Space bar - Single step forward
     * - q - Exit the application
     * - p - Pause/Unpause the application
     * - P - open the Preferences dialog
     * - Right arrow - Single step forward
     *
     * A single step forward corresponds to a single call to
     * MT_FrameBase::doUserStep.  In addition to these keys, the
     * ones defined in MT_GLCanvasBase::onKeyboard are carried
     * out if MT_DO_BASE_KEYS is returned. 
     *
     * Your derived code should look something like this:
     * @code
     * bool derived_result = MT_DO_BASE_KEYS;
     * switch(event.GetKeyCode())
     * {
     *   case 'u':
     *     handleUKey();  break;
     *    ... other cases ...
     * }
     * bool base_result = MT_FrameBase::doKeyboardCallback(event)
     * return derived_result && base_result
     * @endcode
     * This way the base functionality is still carried out.  Note
     * that if your class is not directly derived from MT_FrameBase
     * then the suitable parent class should be substituted.
     *
     * @see MT_GLCanvas::onKeyboard
     * @see MT_GLCanvas::doKeyboardCallback
     * */
    virtual bool doKeyboardCallback(wxKeyEvent& event);
    virtual bool doMouseCallback(wxMouseEvent& event, 
                                 double viewport_x, 
                                 double viewport_y);

    virtual void onMenuFileSaveScreen(wxCommandEvent& event);
    virtual void onMenuFileCreateMovie(wxCommandEvent& event);
    virtual void onMenuFileQuit(wxCommandEvent& event);
    virtual void onMenuFilePreferences(wxCommandEvent& event);
    virtual void onMenuViewZoom(wxCommandEvent& event);
    virtual void onMenuViewUnzoom(wxCommandEvent& event);
    virtual void onMenuViewAutoZoom(wxCommandEvent& event);
    virtual void onMenuHelpCheatSheet(wxCommandEvent& event);

    virtual wxString getCheatSheetText();

    virtual void readUserXML(){};
    virtual void writeUserXML(){};
    virtual void initUserData(){};
    virtual MT_ControlFrameBase* createControlDialog();
    virtual void createUserDialogs(){};

    virtual void makeFileMenu(wxMenu* file_menu);
    virtual void makeEditMenu(wxMenu* edit_menu){};
    virtual void makeViewMenu(wxMenu* view_menu);
    virtual void makeHelpMenu(wxMenu* help_menu);
    virtual void createUserMenus(wxMenuBar* menubar){};

    virtual void doUserGLInitialization(){};
    virtual void doUserGLDrawing(){};

    /* note doUserTimedEvents gets called *before*
     * doStep and doUserStep */
    virtual void doUserTimedEvents(){};
    virtual void doUserStep(){};
    virtual void doReset(bool hard_reset = MT_HARD_RESET);

};


/** @class MT_TimerBase
 *
 * Timer driving the operation of MT_FrameBase
 */
class MT_TimerBase : public wxTimer
{
private:
    MT_FrameBase* m_pFrame;
public:
MT_TimerBase(MT_FrameBase* parent_frame):m_pFrame(parent_frame){};

    void Notify(){m_pFrame->doTimedEvents();};
};


class MT_CheatSheetFrame : public wxDialog
{
public:
    MT_CheatSheetFrame(wxFrame* parent, const wxString& help_text);
};


/* @} */

#endif // MT_FrameBase_H
