#include "MT_CameraSlaveFrame.h"

MT_CameraSlaveFrame::MT_CameraSlaveFrame(wxFrame* parent,
                                         wxWindowID id,
                                         const wxString& title,
                                         const wxPoint& pos,
                                         const wxSize& size,
                                         long style)
    : MT_FrameBase(parent, id, title, pos, size, style)
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

}

void MT_CameraSlaveFrame::prepareToClose()
{
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
}