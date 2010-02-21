#ifndef MT_ROBOTCOMMANDDIALOG_H
#define MT_ROBOTCOMMANDDIALOG_H
/*
 *  MT_RobotCommandDialog.h
 *
 *  Created by Daniel Swain on 7/7/09.
 *
 *  Simple dialog to allow the user to send a text command to any
 *  (or any subset) of the currently connected robots.
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

class MT_RobotCommandDialog : public wxDialog
{
protected:
    // wxTextCtrl for the command to send
    wxTextCtrl* m_pCommandTextCtrl;
    
    // wxCheckBoxes to say whether we want to connect
    wxCheckBox* m_pRobotConnectCheckBox[MT_MAX_NROBOTS];
    
    // wxStaticTexts for the robot names
    wxStaticText* m_pRobotNamesText[MT_MAX_NROBOTS];
    
    // we need to point to this so that we can change it and have the effects last
    MT_AllRobotContainer* m_pRobots;
    
    static bool m_bShown;
    
public: 
    MT_RobotCommandDialog(MT_AllRobotContainer* inRobots,
                       wxWindow* parent, 
                       const wxPoint& pos = wxDefaultPosition, 
                       const wxSize& size = wxSize(500,200));
    ~MT_RobotCommandDialog();
    
    void OnSendButtonClicked(wxCommandEvent& event);
    void OnDoneButtonClicked(wxCommandEvent& event);
    
    bool Show(bool show = true);
    
    DECLARE_EVENT_TABLE()
    
        };

#endif // MT_ROBOTCONNECTDIALOG_H
