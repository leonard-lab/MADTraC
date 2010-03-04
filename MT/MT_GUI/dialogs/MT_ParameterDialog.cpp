/*
 *  MT_ParameterDialog.cpp
 *
 *  Created by Daniel Swain on 8/3/09.
 *
 */

#include <wx/colordlg.h>
#include <wx/cmndata.h>

#include "MT/MT_GUI/dialogs/MT_ParameterDialog.h"
#include "MT/MT_GUI/support/wxSupport.h"

#include <vector>
using namespace std;

MT_DataGroupDialog::MT_DataGroupDialog(MT_DataGroup* datagroup, 
                                       wxFrame* parent, 
                                       bool DestroyGroupOnClose,
                                       void (*pcallbackfunction)(void *ParentObject), 
                                       const wxPoint& pos, 
                                       const wxSize& size)
  : MT_DialogWithUpdate(parent, wxID_ANY, datagroup->GetGroupName(), pos, size, wxDEFAULT_DIALOG_STYLE)
{
    m_pCallBackFunction = pcallbackfunction;
    m_pDataGroup = datagroup;
  
    m_bDestroyGroupOnClose = DestroyGroupOnClose;
  
    m_iNParams = 0;
    for(unsigned int i = 0; i < m_pDataGroup->GetGroupSize(); i++)
    {
        if(!m_pDataGroup->GetDialogIgnore(i))
        {
            m_iNParams++;
        }
    }
    m_viIndexMap.resize(m_pDataGroup->GetGroupSize());

    /* Set up the GUI */
//  MT_Panel* panel = new MT_Panel(this, -1);
    wxBoxSizer* vbox0 = new wxBoxSizer(wxVERTICAL);
    wxGridSizer* grid0 = new wxGridSizer(m_iNParams, 2, 5, 15);
  
    unsigned int num_checkboxes = 0;
    unsigned int num_textboxes = 0;
    unsigned int num_choices = 0;
    unsigned int num_colors = 0;
    for(unsigned int i = 0; i < m_pDataGroup->GetGroupSize(); i++)
    {

        if(m_pDataGroup->GetDialogIgnore(i))
        {
            m_viIndexMap[i] = 0;
            continue;
        }

        grid0->Add(new wxStaticText(this, -1, m_pDataGroup->GetNameString(i)));
        switch(m_pDataGroup->GetDataType(i))
        {
            /* for bools, make a check box */
        case MT_TYPE_BOOL:
        {
            wxCheckBox* cb = new wxCheckBox(this, wxID_HIGHEST+i, wxT(""));
            m_pCheckBoxCtrls.push_back(cb);
            if(m_pDataGroup->GetReadOnly(i))
            {
                cb->Disable();
            }
            m_viIndexMap[i] = num_checkboxes++;
            grid0->Add(cb, 0, wxALIGN_CENTER);
            break;
        }
        case MT_TYPE_CHOICE:
        {
            wxArrayString choices;
            MT_Choice* c = (MT_Choice *)(m_pDataGroup->GetPointer(i));
            for(unsigned int j = 0; j < c->GetMaxValue()+1; j++)
            {
                choices.Add(c->GetName(j));
            }
            wxChoice* ch = new wxChoice(this, wxID_HIGHEST+i, wxDefaultPosition, wxDefaultSize, choices);
            m_pChoiceCtrls.push_back(ch);
            ch->SetSelection(c->GetIntValue());
            if(m_pDataGroup->GetReadOnly(i))
            {
                ch->Disable();
            }
            m_viIndexMap[i] = num_choices++;
            grid0->Add(ch, 0, wxALIGN_CENTER);
            break;
        }
        case MT_TYPE_COLOR:
        {
            wxButton* b = new wxButton(this, wxID_HIGHEST+i, wxT("Select..."));
            m_pColorButtons.push_back(b);
            if(m_pDataGroup->GetReadOnly(i))
            {
                b->Disable();
            }
            m_viIndexMap[i] = num_colors++;
            grid0->Add(b);
            Connect(wxID_HIGHEST+i, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MT_DataGroupDialog::OnColorButtonClicked));
            break;
        }
        default:
        {
            wxTextCtrl* tc = new wxTextCtrl(this, wxID_HIGHEST+i, wxT(""));
            m_pParamTextCtrls.push_back(tc);
            if(m_pDataGroup->GetReadOnly(i))
            {
                tc->Disable();
            }
            m_viIndexMap[i] = num_textboxes++;
            grid0->Add(tc);
            Connect(wxID_HIGHEST+i, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MT_DataGroupDialog::OnTextCheck));
            break;
        }
        }
    }
    
    vbox0->Add(grid0, 0, wxALL, 10);
  
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    hbox1->Add(new wxButton(this, wxID_APPLY, wxT("Apply")), 0, wxALIGN_LEFT | wxRIGHT, 50);
    Connect(wxID_APPLY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MT_DataGroupDialog::OnApplyButtonClicked));
    hbox1->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxALIGN_RIGHT | wxRIGHT, 10);
    Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MT_DataGroupDialog::OnOKButtonClicked));
    hbox1->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxALIGN_RIGHT | wxRIGHT, 10);
    Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MT_DataGroupDialog::OnCancelButtonClicked));
    vbox0->Add(hbox1, 0, wxALIGN_LEFT | wxALL, 10);
  
    //panel->SetSizerAndFit(vbox0);
    SetSizerAndFit(vbox0);
  
    UpdateValues();
  
}

