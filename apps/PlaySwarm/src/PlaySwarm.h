#ifndef PlaySwarm_H
#define PlaySwarm_H

/*
 *  PlaySwarm.h
 *
 *  Created by Daniel Swain on 11/19/09.
 *
 */

#include "MT_Core.h"
#include "MT_GUI.h"

const bool AUTO_PLAY = true;
const bool NO_AUTO_PLAY = false;

const double DEFAULT_OBJECT_LENGTH_FRAC = 0.05;  /* 5 % of viewport width */
const double DEFAULT_OBJECT_ASPECT = 0.125;

const int NONE_GRABBED = -1;

const unsigned int DEFAULT_WINDOW_WIDTH = 1024;
const unsigned int DEFAULT_WINDOW_HEIGHT = 768;

const wxString FILTER_DATA_FILES("Trajectory data files (*.dat; *.clf)|*.dat;*.clf");

enum
{
    ID_MENU_POP_AGENT_PARAMS = MT_ID_HIGHEST + 1,
    ID_MENU_FILE_SAVESCREEN,
};


class AgentParameters : public MT_DataGroup
{
public:
    AgentParameters(double* length, double* aspect, MT_Color* color);
};

class PlaySwarmFrame : public MT_FrameBase
{
protected:
    bool m_bAutoSizeObjects;
    
    bool m_bTails;
    
    bool m_bOcclusions;
    bool m_bFollow;
    
    double m_dObjectLength;
    double m_dObjectWidth;

    MT_Choice m_GlyphChoice;
    
    wxString m_sDataFilePath;
    wxString m_sDataFileDirectory;
    
    std::vector<MT_BufferAgent*> m_vpAgents;
    
    MT_BoundingBox m_BoundingBox;
    
    virtual bool grabAgent(double x, double y);
    double m_dGrabRadius;
    int m_iGrabbedAgent;
        
public:
    PlaySwarmFrame(wxFrame* parent,
                   wxWindowID id = wxID_ANY,
                   const wxString& title = wxT("PlaySwarm"),
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxSize(DEFAULT_WINDOW_WIDTH,
                                               DEFAULT_WINDOW_HEIGHT),
                   long style = MT_FIXED_SIZE_FRAME);
    virtual ~PlaySwarmFrame();
    
    virtual void handleCommandLineArguments(int argc, char** argv);
    virtual void handleOpenWithFile(const wxString& filename);

    virtual void initUserData();
    
    virtual void makeFileMenu(wxMenu* file_menu);
    
    virtual void onMenuFileOpen(wxCommandEvent& event);
    virtual void onMenuFileSaveScreen(wxCommandEvent& event);
    virtual void onMenuFilePreferences(wxCommandEvent& event);
    
    virtual void onMenuAgentParams(wxCommandEvent& event);
    
    virtual void openPositionFile(const char* filename, bool start_play = NO_AUTO_PLAY);
    
    virtual void doUserGLDrawing();
    
    virtual void doUserStep();
    
    virtual void doReset(bool hard_reset = MT_HARD_RESET);
    virtual void doBackup();
    
    virtual bool doMouseCallback(wxMouseEvent& event, double viewport_x, double viewport_y);
    virtual bool doKeyboardCallback(wxKeyEvent& event);

    virtual void writeUserXML();
    virtual void readUserXML();

    virtual void gotoFrame();
};

class PlaySwarmApp : public MT_AppBase
{
    
public:
    MT_FrameWithInit* createMainFrame()
    {
        return new PlaySwarmFrame(NULL);
    }
    
};

#endif // PlaySwarm_H
