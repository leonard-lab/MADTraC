#include "MT_CameraSlaveFrame.h"

const unsigned int MAX_NUM_FRAMES = 15;

enum
{
	ID_PMENU_FRAME00 = wxID_HIGHEST+1,
	ID_PMENU_FRAME01
};

MT_CameraSlaveFrame::MT_CameraSlaveFrame(wxFrame* parent,
                                         wxWindowID id,
                                         const wxString& title,
                                         const wxPoint& pos,
                                         const wxSize& size,
                                         long style)
    : MT_FrameBase(parent, id, title, pos, size, style),
	m_pTrackerFrameGroup(NULL),      
	m_pTracker(NULL),
	m_pCurrentFrame(NULL),
	m_iIndex(0)
{
}

void MT_CameraSlaveFrame::doMasterInitialization()
{
    m_pTimer = createTimer();
    setTimer(MT_DEFAULT_FRAME_PERIOD);

    /* UI Creation */
    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);  
    m_pCanvas = new MT_GLCanvasBase(this, 
                                    MT_ID_CANVAS,
                                    wxDefaultPosition, 
                                    wxSize(640, 480));
    m_pCanvas->Show();
    hbox->Add(m_pCanvas);
    SetSizerAndFit(hbox);
    m_pCanvas->setMTParent((MT_FrameBase *)this);

    m_pPreferences = NULL;

}

void MT_CameraSlaveFrame::prepareToClose()
{
	setImage(NULL);
	m_pTimer->Stop();
	stopTimedEvents();
    doPause();
}

void MT_CameraSlaveFrame::doUserQuit()
{
}

void MT_CameraSlaveFrame::setTimer(int period_msec)
{
	MT_FrameBase::setTimer(period_msec);
}

void MT_CameraSlaveFrame::setTracker(MT_TrackerBase* tracker)
{
	m_pTracker = tracker;
}

void MT_CameraSlaveFrame::setTrackerFrameGroup(MT_TrackerFrameGroup* frameGroup)
{
	m_pTrackerFrameGroup = frameGroup;
}

void MT_CameraSlaveFrame::doUserGLDrawing()
{
	if(m_pTracker)
	{
		m_pTracker->doGLDrawing(m_iIndex);
	}
}

bool MT_CameraSlaveFrame::doMouseCallback(wxMouseEvent& event, 
		double viewport_x,
		double viewport_y)
{
	bool result = MT_DO_BASE_MOUSE;

	if(event.RightUp())
	{
		wxMenu pmenu;

		fillPopupMenu(&pmenu);

		if(pmenu.GetMenuItemCount())
		{
			PopupMenu(&pmenu);
			result = MT_SKIP_BASE_MOUSE;
		}

	}

	bool tresult = MT_FrameBase::doMouseCallback(event, viewport_x, viewport_y);
	return tresult && result;

}

bool MT_CameraSlaveFrame::doKeyboardCallback(wxKeyEvent& event)
{
    bool result = MT_DO_BASE_KEY;

    switch(event.GetKeyCode())
    {
    case 'q':
        /* DO NOT allow quit to be called */
        result = MT_SKIP_BASE_KEY;
        break;
    }

    bool tresult = result;
    if(result == MT_DO_BASE_KEY)
    {
        tresult = MT_FrameBase::doKeyboardCallback(event);
    }
    
    return tresult && result;
}


void MT_CameraSlaveFrame::fillPopupMenu(wxMenu* pmenu)
{
	if(m_pTrackerFrameGroup)
	{
		addTrackerFrameGroupToPopupMenu(pmenu);
	}
}

void MT_CameraSlaveFrame::addTrackerFrameGroupToPopupMenu(wxMenu* pmenu)
{
	pmenu->AppendRadioItem(ID_PMENU_FRAME00, wxT("Frame"));
	Connect(ID_PMENU_FRAME00,
		wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(MT_CameraSlaveFrame::onPopupFrameSelect));

	std::vector<string> frameNames = m_pTrackerFrameGroup->getFrameNames();
	unsigned int n = MT_MIN(frameNames.size(), MAX_NUM_FRAMES);
	for(unsigned int i = 0; i < n; i++)
	{
		pmenu->AppendRadioItem(ID_PMENU_FRAME01+i, 
			MT_StringToWxString(frameNames[i]));
		Connect(ID_PMENU_FRAME01+i,
			wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(MT_CameraSlaveFrame::onPopupFrameSelect));
	}
}

void MT_CameraSlaveFrame::onPopupFrameSelect(wxCommandEvent& event)
{
	int i = event.GetId() - ID_PMENU_FRAME00;

	setView(i);
}

void MT_CameraSlaveFrame::setView(unsigned int i)
{
	if(i > 0)
	{
		setImage(m_pTrackerFrameGroup->getFrame(i - 1));
	}
	else
	{
		setImage(m_pCurrentFrame);
	}
}
