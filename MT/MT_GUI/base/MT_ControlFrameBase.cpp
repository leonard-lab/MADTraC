#include "MT_ControlFrameBase.h"
#include "MT/MT_GUI/support/wxSupport.h"

BEGIN_EVENT_TABLE(MT_ControlFrameBase, wxFrame)
END_EVENT_TABLE()

MT_ControlFrameBase::MT_ControlFrameBase(MT_FrameBase* parent, 
                                         const int Buttons,
                                         const wxPoint& pos, 
                                         const wxSize& size)
: wxFrame(parent, wxID_ANY, wxT("Controls"), pos, size, MT_FIXED_SIZE_FRAME_NO_CLOSEBOX),
    m_pButtonPlayPause(NULL),
    m_pButtonStep(NULL),
    m_bDoEvents(false),
    m_iButtons(Buttons),
    m_pParentFrame(parent)
{

}

void MT_ControlFrameBase::doMasterInitialization()
{
  
    m_sStatusText = wxT("");
  
    /* UI creation */
    wxPanel* panel = new wxPanel(this, -1);
    wxBoxSizer* vbox0 = new wxBoxSizer(wxVERTICAL);

    unsigned int nbuttons = createButtons(vbox0, panel);

    /* set size: 150 wide, MT_BUTTON_HEIGHT + 20 per button tall 
       plus 40 for the header and status bar */
    wxSize newsize = wxSize(150, nbuttons*(MT_BUTTON_HEIGHT + 20)+40);
    wxFrame::SetSizeHints(newsize);
    wxFrame::SetSize(newsize);
    
    panel->SetSizerAndFit(vbox0, panel);
  
    wxFrame::CreateStatusBar();
    
    m_bDoEvents = true;
  
}

unsigned int MT_ControlFrameBase::createButtons(wxBoxSizer* pSizer, wxPanel* pPanel)
{
  
    unsigned int nbuttons = 0;
  
    if(m_iButtons & MT_CF_PLAYPAUSE)
    {
        m_pButtonPlayPause = new wxButton(pPanel, MT_CF_ID_BUTTON_PLAYPAUSE, wxT("Play"));
        pSizer->Add(m_pButtonPlayPause, 0, wxALL | wxCENTER, 10);
        m_pButtonPlayPause->Disable();
        wxFrame::Connect(MT_CF_ID_BUTTON_PLAYPAUSE, wxEVT_COMMAND_BUTTON_CLICKED,
                         wxCommandEventHandler(MT_ControlFrameBase::onButtonPlayPausePressed));
        nbuttons++;
    }

    if(m_iButtons & MT_CF_STEP)
    {
        m_pButtonStep = new wxButton(pPanel, MT_CF_ID_BUTTON_STEP, wxT("Step"));
        pSizer->Add(m_pButtonStep, 0, wxALL | wxCENTER, 10);
        m_pButtonStep->Disable();
        wxFrame::Connect(MT_CF_ID_BUTTON_STEP, wxEVT_COMMAND_BUTTON_CLICKED,
                         wxCommandEventHandler(MT_ControlFrameBase::onButtonStepPressed));
        nbuttons++;
    }

    return nbuttons;
}

void MT_ControlFrameBase::doQuit()
{
    m_bDoEvents = false;
    wxFrame::Close(true);
}

void MT_ControlFrameBase::onButtonPlayPausePressed(wxCommandEvent& WXUNUSED(event))
{
    m_pParentFrame->togglePause();
}

void MT_ControlFrameBase::onButtonStepPressed(wxCommandEvent& WXUNUSED(event))
{
    m_pParentFrame->doStep();
}


void MT_ControlFrameBase::onPlayPause(bool paused)
{
    if(paused)
    {
        m_pButtonPlayPause->SetLabel(wxT("Play"));
        m_pButtonStep->Enable();
    }
    else
    {
        m_pButtonPlayPause->SetLabel(wxT("Pause"));
        m_pButtonStep->Disable();
    }
}

void MT_ControlFrameBase::enableButtons()
{
  
    if(m_pButtonPlayPause)
    {
        m_pButtonPlayPause->Enable();
    }
  
    if(m_pButtonStep)
    {
        m_pButtonStep->Enable();
    }
  
}

void MT_ControlFrameBase::setStatusText(const wxString& statustext)
{
    if(statustext != m_sStatusText)
    {
        wxFrame::SetStatusText(statustext);
        m_sStatusText = statustext;
    }
}
