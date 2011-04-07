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
    : MT_TrackerFrameBase(parent, id, title, pos, size, style),
      m_iIndex(0),
	  m_pTFBParent(NULL)
{
}

void MT_CameraSlaveFrame::doMasterInitialization()
{
	m_bAmSlave = true;

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

	setSizeByClient(640, 480);
	setViewport(MT_BlankRectangle, true);
	lockCurrentViewportAsOriginal();

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

void MT_CameraSlaveFrame::doUserGLDrawing()
{
	if(m_pTracker)
	{
		m_pTracker->doGLDrawing(m_iIndex);
	}
	if(m_pTFBParent)
	{
		m_pTFBParent->doSlaveGLDrawing(m_iIndex);
	}
}

void MT_CameraSlaveFrame::setMTParent(MT_TrackerFrameBase* parent)
{
	if(!m_pTFBParent)
	{
		m_pTFBParent = parent;
	}
}

bool MT_CameraSlaveFrame::doMouseCallback(wxMouseEvent& event, double viewport_x, double viewport_y)
{
    bool result = MT_DO_BASE_MOUSE;

	bool presult = MT_DO_BASE_MOUSE;
	if(m_pTFBParent)
	{
		presult = m_pTFBParent->doSlaveMouseCallback(event, viewport_x, viewport_y, m_iIndex);
	}
	result = result && presult;

    bool tresult = result;
    if(result == MT_DO_BASE_MOUSE)
    {
        tresult = MT_TrackerFrameBase::doMouseCallback(event, viewport_x, viewport_y);
    }
    
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

	bool presult = MT_DO_BASE_KEY;
	if(m_pTFBParent)
	{
		presult = m_pTFBParent->doSlaveKeyboardCallback(event, m_iIndex);
	}
	result = result && presult;

    bool tresult = result;
    if(result == MT_DO_BASE_KEY)
    {
        tresult = MT_TrackerFrameBase::doKeyboardCallback(event);
    }
    
    return tresult && result;
}

void MT_CameraSlaveFrame::doUserStep()
{
	/* short-circuit the trackerframe step, since this
	   runs the tracker and we're not in charge of that!
    */
	MT_FrameBase::doUserStep();
}