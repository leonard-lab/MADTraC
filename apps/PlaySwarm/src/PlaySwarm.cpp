/*
 *  PlaySwarm.cpp
 *
 *  Created by Daniel Swain on 11/19/09.
 *
 */

#include "PlaySwarm.h"
#include "Occlusion.h"

#include <wx/filename.h>

IMPLEMENT_APP(PlaySwarmApp)

static bool g_bMouseDragging = false;
static bool g_bFlagMouseUp = false;
static int g_iMenuAgent = NONE_GRABBED;

const char* glyph_names[] = {"Fish", "Arrow", "Ellipse"};

AgentParameters::AgentParameters(double* length, double* aspect, MT_Color* color)
    :MT_DataGroup("Agent Parameters")
{
    MT_DataGroup::AddDouble("Length", length);
    MT_DataGroup::AddDouble("Aspect Ratio", aspect);
    MT_DataGroup::AddColor("Color", color);
}


PlaySwarmFrame::PlaySwarmFrame(wxFrame* parent,
                               wxWindowID id,
                               const wxString& title,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style)
    : MT_FrameBase(parent, id, title, pos, size, style), 
      m_bAutoSizeObjects(true),
      m_bTails(false),
      m_bOcclusions(false),
      m_GlyphChoice(glyph_names, 3, 1),
      m_BoundingBox(-1,1,-1,1)
{
#ifdef MT_HAVE_CLF
    CLFpp::DisableHDF5Warnings();
#endif
    m_sDescriptionText = wxString("PlaySwarm - Playback trajectory data.\n");
}

PlaySwarmFrame::~PlaySwarmFrame()
{
  
/* safely free the agents */
    for(unsigned int i = 0; i < m_vpAgents.size(); i++)
    {
        if(m_vpAgents[i])
        {
            delete m_vpAgents[i];
        }
    }
  
}

void PlaySwarmFrame::writeUserXML()
{
}

void PlaySwarmFrame::readUserXML()
{
}

bool PlaySwarmFrame::grabAgent(double x, double y)
{
  
    if(m_iGrabbedAgent != NONE_GRABBED)
    {
        return true;
    }
  
    if(m_dGrabRadius <= 0)
    {
        m_dGrabRadius = 1.0*m_dObjectLength*m_dObjectLength;
    }
  
    double min_d = -1;
    double min_i = -1;
    double dx, dy;
    MT_agent* ca;
    for(unsigned int i = 0; i < m_vpAgents.size(); i++)
    {
        ca = m_vpAgents[i];
        dx = (x - ca->x())*(x - ca->x());
        dy = (y - ca->y())*(y - ca->y());
        if(min_d < 0 || (dx + dy < min_d))
        {
            min_i = i;
            min_d = dx + dy;
        }
    }
  
    if(min_d < m_dGrabRadius)
    {
        m_iGrabbedAgent = min_i;
        return true;
    }
    else
    {
        m_iGrabbedAgent = NONE_GRABBED;
        return false;
    }
  
}

bool PlaySwarmFrame::doMouseCallback(wxMouseEvent& event, double viewport_x, double viewport_y)
{
    bool result = MT_DO_BASE_MOUSE;
/* Check to see if this was a click and if it was "on" an agent (i.e. within m_dGrabRadius) */
    if((event.LeftDown() || event.RightDown()) && grabAgent(viewport_x,viewport_y))
    {
        if(event.RightDown())
        {
            wxMenu pmenu;
            wxString label;
            g_iMenuAgent = m_iGrabbedAgent;
            label.Printf("Parameters for Agent %d", m_iGrabbedAgent);
            pmenu.Append(ID_MENU_POP_AGENT_PARAMS,label);
            Connect(ID_MENU_POP_AGENT_PARAMS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PlaySwarmFrame::onMenuAgentParams));
            PopupMenu(&pmenu);
            result = MT_SKIP_BASE_MOUSE;
        }
    }
/* If we've previously grabbed an agent, do whatever needs to be done */
    else if(m_iGrabbedAgent != NONE_GRABBED)
    {
/* If we're releasing the mouse button, release the agent */
        if(event.LeftUp() || event.RightUp())
        {
            m_iGrabbedAgent = NONE_GRABBED;
        }
    }
