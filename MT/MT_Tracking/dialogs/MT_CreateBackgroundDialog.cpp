/*
 *  MT_CreateBackgroundDialog.cpp
 *
 *  Created by Daniel Swain on 11/4/09.
 *
 */

#include "MT/MT_Tracking/dialogs/MT_CreateBackgroundDialog.h"

#include "MT/MT_GUI/support/wxSupport.h"
#include "MT/MT_Core/support/mathsupport.h"
#include "MT/MT_Core/gl/glSupport.h"

enum
{
    ID_NUM_FRAMES = wxID_HIGHEST+1,
    ID_PATH,
    ID_INPAINT_CHECK,
    ID_INPAINT_RADIUS,
    ID_CANVAS
};

static bool g_bInpaint = false;
static wxRect g_CurrentRect;

MT_bgcanvas::MT_bgcanvas(wxWindow* parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size)
  : MT_GLCanvasBase(parent, id, pos, size)
{
}

void MT_bgcanvas::doGLDrawing()
{
    MT_GLCanvasBase::doGLDrawing();
    if(g_bInpaint)
    {
        MT_DrawRectangle(g_CurrentRect.x, g_CurrentRect.y, g_CurrentRect.width, g_CurrentRect.height);
    }
}

bool MT_bgcanvas::doMouseCallback(wxMouseEvent& event, double viewport_x, double viewport_y)
{
    if(!g_bInpaint)
    {
        return MT_GLCanvasBase::doMouseCallback(event, viewport_x, viewport_y);
    }

    static long firstx = 0;
    static long firsty = 0;
    
    /*long x = event.GetX();
      long y = GetSize().GetHeight() - event.GetY(); */
  
    if(event.LeftDown())
    {
        firstx = viewport_x;
        firsty = viewport_y;
    }

    if(event.LeftIsDown())
    {
        g_CurrentRect = wxRect(MT_MIN(viewport_x,firstx),MT_MIN(viewport_y,firsty), fabs(firstx-viewport_x), fabs(firsty-viewport_y));
    }
  
    Refresh(false);

    return MT_GLCanvasBase::doMouseCallback(event, viewport_x, viewport_y);

}

MT_CreateBackgroundDialog::MT_CreateBackgroundDialog(IplImage* sample_frame,
                                                     const wxString& directory,
                                                     wxWindow* parent, 
                                                     wxWindowID id)
  : wxDialog(parent, id, wxT("Create Background Frame"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  
    /* add 250 to the width make sure the UI elements
     * still make it onto the screen (have space for height, but not using it) */
    const int ui_width = 250, ui_height = 0;
    wxSize csize = MT_FitSizeToScreen(
            wxSize(sample_frame->width + ui_width, sample_frame->height + ui_height));
    /* now subtract them back off to get the canvas size */
    csize.DecBy(ui_width, ui_height);
  
    g_CurrentRect = wxRect(0, 0, sample_frame->width, sample_frame->height);
  
    m_pCanvas = new MT_bgcanvas(this, ID_CANVAS, wxDefaultPosition, csize);
    m_pCanvas->setImage(sample_frame);
    m_pCanvas->setViewport(MT_Rectangle(0, sample_frame->width, 0, sample_frame->height));
    m_pCanvas->Show();
  
    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->Add(m_pCanvas);
  
    wxBoxSizer* vbox1 = new wxBoxSizer(wxVERTICAL);
  
    vbox1->Add(new wxStaticText(this, wxID_ANY, wxT("Number of Frames to Average")), 0, wxALL | wxALIGN_LEFT, 10);
  
    wxString value_string = wxT("100");
    m_pNumFramesCtrl = new wxTextCtrl(this, ID_NUM_FRAMES, value_string, wxDefaultPosition, wxSize(200,20), wxTE_RIGHT);
    Connect(ID_NUM_FRAMES, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MT_CreateBackgroundDialog::OnNumToAvgChange));
    vbox1->Add(m_pNumFramesCtrl, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
  
    vbox1->Add(new wxStaticText(this, wxID_ANY, wxT("Output Filename")), 0, wxALL | wxALIGN_LEFT, 10);
    m_pPathCtrl = new wxTextCtrl(this, wxID_ANY, directory + wxT("/background.bmp"), wxDefaultPosition, wxSize(200,20));
    vbox1->Add(m_pPathCtrl, 0, wxLEFT | wxRIGHT , 10);
    vbox1->Add(new wxButton(this, ID_PATH, wxT("Change...")), 0, wxRIGHT | wxALIGN_RIGHT, 20);
    Connect(ID_PATH, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MT_CreateBackgroundDialog::OnPathButtonClicked));
             
    m_pDoInpaintCheck = new wxCheckBox(this, ID_INPAINT_CHECK, wxT("Inpaint Highlights"));
    vbox1->Add(m_pDoInpaintCheck, 0, wxALL, 10);
    Connect(ID_INPAINT_CHECK, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MT_CreateBackgroundDialog::OnInpaintClicked));
  
    m_pRadiusLabel = new wxStaticText(this, wxID_ANY, wxT("Inpaint Radius"));
    vbox1->Add(m_pRadiusLabel, 0, wxALL | wxALIGN_LEFT, 10);
    m_pRadiusCtrl = new wxTextCtrl(this, ID_INPAINT_RADIUS, wxT("4"), wxDefaultPosition, wxSize(200,20), wxTE_RIGHT);
    Connect(ID_INPAINT_RADIUS, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MT_CreateBackgroundDialog::OnRadiusChange));
    vbox1->Add(m_pRadiusCtrl, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    m_pDrawHint = new wxStaticText(this, wxID_ANY, wxT("Draw rectangle to set inpaint ROI."));
    vbox1->Add(m_pDrawHint, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    m_pRadiusCtrl->Disable();
    m_pRadiusLabel->Disable();
    m_pDrawHint->Disable();
    m_pRadiusCtrl->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    //hbox->Add(vbox1);
  
    //wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    //vbox->Add(hbox, 0, wxALL, 10);

    hbox->Add(vbox1);
    vbox1->AddStretchSpacer();
    vbox1->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_BOTTOM | wxALIGN_RIGHT, 10);
  
    SetSizerAndFit(hbox);
  
}

