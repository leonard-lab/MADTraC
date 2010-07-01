/*
 *  MT_NumericInputDialog.cpp
 *
 *  Created by Daniel Swain on 11/3/09.
 *
 */

#include "MT_NumericInputDialog.h"

#include <wx/defs.h>  /* for wxID_HIGHEST */

#include "MT/MT_GUI/support/wxSupport.h" /* for validation and clamping */
#include "MT/MT_Core/support/mathsupport.h"  /* for MT_max_double, _min_double */

MT_NumericInputDialog::MT_NumericInputDialog(wxFrame* parent, 
                                             wxWindowID id, 
                                             const wxString& title, 
                                             const wxString& message,
                                             double value,
                                             double min_val = MT_min_double,
                                             double max_val = MT_max_double,
                                             bool isinteger)
 :wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
    m_bIsInteger = isinteger;
    m_dValue = value;
    m_dMinValue = min_val;
    m_dMaxValue = max_val;
  
    wxString value_string;
    if(isinteger)
    {
        value_string.Printf(wxT("%d"), (int) m_dValue);
    }
    else
    {
        value_string.Printf(wxT("%lf"), m_dValue);
    }
  
    wxBoxSizer* vbox0 = new wxBoxSizer(wxVERTICAL);
    
    vbox0->Add(new wxStaticText(this, wxID_ANY, message), 0, wxALL | wxALIGN_LEFT, 10);
  
    m_pInputTextCtrl = new wxTextCtrl(this, wxID_HIGHEST, value_string, wxDefaultPosition, wxSize(300, 20), wxTE_RIGHT);
    Connect(wxID_HIGHEST, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MT_NumericInputDialog::OnTextChange));
    vbox0->Add(m_pInputTextCtrl, 0, wxALL, 10);
  
    vbox0->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_RIGHT, 10);

    SetSizerAndFit(vbox0);
  
    m_pInputTextCtrl->SetFocus();
    m_pInputTextCtrl->SetSelection(-1,-1);
  
}

void MT_NumericInputDialog::OnTextChange(wxCommandEvent& event)
{
  
    if(!(m_pInputTextCtrl->IsModified()))
    {
        return;
    }
  
    if(m_bIsInteger)
    {
        MT_ValidateTextCtrlInteger(m_pInputTextCtrl);
    }
    else
    {
        MT_ValidateTextCtrlNumeric(m_pInputTextCtrl);
    }
}

double MT_NumericInputDialog::GetValue()
{
    if(m_bIsInteger)
    {
        m_dValue = MT_ClampTextCtrlFloat(m_pInputTextCtrl, m_dMinValue, m_dMaxValue);
    }
    return m_dValue;
}
