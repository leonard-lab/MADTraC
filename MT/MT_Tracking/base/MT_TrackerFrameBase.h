#ifndef MT_TrackerFrameBase_H
#define MT_TrackerFrameBase_H

/*
 *  wxTrackerFrameCore.h
 *
 *  Created by Daniel Swain on 11/18/09.
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

/* Headers specific to the user interface */
#include "MT/MT_GUI/base/MT_FrameBase.h"
#include "MT/MT_GUI/base/MT_ControlFrameBase.h"

/* Headers for functionality */

// FrameCapture is the camera/avi capture class
#include "MT/MT_Tracking/capture/MT_Capture.h"
#include "MT/MT_Tracking/base/MT_TrackerBase.h"


/* ---------- Definitions of default parameters --------- */
/* (those pertinent to this module) */

// default directories to search for files - these get overwritten
//  by the XML settings once you access them once yourself
#define MT_DEFAULT_AVI_DIR wxT(".")
#define MT_DEFAULT_ROI_DIR wxT(".")
#define MT_DEFAULT_BACKGROUND_DIR wxT(".")
#define MT_DEFAULT_DATA_DIR wxT(".")

/* button selectors for control frame */
#define MT_TCF_TRACK      0x04
#define MT_TCF_DEFAULT_BUTTONS MT_CF_DEFAULT_BUTTONS | MT_TCF_TRACK


/* ----------- Non-class data structures ---------------- */

// enums to keep track of which frame we want to view
enum
{
    MT_VIEW_FRAME,
    MT_VIEW_DIFF,
    MT_VIEW_THRESH
};

enum
{
    MT_TCF_ID_BUTTON_TRACK = MT_CF_ID_HIGHEST + 1,

    MT_TCF_ID_HIGHEST_  /* leave this as the last one */
};
/** @def MT_TCF_ID_HIGHEST
 * @hideinitializer
 * Highest ID claimed by the MT tracker control frame base class.  
 * If you need to define IDs for your derived class,
 * define them like this:
 * @code
 * enum
 * {
 *   ID_MY_BUTTON = MT_TCF_ID_HIGHEST + 1,
 *   ID_MY_MENU_ITEM,
 *   etc...
 * };
 * @endcode
 *
 * @note This <em>shouldn't</em> be necessary in classes
 * that are not derived from MT_TrackerControlFrameBase.
 */
#define MT_TCF_ID_HIGHEST MT_TCF_ID_HIGHEST_

const unsigned int MT_TFB_MAX_NUM_VIEWS = 10;
const unsigned int MT_TFB_MAX_NUM_TRACKER_PARAM_GROUPS = 10;
const unsigned int MT_TFB_MAX_NUM_TRACKER_REPORTS = 10;

enum
{
    MT_TFB_ID_MENU_CAPTURE_FROM_CAMERA = MT_ID_HIGHEST + 1,
    MT_TFB_ID_MENU_FILE_SELECT_ROI,
    MT_TFB_ID_MENU_FILE_CREATE_BACKGROUND,
    MT_TFB_ID_MENU_FILE_SELECT_BACKGROUND,
    MT_TFB_ID_MENU_FILE_SELECT_DATAFILE,
    MT_TFB_ID_MENU_VIEW_FRAME00,
    MT_TFB_ID_MENU_VIEW_FRAME01,
    MT_TFB_ID_MENU_VIEW_FRAME02,
    MT_TFB_ID_MENU_VIEW_FRAME03,
    MT_TFB_ID_MENU_VIEW_FRAME04,
    MT_TFB_ID_MENU_VIEW_FRAME05,
    MT_TFB_ID_MENU_VIEW_FRAME06,
    MT_TFB_ID_MENU_VIEW_FRAME07,
    MT_TFB_ID_MENU_VIEW_FRAME08,
    MT_TFB_ID_MENU_VIEW_FRAME09,
    MT_TFB_ID_MENU_TRACKER_TRAIN,
    MT_TFB_ID_MENU_TRACKER_PARAMS00,
    MT_TFB_ID_MENU_TRACKER_PARAMS01,
    MT_TFB_ID_MENU_TRACKER_PARAMS02,
    MT_TFB_ID_MENU_TRACKER_PARAMS03,
    MT_TFB_ID_MENU_TRACKER_PARAMS04,
    MT_TFB_ID_MENU_TRACKER_PARAMS05,
    MT_TFB_ID_MENU_TRACKER_PARAMS06,
    MT_TFB_ID_MENU_TRACKER_PARAMS07,
    MT_TFB_ID_MENU_TRACKER_PARAMS08,
    MT_TFB_ID_MENU_TRACKER_PARAMS09,
    MT_TFB_ID_MENU_TRACKER_REPORTS00,
    MT_TFB_ID_MENU_TRACKER_REPORTS01,
    MT_TFB_ID_MENU_TRACKER_REPORTS02,
    MT_TFB_ID_MENU_TRACKER_REPORTS03,
    MT_TFB_ID_MENU_TRACKER_REPORTS04,
    MT_TFB_ID_MENU_TRACKER_REPORTS05,
    MT_TFB_ID_MENU_TRACKER_REPORTS06,
    MT_TFB_ID_MENU_TRACKER_REPORTS07,
    MT_TFB_ID_MENU_TRACKER_REPORTS08,
    MT_TFB_ID_MENU_TRACKER_REPORTS09,
    MT_TFB_ID_MENU_TRACKER_NOTE,

