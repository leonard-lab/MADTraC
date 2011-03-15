/* Standard WX include block */
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
/* End standard WX include block */

/* wx-specific includes */
#include <wx/glcanvas.h>

#include "MT/MT_GUI/base/MT_FrameBase.h"
#include "MT/MT_Core/gl/glSupport.h"

class MT_CameraSlaveFrame : public MT_FrameBase
{
public:
    MT_CameraSlaveFrame(wxFrame* parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& title = wxT("Window"),
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxSize(640,480),
                        long style = MT_FIXED_SIZE_FRAME);

    virtual void doMasterInitialization();

    void setImage(IplImage* image)
    {
        MT_FrameBase::setImage(image);
    }

	virtual void prepareToClose();
	virtual void setTimer(int period_msec);
	virtual void doUserGLDrawing();

	virtual void doUserQuit();

};