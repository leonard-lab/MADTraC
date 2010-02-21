#ifndef MT_NumericInputDialog_H
#define MT_NumericInputDialog_H

/*
 *  MT_NumericInputDialog.h
 *
 *  Created by Daniel Swain on 11/3/09.
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

const bool MT_INPUT_INTEGER = true;
const bool MT_INPUT_DOUBLE = false;

class MT_NumericInputDialog : public wxDialog
{
protected:
    bool m_bIsInteger;
    
    double m_dValue;
    double m_dMinValue;
    double m_dMaxValue;
    
    wxTextCtrl* m_pInputTextCtrl;
    
    void OnTextChange(wxCommandEvent& event);
    
public:
    MT_NumericInputDialog(wxFrame* parent, 
                          wxWindowID id, 
                          const wxString& title, 
                          const wxString& message, 
                          double value, 
                          double min_val, 
                          double max_val, 
                          bool isinteger = MT_INPUT_DOUBLE);
    double GetValue();
};

#endif // MT_NumericInputDialog_H