    MT_TFB_ID_HIGHEST_
};

const unsigned int MT_MAX_NUM_TRACKER_REPORTS = 10;

/** @def MT_TFB_ID_HIGHEST
 * @hideinitializer
 * Highest ID claimed by the MT tracker frame base class.  
 * If you need to define IDs for your derived class,
 * define them like this:
 * @code
 * enum
 * {
 *   ID_MY_BUTTON = MT_TFB_ID_HIGHEST + 1,
 *   ID_MY_MENU_ITEM,
 *   etc...
 * };
 * @endcode
 *
 * @note This <em>shouldn't</em> be necessary in classes
 * that are not derived from MT_TrackerFrameBase.
 */
#define MT_TFB_ID_HIGHEST MT_TFB_ID_HIGHEST_


/* Forward class definitions */
class MT_TrackerFrameBase;

class MT_TrackerControlFrameBase : public MT_ControlFrameBase
{
    friend class MT_ControlFrameBase;
    friend class MT_FrameBase;
private:
    MT_TrackerFrameBase* m_pParentTrackerFrame;
    wxButton* m_pButtonTrack;

protected:

public:
    /* Constructor */
    MT_TrackerControlFrameBase(MT_TrackerFrameBase* parent,
                               const int Buttons = MT_TCF_DEFAULT_BUTTONS,
                               const wxPoint& pos = wxDefaultPosition, 
                               const wxSize& size = wxSize(150,300));
    virtual ~MT_TrackerControlFrameBase(){};

    virtual void doMasterInitialization();

    virtual unsigned int createButtons(wxBoxSizer* pSizer, wxPanel* pPanel);

    virtual void onButtonTrackPressed(wxCommandEvent& event);

    virtual void onTrackingToggle(bool tracking);

    virtual void enableButtons();

};

class MT_TrackerFrameBase : public MT_FrameBase
{
    friend class MT_TrackerControlFrameBase;
private:

    // various directories / paths
    wxString m_sAVIDirectory;
    wxString m_sROIDirectory;
    wxString m_sBackgroundDirectory;
    wxString m_sDataFileDirectory;
    wxString m_sAVIPath;
    wxString m_sROIPath;
    wxString m_sBackgroundPath;
    wxString m_sDataFilePath;

    // state flags
    bool m_bTracking;
    // keep track of which frame we should be viewing (from the enum above)
    int m_iView;

    void updateMenusOnStartTracker();
    void addTrackerToViewMenu(wxMenu* view_menu);
    void addDataGroupsToTrackerMenu(wxMenu* tracker_menu);
    void addDataReportsToTrackerMenu(wxMenu* tracker_menu);
    void enableFrame();

protected:

    /* pointer to the control frame 
     *  - protected b/c needs to be available to children,
     *  but not the outside world */
    MT_TrackerControlFrameBase* m_pTrackerControlFrame;

    // pointer to our capture
    MT_Capture* m_pCapture;
    /* pointer to a base tracker class */
    MT_TrackerBase* m_pTracker;

    bool getIsTracking() const {return m_bTracking;};
    
    IplImage* m_pCurrentFrame;

    long m_lTrackerDrawingFlags;

    bool selectAVIFile();
    void selectROI();
    void createBackground();
    void selectBackground();
    void selectDataFile();

    bool setupCameraCapture();
    bool setupAVICapture(const char* filename);
    