/* "Normal" - i.e. no agent is grabbed, behavior */
    else
    {
    
        if(event.RightDown())
        {
/*      wxMenu pmenu;
        pmenu.Append(wxID_ANY,"Normal popup.");
        PopupMenu(&pmenu); */
            result = MT_SKIP_BASE_MOUSE; 
        }
    }
  
    bool base_result = MT_FrameBase::doMouseCallback(event, viewport_x, viewport_y);
    return result && base_result;
  
}

bool PlaySwarmFrame::doKeyboardCallback(wxKeyEvent& event)
{
  
    char key = event.GetKeyCode();
    
    switch(key)
    {
    case 'f':
        gotoFrame();  break;
    case 't':
        m_bTails = !m_bTails; break;
    case 'o':
        m_bOcclusions = !m_bOcclusions; break;
    case 'r':
        doReset(MT_SOFT_RESET); break;
    case 'n':
        OCC_nextAOI(); break;
    case 'v':
        OCC_nextVisualAngle(); break;
    case WXK_DELETE:
    case WXK_BACK:
    case ':':  /* On a mac this is the right arrow too for some reason */
        doBackup(); break;
    }
  
    bool base_result = MT_FrameBase::doKeyboardCallback(event);
    return MT_DO_BASE_KEY && base_result;
  
}

void PlaySwarmFrame::handleCommandLineArguments(int argc, char** argv)
{
 
    /* Note doing this rather than m_bTails = parser.Found("t") allows
     * the value from the XML file to be accepted. */
    if(m_CmdLineParser.Found("t"))
    {
        m_bTails = true;
    }

    if(m_CmdLineParser.Found("o"))
    {
        m_bOcclusions = true;
    }
  
    long temp;
    if(!m_CmdLineParser.Found("l", &temp))
    {
        /* automatically size the object */
        m_bAutoSizeObjects = true;
    }
    else
    {
        m_bAutoSizeObjects = false;
        m_dObjectLength = (double) temp;
    }
  
    if(!m_CmdLineParser.Found("w", &temp))
    {
        m_bAutoSizeObjects = true;
    }
    else
    {
        m_bAutoSizeObjects = false;
        m_dObjectWidth = (double) temp;
    }
  
    /* if a parameter ( = command line option not preceeded by "-"
     * or "--" )  was given, it's the filename to load */
    if(m_CmdLineParser.GetParamCount())
    {
        wxString filename = m_CmdLineParser.GetParam(0);
        openPositionFile(filename.c_str(), AUTO_PLAY);
    }
  
}

void PlaySwarmFrame::handleOpenWithFile(const wxString& filename)
{
    openPositionFile(filename.c_str(), AUTO_PLAY);
}

void PlaySwarmFrame::initUserData()
{
    m_Server.doInit();
  
    m_pPreferences->AddBool("Show Tails", &m_bTails);
    m_pPreferences->AddBool("Show Occlusions", &m_bOcclusions);
    m_pPreferences->AddDouble("Global Object Length", &m_dObjectLength);
    m_pPreferences->AddDouble("Global Object Width", &m_dObjectWidth);
    m_pPreferences->AddChoice("Object Representation", &m_GlyphChoice);

    m_dGrabRadius = 0;
    m_iGrabbedAgent = NONE_GRABBED;
 
    m_sDataFilePath = wxT("");
    m_sDataFileDirectory = wxT("");
  
    m_PathGroup.AddPath("Data_Directory", &m_sDataFileDirectory);

    m_CmdLineParser.AddSwitch("t", "tails", "Draw trajectory tails.");
    m_CmdLineParser.AddSwitch("o", "occlusions", "Calculate and draw occlusions.");
    m_CmdLineParser.AddOption("l", 
                              "object-length", 
                              "Standard object length (drawing units).", 
                              wxCMD_LINE_VAL_NUMBER, 
                              wxCMD_LINE_PARAM_OPTIONAL);
    m_CmdLineParser.AddOption("w",
                              "object-width",
                              "Standard object width (drawing units).",
                              wxCMD_LINE_VAL_NUMBER,
                              wxCMD_LINE_PARAM_OPTIONAL);
    m_CmdLineParser.AddParam("Position data file.",
                             wxCMD_LINE_VAL_STRING,
                             wxCMD_LINE_PARAM_OPTIONAL);
  
}

void PlaySwarmFrame::makeFileMenu(wxMenu* file_menu)
{
    file_menu->Append(wxID_FILE, wxT("Open File..."));
    wxFrame::Connect(wxID_FILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PlaySwarmFrame::onMenuFileOpen));

    file_menu->AppendSeparator();

    /* will append preferences and exit to menu */
    MT_FrameBase::makeFileMenu(file_menu);

}