void MT_DataGroupDialog::UpdateValues()
{
 
    if(!m_bAcceptUpdate)
    {
        return;
    }
    
    for(unsigned int i = 0; i < m_pDataGroup->GetGroupSize(); i++)
    {
        if(m_pDataGroup->GetDialogIgnore(i))
        {
            continue;
        }

        switch(m_pDataGroup->GetDataType(i))
        {
        case MT_TYPE_BOOL:
            m_pCheckBoxCtrls[m_viIndexMap[i]]->SetValue(m_pDataGroup->GetStringValue(i) == "true");
            break;
        case MT_TYPE_CHOICE:
        {
            MT_Choice* c = (MT_Choice *)(m_pDataGroup->GetPointer(i));
            m_pChoiceCtrls[m_viIndexMap[i]]->SetSelection(c->GetIntValue());
            break;
        }
        case MT_TYPE_COLOR:
            break;
        default:
            m_pParamTextCtrls[m_viIndexMap[i]]->SetValue(m_pDataGroup->GetStringValue(i));
            break;
        }
    }
  
}

void MT_DataGroupDialog::OnApplyButtonClicked(wxCommandEvent& event)
{
    WriteValues();
}

void MT_DataGroupDialog::OnOKButtonClicked(wxCommandEvent& event)
{
    WriteValues();

    if(m_bDestroyGroupOnClose)
    {
        delete m_pDataGroup;
        m_pDataGroup = NULL;
    }
    Destroy();
}

void MT_DataGroupDialog::OnCancelButtonClicked(wxCommandEvent& event)
{
    if(m_bDestroyGroupOnClose)
    {
        delete m_pDataGroup;
        m_pDataGroup = NULL;
    }
    Destroy();
}

void MT_DataGroupDialog::WriteValues()
{
    string val;
    for(unsigned int i = 0; i < m_pDataGroup->GetGroupSize(); i++)
    {
        if(m_pDataGroup->GetDialogIgnore(i))
        {
            continue;
        }
        switch(m_pDataGroup->GetDataType(i))
        {
        case MT_TYPE_BOOL:
        {
            if(m_pCheckBoxCtrls[m_viIndexMap[i]]->IsChecked())
            {
                val = "true";
            }
            else
            {
                val = "false";
            }
            m_pDataGroup->SetStringValue(i, val);
            break;
        }
        case MT_TYPE_CHOICE:
        {
            MT_Choice* c = (MT_Choice *)(m_pDataGroup->GetPointer(i));
            unsigned int j = m_pChoiceCtrls[m_viIndexMap[i]]->GetSelection();
            c->SetIntValue(j);
            break;
        }
        case MT_TYPE_COLOR:
        {
            MT_Color* c = (MT_Color *)(m_pDataGroup->GetPointer(i));
            m_pDataGroup->SetStringValue(i, c->GetHexString());
            break;
        }
        default:
        {
            val = (string) (m_pParamTextCtrls[m_viIndexMap[i]]->GetValue());
            m_pDataGroup->SetStringValue(i, val);
            break;
        }
        }
    }
  
    UpdateValues();
  
}