    /* gets called after setupCameraCapture and setupAVICapture
     * if they were successful */
    virtual void onNewCapture(){};

    void selectFile();

    void doTrackerGLDrawing();
    void initTrackerFrameData();
    void writeTrackerXML();
    void doTrackerStep();

    /* virtual because the robot frame needs to interject here */
    virtual bool startTracking();

public:
    // constructor
    MT_TrackerFrameBase(wxFrame* parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& title = wxT("Window"),
                        const wxPoint& pos = wxDefaultPosition, 
                        const wxSize& size = wxSize(640,480),     
                        long style = MT_FIXED_SIZE_FRAME); 
    // destructor needs to handle memory management
    virtual ~MT_TrackerFrameBase();

    virtual void doUserQuit()
    { MT_FrameBase::doUserQuit(); };

    virtual void handleCommandLineArguments(int argc, wxChar** argv);
    virtual void handleOpenWithFile(const wxString& filename);

    virtual bool doKeyboardCallback(wxKeyEvent& event);
    virtual bool doMouseCallback(wxMouseEvent& event, double viewport_x, double viewport_y);

    virtual void onMenuFileQuit(wxCommandEvent& event)
    { MT_FrameBase::onMenuFileQuit(event); };
    virtual void onMenuFilePreferences(wxCommandEvent& event)
    { MT_FrameBase::onMenuFilePreferences(event); };
    virtual void onMenuViewZoom(wxCommandEvent& event)
    { MT_FrameBase::onMenuViewZoom(event); };
    virtual void onMenuViewUnzoom(wxCommandEvent& event)
    { MT_FrameBase::onMenuViewUnzoom(event); };
    virtual void onMenuViewAutoZoom(wxCommandEvent& event)
    { MT_FrameBase::onMenuViewAutoZoom(event); };
    virtual void onMenuHelpCheatSheet(wxCommandEvent& event)
    { MT_FrameBase::onMenuHelpCheatSheet(event); };

    virtual void onMenuFileCamera(wxCommandEvent& event);
    virtual void onMenuFileSelectAVI(wxCommandEvent& event);
    virtual void onMenuFileSelectROI(wxCommandEvent& event);
    virtual void onMenuFileCreateBackground(wxCommandEvent& event);
    virtual void onMenuFileSelectBackground(wxCommandEvent& event);
    virtual void onMenuFileSelectDataFile(wxCommandEvent& event);
    virtual void onMenuViewSelect(wxCommandEvent& event);
    virtual void onMenuTrackerTrain(wxCommandEvent& event);
    virtual void onMenuTrackerParams(wxCommandEvent& event);
    virtual void onMenuTrackerReports(wxCommandEvent& event);
    virtual void onMenuTrackerNote(wxCommandEvent& event);

    virtual wxString getCheatSheetText()
    { return MT_FrameBase::getCheatSheetText(); };

    virtual void readUserXML(){MT_FrameBase::readUserXML();};
    virtual void writeUserXML();
    virtual void initUserData();
    virtual MT_ControlFrameBase* createControlDialog();
    virtual void createUserDialogs(){MT_FrameBase::createUserDialogs();};

    virtual void makeFileMenu(wxMenu* file_menu);
    virtual void makeEditMenu(wxMenu* edit_menu)
    { MT_FrameBase::makeEditMenu(edit_menu); };
    virtual void makeViewMenu(wxMenu* view_menu)
    { MT_FrameBase::makeViewMenu(view_menu); };
    virtual void makeTrackerMenu(wxMenu* tracker_menu);
    virtual void makeHelpMenu(wxMenu* help_menu)
    { MT_FrameBase::makeHelpMenu(help_menu); };
    virtual void createUserMenus(wxMenuBar* menubar);

    virtual void doUserGLInitialization()
    { MT_FrameBase::doUserGLInitialization(); };
    virtual void doUserGLDrawing();

    virtual void doUserTimedEvents()
    { MT_FrameBase::doUserTimedEvents(); };
    virtual void doUserStep();

    virtual void initTracker();

    bool getIsTracking(){return m_bTracking;};

};

class MT_XDFNoteDialog : public wxDialog
{
private:
    wxTextCtrl* m_pTextCtrl;
    wxString* m_pNote;
public:
    MT_XDFNoteDialog(wxFrame* parent, wxString* note);

    void onOKClicked(wxCommandEvent& event);
};


#endif // MT_TrackerFrameBase_H
