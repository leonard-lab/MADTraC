#ifndef MT_RobotFrameBase_H
#define MT_RobotFrameBase_H

/*
 *  MT_RobotFrameBase.h
 *
 *  Created by Daniel Swain on 1/4/10.
 *
 */

#include "MT/MT_Tracking/base/MT_TrackerFrameBase.h"

/* robot includes */
#include "MT/MT_Robot/robot/AllRobotContainer.h"
#include "MT/MT_Robot/dialogs/MT_JoyStickFrame.h"
#include "MT/MT_Robot/dialogs/MT_RobotCommandDialog.h"
#include "MT/MT_Robot/dialogs/MT_RobotConnectDialog.h"

/* buttons for control frame */
#define MT_RCF_AUTOID     0x08

enum
{
    MT_RFB_ID_MENU_ROBOTS_CONNECT = MT_TFB_ID_HIGHEST + 1,
    MT_RFB_ID_MENU_ROBOTS_JOYSTICK,
    MT_RFB_ID_MENU_ROBOTS_COMMAND,
    
    MT_RFB_ID_HIGHEST_
};

#define MT_RFB_ID_HIGHEST MT_RFB_ID_HIGHEST_

enum
{
    MT_RCF_ID_BUTTON_AUTOIDENTIFY = MT_TCF_ID_HIGHEST + 1,

    MT_RCF_ID_HIGHEST_
};

#define MT_RCF_ID_HIGHEST MT_RCF_ID_HIGHEST_

class MT_RobotControlFrameBase;

class MT_RobotFrameBase : public MT_TrackerFrameBase
{
    friend class MT_RobotControlFrameBase;

private:
    /* joystick control dialog */
    MT_JoyStickFrame* m_pJoyStickFrame;

    /* Robot specific flags */
    /* state flag - are we currently trying to auto-id the robots? */
    bool m_bAutoIdentify;
    /* flag whether or not the robots have been previously identified */
    bool m_bRobotsIdentified;

protected:
    /* correctly-cast pointer to control frame */
    MT_RobotControlFrameBase* m_pRobotControlFrame;

    /* Robot variables */
    /* robot container object */

    MT_AllRobotContainer m_Robots;
    /* Tracked Objects - we'll get these from the tracker */
    MT_TrackedObjectsBase* m_pTrackedObjects;

    void initRobotFrameData();

    virtual void doControl();

    virtual bool startTracking();

    bool toggleAutoIdentify();
    void doAutoIdentify(bool DoAutoID = true);

public:
    // constructor
    MT_RobotFrameBase(wxFrame* parent,
                      wxWindowID id = wxID_ANY,
                      const wxString& title = wxT("Tracker View"),
                      const wxPoint& pos = wxDefaultPosition, 
                      const wxSize& size = wxSize(640,480),     
                      long style = MT_FIXED_SIZE_FRAME);
    // destructor needs to handle memory management
    ~MT_RobotFrameBase();

    virtual void doUserQuit();

    virtual void handleCommandLineArguments(int argc, wxChar** argv);
    virtual void handleOpenWithFile(const wxString& filename);

    virtual bool doKeyboardCallback(wxKeyEvent& event);
    virtual bool doMouseCallback(wxMouseEvent& event, double viewport_x, double viewport_y);

    /* menu callbacks */
    virtual void onMenuRobotsConnect(wxCommandEvent& event);
    virtual void onMenuRobotsJoystick(wxCommandEvent& event);
    virtual void onMenuRobotsCommand(wxCommandEvent& event);

    virtual void readUserXML();
    virtual void writeUserXML();
    virtual void initUserData();
    virtual MT_ControlFrameBase* createControlDialog();
    
    virtual void makeRobotMenu(wxMenu* robot_menu);
    virtual void createUserMenus(wxMenuBar* menubar);

    virtual void doUserGLDrawing();
    virtual void doUserTimedEvents();
    virtual void doUserStep();

    virtual void initTracker(){m_pTracker = NULL;};

    virtual void doUserControl(){};

    virtual void updateRobotStatesFromTracker();

    virtual MT_RobotBase* getNewRobot(const char* config, const char* name);    

};

class MT_RobotControlFrameBase: public MT_TrackerControlFrameBase
{
    friend class MT_ControlFrameBase;
    friend class MT_FrameBase;
    friend class MT_RobotFrameBase;

private:
    MT_RobotFrameBase* m_pParentRobotFrame;
    wxButton* m_pButtonAutoIdentify;

public:

    MT_RobotControlFrameBase(MT_RobotFrameBase* parent,
                             const int Buttons = MT_TCF_DEFAULT_BUTTONS | MT_RCF_AUTOID,
                             const wxPoint& pos = wxDefaultPosition, 
                             const wxSize& size = wxSize(150,300));
    virtual ~MT_RobotControlFrameBase(){};

    virtual void doMasterInitialization();

    virtual unsigned int createButtons(wxBoxSizer* pSizer, wxPanel* pPanel);

    virtual void onButtonAutoIdentifyPressed(wxCommandEvent& event);

    virtual void onAutoIDToggle(bool AutoID);
    virtual void onTrackingToggle(bool tracking);

    virtual void enableButtons();

};



#endif // MT_RobotFrameBase_H
