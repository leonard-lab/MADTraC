#ifndef MT_MovieExporterDialog_H
#define MT_MovieExporterDialog_H

/*
 *  MT_MovieExporterDialog.h
 *
 *  Created by Daniel Swain on 12/20/09.
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

#include "MT/MT_GUI/base/MT_FrameBase.h"
#include "MT/MT_Core/fileio/MovieExporter.h"

class MT_MovieExporterDialog : public wxDialog
{
private:
    wxString m_sDirectory;
    wxString m_sPath;
    int m_iFrameWidth;
    int m_iFrameHeight;
    wxStaticText* m_pPathDescriptor;
    wxTextCtrl* m_pPathCtrl;
    wxRadioBox* m_pRadioBox;
    wxTextCtrl* m_pNumToSkipCtrl;
    wxTextCtrl* m_pFPSCtrl;
    MT_ME_Type m_METype;

    void onPathSelect(wxCommandEvent& event);
    void onNumToSkipChange(wxCommandEvent& event);
    void onFPSChange(wxCommandEvent& event);
    void onRadioBoxClicked(wxCommandEvent& event);
public:
    MT_MovieExporterDialog(const wxString& directory,
                           MT_ME_Type method,
                           int frame_width,
                           int frame_height,
                           int num_to_skip,
                           double FPS,
                           wxWindow* parent,
                           wxWindowID id = wxID_ANY);

    void getInfo(wxString* filename, wxString* directory, MT_ME_Type* method, int* num_to_skip, double* FPS);
    void getInfo(MT_MovieExporter* exporter, wxString* directory);
};

#endif // MT_MovieExporterDialog_H