void PlaySwarmFrame::onMenuFileOpen(wxCommandEvent& event)
{
  
    int result = MT_OpenFileDialog(this, 
                                   m_sDataFileDirectory, 
                                   wxT("Select Data File"), 
                                   FILTER_DATA_FILES, 
                                   &m_sDataFilePath, 
                                   &m_sDataFileDirectory);
  
    if(result == wxID_OK)
    {
        openPositionFile(m_sDataFilePath.c_str(), NO_AUTO_PLAY);
    }
  
}

void PlaySwarmFrame::onMenuFileSaveScreen(wxCommandEvent& event)
{
    saveScreen();
}

void PlaySwarmFrame::onMenuFilePreferences(wxCommandEvent& event)
{
    MT_FrameBase::onMenuFilePreferences(event);
}

void PlaySwarmFrame::onMenuAgentParams(wxCommandEvent& event)
{
    if(g_iMenuAgent == NONE_GRABBED)
    {
        return;
    }

    AgentParameters* params = new AgentParameters(&(m_vpAgents[g_iMenuAgent]->size), &(m_vpAgents[g_iMenuAgent]->aspect),&(m_vpAgents[g_iMenuAgent]->myColor));
    MT_DataGroupDialog* dlg = new MT_DataGroupDialog(params, this, true);
    dlg->Show();
    dlg->Raise();

}

void PlaySwarmFrame::openPositionFile(const char* filename,
                                      bool start_play)
{
    if(CLFpp::IsCLF(filename))
    {
        MT_CLFtoAgentReader reader(filename, "x", "y", "z");
        m_vpAgents = reader.getAgents();
        m_BoundingBox = reader.getBoundingBox();
    }
    else
    {

        MT_ParticleFile file(filename);
  
        if(!file.IsValid())
        {
            MT_ShowErrorDialog(this,
                               wxT("Could not open file ")
                               + wxString(filename) + wxT("."));
            m_vpAgents.resize(0);
        }
        else
        {
            m_BoundingBox = file.GetBoundingBox();
            m_vpAgents = file.GetAgents();
        }
    }

    if(m_bAutoSizeObjects)
    {
        m_dObjectLength = DEFAULT_OBJECT_LENGTH_FRAC
            *fabs(m_BoundingBox.xmax - m_BoundingBox.xmin);
        m_dObjectWidth = DEFAULT_OBJECT_ASPECT*m_dObjectLength;

        /* TEMPORARY */
        //m_dObjectLength = 4.0;
        //m_dObjectWidth = 0.8;
    }
    for(unsigned int i = 0; i < m_vpAgents.size(); i++)
    {
        m_vpAgents[i]->PopulateSpeedThetaBuffers();
        m_vpAgents[i]->size = (m_dObjectLength);
        m_vpAgents[i]->aspect = m_dObjectWidth/m_dObjectLength;
        m_vpAgents[i]->setBlindAngle(MT_DEG2RAD*20);
        m_vpAgents[i]->Update();  /* sets state for drawing */
    }
    
    wxFrame::Refresh(true);

    m_bFollow = false;
    setViewport(MT_RectangleFromBoundingBox(m_BoundingBox));
    lockCurrentViewportAsOriginal();
    
    setPause(!start_play);


}


void PlaySwarmFrame::doUserGLDrawing()
{  
    static double prev_length = m_dObjectLength;
    static double prev_width = m_dObjectWidth;
  
    if((m_dObjectLength != prev_length) || (m_dObjectWidth != prev_width)) 
    {
        for(unsigned int i = 0; i < m_vpAgents.size(); i++)
        {
            m_vpAgents[i]->size = m_dObjectLength;
            m_vpAgents[i]->aspect = m_dObjectWidth/m_dObjectLength;
        }
        prev_length = m_dObjectLength;
        prev_width = m_dObjectWidth;
    }
  
    MT_BufferAgent* ca;
    for(unsigned int i = 0; i < m_vpAgents.size(); i++)
    {
        ca = m_vpAgents[i];
        MT_GLDrawGlyph(m_GlyphChoice.GetIntValue() + 1, ca->myColor, ca->getPos(), ca->gettheta(), ca->size, ca->aspect);
        if(m_bTails)
        {
            /*ca->glDrawTail(getXMin(), getXMax(), getYMin(), getYMax());*/
        }
    }
  
    if(m_bOcclusions)
    {
        CalculateAndDrawOcclusions(m_vpAgents);
    }
  
    if(m_bFollow)
    {
        MT_BoundingBox curr_box;
        for(unsigned int i = 0; i < m_vpAgents.size(); i++)
        {
            curr_box.ShowX(m_vpAgents[i]->x());
            curr_box.ShowY(m_vpAgents[i]->y());
        }
        curr_box.xmin = curr_box.xmin - m_dObjectLength;
        curr_box.xmax = curr_box.xmax + m_dObjectLength;
        curr_box.ymin = curr_box.ymin - m_dObjectLength;
        curr_box.ymax = curr_box.ymax + m_dObjectLength;
        zoomTo(MT_Rectangle(curr_box.xmin, curr_box.xmax, curr_box.ymin, curr_box.ymax));
    
        if(g_bFlagMouseUp)
        {
            g_bFlagMouseUp = false;
        }
    }
    else
    {
    }
  
}

