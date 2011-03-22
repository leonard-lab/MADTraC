/*
 *  MT_TrackerFrameBase.cpp
 *
 */

#include "MT_TrackerFrameBase.h"

#include <wx/progdlg.h>
#include <wx/filename.h>

#include "MT/MT_GUI/support/wxSupport.h"           // for MT_GetXMLPath
#include "MT/MT_Tracking/cv/MT_MakeBackgroundFrame.h"
#include "MT/MT_Tracking/dialogs/MT_CreateBackgroundDialog.h"
#include "MT/MT_GUI/dialogs/MT_ParameterDialog.h"
#include "MT/MT_GUI/dialogs/MT_DataDialogs.h"

/********************************************************************/
/*      MT_TrackerControlFrameBase ctors and dtors                  */
/********************************************************************/

MT_TrackerControlFrameBase::MT_TrackerControlFrameBase(MT_TrackerFrameBase* parent, 
                                                       const int Buttons,
                                                       const wxPoint& pos, 
                                                       const wxSize& size)
  : MT_ControlFrameBase(parent, Buttons, pos, size),
    m_pParentTrackerFrame(parent),
    m_pButtonTrack(NULL)
{
}

/********************************************************************/
/*      MT_TrackerControlFrameBase public member functions          */
/********************************************************************/

void MT_TrackerControlFrameBase::doMasterInitialization()
{
    MT_ControlFrameBase::doMasterInitialization();
}

unsigned int MT_TrackerControlFrameBase::createButtons(wxBoxSizer* pSizer, wxPanel* pPanel)
{
    unsigned int nbuttons = MT_ControlFrameBase::createButtons(pSizer, pPanel);

    if(getButtons() & MT_TCF_TRACK)
    {
        m_pButtonTrack = new wxButton(pPanel, 
                                      MT_TCF_ID_BUTTON_TRACK, 
                                      wxT("Start Tracking"));
        pSizer->Add(m_pButtonTrack, 0, wxALL | wxCENTER, 10);
        m_pButtonTrack->Disable();
        Connect(MT_TCF_ID_BUTTON_TRACK, 
                wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(MT_TrackerControlFrameBase::onButtonTrackPressed));
        nbuttons++;
    }

    return nbuttons;
}

// what happens when we press the "Start Tracking" button
void MT_TrackerControlFrameBase::onButtonTrackPressed(wxCommandEvent& WXUNUSED(event))
{
    m_pParentTrackerFrame->startTracking();
}

// function to toggle the tracking state BUTTON
//  - gets called for example when we click "start tracking"
//  - doesn't do anything functional for the tracker, only the button
//  - the "tracking" flag is an override - setting it to true tells us to start
//      tracking even if we're already tracking
void MT_TrackerControlFrameBase::onTrackingToggle(bool tracking)
{
    // depends on whether we're already tracking or not
    if(tracking)
    {
        // if not, then change the label of the control
        //m_pButtonTrack->SetLabel(wxT("Stop Tracking"));
        if(m_pButtonTrack)  /* only if we *have* the button */
        {
            m_pButtonTrack->Disable();
        }
    }
    else
    { 
        if(m_pButtonTrack)
        {
            m_pButtonTrack->SetLabel(wxT("Start Tracking"));
        }
    }
}

void MT_TrackerControlFrameBase::enableButtons()
{
    if(m_pButtonTrack)
    {
        m_pButtonTrack->Enable();
    }

    MT_ControlFrameBase::enableButtons();
}


/********************************************************************/
/*         MT_TrackerFrameBase ctors and dtors                      */
/********************************************************************/

MT_TrackerFrameBase::MT_TrackerFrameBase(wxFrame* parent,
                                         wxWindowID id,
                                         const wxString& title, 
                                         const wxPoint& pos, 
                                         const wxSize& size,
                                         long style)
  : MT_FrameBase(parent, id, title, pos, size, style),
    m_sAVIDirectory(MT_DEFAULT_AVI_DIR),
    m_sROIDirectory(MT_DEFAULT_ROI_DIR),
    m_sBackgroundDirectory(MT_DEFAULT_BACKGROUND_DIR),
    m_sDataFileDirectory(MT_DEFAULT_DATA_DIR),
    m_sAVIPath(wxT("")),
    m_sROIPath(wxT("")),
    m_sBackgroundPath(wxT("")),
    m_sDataFilePath(wxT("")),
    m_bTracking(false),
    m_iView(MT_VIEW_FRAME),
    m_pTrackerControlFrame(NULL),
    m_pCapture(NULL),
    m_pTracker(NULL),
    m_pCurrentFrame(NULL),
    m_lTrackerDrawingFlags(0xFF)
{
}

