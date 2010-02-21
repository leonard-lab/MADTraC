#ifndef SEGMENTERGUI_H
#define SEGMENTERGUI_H

/** @addtogroup GY_Segmenter
 * @{ */

/** @file
 *  SegmenterGUI.h
 *
 *  A (functional) example of how the MT framework can be used
 *  to implement a tracker app.  The code here is specific to this
 *  application and relies on base functionality built into the
 *  MT_TrackerFrameBase, MT_FrameBase, and MT_AppBase classes.
 *  
 *  This file focuses on the GUI / interface portion of the code.
 *  To see how the actual tracker is derived from MT_TrackerBase,
 *  see GYSegmenter.h.
 *
 *  Here we derive two classes:
 *   - SegmenterApp from MT_AppBase just creates the main frame
 *      (window) and is really only formally necessary.
 *   - SegmenterFrame from MT_TrackerFrameBase defines how this
 *      application should behave and which modules it should use.
 *
 *  Created by Daniel Swain on 10/18/09.
 *
 */

#include "MT_Core.h"
#include "MT_GUI.h"
#include "MT_Tracking.h"

/** @class SegmenterFrame
 *
 * @brief Main frame (window) of the Segmenter applicaiton.
 *
 * Needs to define
 * - Which tracking module we want to use on SegmenterFrame::initTracker
 * - Any frame-level data specific to this application in 
 *      SegmenterFrame::initUserData
 * - Any command line arguments in SegmenterFrame::initUserData and 
 *      SegmenterFrame::handleCommandLineArguments
 *
 */
class SegmenterFrame : public MT_TrackerFrameBase
{
protected:
    /** The derivative of MT_TrackerBase that we use as a tracking module.
     * There is a pointer MT_TrackerFrameBase::m_pTracker that is casted
     * down to the base class.  It's a good idea to have a pointer to
     * the derivative in case we want to call any methods not defined
     * in MT_TrackerBase. */
    GYSegmenter* m_pGYTracker;
    /** Our tracker needs to know how many objects we're going to track. */
    unsigned int m_iDefaultNumToTrack;
    /** A flag we'll use to determine if the user specified the number
     * to track from the command line (i.e. -n).  Otherwise it'll be read from the
     * XML file. */
    bool m_bNumToTrackInCmdLine;

public:
    /** SegmenterFrame ctor.  Initializes m_iDefaultNumToTrack to 1 
     * (it should get overwritten later) m_bNumToTrackInCmdLine to false. 
     *
     * @param parent Parent frame - should be NULL.
     * @param id ID for wx event system - default wxID_ANY should be OK. 
     * @param title Window title string - default is "Tracker View".
     * @param pos Initial position of window - default wxDefaultPosition
     *                  is the top left corner of the screen, but should
     *                  get overwritten by the setting in the XML file
     *                  after the first time
     * @param size Initial size of the window - default 640x480 will get
     *                  changed automatically when a video is loaded. 
     * @param style Window style - default MT_FIXED_SIZE_FRAME is a window
     *                  that cannot be resized and has no maximize button.*/
    SegmenterFrame(wxFrame* parent,
                   wxWindowID id = wxID_ANY,
                   const wxString& title = wxT("Tracker View"), 
                   const wxPoint& pos = wxDefaultPosition, 
                   const wxSize& size = wxSize(640,480),     
                   long style = MT_FIXED_SIZE_FRAME);

    /** The dtor should handle memory management at the frame level.  In this
     * case we don't really need to do anything. */
    virtual ~SegmenterFrame(){};

    /** This function gets called when the user clicks "Start Tracking" (or
     * specifies on the command line that tracking should start immediately).
     * It initializes the tracking module by creating a new instance of the
     * MT_TrackerBase derived class GYSegmenter.  Since our segmenter needs
     * to know how many individuals to look for, it asks the user for input
     * if that input has not already been specified at the command line. */
    virtual void initTracker();

    /** This function gets called when the frame first appears.  It sets
     * up command line options specific to this app, adds 
     * m_iDefaultNumToTrack to the preferences that will get written to /
     * read from the XML file, and calls 
     * MT_TrackerFrameBase::initTrackerFrameData to make sure the base
     * initialization is done. */
    virtual void initUserData();

    /** This function is called after the application starts (after
     * all the ctors and initUserData up to handle any arguments that
     * were specified at the command line. Also calls 
     * MT_TrackerFrameBase::handleCommandLineArguments to make sure
     * the base functionality gets carried out. 
     *
     * The argc and argv variables provided here correspond to the 
     * standard definition of
     * @code
     * int main(int argc, char** argv){...}
     * @endcode
     * However, I strongly suggest using the wx command line parser,
     * which automatically parses the arguments in a fairly robust
     * and extensible manner.  See the code here for an example. */
    virtual void handleCommandLineArguments(int argc, char** argv);

};

/** @class SegmenterApp
 *
 * @brief Application class for GY Segmenter
 *
 * Each application must derive a class from MT_AppBase. This
 * defines the main entry point for the application.  The only
 * required method here is a derivative of
 * MT_AppBase::createMainFrame (here SegmenterApp::createMainFrame)
 * which should return a pointer to a new derivative of
 * MT_TrackerFrameBase.
 *
 * Note that in the cpp we also have the line
 * @code
 * IMPLEMENT_APP(SegmenterApp)
 * @endcode
 * to let the wx system know which class is our "application" class.
 *
 */
class SegmenterApp : public MT_AppBase
{

public:
    /** Creates a SegmenterFrame - this kind of starts things going.  
     * Note that this line does not cause the frame to be displayed, 
     * it only creates it in memory (hence calls the ctor, etc). 
     * The app class will show it for you. 
     *
     * See code, does something like
     * @code
     * return new SegmenterFrame( ... with parameters ... ); 
     * @endcode
     * */
    MT_FrameWithInit* createMainFrame()
    {
        return new SegmenterFrame(NULL, /* no parent frame 
                                           (the app is the parent) */
                                  wxID_ANY,           /* no need for a specific ID */
                                  wxT("Tracker"),     /* window title bar text  */
                                  wxDefaultPosition,  /* position of the window 
                                                         (probably top left) */
                                  wxSize(640,480));   /* window size (gets resized
                                                         when you load a video /
                                                         initialize camera */
    }
};

/* @} */

#endif // SEGMENTERGUI_H
