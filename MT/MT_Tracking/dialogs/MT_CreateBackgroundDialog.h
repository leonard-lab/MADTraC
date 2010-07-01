#ifndef MT_CreateBackgroundDialog_H
#define MT_CreateBackgroundDialog_H

/*
 *  wxCreateBackgroundDialog.h
 *
 *  Created by Daniel Swain on 11/4/09.
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

#ifdef MT_HAVE_OPENCV_FRAMEWORK
  #include <OpenCV/OpenCV.h>
#else
  #include <cv.h>
  #include <cvaux.h>
  #include <highgui.h>
  #include <ml.h>
#endif

#include "MT/MT_GUI/base/MT_FrameBase.h"

class MT_bgcanvas : public MT_GLCanvasBase
{
protected:

public:
    
    MT_bgcanvas(wxWindow* parent,
             wxWindowID id = wxID_ANY,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize);
    
    bool doMouseCallback(wxMouseEvent& event, double viewport_x, double viewport_y);
    
    void doGLDrawing();
    
};

class MT_CreateBackgroundDialog : public wxDialog
{
protected:
    MT_GLCanvasBase* m_pCanvas;
    
    wxTextCtrl* m_pNumFramesCtrl;
    wxTextCtrl* m_pPathCtrl;
    wxTextCtrl* m_pRadiusCtrl;
    
    wxStaticText* m_pRadiusLabel;
    wxStaticText* m_pDrawHint;
    
    wxCheckBox* m_pDoInpaintCheck;
    
    void OnNumToAvgChange(wxCommandEvent& event);
    void OnPathButtonClicked(wxCommandEvent& event);
    void OnInpaintClicked(wxCommandEvent& event);
    void OnRadiusChange(wxCommandEvent& event);

public:
    MT_CreateBackgroundDialog(IplImage* sample_frame,
                              const wxString& directory,
                              wxWindow* parent, 
                              wxWindowID id = wxID_ANY);
    
    void GetInfo(long* num_to_avg, wxString* path, bool* inpaint, unsigned int* inpaint_radius, wxRect* inpaint_roi);
};

#endif // MT_CreateBackgroundDialog_H
