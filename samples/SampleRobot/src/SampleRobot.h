#ifndef RobotBasicGUI_H
#define RobotBasicGUI_H

/*
 *  RobotBasicGUI.h
 *
 *  Created by Daniel Swain on 1/4/10.
 *
 */

#include "MT_Core.h"
#include "MT_GUI.h"
#include "MT_Tracking.h"
#include "MT_Robot.h"

class RobotBasicFrame : public MT_RobotFrameBase
{
protected:

    GYSegmenter* m_pGYTracker;
    unsigned int m_iDefaultNumToTrack;
    bool m_bNumToTrackInCmdLine;

public:
    RobotBasicFrame(wxFrame* parent,
                    wxWindowID id = wxID_ANY,
                    const wxString& title = wxT("Basic Robot Application"),
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxSize(640,480),
                    long style = MT_FIXED_SIZE_FRAME);

    virtual ~RobotBasicFrame(){};

    virtual void initTracker();

    virtual void initUserData();

    virtual void handleCommandLineArguments(int argc, char** argv);
       
};

class RobotBasicApp : public MT_AppBase
{
public:
    MT_FrameWithInit* createMainFrame()
    {
        return new RobotBasicFrame(NULL);
    }
};

#endif // RobotBasicGUI_H
