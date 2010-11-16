#ifndef MT_ROBOTCONNECTDIALOG_H
#define MT_ROBOTCONNECTDIALOG_H

/*
 *  MT_RobotConnectDialog.h
 *
 *  Simple dialog to allow the user to connect/disconnect from
 *  the robots.  Also allows the user to change the port names
 *  for the robots.
 *
 *  Created by Daniel Swain on 12/4/08.
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

#include "MT/MT_Robot/robot/AllRobotContainer.h"

class MT_JoyStickFrame;
class MT_RobotFrameBase;

class MT_RobotConnectDialog : public wxDialog
{
protected:
    MT_JoyStickFrame* m_pParentJSFrame;
    MT_RobotFrameBase* m_pParentRobotFrame;
    
    // wxTextCtrls for the robot ports
    wxTextCtrl* RobotPortCtrl[7];
    
    // wxCheckBoxes to say whether we want to connect
    wxCheckBox* RobotConnectChk[7];
    
    // wxStaticTexts for the robot statuses
    wxStaticText* RobotStatus[7];
    
    // wxStaticText to let us know what's going on
    wxStaticText* ConnectStatus;
    
    // we need to point to this so that we can change it and have the effects last
    MT_AllRobotContainer* TheRobots;

    MT_RobotBase* NewRobotFunction(const char* config, const char* name);    
    
    static bool m_bShown;

    
public: 
    MT_RobotConnectDialog(MT_AllRobotContainer* inRobots,
                          MT_JoyStickFrame* js_parent,
                          MT_RobotFrameBase* rf_parent = NULL,
                          const wxPoint& pos = wxDefaultPosition, 
                          const wxSize& size = wxSize(500,275));
    ~MT_RobotConnectDialog();
    
    void OnConnectButtonClicked(wxCommandEvent& event);
    void OnOKButtonClicked(wxCommandEvent& event);
    
    bool Show(bool show = true);
    
    DECLARE_EVENT_TABLE()
    
        };


#endif // MT_ROBOTCONNECTDIALOG_H