void PlaySwarmFrame::doUserStep()
{
  
    MT_BoundingBox lims;

    for(unsigned int i = 0; i < m_vpAgents.size(); i++)
    {
        m_vpAgents[i]->integrate();
        m_vpAgents[i]->Update();
        lims.ShowX(m_vpAgents[i]->x() - m_vpAgents[i]->getSize());
        lims.ShowX(m_vpAgents[i]->x() + m_vpAgents[i]->getSize());
        lims.ShowY(m_vpAgents[i]->y() - m_vpAgents[i]->getSize());
        lims.ShowY(m_vpAgents[i]->y() + m_vpAgents[i]->getSize());
    }
  
    MT_Rectangle limsv = MT_RectangleFromBoundingBox(lims);
    tellObjectLimits(limsv, 0.05);
  
}

void PlaySwarmFrame::doReset(bool hard_reset)
{
    MT_FrameBase::doReset(hard_reset);
    for(unsigned int i = 0; i < m_vpAgents.size(); i++)
    {
        m_vpAgents[i]->Reset(0,0,0,0);
    }
}

void PlaySwarmFrame::doBackup()
{
    for(unsigned int i = 0; i < m_vpAgents.size(); i++)
    {
        m_vpAgents[i]->RWND(1);
    }
}

void PlaySwarmFrame::gotoFrame()
{

    long max_frames = m_vpAgents[0]->GetBufferLength();
    int curr_pos = m_vpAgents[0]->GetBufferPosition();
    long frame_no = curr_pos;
        
    setPause(true);

    MT_NumericInputDialog* dlg = new MT_NumericInputDialog(
        this,             /* parent is this frame */
        wxID_ANY,         /* ID is unimportant */
        wxT("PlaySwarm"), /* Title for the dialog */
        wxT("Go To Frame Number"), /* Prompt */
        curr_pos, /* default number in the dlg */
        0,                   /* minimum admissible input */
        (double) max_frames,  /* maximum admissible input */
        MT_INPUT_INTEGER);      /* input must be an integer */

    int result = dlg->ShowModal();

    if(result == wxID_OK)
    {
        frame_no = dlg->GetValue();
    }

    dlg->Destroy();

    MT_BoundingBox lims;
    int offset = frame_no - curr_pos;
    if(offset > 0)
    {
        for(unsigned int i = 0; i < m_vpAgents.size(); i++)
        {
            m_vpAgents[i]->FFWD(offset);
        }
    }
    else
    {
        for(unsigned int i = 0; i < m_vpAgents.size(); i++)
        {
            m_vpAgents[i]->RWND(-offset);
        }
    }



    for(unsigned int i = 0; i < m_vpAgents.size(); i++)
    {
        if(offset > 0)
        {
            m_vpAgents[i]->FFWD(offset);
        }
        else
        {
            m_vpAgents[i]->RWND(offset);
        }
        lims.ShowX(m_vpAgents[i]->x() - m_vpAgents[i]->getSize());
        lims.ShowX(m_vpAgents[i]->x() + m_vpAgents[i]->getSize());
        lims.ShowY(m_vpAgents[i]->y() - m_vpAgents[i]->getSize());
        lims.ShowY(m_vpAgents[i]->y() + m_vpAgents[i]->getSize());
    }
  
    MT_Rectangle limsv = MT_RectangleFromBoundingBox(lims);
    tellObjectLimits(limsv, 0.05);

    zoomTo(limsv);
}
