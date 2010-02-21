/*
 *  MT_RobotFrameBase.cpp
 *
 *  Created by Daniel Swain on 1/4/10.
 *
 */

#include "MT_RobotFrameBase.h"

MT_RobotFrameBase::MT_RobotFrameBase(wxFrame* parent,
                                     wxWindowID id,
                                     const wxString& title,
                                     const wxPoint& pos, 
                                     const wxSize& size,     
                                     long style)
  : MT_TrackerFrameBase(parent, id, title, pos, size, style),
    m_pRobotControlFrame(NULL),
    m_Robots(),
    m_pTrackedObjects(NULL),
    m_pJoyStickFrame(NULL),
    m_bAutoIdentify(false),
    m_bRobotsIdentified(false)
{
}

MT_RobotFrameBase::~MT_RobotFrameBase()
{

}

void MT_RobotFrameBase::handleCommandLineArguments(int argc, char** argv)
{
    MT_TrackerFrameBase::handleCommandLineArguments(argc, argv);
}

void MT_RobotFrameBase::handleOpenWithFile(const wxString& filename)
{
    MT_TrackerFrameBase::handleOpenWithFile(filename);
}

bool MT_RobotFrameBase::doKeyboardCallback(wxKeyEvent& event)
{
    bool tresult = MT_TrackerFrameBase::doKeyboardCallback(event);
    return tresult && MT_DO_BASE_KEY;
}

bool MT_RobotFrameBase::doMouseCallback(wxMouseEvent& event, double viewport_x, double viewport_y)
{
    bool tresult = MT_TrackerFrameBase::doMouseCallback(event, viewport_x, viewport_y);
    return tresult && MT_DO_BASE_MOUSE;
}


void MT_RobotFrameBase::onMenuRobotsConnect(wxCommandEvent& event)
{
    MT_RobotConnectDialog* dlg = new MT_RobotConnectDialog(&m_Robots, this);
    registerDialogForXML(dlg);
    dlg->Show(true);
}

void MT_RobotFrameBase::onMenuRobotsJoystick(wxCommandEvent& event)
{
    m_pJoyStickFrame->Show(true);
    registerDialogForXML(m_pJoyStickFrame);
    m_pJoyStickFrame->EnableEvents();
}

void MT_RobotFrameBase::onMenuRobotsCommand(wxCommandEvent& event)
{
    MT_RobotCommandDialog* dlg = new MT_RobotCommandDialog(&m_Robots, this);
    registerDialogForXML(dlg);
    dlg->Show(true);
}

void MT_RobotFrameBase::readUserXML()
{
    MT_TrackerFrameBase::readUserXML();

    float maxspeed = MT_DEFAULT_MAX_SPEED;
    float maxturningrate = MT_DEFAULT_MAX_TURNING_RATE;
    MT_ReadRobotXML(&m_Robots, &maxspeed, &maxturningrate, &m_XMLSettingsFile, false);
    m_Robots.MaxGamePadSpeed = maxspeed;
    m_Robots.MaxGamePadTurningRate = maxturningrate;

}

void MT_RobotFrameBase::writeUserXML()
{
    MT_WriteRobotXML(&m_Robots, 
                  m_Robots.MaxGamePadSpeed,
                  m_Robots.MaxGamePadTurningRate,
                  &m_XMLSettingsFile,
                  false);
    MT_TrackerFrameBase::writeUserXML();
}

void MT_RobotFrameBase::initUserData()
{
    MT_RobotFrameBase::initRobotFrameData();
    MT_TrackerFrameBase::initUserData();
}

void MT_RobotFrameBase::initRobotFrameData()
{
    m_pJoyStickFrame = new MT_JoyStickFrame(this, &m_Robots);
}

void MT_RobotFrameBase::createUserMenus(wxMenuBar* menubar)
{
    wxMenu* robot_menu = new wxMenu;
    makeRobotMenu(robot_menu);
    menubar->Append(robot_menu, wxT("Robots"));

    /* adds tracker menu */
    MT_TrackerFrameBase::createUserMenus(menubar);
}