void MT_DataGroupDialog::OnTextCheck(wxCommandEvent& event)
{
    int id = event.GetId() - wxID_HIGHEST;
    wxTextCtrl* textbox = m_pParamTextCtrls[m_viIndexMap[id]];
    if(!(textbox->IsModified()))
    {  /* change was programmatic - does not call for validation */
        return;
    }
  
    if(m_pDataGroup->GetDataType(id) != MT_TYPE_STRING)
    {
        MT_ValidateTextCtrlNumeric(textbox);
    }
  
}

void MT_DataGroupDialog::OnColorButtonClicked(wxCommandEvent& event)
{
    int id = event.GetId() - wxID_HIGHEST;
  
    if(m_pDataGroup->GetDataType(id) != MT_TYPE_COLOR)
    {
        return;
    }
  
    MT_Color* c = ((MT_Color *)m_pDataGroup->GetPointer(id));
  
    wxColourData data;
    wxColour current;
    wxString ch = wxT("#") + wxString(c->GetHexString());
    current.Set(ch);
    data.SetColour(current);
    wxColourDialog dlg(this, &data);
  
    dlg.ShowModal();
  
    data = dlg.GetColourData();
  
    current = data.GetColour();
    ch = current.GetAsString(wxC2S_HTML_SYNTAX);
    ch = ch.AfterFirst('#');
    
    c->SetHexValue(ch.c_str());
  
}

BEGIN_EVENT_TABLE(MT_ParameterDialog, wxDialog)
EVT_BUTTON(wxID_APPLY, MT_ParameterDialog::OnApplyButtonClicked)
EVT_BUTTON(wxID_OK, MT_ParameterDialog::OnOKButtonClicked)
EVT_BUTTON(wxID_CANCEL, MT_ParameterDialog::OnCancelButtonClicked)
END_EVENT_TABLE()

MT_ParameterDialog::MT_ParameterDialog(std::vector<double>&  ParamVector,
                                       const std::vector<string>& ParamNames,
                                       void (*inpCallBackFunction)(void* ParentObject),
                                       const wxString& FrameCaption,
                                       wxFrame* parent,
                                       const wxPoint& pos,
                                       const wxSize& size)
: MT_DialogWithUpdate(parent, wxID_ANY, FrameCaption, pos, size, wxDEFAULT_DIALOG_STYLE)
{
    pCallBackFunction = inpCallBackFunction;
  
    // get number of supplied parameters
    nparams = ParamVector.size();
  
    m_pParamNamesText.resize(nparams);
    m_pParamTextCtrl.resize(nparams);
  
    m_pParams = &ParamVector;
  
    // resize accordingly
    wxSize newsize = wxSize(340, nparams*30 + 60);
    SetSizeHints(newsize);
    SetSize(newsize);
  
    // set up the GUI
    wxPanel* panel = new wxPanel(this, -1);
    wxBoxSizer* vbox0 = new wxBoxSizer(wxVERTICAL);
  
    wxGridSizer* grid0 = new wxGridSizer(nparams, 2, 5, 15);
    wxString ParamString;
  
    for(int i = 0; i < nparams; i++)
    {
    
        ParamString.Printf("%5.3f", ParamVector[i]);
        m_pParamNamesText[i] = new wxStaticText(panel, -1, ParamNames[i]);
        m_pParamTextCtrl[i] = new wxTextCtrl(panel, -1, ParamString, wxDefaultPosition, wxSize(100,20));
        grid0->Add(m_pParamNamesText[i]);
        grid0->Add(m_pParamTextCtrl[i]);
    
    }
    vbox0->Add(grid0, 0, wxALL, 10);
  
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    hbox1->Add(new wxButton(panel, wxID_APPLY, wxT("Apply")), 0, wxALIGN_LEFT | wxRIGHT, 50);
    hbox1->Add(new wxButton(panel, wxID_OK, wxT("OK")), 0, wxALIGN_RIGHT | wxRIGHT, 10);
    hbox1->Add(new wxButton(panel, wxID_CANCEL, wxT("Cancel")), 0, wxALIGN_RIGHT | wxRIGHT, 10);
    vbox0->Add(hbox1, 0, wxALIGN_LEFT | wxALL, 10);
  
    panel->SetSizerAndFit(vbox0);
  
}

