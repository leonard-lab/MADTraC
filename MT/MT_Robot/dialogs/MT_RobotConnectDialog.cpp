/*
 *  MT_RobotConnectDialog.cpp
 *
 *  Created by Daniel Swain on 12/4/08.
 *
 */

#include "MT_RobotConnectDialog.h"
#include "MT/MT_GUI/support/wxSupport.h"
#include "MT/MT_Robot/dialogs/MT_JoyStickFrame.h"
#include "MT/MT_Robot/base/MT_RobotFrameBase.h"

//#include "MT/MT_Robot/robot/SteeredRobot.h"  /* the default robot */

/* sizes vary a little depending upon implementation:
 *  STATUS_WIDTH = width of the space to the left of the Connect / OK buttons,
 *                  which also displays status during connect
 */
#ifdef __APPLE__
const int STATUS_WIDTH = 315;
#else
const int STATUS_WIDTH = 285;
#endif

bool MT_RobotConnectDialog::m_bShown = false;

enum
{
    ID_CONNECT_DLG_CONNECTBTN = wxID_HIGHEST + 1,
    ID_CONNECT_DLG_OKBTN
};

BEGIN_EVENT_TABLE(MT_RobotConnectDialog, wxDialog)
EVT_BUTTON(ID_CONNECT_DLG_CONNECTBTN, MT_RobotConnectDialog::OnConnectButtonClicked)
EVT_BUTTON(ID_CONNECT_DLG_OKBTN, MT_RobotConnectDialog::OnOKButtonClicked)
END_EVENT_TABLE()

MT_RobotConnectDialog::MT_RobotConnectDialog(MT_AllRobotContainer* inRobots,
                                             MT_JoyStickFrame* js_parent,
                                             MT_RobotFrameBase* rf_parent,
                                             const wxPoint& pos, 
                                             const wxSize& size)
: wxDialog(rf_parent ? (wxFrame *) rf_parent : (wxFrame *) js_parent,
           wxID_ANY,
           wxT("Choose Robot Connections"),
           pos,
           size,
           wxDEFAULT_DIALOG_STYLE),
    m_pParentJSFrame(js_parent),
    m_pParentRobotFrame(rf_parent)
{

    TheRobots = inRobots;
  
    // Vetical Box to hold all of the components of the layout
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
  
    // FlexGrid for the gridded robot controls (of different sizes)
    //  (MT_MAX_NROBOTS robots + 1 header) and 4 columns (name, port, connect box, status)
    //     with 5px row/col spacing
    wxFlexGridSizer* Grid = new wxFlexGridSizer(MT_MAX_NROBOTS + 1,4,5,5);
  
    // Grid headers, Connect gets a little space ont he left and right
    Grid->Add(new wxStaticText(this, -1, wxT("Name")));
    Grid->Add(new wxStaticText(this, -1, wxT("Port Name")));
    Grid->Add(new wxStaticText(this, -1, wxT("Connect")), 0, wxLEFT | wxRIGHT, 5);
    Grid->Add(new wxStaticText(this, -1, wxT("Status")));
    
    // Add a row for each robot
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        
        // leave lots of space for a long port name
        RobotPortCtrl[i] = new wxTextCtrl(this, -1, MT_StringToWxString(TheRobots->PortName[i]), wxDefaultPosition, wxSize(300,20));
        // each individual check box does not get a label
        RobotConnectChk[i] = new wxCheckBox(this, -1, wxT(""));
    
        // check to see if the robot is connected
        //   NOTE it is very important that IsPhysical is checked first,
        //     since it will return false if the robot is a NULL pointer
        //     - the && operator stops immediately if the first operand
        //        is false, avoiding a bad exception i.e. (NULL)->IsConnected()
        if(TheRobots->IsPhysical(i) && TheRobots->IsConnected(i))
        {
            // connected -> set the check box and status "OK"
            RobotConnectChk[i]->SetValue(true);
            RobotStatus[i] = new wxStaticText(this, -1, wxT("OK"));  
            RobotStatus[i]->SetForegroundColour(wxT("GREEN"));
        }
        else
        {
            // otherwise leave check box off and status "N/C" for not connected
            RobotStatus[i] = new wxStaticText(this, -1, wxT("N/C"));
            RobotStatus[i]->SetForegroundColour(wxT("BLACK"));
        }
    
        // the robot names are static, no need to refer to it elsewhere
        Grid->Add(new wxStaticText(this, -1, MT_StringToWxString(TheRobots->RobotName[i])));
        Grid->Add(RobotPortCtrl[i]);
        Grid->Add(RobotConnectChk[i],0,wxALIGN_CENTER);   // we want these to be centered
        Grid->Add(RobotStatus[i],0,wxALIGN_CENTER);       // these too
    }

    // Add the grid with 10px spacing on each side
    vbox->Add(Grid,0,wxALL, 10);
  
    // horizontal sizer for the connect/ok buttons
    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
  
    // adds blank space to offset the buttons to the right side of the frame
    //   though we'll use this space later to let the user know we're trying to connect
    ConnectStatus = new wxStaticText(this, -1, wxT(""),wxDefaultPosition,wxSize(STATUS_WIDTH,20));
    hbox->Add(ConnectStatus);
  
    // add the "Connect" button, padded with 10 px to the right
    hbox->Add(new wxButton(this, ID_CONNECT_DLG_CONNECTBTN, wxT("Connect")),0,wxALIGN_RIGHT | wxRIGHT, 10);
    // add the "OK" (return) button
    hbox->Add(new wxButton(this, ID_CONNECT_DLG_OKBTN, wxT("OK")),0,wxALIGN_RIGHT);
  
    // add the buttons to the main sizer
    vbox->Add(hbox, 0, wxALL, 10);
  
    // tell the frame what sizer to use - ensures the right sizes
    SetSizerAndFit(vbox);

}