MT_TrackerFrameBase::~MT_TrackerFrameBase()
{
    if(m_pCapture)
    {
        delete m_pCapture;
        m_pCapture = NULL;
    }

    if(m_pTracker)
    {
        delete m_pTracker;
        m_pTracker = NULL;
    }
}

/********************************************************************/
/*     MT_TrackerFrameBase private member functions                 */
/********************************************************************/

void MT_TrackerFrameBase::updateMenusOnStartTracker()
{

    /* recall menu bar */
    wxMenuBar* menubar = wxFrame::GetMenuBar();

    /* recall view menu and modify */
    int menu_pos = menubar->FindMenu(wxT("View"));
    /* gracefully handle the possibility that the menu doesn't exist */
    if(menu_pos != wxNOT_FOUND)
    {
        wxMenu* view_menu = menubar->GetMenu(menu_pos);
        addTrackerToViewMenu(view_menu);
    }

    /* recall tracker menu and modify */
    menu_pos = menubar->FindMenu(wxT("Tracker"));
    /* gracefully handle the possibility that the menu doesn't exist */
    if(menu_pos != wxNOT_FOUND)
    {
        wxMenu* tracker_menu = menubar->GetMenu(menu_pos);
        makeTrackerMenu(tracker_menu);
    }

    /* store the modified menu bar */
    wxFrame::SetMenuBar(menubar);
}

void MT_TrackerFrameBase::addTrackerToViewMenu(wxMenu* view_menu)
{
    /* if there's no tracker and/or the tracker only has one
     * frame view, there's no need to do any of this */
    if(!m_pTracker || !(m_pTracker->getNumProcessedFrames()))
    {
        return;
    }

    view_menu->AppendSeparator();

    view_menu->AppendRadioItem(MT_TFB_ID_MENU_VIEW_FRAME00, wxT("&Frame"));
    Connect(MT_TFB_ID_MENU_VIEW_FRAME00, 
            wxEVT_COMMAND_MENU_SELECTED, 
            wxCommandEventHandler(MT_TrackerFrameBase::onMenuViewSelect));

    MT_TrackerFrameGroup* frames = m_pTracker->getFrameGroup();
    if(frames)
    {
        std::vector<string> FrameNames = frames->getFrameNames();
        unsigned int n_views = MT_MIN(FrameNames.size(), MT_TFB_MAX_NUM_VIEWS);
        for(unsigned int i = 0; i < n_views; i++)
        {
            view_menu->AppendRadioItem(MT_TFB_ID_MENU_VIEW_FRAME01+i, MT_StringToWxString(FrameNames[i]));
            Connect(MT_TFB_ID_MENU_VIEW_FRAME01+i, 
                    wxEVT_COMMAND_MENU_SELECTED, 
                    wxCommandEventHandler(MT_TrackerFrameBase::onMenuViewSelect));
        }
    }

}

void MT_TrackerFrameBase::addDataGroupsToTrackerMenu(wxMenu* tracker_menu)
{
    if(!m_pTracker)
    {
        fprintf(stderr, "Warning: No tracker upon call to");
        fprintf(stderr, " addDataGroupsToTrackerMenu in");
        fprintf(stderr, " MT_TrackerFrameBase.cpp.  Will not");
        fprintf(stderr, " be able to comply.\n");
        return;
    }

    unsigned int NGroups = m_pTracker->getNumDataGroups();
    MT_DataGroup* dg = NULL;
    if(NGroups > 0)
    {
        tracker_menu->AppendSeparator();
        
        unsigned int ngroups = MT_MIN(NGroups, MT_TFB_MAX_NUM_TRACKER_PARAM_GROUPS);
        for(unsigned int i = 0; i < ngroups; i++)
        {
            dg = m_pTracker->getDataGroup(i);
            if(dg)
            {
                tracker_menu->Append(MT_TFB_ID_MENU_TRACKER_PARAMS00+i, MT_StringToWxString(dg->GetGroupName()));
                wxFrame::Connect(MT_TFB_ID_MENU_TRACKER_PARAMS00+i, 
                                 wxEVT_COMMAND_MENU_SELECTED, 
                                 wxCommandEventHandler(MT_TrackerFrameBase::onMenuTrackerParams));
            } 
        }
    }
}

