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

#include "MT/MT_Tracking/base/MT_TrackerFrameBase.h"
//#include "MT/MT_Tracking/base/MT_TrackerBase.h"
//#include "MT/MT_Core/gl/glSupport.h"

class MT_CameraSlaveFrame : public MT_TrackerFrameBase
{
protected:
	int m_iIndex;
	MT_TrackerFrameBase* m_pTFBParent;

public:
    MT_CameraSlaveFrame(wxFrame* parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& title = wxT("Window"),
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxSize(640,480),
                        long style = MT_FIXED_SIZE_FRAME);

	virtual void setMTParent(MT_TrackerFrameBase* parent);

    virtual void doMasterInitialization();

	virtual bool doKeyboardCallback(wxKeyEvent& event);
	virtual bool doMouseCallback(wxMouseEvent& event, double viewport_x, double viewport_y);

	virtual void doUserStep();

	void setIndex(int index){m_iIndex = index;};

	virtual void prepareToClose();
	virtual void setTimer(int period_msec);
	virtual void doUserGLDrawing();

	virtual void doUserQuit();

};