void MT_CreateBackgroundDialog::OnNumToAvgChange(wxCommandEvent& event)
{
    if(!(m_pNumFramesCtrl->IsModified()))
    {
        return;
    }
  
    MT_ValidateTextCtrlInteger(m_pNumFramesCtrl);
}

void MT_CreateBackgroundDialog::OnRadiusChange(wxCommandEvent& event)
{
    if(!(m_pRadiusCtrl->IsModified()))
    {
        return;
    }
  
    MT_ValidateTextCtrlInteger(m_pRadiusCtrl);
}

void MT_CreateBackgroundDialog::OnPathButtonClicked(wxCommandEvent& event)
{
  
    wxString path = m_pPathCtrl->GetValue();
    wxString result;
    wxString result_dir;
    int dlg_result = MT_SaveFileDialog(this, path, wxT("Save Background As..."), MT_FILTER_IMAGE_FILES, &result, &result_dir);
  
    if(dlg_result == wxID_OK)
    {
        m_pPathCtrl->SetValue(result);
    }
  
}

void MT_CreateBackgroundDialog::OnInpaintClicked(wxCommandEvent& event)
{
    if(m_pDoInpaintCheck->GetValue())
    {
        m_pRadiusCtrl->Enable();
        m_pRadiusLabel->Enable();
        m_pRadiusCtrl->SetForegroundColour(*wxBLACK);
        m_pDrawHint->Enable();
        g_bInpaint = true;
    }
    else
    {
        m_pRadiusLabel->Disable();
        m_pRadiusCtrl->Disable();
        m_pRadiusCtrl->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        m_pDrawHint->Disable();
        g_bInpaint = false;
    }
}

void MT_CreateBackgroundDialog::GetInfo(long* num_to_avg, 
                                        wxString* path, 
                                        bool* inpaint, 
                                        unsigned int* inpaint_radius, 
                                        wxRect* inpaint_roi)
{
    *path = m_pPathCtrl->GetValue();
    double v = MT_ClampTextCtrlFloat(m_pNumFramesCtrl, 0, MT_max_uint);
    *num_to_avg = (long) v;
    *inpaint = m_pDoInpaintCheck->GetValue();
    v = MT_ClampTextCtrlFloat(m_pRadiusCtrl, 0, MT_max_uint);
    *inpaint_radius = (unsigned int) v;
    *inpaint_roi = g_CurrentRect;
}