void MT_TrackerFrameBase::addDataReportsToTrackerMenu(wxMenu* tracker_menu)
{
    unsigned int NReports = m_pTracker->getNumDataReports();
    MT_DataReport* dr = NULL;
    if(NReports)
    {
        tracker_menu->AppendSeparator();

        unsigned int nreports = MT_MIN(NReports, MT_MAX_NUM_TRACKER_REPORTS);
        for(unsigned int i = 0; i < nreports; i++)
        {
            dr = m_pTracker->getDataReport(i);
            if(dr)
            {
                tracker_menu->Append(MT_TFB_ID_MENU_TRACKER_REPORTS00+i, 
                                     MT_StringToWxString(dr->GetGroupName()));
                wxFrame::Connect(MT_TFB_ID_MENU_TRACKER_REPORTS00+i,
                                 wxEVT_COMMAND_MENU_SELECTED,
                                 wxCommandEventHandler(MT_TrackerFrameBase::onMenuTrackerReports));
            }
        }
    }
}

void MT_TrackerFrameBase::enableFrame()
{

    if(m_iView > 0 && m_pTracker)
    {
        setImage(m_pTracker->getProcessedFrame(m_iView - 1));
    }
    else  // should always be OK
    {
        setImage(m_pCurrentFrame);
    }

}
/********************************************************************/
/*     MT_TrackerFrameBase protected member functions               */
/********************************************************************/

