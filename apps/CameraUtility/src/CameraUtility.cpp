/*
 *  CameraUtility.cpp
 *
 *  Created by Daniel Swain on 1/24/10.
 *
 */

#include "CameraUtility.h"

IMPLEMENT_APP(CameraUtilityApp)

CameraUtilityFrame::CameraUtilityFrame(wxFrame* parent, 
                wxWindowID id, 
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                long style) 
: MT_TrackerFrameBase(parent, id, title, pos, size, style)
{
}

void CameraUtilityFrame::onNewCapture()
{
    startTracking();
}

MT_ControlFrameBase* CameraUtilityFrame::createControlDialog()
{
    /* no "Start Tracking" button */
    m_pTrackerControlFrame = new MT_TrackerControlFrameBase(this, MT_CF_DEFAULT_BUTTONS);
    return (MT_ControlFrameBase*) m_pTrackerControlFrame;
}

void CameraUtilityFrame::makeFileMenu(wxMenu* file_menu)
{
    /* mostly copied from MT_TrackerFrameBase, just removed extra elements */
    file_menu->Append(wxID_OPEN, wxT("&Open Avi..."));
    wxFrame::Connect(wxID_OPEN,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_TrackerFrameBase::onMenuFileSelectAVI));

    file_menu->Append(MT_TFB_ID_MENU_CAPTURE_FROM_CAMERA, wxT("&Camera..."));
    wxFrame::Connect(MT_TFB_ID_MENU_CAPTURE_FROM_CAMERA,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_TrackerFrameBase::onMenuFileCamera));

    file_menu->AppendSeparator();

    file_menu->Append(MT_TFB_ID_MENU_FILE_CREATE_BACKGROUND, wxT("&Create Background..."));
    wxFrame::Connect(MT_TFB_ID_MENU_FILE_CREATE_BACKGROUND,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_TrackerFrameBase::onMenuFileCreateBackground));

    file_menu->AppendSeparator();

    /* will append preferences and exit to menu */
    MT_FrameBase::makeFileMenu(file_menu);

}

void CameraUtilityFrame::initUserData()
{
    MT_TrackerFrameBase::initUserData();
    
    m_pPreferences->AddInt("Frame Period msec", &m_iOverrideFramePeriod_msec);
    
    setTimer(m_iOverrideFramePeriod_msec);
}

void CameraUtilityFrame::doUserStep()
{
    if(m_iOverrideFramePeriod_msec != m_iPrevFramePeriod_msec)
    {
        setTimer(m_iOverrideFramePeriod_msec);
    }
    m_iPrevFramePeriod_msec = m_iOverrideFramePeriod_msec;
    
    MT_TrackerFrameBase::doUserStep();
}