MT_RobotConnectDialog::~MT_RobotConnectDialog()
{
    m_bShown = false;
    // we should destroy these controls on exit so they don't stay on the heap
    for(unsigned int i = 0; i < 7; i++)
    {
        RobotPortCtrl[i]->Destroy();
        RobotConnectChk[i]->Destroy();
        RobotStatus[i]->Destroy();
    }
    ConnectStatus->Destroy();
  
}

void MT_RobotConnectDialog::OnOKButtonClicked(wxCommandEvent& WXUNUSED(event))
{
    // Store the port names
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
        TheRobots->PortName[i] = (const char*) RobotPortCtrl[i]->GetValue().mb_str();
    }
  
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

MT_RobotBase* MT_RobotConnectDialog::NewRobotFunction(const char* config,
                                                      const char* name)
{
    if(m_pParentRobotFrame)
    {
        return m_pParentRobotFrame->getNewRobot(config, name);
    }
    else
    {
        return m_pParentJSFrame->getNewRobot(config, name);
    }
}

void MT_RobotConnectDialog::OnConnectButtonClicked(wxCommandEvent& WXUNUSED(event))
{
  
    for(unsigned int i = 0; i < MT_MAX_NROBOTS; i++)
    {
   
        if(RobotConnectChk[i]->GetValue())
        {
            // if checked, and the robot is not already connected
            if(!(TheRobots->IsPhysical(i) && TheRobots->IsConnected(i)))
            {
                // Let the user know what's going on
                ConnectStatus->SetLabel(wxT("Attempting to connect to ") + MT_StringToWxString(TheRobots->RobotName[i]));
                // Calling Update() makes sure the message shows up
                Update();
        
                // Try to connect
                wxString Port = RobotPortCtrl[i]->GetValue();
                MT_RobotBase* newBot = NewRobotFunction((const char*) Port.mb_str(),
                                                        TheRobots->RobotName[i].c_str());

                if(!newBot)
                {
                    return;
                }
        
                // clear the status area
                ConnectStatus->SetLabel(wxT(""));
        
                // if successful
                if(newBot->IsConnected())
                {
          
                    // make sure the robot is stopped, but ready
                    newBot->SafeStop();
                    newBot->Go();
          
                    // label the status as OK
                    RobotStatus[i]->SetLabel(wxT("OK"));
                    RobotStatus[i]->SetForegroundColour(wxT("GREEN"));
          
                    // stick it in the list, do bookkeeping
                    TheRobots->SetBot(i,newBot);
          
                }
                else
                {      // not successful
                    delete newBot;
                    RobotConnectChk[i]->SetValue(false);
                    RobotStatus[i]->SetLabel(wxT("FAIL"));
                    RobotStatus[i]->SetForegroundColour(wxT("RED"));
                }
            }
        }  // checked
        else
        {   // not checked, but the robot is connected
            if(TheRobots->IsPhysical(i) && TheRobots->IsConnected(i))
            {  // disconnect by deleting the object
        
                // safely deletes and does bookkeeping
                TheRobots->ClearBot(i);
        
                // change status back to "N/C"
                RobotStatus[i]->SetLabel(wxT("N/C"));
                RobotStatus[i]->SetForegroundColour(wxT("BLACK"));
            }
            else
            {
                // make sure the status just says N/C
                RobotStatus[i]->SetLabel(wxT("N/C"));
                RobotStatus[i]->SetForegroundColour(wxT("BLACK"));
            }
        }  // not checked    
    
    }  // each potential robot

}

bool MT_RobotConnectDialog::Show(bool show)
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