MT_ParameterDialog::MT_ParameterDialog(std::vector<double>&  ParamVector,
                                       const wxString* ParamNames,
                                       void (*inpCallBackFunction)(void* ParentObject),
                                       const wxString& FrameCaption,
                                       wxFrame* parent,
                                       const wxPoint& pos,
                                       const wxSize& size)
  : MT_DialogWithUpdate(parent, wxID_ANY, FrameCaption, pos, size, wxDEFAULT_DIALOG_STYLE)
{
    pCallBackFunction = inpCallBackFunction;
  
    // get number of supplied parameters
    nparams = ParamVector.size();

    m_pParamNamesText.resize(nparams);
    m_pParamTextCtrl.resize(nparams);
  
    m_pParams = &ParamVector;
  
    // resize accordingly
    wxSize newsize = wxSize(340, nparams*30 + 60);
    SetSizeHints(newsize);
    SetSize(newsize);
      
    // set up the GUI
    wxPanel* panel = new wxPanel(this, -1);
    wxBoxSizer* vbox0 = new wxBoxSizer(wxVERTICAL);
  
    wxGridSizer* grid0 = new wxGridSizer(nparams, 2, 5, 15);
    wxString ParamString;
  
    for(int i = 0; i < nparams; i++)
    {
    
        ParamString.Printf("%5.3f", ParamVector[i]);
        m_pParamNamesText[i] = new wxStaticText(panel, -1, ParamNames[i]);
        m_pParamTextCtrl[i] = new wxTextCtrl(panel, -1, ParamString, wxDefaultPosition, wxSize(100,20));
        grid0->Add(m_pParamNamesText[i]);
        grid0->Add(m_pParamTextCtrl[i]);
    
    }
    vbox0->Add(grid0, 0, wxALL, 10);
  
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    hbox1->Add(new wxButton(panel, wxID_APPLY, wxT("Apply")), 0, wxALIGN_LEFT | wxRIGHT, 50);
    hbox1->Add(new wxButton(panel, wxID_OK, wxT("OK")), 0, wxALIGN_RIGHT | wxRIGHT, 10);
    hbox1->Add(new wxButton(panel, wxID_CANCEL, wxT("Cancel")), 0, wxALIGN_RIGHT | wxRIGHT, 10);
    vbox0->Add(hbox1, 0, wxALIGN_LEFT | wxALL, 10);
  
    panel->SetSizerAndFit(vbox0);
  
}

MT_ParameterDialog::~MT_ParameterDialog()
{

}


void MT_ParameterDialog::OnApplyButtonClicked(wxCommandEvent& event)
{
  
    SetParams();
  
}

void MT_ParameterDialog::OnOKButtonClicked(wxCommandEvent& event)
{
  
    SetParams();
    Destroy();
  
}

void MT_ParameterDialog::OnCancelButtonClicked(wxCommandEvent& event)
{
  
    Destroy();
  
}

void MT_ParameterDialog::SetParams()
{
  
    double val;
  
    for(int i = 0; i < nparams; i++)
    {
        val = MT_ClampTextCtrlFloat(m_pParamTextCtrl[i], 0, 10000.0);
        m_pParams->at(i) = val;
    }
  
    if(pCallBackFunction)
    {
        pCallBackFunction((void *)GetParent());
    }
  
}