bool MT_TrackerFrameBase::selectAVIFile()
{

    // we should pause everything when this starts
    doPause();

    wxString path;
    int result = MT_OpenFileDialog(this,
                                m_sAVIDirectory,
                                wxT("Select Movie File"),
                                MT_FILTER_MOVIE_FILES,
                                &path,
                                &m_sAVIDirectory);

    if(result == wxID_OK)
    {
        if(!setupAVICapture((const char*) path.mb_str()))
        {
            MT_ShowErrorDialog(this, wxT("Unable to open ") + path);
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }

} 


void MT_TrackerFrameBase::selectROI()
{

    // we should pause everything when this starts
    doPause();

    int result = MT_OpenFileDialog(this,
                                m_sROIDirectory,
                                wxT("Select ROI Image"),
                                MT_FILTER_IMAGE_FILES,
                                &m_sROIPath,
                                &m_sROIDirectory);

    if(result == wxID_OK)
    {
        if(m_pTracker)
        {
            // tell the tracker to set this as the ROI image
            m_pTracker->setROIImage((const char*) m_sROIPath.mb_str());
            // otherwise just hold on to the path and set it when the tracker starts
        }  
    }

}

void MT_TrackerFrameBase::createBackground()
{

    bool previous_paused = getPaused();
    doPause();

    // see if the capture is set up or not yet
    if(m_pCapture->getMode() == MT_FC_MODE_OFF)
    {
        // it's not, so try to get the user to select a file
        bool success = selectAVIFile();

        // if it didn't work, bail
        if(!success)
        {
            setPause(previous_paused);
            return;
        }
    }  

    /* show the background setup dialog */
    MT_CreateBackgroundDialog* dlg = 
        new MT_CreateBackgroundDialog(m_pCurrentFrame, 
                                      m_sBackgroundDirectory, this);

    int dialog_result = dlg->ShowModal();

    if(dialog_result == wxID_CANCEL)
    {
        return;
    }

    long navg = 100;
    bool inpaint = false;
    unsigned int radius = 5;
    wxRect roi(0, 0, m_pCurrentFrame->width, m_pCurrentFrame->height);

    dlg->GetInfo(&navg, &m_sBackgroundPath, &inpaint, &radius, &roi);
    dlg->Destroy();

    wxFileName bgfilename(m_sBackgroundPath);
    if(!bgfilename.IsDirWritable())
    {
        MT_ShowErrorDialog(this, wxT("Can't write to directory ") + bgfilename.GetPath());
        return;
    }
    else
    {
        m_sBackgroundDirectory = bgfilename.GetPath();
    }

    IplImage* BGFrame = cvCreateImage(m_pCapture->getFrameSize(), IPL_DEPTH_8U, m_pCapture->getNChannels());

    CvRect roi_rect = cvRect(roi.x, m_pCurrentFrame->height - roi.GetBottom(), roi.width, roi.height);
    MT_BackgroundFrameCreator* MakeBG = new MT_BackgroundFrameCreator(BGFrame, 
                                                                      m_pCapture, 
                                                                      MT_MAKEBG_MODE_STEP, 
                                                                      navg, 
                                                                      inpaint, 
                                                                      radius, 
                                                                      roi_rect);

    int success = 1;
    if(MakeBG->GetStatus() == MT_MAKEBG_OK)
    {
        wxProgressDialog* waitbox = new wxProgressDialog(wxT(""),wxT("Creating Background Frame."));
        waitbox->Show();
        // finally time to create the background frame! (for now uses all frames)
        for(double i = 0; i < navg; i++)
        {
            MakeBG->DoStep();
            waitbox->Update(floor(100.0*i/navg));
        }
        waitbox->Update(100);
        delete waitbox;
    }
    else
    {
        success = 0;
    }

    if(!success || (MakeBG->GetStatus() != MT_MAKEBG_OK))
    {
        success = 0;
    }

    MakeBG->Finish();
    delete MakeBG;

    // success in creating background frame?
    if(!success)
    {
        // no -> tell user there was an error.
        wxMessageDialog* messagebox = new wxMessageDialog(this, 
                                                          wxT("Unable to create background frame."), 
                                                          wxT("Error."),
                                                          wxOK);
        messagebox->ShowModal();
        messagebox->Destroy();
    }
    else
    {
        // yes -> save the background frame
        cvSaveImage((const char*) m_sBackgroundPath.mb_str(), BGFrame);

        // if tracking, set this as the bg frame
        if(m_pTracker)
        {
            m_pTracker->setBackgroundImage((const char*) m_sBackgroundPath.mb_str());
        }
    }

    // free the image
    cvReleaseImage(&BGFrame);

    setPause(previous_paused);

}

void MT_TrackerFrameBase::selectBackground()
{

    // we should pause everything when this starts
    doPause(); 

    int result = MT_OpenFileDialog(this,
                                m_sBackgroundDirectory,
                                wxT("Select Background Image"),
                                MT_FILTER_IMAGE_FILES,
                                &m_sBackgroundPath,
                                &m_sBackgroundDirectory);

    if(result == wxID_OK)
    {
        if(m_pTracker)
        {
            // tell the tracker to set this as the ROI image
            m_pTracker->setBackgroundImage((const char*) m_sBackgroundPath.mb_str());
            // otherwise just hold on to the path and set it when the tracker starts
        }  
    }  
}

void MT_TrackerFrameBase::selectDataFile()
{

    // we should pause everything when this starts
    doPause();

    int result = MT_SaveFileDialog(this,
                                m_sDataFileDirectory,
                                wxT("Select Data File"),
                                wxT(""),
                                &m_sDataFilePath,
                                &m_sDataFileDirectory,
                                MT_CONFIRM_OVERWRITE);

    if(result == wxID_OK)
    {
        if(m_pTracker)
        {
            // tell the tracker to set this as the ROI image
            m_pTracker->setDataFile((const char*) m_sDataFilePath.mb_str());
            // otherwise just hold on to the path and set it when the tracker starts
        }  
    }   
}


bool MT_TrackerFrameBase::setupCameraCapture()
{
    // make sure we're paused
    doPause();

    // TODO: Need an intermediary dialog to get camera settings...

    if(!m_pCapture->initCaptureFromCamera(MT_FC_DEFAULT_FW,
		MT_FC_DEFAULT_FH,
		MT_FC_SHOWDIALOG,
		MT_FC_NOFLIP,
		MT_FC_NOFLIP,
		MT_CAP_DEFAULT))
    {
        return false;
    }

    // show the first frame
    m_pCurrentFrame = m_pCapture->getFrame();
    setImage(m_pCurrentFrame);

    // find the capture size
    int framewidth_pixels = m_pCapture->getFrameWidth();
    int frameheight_pixels = m_pCapture->getFrameHeight();
    // set the window size, giving priority to the client area
    //   automatically scales when frames are larger than screen area
    setSizeByClient(framewidth_pixels, frameheight_pixels);

	//A really sleazy hack, but the initial zoom isn't set properly when you actually make this the correct size
	//because setViewport thinks it doesn't have to redraw.
	setViewport(MT_Rectangle(0,framewidth_pixels + 0.1,0,frameheight_pixels + 0.1));
    //setViewport(MT_BlankRectangle);
    lockCurrentViewportAsOriginal();

    // don't unpause yet, but start the timer
    setTimer(25);

    m_pTrackerControlFrame->enableButtons();

    m_sAVIPath = wxT("Camera Capture.");

    onNewCapture();

    return true;

}


bool MT_TrackerFrameBase::setupAVICapture(const char* filename)
{

    // make sure we're paused
    doPause();

    if(!m_pCapture->initCaptureFromFile(filename))
    {
        return false;
    }

    // frame period in msec, set to 0 and override with UI
    int FramePeriod_msec = 0;

    // show the first frame
    m_pCurrentFrame = m_pCapture->getFrame();
    setImage(m_pCurrentFrame);

    // find the capture size
    int framewidth_pixels = m_pCapture->getFrameWidth();
    int frameheight_pixels = m_pCapture->getFrameHeight();
    // set the window size, giving priority to the client area
    //   automatically scales when frames are larger than screen area
    setSizeByClient(framewidth_pixels, frameheight_pixels);
    setViewport(MT_BlankRectangle);
    lockCurrentViewportAsOriginal();
    // if we overrode the frame period, use that, otherwise use the file's natural rate
    if(FramePeriod_msec == 0)
    {
        FramePeriod_msec = m_pCapture->getFramePeriod_msec();
    }

    FramePeriod_msec = 25;


    // don't unpause yet, but start the timer  
    setTimer(FramePeriod_msec);

    // Tell the CoreControlFrame to do whatever it needs to do when loading an avi
    m_pTrackerControlFrame->enableButtons();

    m_sAVIPath = MT_StringToWxString(filename);
    m_sAVIDirectory = wxPathOnly(m_sAVIPath);

    onNewCapture();

    return true;

}

void MT_TrackerFrameBase::doTrackerGLDrawing()
{
    if(m_pTracker)
    {
        drawStatusBar(m_pCapture->getProgressFraction());
        /* typically this means "draw everything" 
           you are free to override this in derived classes. */
        m_pTracker->doGLDrawing(m_lTrackerDrawingFlags); 
    }
}

void MT_TrackerFrameBase::initTrackerFrameData()
{
    /* NOTE: XML doesn't like spaces in tag names */
    m_PathGroup.AddPath(wxT("Movie_Directory"), &m_sAVIDirectory);
    m_PathGroup.AddPath(wxT("ROI_Directory"), &m_sROIDirectory);
    m_PathGroup.AddPath(wxT("Background_Directory"), &m_sBackgroundDirectory);
    m_PathGroup.AddPath(wxT("Data_Directory"), &m_sDataFileDirectory);

    /* initialize the frame capture */
    m_pCapture = new MT_Capture(MT_FC_NO_INIT);

    m_pPreferences->SetDialogIgnore(m_pPreferences->GetIndexByName("Background Color"));

    m_CmdLineParser.AddParam(wxT("Movie File."),
                             wxCMD_LINE_VAL_STRING,
                             wxCMD_LINE_PARAM_OPTIONAL);
    m_CmdLineParser.AddOption(wxT("o"), 
                              wxT("output-datafile"),
                              wxT("Output data file (xdf format)."),
                              wxCMD_LINE_VAL_STRING,
                              wxCMD_LINE_PARAM_OPTIONAL);
    m_CmdLineParser.AddOption(wxT("b"),
                              wxT("background"),
                              wxT("Background image."),
                              wxCMD_LINE_VAL_STRING,
                              wxCMD_LINE_PARAM_OPTIONAL);
    m_CmdLineParser.AddOption(wxT("r"),
                              wxT("roi"),
                              wxT("ROI mask image."),
                              wxCMD_LINE_VAL_STRING,
                              wxCMD_LINE_PARAM_OPTIONAL);
    m_CmdLineParser.AddSwitch(wxT("T"), wxT("Track-now"), wxT("Start tracking right away."));

}

void MT_TrackerFrameBase::writeTrackerXML()
{
    if(m_pTracker)
    {
        for(unsigned int i = 0; i < m_pTracker->getNumDataGroups(); i++)
        {
            WriteDataGroupToXML(&m_XMLSettingsFile, m_pTracker->getDataGroup(i));
        }
    }
}

void MT_TrackerFrameBase::doTrackerStep()
{
    acquireFrames();

    // do tracking if we've started that
    if(m_bTracking)
    {
		runTracker();
        MT_BoundingBox b = m_pTracker->getObjectBoundingBox();
        tellObjectLimits(MT_RectangleFromBoundingBox(b), 0.05);
    }

    if(haveControlFrame() && m_bTracking)
    {
        wxString statustext;

        statustext.Printf(wxT("%d blobs, %3.1f FPS"), m_pTracker->getNFound(), m_pTracker->getFrameRate());

        setControlFrameStatusText(statustext);
    }

    if(m_pCapture && m_pCapture->getIsAtEnd())
    {
        doReset();
    }

}

void MT_TrackerFrameBase::acquireFrames()
{
   // get the frame from the capture object
    m_pCurrentFrame = m_pCapture->getFrame();
}

void MT_TrackerFrameBase::runTracker()
{
	m_pTracker->doTracking(m_pCurrentFrame);
}

bool MT_TrackerFrameBase::startTracking()
{

    bool pausestate = getPaused();
    doPause();

    if(m_bTracking)
    {
        /* do nothing if already tracking... */
    }
    else
    {
        // check to see if we've already initialized a tracker
        if(m_pTracker)
        {
            // we have, so we need to delete it
            delete m_pTracker;
            m_pTracker = NULL;
        }

        initTracker();

        m_pTracker->setSourceName((const char*) m_sAVIPath.mb_str());

        updateMenusOnStartTracker();

        // set the ROI if it's been loaded
        if(m_sROIPath != wxT(""))
        {
            m_pTracker->setROIImage((const char*) m_sROIPath.mb_str());
        }
        // set the BG if it's been loaded
        if(m_sBackgroundPath != wxT(""))
        {
            m_pTracker->setBackgroundImage((const char*) m_sBackgroundPath.mb_str());
        }
        // set the DataFile if it's been selected
        if(m_sDataFilePath != wxT(""))
        {
            m_pTracker->setDataFile((const char*) m_sDataFilePath.mb_str());
        }

        for(unsigned int i = 0; i < m_pTracker->getNumDataGroups(); i++)
        {
            ReadDataGroupFromXML(m_XMLSettingsFile, m_pTracker->getDataGroup(i));
        }

        // if everything was sucessful, toggle the value of the flag
        m_bTracking = true;
        // update the control frame
        m_pTrackerControlFrame->onTrackingToggle(m_bTracking);

    }

    setPause(pausestate);

    // return true if we are now tracking
    return m_bTracking;

}


/********************************************************************/
/*     MT_TrackerFrameBase public member functions                  */
/********************************************************************/

void MT_TrackerFrameBase::handleCommandLineArguments(int argc, wxChar** argv)
{
    wxString v;
    wxString cwd = wxGetCwd();

    if(m_CmdLineParser.Found(wxT("o"), &v))
    {
        MT_GetAbsolutePath(v, &m_sDataFilePath, &m_sDataFileDirectory);
    }

    if(m_CmdLineParser.Found(wxT("b"), &v))
    {
        MT_GetAbsolutePath(v, &m_sBackgroundPath, &m_sBackgroundDirectory);
    }

    if(m_CmdLineParser.Found(wxT("r"), &v))
    {
        MT_GetAbsolutePath(v, &m_sROIPath, &m_sROIDirectory);
    }

    if(m_CmdLineParser.GetParamCount())
    {
        v = m_CmdLineParser.GetParam(0);
        MT_GetAbsolutePath(v, &m_sAVIPath, &m_sAVIDirectory);
        setupAVICapture(m_sAVIPath.mb_str());

        if(m_CmdLineParser.Found(wxT("T")))
        {
            startTracking();
            setPause(false);
        }
    }

    MT_FrameBase::handleCommandLineArguments(argc, argv);
}

void MT_TrackerFrameBase::handleOpenWithFile(const wxString& filename)
{
    if(setupAVICapture((const char*) filename.mb_str()))
    {
    }
    MT_FrameBase::handleOpenWithFile(filename);
}

bool MT_TrackerFrameBase::doKeyboardCallback(wxKeyEvent& event)
{
    return MT_FrameBase::doKeyboardCallback(event);
}

bool MT_TrackerFrameBase::doMouseCallback(wxMouseEvent& event, double viewport_x, double viewport_y)
{
    return MT_FrameBase::doMouseCallback(event, viewport_x, viewport_y);
}

void MT_TrackerFrameBase::onMenuFileCamera(wxCommandEvent& WXUNUSED(event))
{
    if(!setupCameraCapture())
    {
        MT_ShowErrorDialog(this, wxT("Unable to create capture from camera.  Check settings and try again."));
    }
}

void MT_TrackerFrameBase::onMenuFileSelectAVI(wxCommandEvent& WXUNUSED(event))
{
    selectAVIFile();
}

void MT_TrackerFrameBase::onMenuFileSelectROI(wxCommandEvent& WXUNUSED(event))
{
    selectROI();
}

void MT_TrackerFrameBase::onMenuFileCreateBackground(wxCommandEvent& WXUNUSED(event))
{
    createBackground();
}

void MT_TrackerFrameBase::onMenuFileSelectBackground(wxCommandEvent& WXUNUSED(event))
{
    selectBackground();
}

void MT_TrackerFrameBase::onMenuFileSelectDataFile(wxCommandEvent& WXUNUSED(event))
{
    selectDataFile();
}

void MT_TrackerFrameBase::onMenuViewSelect(wxCommandEvent& event)
{

    wxMenuBar* menubar = GetMenuBar();
    int viewmenuid = menubar->FindMenu(wxT("View"));
    if(viewmenuid == wxNOT_FOUND)
    {
        return;
    }

    unsigned int nframes;
    if(!m_pTracker)
    {
        nframes = 1;
    }
    else
    {
        nframes = m_pTracker->getNumProcessedFrames() + 1;
    }

    for(unsigned int i = 0; i < nframes; i++)
    {
        if(menubar->IsChecked(MT_TFB_ID_MENU_VIEW_FRAME00+i))
        {
            m_iView = i;
        }
    }

    enableFrame();

}

void MT_TrackerFrameBase::onMenuTrackerTrain(wxCommandEvent& WXUNUSED(event))
{
    if(m_pTracker && m_pCurrentFrame)
    {
        m_pTracker->doTrain(m_pCurrentFrame);
    }
}

void MT_TrackerFrameBase::onMenuTrackerParams(wxCommandEvent& event)
{
    int parameter_group_id = event.GetId() - MT_TFB_ID_MENU_TRACKER_PARAMS00;
    if(m_pTracker)
    {
        MT_DataGroup* dg = m_pTracker->getDataGroup(parameter_group_id);
        if(dg)
        {
            MT_DataGroupDialog* dlg = new MT_DataGroupDialog(dg, this);
            registerDialogForXML(dlg);
            dlg->Show();      
        }
    }

}

void MT_TrackerFrameBase::onMenuTrackerReports(wxCommandEvent& event)
{
    int report_id = event.GetId() - MT_TFB_ID_MENU_TRACKER_REPORTS00;
    if(m_pTracker)
    {
        MT_DataReport* dr = m_pTracker->getDataReport(report_id);
        if(dr)
        {
            MT_DataReportDialog* dlg = new MT_DataReportDialog(dr, this);
            registerDialogForXML(dlg);
            string key = "TrackerReport";
            char keyid[5];
            sprintf(keyid, "%d", report_id);
            registerUpdatableDialog(dlg, key + keyid);
        }
    }

}

void MT_TrackerFrameBase::onMenuTrackerNote(wxCommandEvent& event)
{
    if(m_pTracker)
    {
        std::string exist_note = "";
        m_pTracker->getNote(&exist_note);
        wxString note = MT_StringToWxString(exist_note);

        MT_XDFNoteDialog* dlg = new MT_XDFNoteDialog(this, &note);
        int result = dlg->ShowModal();
        if(result == wxID_OK)
        {
            m_pTracker->setNote((const char*) note.mb_str());
        }
        dlg->Destroy();
    }
}

void MT_TrackerFrameBase::writeUserXML()
{
    MT_TrackerFrameBase::writeTrackerXML();
}

void MT_TrackerFrameBase::initUserData()
{
    MT_TrackerFrameBase::initTrackerFrameData();
    MT_FrameBase::initUserData();
}

MT_ControlFrameBase* MT_TrackerFrameBase::createControlDialog()
{
    m_pTrackerControlFrame = new MT_TrackerControlFrameBase(this);
    return (MT_ControlFrameBase *) m_pTrackerControlFrame;
}

void MT_TrackerFrameBase::makeFileMenu(wxMenu* file_menu)
{
    file_menu->Append(wxID_OPEN, wxT("&Open Avi..."));
    wxFrame::Connect(wxID_OPEN,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_TrackerFrameBase::onMenuFileSelectAVI));

    file_menu->Append(MT_TFB_ID_MENU_CAPTURE_FROM_CAMERA, wxT("&Camera..."));
    wxFrame::Connect(MT_TFB_ID_MENU_CAPTURE_FROM_CAMERA,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_TrackerFrameBase::onMenuFileCamera));

    file_menu->AppendSeparator();

    file_menu->Append(MT_TFB_ID_MENU_FILE_SELECT_ROI, wxT("Select &ROI..."));
    wxFrame::Connect(MT_TFB_ID_MENU_FILE_SELECT_ROI,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_TrackerFrameBase::onMenuFileSelectROI));

    file_menu->Append(MT_TFB_ID_MENU_FILE_CREATE_BACKGROUND, wxT("&Create Background..."));
    wxFrame::Connect(MT_TFB_ID_MENU_FILE_CREATE_BACKGROUND,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_TrackerFrameBase::onMenuFileCreateBackground));

    file_menu->Append(MT_TFB_ID_MENU_FILE_SELECT_BACKGROUND, wxT("Select &Background..."));
    wxFrame::Connect(MT_TFB_ID_MENU_FILE_SELECT_BACKGROUND,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_TrackerFrameBase::onMenuFileSelectBackground));

    file_menu->AppendSeparator();

    file_menu->Append(MT_TFB_ID_MENU_FILE_SELECT_DATAFILE, wxT("Select Data Output &File..."));
    wxFrame::Connect(MT_TFB_ID_MENU_FILE_SELECT_DATAFILE,
                     wxEVT_COMMAND_MENU_SELECTED,
                     wxCommandEventHandler(MT_TrackerFrameBase::onMenuFileSelectDataFile));

    /* will append preferences and exit to menu */
    MT_FrameBase::makeFileMenu(file_menu);
}

