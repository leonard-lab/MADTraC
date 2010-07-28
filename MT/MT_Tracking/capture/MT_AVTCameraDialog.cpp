/*
 *  MT_AVTCameraDialog.cpp
 *
 *  Created by Valeri Karpov on 7/19/10.
 *
 */

#include "MT_AVTCameraDialog.h"

BEGIN_EVENT_TABLE(MT_AVTCameraDialog, wxDialog)
EVT_BUTTON(MT_CAMERA_DIALOG_OK_BTN, MT_AVTCameraDialog::OnOkButtonPressed)
END_EVENT_TABLE()

MT_AVTCameraDialog::MT_AVTCameraDialog(wxWindow* parent, 
                                       const wxPoint& pos,
                                       wxString* cameras,
                                       int camerasLength)
: wxDialog(parent, wxID_ANY, wxT("Choose AVT Camera"), pos, wxSize(250, 100), wxDEFAULT_DIALOG_STYLE)
{
    // Vetical Box to hold all of the components of the layout
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    m_pCameraSelectBox = new wxComboBox(this, wxID_ANY, cameras[0], wxDefaultPosition, wxDefaultSize, camerasLength, cameras);
    vbox->Add(m_pCameraSelectBox, 0, wxCENTER, 25);
    m_pOkButton = new wxButton(this, MT_CAMERA_DIALOG_OK_BTN, wxT("Connect"));
    vbox->Add(m_pOkButton, 0, wxCENTER, 25);
    SetSizer(vbox);
}

MT_AVTCameraDialog::~MT_AVTCameraDialog()
{ }

void MT_AVTCameraDialog::OnOkButtonPressed(wxCommandEvent& WXUNUSED(event))
{
    wxDialog::EndModal(wxID_OK);
}

int MT_AVTCameraDialog::GetCameraSelection()
{
    int i = m_pCameraSelectBox->GetCurrentSelection();
    if (i == -1)
    {
        i = 0;
    }
    return i;
}

bool MT_AVTCameraDialog::Show()
{
    if (wxDialog::ShowModal() == wxID_OK)
    {
        return true;
    }
    return false;
}