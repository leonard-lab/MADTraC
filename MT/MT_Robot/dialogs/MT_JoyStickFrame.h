#ifndef MT_JOYSTICKFRAME_H
#define MT_JOYSTICKFRAME_H

/*
 *  MT_JoyStickFrame.h
 *
 *  Main classes for a the standalone joystick control app.
 *
 *  Can also be called by other apps to display is a child of another frame.
 *  Use HaveMenus = false in constructor for this mode.  You'l need to supply
 *  clock ticks too by calling DoTimedEvents() externally.
 *
 *  Created by Daniel Swain on 12/2/08.
 *
 */

/* Standard WX include block */
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
/* End standard WX include block */

#include <wx/menu.h>

#include "MT/MT_GUI/base/MT_AppBase.h" /* for MT_FrameWithInit */
#include "MT/MT_GUI/support/wxSupport.h"
#include "MT/MT_GUI/support/wxXMLSupport.h"

#include "MT/MT_Robot/robot/GamePadController.h"
#include "MT/MT_Robot/robot/AllRobotContainer.h"

#include "MT_RobotConnectDialog.h"
#include "MT_RobotCommandDialog.h"

#define MT_USE_XML

const bool MT_JSF_AS_CHILD = true;
const bool MT_JSF_STANDALONE = false;

const unsigned int MT_JOYSTICK_REFRESH_PERIOD = 25;

class MT_JoyStickFrameTimer; 

class MT_JoyStickFrame : public MT_FrameWithInit
{

private:

    wxStaticText* XYPosText;
    wxStaticText* WZPosText;
    wxStaticText* ButtonText;

    wxChoice* XYRobotChoice;
    wxChoice* WZRobotChoice;

    wxTextCtrl* MaxSpeedCtrl;
    wxTextCtrl* MaxTurningRateCtrl;

    wxArrayString XYChoices;
    wxArrayString WZChoices;

    wxButton* ButtonToggleJoyStick;

    float MaxTurningRate;
    float MaxSpeed;

    MT_AllRobotContainer* TheRobots;
    MT_GamePadController myGamePadController;
    bool GamePadValid;
    bool DoEvents;
    bool DoControl;
    bool IsChild;

    void AttachRobotsToGamePad();
    void UpdateRobotChoices(bool FlaggedChanges = false);

    void OnMaxTurningRateChange(wxCommandEvent& event);
    void OnMaxSpeedChange(wxCommandEvent& event);

    MT_XMLFile m_RobotConfigXML;
    bool m_bOwnRobots;

    MT_JoyStickFrameTimer* m_pTimer;

    void registerDialogForXML(wxWindow* dialog);
    void onChildMove(wxCommandEvent& event);

protected:

public:

    /*        MT_JoyStickFrame(MT_AllRobotContainer* inRobots,
              float maxspeed,
              float maxturningrate,
              wxFrame* parent, 
              wxWindowID id = wxID_ANY,
              const wxPoint& pos = wxDefaultPosition, 
              bool HaveMenus = true); */
    MT_JoyStickFrame(wxFrame* parent,
                     MT_AllRobotContainer* inRobots = NULL,
                     bool asChild = MT_JSF_AS_CHILD,
                     wxWindowID id = wxID_ANY);

    ~MT_JoyStickFrame();

    void OnWindowClosed(wxCloseEvent& event);

    void SetXYText(const wxString& xylabel);
    void SetWZText(const wxString& wzlabel);
    void SetButtonText(const wxString& buttonlabel);

    void OnMenuFileExit(wxCommandEvent& event);
    void OnMenuHelpAbout(wxCommandEvent& event);
    void OnMenuRobotConnections(wxCommandEvent& event);
    void OnMenuRobotCommand(wxCommandEvent& event);

    void OnButtonToggleJoyStick(wxCommandEvent& event);

    void OnXYChoice(wxCommandEvent& event);
    void OnWZChoice(wxCommandEvent& event);

    void CheckMaxSpeed(wxCommandEvent& event);
    void CheckMaxTurningRate(wxCommandEvent& event);

    void DoTimedEvents();
    void DoQuit();

    void EnableEvents();

	void onMaxSpeedCtrlTabKey(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()

        };

#endif // MT_JOYSTICKFRAME_H