void MT_RobotFrameBase::makeRobotMenu(wxMenu* robot_menu)
{
    robot_menu->Append(MT_RFB_ID_MENU_ROBOTS_CONNECT, wxT("&Connections..."));
    Connect(MT_RFB_ID_MENU_ROBOTS_CONNECT, 
            wxEVT_COMMAND_MENU_SELECTED, 
            wxCommandEventHandler(MT_RobotFrameBase::onMenuRobotsConnect));
    robot_menu->Append(MT_RFB_ID_MENU_ROBOTS_JOYSTICK, wxT("&Joystick Control..."));
    Connect(MT_RFB_ID_MENU_ROBOTS_JOYSTICK, 
            wxEVT_COMMAND_MENU_SELECTED, 
            wxCommandEventHandler(MT_RobotFrameBase::onMenuRobotsJoystick));
    robot_menu->Append(MT_RFB_ID_MENU_ROBOTS_COMMAND, wxT("&Send Command..."));
    Connect(MT_RFB_ID_MENU_ROBOTS_COMMAND, 
            wxEVT_COMMAND_MENU_SELECTED, 
            wxCommandEventHandler(MT_RobotFrameBase::onMenuRobotsCommand));
}

MT_ControlFrameBase* MT_RobotFrameBase::createControlDialog()
{
    m_pRobotControlFrame = new MT_RobotControlFrameBase(this);
    m_pTrackerControlFrame = (MT_TrackerControlFrameBase *) m_pRobotControlFrame; 
    return (MT_ControlFrameBase*) m_pRobotControlFrame;
}

void MT_RobotFrameBase::doUserTimedEvents()
{

}

void MT_RobotFrameBase::doUserStep()
{
    /* do tracking first, then control */
    MT_TrackerFrameBase::doUserStep();

    if(m_bAutoIdentify)
    {
        doAutoIdentify();
    }

    updateRobotStatesFromTracker();

    doControl();
}

void MT_RobotFrameBase::updateRobotStatesFromTracker()
{
    int ti;
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        ti = m_Robots.TrackingIndex[i];
        if(ti != MT_NOT_TRACKED)
        {
            m_Robots.UpdateState(i, 
                                 m_pTrackedObjects->getX(ti), 
                                 m_pTrackedObjects->getY(ti), 
                                 m_pTrackedObjects->getOrientation(ti));
        }
    }
}

void MT_RobotFrameBase::doControl()
{
    doUserControl();
}

bool MT_RobotFrameBase::startTracking()
{
    bool now_tracking = MT_TrackerFrameBase::startTracking();

    if(now_tracking && m_pTracker)
    {
        /* grab the tracked objects from the tracker */
        m_pTrackedObjects = m_pTracker->getTrackedObjects();
    }

    return now_tracking;
}

bool MT_RobotFrameBase::toggleAutoIdentify()
{

    // it only makes sense to do any of this if the tracker is up and running
    if(getIsTracking())
    {

        // toggle the value of the auto id flag    
        if(m_bAutoIdentify)
        {
            m_bAutoIdentify = false;
            // stop the auto-id process
            doAutoIdentify(false);
        }
        else
        {
            m_bAutoIdentify = true;
            m_bRobotsIdentified = false;
        }

    }

    return m_bAutoIdentify;

}

