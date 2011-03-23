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
#include "MT/MT_Tracking/base/MT_TrackerBase.h"
#include "MT/MT_Core/gl/glSupport.h"

class MT_CameraSlaveFrame : public MT_FrameBase
{
protected:
	MT_TrackerFrameGroup* m_pTrackerFrameGroup;
	MT_TrackerBase* m_pTracker;
	IplImage* m_pCurrentFrame;
	int m_iIndex;
    int m_iView;
    bool m_bHaveFrame;

public:
    MT_CameraSlaveFrame(wxFrame* parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& title = wxT("Window"),
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxSize(640,480),
                        long style = MT_FIXED_SIZE_FRAME);

    virtual void doMasterInitialization();

	virtual bool doMouseCallback(wxMouseEvent& event, 
		double viewport_x,
		double viewport_y);
	virtual bool doKeyboardCallback(wxKeyEvent& event);

    void setImage(IplImage* image)
    {
        MT_FrameBase::setImage(image);
    }
	void setFrame(IplImage* frame);

	virtual void fillPopupMenu(wxMenu* pmenu);

	void setTracker(MT_TrackerBase* tracker);
	void setTrackerFrameGroup(MT_TrackerFrameGroup* frameGroup);
	void setIndex(int index){m_iIndex = index;};
	void setView(unsigned int i);

	void addTrackerFrameGroupToPopupMenu(wxMenu* pmenu);

	virtual void onPopupFrameSelect(wxCommandEvent& event);

	virtual void prepareToClose();
	virtual void setTimer(int period_msec);
	virtual void doUserGLDrawing();

	virtual void doUserQuit();

};
