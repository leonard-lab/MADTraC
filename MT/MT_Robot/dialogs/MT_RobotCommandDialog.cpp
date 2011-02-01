/*
 *  MT_RobotCommandDialog.cpp
 *
 *  Created by Daniel Swain on 7/7/09.
 *
 */

#include "MT_RobotCommandDialog.h"
#include "MT/MT_GUI/support/wxSupport.h"

enum
{
    ID_COMMAND_DLG_SENDBTN = wxID_HIGHEST + 1,
    ID_COMMAND_DLG_DONEBTN
};

bool MT_RobotCommandDialog::m_bShown = false;

BEGIN_EVENT_TABLE(MT_RobotCommandDialog, wxDialog)
EVT_BUTTON(ID_COMMAND_DLG_SENDBTN, MT_RobotCommandDialog::OnSendButtonClicked)
EVT_BUTTON(ID_COMMAND_DLG_DONEBTN, MT_RobotCommandDialog::OnDoneButtonClicked)
END_EVENT_TABLE()

MT_RobotCommandDialog::MT_RobotCommandDialog(MT_AllRobotContainer* inRobots,
                                       wxWindow* parent, 
                                       const wxPoint& pos, 
                                       const wxSize& size)
: wxDialog(parent, wxID_ANY, wxT("Robot Command Sender"), pos, size, wxDEFAULT_DIALOG_STYLE)
{
  
    m_pRobots = inRobots;
  
    wxPanel* panel = new wxPanel(this, -1);
    wxBoxSizer* vbox0 = new wxBoxSizer(wxVERTICAL);
  
    wxBoxSizer* vbox1 = new wxBoxSizer(wxVERTICAL);
    vbox1->Add(new wxStaticText(panel, -1, wxT("Command")));
    m_pCommandTextCtrl = new wxTextCtrl(panel, -1, MT_StringToWxString(m_pRobots->LastCommand), wxDefaultPosition, wxSize(450,20));
    vbox1->Add(m_pCommandTextCtrl,0,wxTOP,2);  
    vbox0->Add(vbox1, 0, wxALL, 10);
  
    wxGridSizer* grid0 = new wxGridSizer(2, MT_MAX_NROBOTS, 5, 15);
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
    
        m_pRobotNamesText[i] = new wxStaticText(panel, -1, MT_StringToWxString(m_pRobots->RobotName[i]));
        if( !(m_pRobots->IsPhysical(i)) || !(m_pRobots->IsConnected(i)))
        {
            m_pRobotNamesText[i]->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        }
        grid0->Add(m_pRobotNamesText[i],0,wxALIGN_CENTER);
    
    }
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
    
        m_pRobotConnectCheckBox[i] = new wxCheckBox(panel, -1, wxT(""));
        if( !(m_pRobots->IsPhysical(i)) || !(m_pRobots->IsConnected(i)) )
        {
            m_pRobotConnectCheckBox[i]->Disable();
        }
        grid0->Add(m_pRobotConnectCheckBox[i], 0, wxALIGN_CENTER);
    
    }
    vbox0->Add(grid0, 0, wxALL, 10);
   
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    hbox1->Add(new wxButton(panel, ID_COMMAND_DLG_SENDBTN, wxT("Send")), 0, wxALIGN_RIGHT | wxRIGHT, 10);
    hbox1->Add(new wxButton(panel, ID_COMMAND_DLG_DONEBTN, wxT("Done")), 0, wxALIGN_RIGHT | wxRIGHT, 10);
    vbox0->Add(hbox1, 0, wxALIGN_RIGHT | wxALL, 10);
  
    panel->SetSizerAndFit(vbox0);
  
}

MT_RobotCommandDialog::~MT_RobotCommandDialog()
{
  
    m_pCommandTextCtrl->Destroy();
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        m_pRobotConnectCheckBox[i]->Destroy();
        m_pRobotNamesText[i]->Destroy();
    }
  
}

void MT_RobotCommandDialog::OnSendButtonClicked(wxCommandEvent& WXUNUSED(event))
{
    wxString TheCommand = m_pCommandTextCtrl->GetValue();
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        if(m_pRobotConnectCheckBox[i]->GetValue() && m_pRobots->IsPhysical(i) && m_pRobots->IsConnected(i))
        {
            m_pRobots->GetRobot(i)->SendCommand((const char*) TheCommand.mb_str());
        }
    }
    m_pRobots->LastCommand = (const char*) TheCommand.mb_str();
}

void MT_RobotCommandDialog::OnDoneButtonClicked(wxCommandEvent& WXUNUSED(event))
{
  
    m_bShown = false;
    
    /* handle destruction properly */
    if(IsModal())
    {
        // this tells the caller that we exited "OK"
        EndModal(wxID_OK);
    }
    else
    {
        Destroy();
    }
  
}

bool MT_RobotCommandDialog::Show(bool show)
{
    if(show)
    {
        if(!m_bShown)
        {
            m_bShown = true;
            wxDialog::Show(true);
        }
        /* if m_bShown, do nothing */
    }
    else
    {
        wxDialog::Show(false);
        m_bShown = false;
    }

    return true;
  
}