void MT_RobotFrameBase::doAutoIdentify(bool DoAutoID)
{

    /* we'll want to know if this has changed */
    static int RobotToID = MT_NONE_AVAILABLE;

    /* if we're flagged to stop, then stop whichever robot may be moving for autoID */
    if(!DoAutoID)
    {
        if(RobotToID != MT_NONE_AVAILABLE)
        {
            m_Robots.GetRobot(RobotToID)->SetSpeedOmega(0,0);
        }
        RobotToID = MT_NONE_AVAILABLE;
        return;
    }

    /* first, make sure there's a robot to identify */
    int newRobotToID = m_Robots.GetNextUntracked();

    if(newRobotToID == MT_NONE_AVAILABLE)
    {
        /* none available! */
        RobotToID = MT_NONE_AVAILABLE;
        m_bRobotsIdentified = true;
        m_bAutoIdentify = false;
        /* change the label on the control frame  */
        m_pRobotControlFrame->onAutoIDToggle(false);
        return;
    }

    /* if the robots have been previously identified, reset the IDs */
    if(m_bRobotsIdentified)
    {
        m_Robots.ClearTrackingIDs();
    }

    /* OK, now we've got a robot to identify
       if it's new, we want to start it moving slowly */
    if(newRobotToID != RobotToID)
    {
        /* remember which robot for next time */
        RobotToID = newRobotToID;
        /* start it moving */
        m_Robots.GetRobot(RobotToID)->SetSpeedOmega(0.1,0);
    }

    /* loop through tracked objects (until match is found) */
    for(int i = 0; i < m_pTrackedObjects->getNumObjects(); i++)
    {
        /* if this TO has been in 5 or more frames and not attached to a robot
           and is moving */
        if( (m_pTrackedObjects->getNumConsecutiveFrames(i) > 5) && 
            (m_pTrackedObjects->getRobotIndex(i) == MT_NO_ROBOT) &&
            (m_pTrackedObjects->getIsMoving(i)) )
        {
            /* stop the robot */
            m_Robots.GetRobot(RobotToID)->SetSpeedOmega(0,0);
            /* attach the current robot */
            m_pTrackedObjects->setRobotIndex(i, RobotToID);
            /* assign the TO ID to the robot */
            m_Robots.TrackingIndex[RobotToID] = i;
        }
    }

}

void MT_RobotFrameBase::doUserQuit()
{
    MT_TrackerFrameBase::doUserQuit();
}

void MT_RobotFrameBase::doUserGLDrawing()
{
    MT_TrackerFrameBase::doUserGLDrawing();
}


MT_RobotControlFrameBase::MT_RobotControlFrameBase(MT_RobotFrameBase* parent, 
                                                   const int Buttons,
                                                   const wxPoint& pos, 
                                                   const wxSize& size)
  : MT_TrackerControlFrameBase(parent, Buttons, pos, size)
{
    m_pParentRobotFrame = parent;
}

void MT_RobotControlFrameBase::doMasterInitialization()
{
    MT_TrackerControlFrameBase::doMasterInitialization();
}

unsigned int MT_RobotControlFrameBase::createButtons(wxBoxSizer* pSizer, wxPanel* pPanel)
{
    unsigned int nbuttons = MT_TrackerControlFrameBase::createButtons(pSizer, pPanel);

    if(getButtons() & MT_RCF_AUTOID)
    {
        m_pButtonAutoIdentify = new wxButton(pPanel, 
                                             MT_RCF_ID_BUTTON_AUTOIDENTIFY, 
                                             wxT("AutoID Robots"));
        pSizer->Add(m_pButtonAutoIdentify, 0, wxALL | wxCENTER, 10);
        m_pButtonAutoIdentify->Disable();
        Connect(MT_RCF_ID_BUTTON_AUTOIDENTIFY, 
                wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(
                    MT_RobotControlFrameBase::onButtonAutoIdentifyPressed));
        nbuttons++;
    }
    return nbuttons;
}

void MT_RobotControlFrameBase::onButtonAutoIdentifyPressed(wxCommandEvent& WXUNUSED(event))
{
    onAutoIDToggle(m_pParentRobotFrame->toggleAutoIdentify());
}

void MT_RobotControlFrameBase::onAutoIDToggle(bool AutoID)
{

    if(AutoID == false)
    {
        m_pButtonAutoIdentify->SetLabel(wxT("Reset Auto Identify"));
    }
    else
    {
        m_pButtonAutoIdentify->SetLabel(wxT("Stop Auto Identify"));
    }

}

void MT_RobotControlFrameBase::onTrackingToggle(bool tracking)
{

    MT_TrackerControlFrameBase::onTrackingToggle(tracking);

    if(m_pButtonAutoIdentify)
    {
        if(tracking)
        {
            m_pButtonAutoIdentify->Enable();
        }
        else
        {
            m_pButtonAutoIdentify->Disable();
        }
    }

}

void MT_RobotControlFrameBase::enableButtons()
{
    if(m_pButtonAutoIdentify)
    {
        m_pButtonAutoIdentify->Enable();
    }
    MT_TrackerControlFrameBase::enableButtons();
}