void MT_TrackerFrameBase::makeTrackerMenu(wxMenu* tracker_menu)
{

    tracker_menu->Append(MT_TFB_ID_MENU_TRACKER_TRAIN, wxT("&Train On Current Frame"));
    addDataGroupsToTrackerMenu(tracker_menu);
    addDataReportsToTrackerMenu(tracker_menu);

    tracker_menu->AppendSeparator();

    tracker_menu->Append(MT_TFB_ID_MENU_TRACKER_NOTE, wxT("XDF &Note..."));
    Connect(MT_TFB_ID_MENU_TRACKER_NOTE,
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MT_TrackerFrameBase::onMenuTrackerNote));

}

void MT_TrackerFrameBase::createUserMenus(wxMenuBar* menubar)
{
    wxMenu* tracker_menu = new wxMenu;
    menubar->Append(tracker_menu, wxT("Tracker"));
}

void MT_TrackerFrameBase::doUserGLDrawing()
{
    MT_TrackerFrameBase::doTrackerGLDrawing();
}

void MT_TrackerFrameBase::doUserStep()
{
    MT_TrackerFrameBase::doTrackerStep();
    MT_FrameBase::doUserStep();
}

void MT_TrackerFrameBase::initTracker()
{
    m_pTracker = new MT_TrackerBase(m_pCurrentFrame);
}


MT_XDFNoteDialog::MT_XDFNoteDialog(wxFrame* parent, wxString* note)
    : wxDialog(parent,
               wxID_ANY,
               wxT("Experiment Data File Note"),
               wxDefaultPosition,
               wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE),
      m_pNote(note)
{
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    m_pTextCtrl = new wxTextCtrl(this, 
                                 wxID_ANY,
                                 *note,
                                 wxDefaultPosition,
                                 wxSize(300, 300),
                                 wxTE_MULTILINE | wxTE_LEFT);
    vbox->Add(m_pTextCtrl, 0, wxALL | wxALIGN_LEFT, 10);
    vbox->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_RIGHT, 10);

    Connect(wxID_OK,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(MT_XDFNoteDialog::onOKClicked));

    SetSizerAndFit(vbox);
}

void MT_XDFNoteDialog::onOKClicked(wxCommandEvent& event)
{
    *m_pNote = m_pTextCtrl->GetValue();

    EndModal(wxID_OK);
}
