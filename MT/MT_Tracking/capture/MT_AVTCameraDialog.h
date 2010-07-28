#ifndef MT_AVTCAMERADIALOG_H
#define MT_AVTCAMERADIALOG_H

/*
 *  MT_AVTCameraDialog.h
 *
 *  Created by Valeri Karpov on 7/19/2010.
 *
 *  Dialog to select which camera you want to use for AVT
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

enum
{
    MT_CAMERA_DIALOG_OK_BTN = wxID_HIGHEST + 1,

};

class MT_AVTCameraDialog : public wxDialog
{
protected:
    wxComboBox* m_pCameraSelectBox;
    wxButton* m_pOkButton;

public:
    MT_AVTCameraDialog(wxWindow* parent, const wxPoint& pos, wxString* cameras, int camerasLength);
    ~MT_AVTCameraDialog();

    //void OnDoneButtonClicked(wxCommandEvent& event);

    bool Show();
    int GetCameraSelection();
    virtual void OnOkButtonPressed(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif