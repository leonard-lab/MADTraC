#ifndef MT_XDFDIALOGS_H
#define MT_XDFDIALOGS_H

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
#include "MT/MT_Core/fileio/ExperimentDataFile.h"

class MT_LoadXDFDialog : public wxDialog
{
protected:
    MT_ExperimentDataFile* m_pXDF;
    MT_DataGroup* m_pDG;

    wxChoice* m_pXChoice;
    wxChoice* m_pYChoice;
    wxTextCtrl* m_pTimeStepCtrl;

    wxArrayString m_asXChoices;
    wxArrayString m_asYChoices;

    wxString m_sXChoice;
    wxString m_sYChoice;
    
    bool m_bAmDerived;

    unsigned int m_iFramePeriodMSec;
    unsigned int m_iXIndex;
    unsigned int m_iYIndex;

    virtual void initData();
    virtual void initGUI();
    
public:
    MT_LoadXDFDialog(wxWindow* parent,
                     MT_ExperimentDataFile* pXDF,
                     bool am_derived = false);

    void onTSChanged(wxCommandEvent& event);
    void onOKClicked(wxCommandEvent& event);

    virtual void getInfo(std::string* x_name,
                         std::string* y_name,
                         unsigned int* frame_period_msec);

};

#endif /* MT_XDFDIALOGS_H */
