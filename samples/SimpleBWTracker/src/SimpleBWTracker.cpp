#include "SimpleBWTracker.h"

/* default parameter values */
const double DEFAULT_MIN_BLOB_PERIMETER = 10;
const double DEFAULT_MIN_BLOB_AREA = 10;
const double DEFAULT_MAX_BLOB_PERIMETER = 1000;
const double DEFAULT_MAX_BLOB_AREA = 1000;
const MT_Color DEFAULT_BLOB_COLOR = MT_Red;

/**********************************************************************
 * Tracker Class
 *********************************************************************/

SimpleBWTracker::SimpleBWTracker(IplImage* ProtoFrame)
    : MT_TrackerBase(ProtoFrame),
      m_dMinBlobPerimeter(         DEFAULT_MIN_BLOB_PERIMETER  ),
      m_dMinBlobArea(              DEFAULT_MIN_BLOB_AREA       ),
      m_dMaxBlobPerimeter(         DEFAULT_MAX_BLOB_PERIMETER  ),
      m_dMaxBlobArea(              DEFAULT_MAX_BLOB_AREA       ),
      m_bDrawBlobs(                true                        ),
      m_BlobColor(                 DEFAULT_BLOB_COLOR          ),
      m_dDt(0),
      m_iFrameCounter(0)
{
    doInit(ProtoFrame);
}

void SimpleBWTracker::doInit(IplImage* ProtoFrame)
{
    /* Not using the built-in tracked objects functions */
    m_pTrackedObjects = NULL;

    m_pGSFrame = NULL;
    m_pDiffFrame = NULL;
    m_pThreshFrame = NULL;

    m_iNChannelsPerFrame = ProtoFrame->nChannels;

    /* This initializes variables to safe values and
     * calls doTrain to set the background frame,
     * frame sizes, and calls createFrames(). */
    MT_TrackerBase::doInit(ProtoFrame);

    /* sets up the frames that are available in the "view" menu */
    m_pTrackerFrameGroup = new MT_TrackerFrameGroup();
    m_pTrackerFrameGroup->pushFrame(&m_pDiffFrame,      "Diff Frame");
    m_pTrackerFrameGroup->pushFrame(&m_pThreshFrame,    "Threshold Frame");

    /* set up the parameter groups for parameter modification, etc. */
    /* first group is for blob tracking parameters */
    MT_DataGroup* dg_blob = new MT_DataGroup("Blob Tracking Parameters");
    dg_blob->AddUInt("Difference Threshold", /* parameter name */
                     &m_iBlobValThresh,      /* pointer to variable */
                     MT_DATA_READWRITE,      /* read-only or not */
                     0,                      /* minimum value */
                     255);                   /* maximum value */
    dg_blob->AddDouble("Minimum Blob Perimeter", &m_dMinBlobPerimeter);
    dg_blob->AddDouble("Minimum Blob Area",      &m_dMinBlobArea);
    dg_blob->AddDouble("Maximum Blob Perimeter", &m_dMaxBlobPerimeter);
    dg_blob->AddDouble("Maximum Blob Area",      &m_dMaxBlobArea);

    /* second group for drawing parameters */
    MT_DataGroup* dg_draw = new MT_DataGroup("Drawing Parameters");
    dg_draw->AddBool("Draw Blob",   &m_bDrawBlobs);
    dg_draw->AddColor("Blob Color", &m_BlobColor);

    /* now stuff the parameter groups into m_vDataGroups, which
     * MT_TrackerBase will automagically report to the GUI */
    m_vDataGroups.resize(0);
    m_vDataGroups.push_back(dg_blob);
    m_vDataGroups.push_back(dg_draw);

    /* Set up data reports (tables of output data visible to the user */
    MT_DataReport* dr_blob = new MT_DataReport("Blob Tracker Blob Data");
    dr_blob->AddDouble("X [px]",            &m_vdBlobXs);
    dr_blob->AddDouble("Y [px]",            &m_vdBlobYs);
    dr_blob->AddDouble("Perimeter [px]",    &m_vdBlobPs);
    dr_blob->AddDouble("Area [px]",         &m_vdBlobAs);
    dr_blob->AddDouble("Orientation [rad]", &m_vdBlobOs);

    m_vDataReports.resize(0);
    m_vDataReports.push_back(dr_blob);

}

void SimpleBWTracker::createFrames()
{
    /* this makes sure that the BG_frame is created */
    MT_TrackerBase::createFrames();

    /* just for convenience in the following */
    CvSize framesize = cvSize(FrameWidth, FrameHeight);

    /* Safely allocate space for the frames we'll need */
    if(m_pGSFrame)
    {
        cvReleaseImage(&m_pGSFrame);
    }
    m_pGSFrame = cvCreateImage(framesize, IPL_DEPTH_8U, 1);

    if(m_pDiffFrame)
    {
        cvReleaseImage(&m_pDiffFrame);
    }
    m_pDiffFrame = cvCreateImage(framesize, IPL_DEPTH_8U, 1);

    if(m_pThreshFrame)
    {
        cvReleaseImage(&m_pThreshFrame);
    }
    m_pThreshFrame = cvCreateImage(framesize, IPL_DEPTH_8U, 1);

} /* endof createFrames */

void SimpleBWTracker::releaseFrames()
{
    /* Safely deallocate frames used by this tracker */
    if(m_pGSFrame)
    {
        cvReleaseImage(&m_pGSFrame);
    }
    if(m_pDiffFrame)
    {
        cvReleaseImage(&m_pDiffFrame);
    }
    if(m_pThreshFrame)
    {
        cvReleaseImage(&m_pThreshFrame);
    }
} /* endof releaseFrames */

/* Main tracking function - gets called by MT_TrackerFrameBase every
 * time step when the application is not paused. */
void SimpleBWTracker::doTracking(IplImage* frame)
{

    /* time-keeping, if necessary
     * NOTE this is not necessary for keeping track of frame rate */
    static double t_prev = MT_getTimeSec();
    double t_now = MT_getTimeSec();
    m_dDt = t_now - t_prev;
    t_prev = t_now;

    /* keeping track of the frame number, if necessary */
    m_iFrameCounter++;

    /* Convert frame to grayscale, if necessary */
    if(frame->nChannels == 3)
    {
        cvCvtColor(frame, m_pGSFrame, CV_BGR2GRAY);
    }
    else
    {
        cvCopy(frame, m_pGSFrame);
    }

    /* call worker functions - implemented this way for readability */
    doImageProcessing();
    doSegmentation();

    /* convert blob results to std::vectors */
    m_vdBlobXs = m_Blobs.GetSTLResult(MT_CBlobGetXCenterOfMass());
    m_vdBlobYs = m_Blobs.GetSTLResult(MT_CBlobGetYCenterOfMass());
    m_vdBlobPs = m_Blobs.GetSTLResult(CBlobGetPerimeter());
    m_vdBlobAs = m_Blobs.GetSTLResult(CBlobGetArea());
    m_vdBlobOs = m_Blobs.GetSTLResult(MT_CBlobGetHeadOrientation());

    /* adjust for the image origin vs. coordinate origin */
    for(unsigned int i = 0; i < m_iNBlobsFound; i++)
    {
        m_vdBlobYs[i] = FrameHeight - m_vdBlobYs[i];
    }
    
}

void SimpleBWTracker::doImageProcessing()
{
    /* perform sign-aware background subtraction */
    cvCmp(BG_frame, m_pGSFrame, m_pThreshFrame, CV_CMP_GT);
    cvSub(BG_frame, m_pGSFrame, m_pDiffFrame);
    cvAnd(m_pDiffFrame, m_pThreshFrame, m_pDiffFrame);

    /* apply the ROI mask if available */
    if(ROI_frame)
    {
        cvAnd(m_pDiffFrame, ROI_frame, m_pDiffFrame);
    }

    /* threshold */
    cvThreshold(m_pDiffFrame,
                m_pThreshFrame,
                m_iBlobValThresh,
                255.0,
                CV_THRESH_BINARY);

}

void SimpleBWTracker::doSegmentation()
{

    /* using CBlobsLib */
    m_Blobs = CBlobResult(m_pThreshFrame, NULL, 0);
  
    /* Blob filtering - by size */
    m_Blobs.Filter(m_Blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, m_dMinBlobArea);
    m_Blobs.Filter(m_Blobs, B_INCLUDE, CBlobGetArea(), B_LESS, m_dMaxBlobArea);
    /* Certain conditions tend to generate a blob with this property,
     * so we delete it */
    m_Blobs.Filter(m_Blobs,
                   B_EXCLUDE,
                   CBlobGetXCenter(),
                   B_EQUAL,
                   0.5*((double) m_pThreshFrame->width) + 0.5 );
  
    /* number of blobs found */
    m_iNBlobsFound = m_Blobs.GetNumBlobs();

}


void SimpleBWTracker::doGLDrawing(int flags)
{
    MT_R3 blobcenter;
    blobcenter.setz(0);

    if(m_bDrawBlobs)
    {
        for(unsigned int i = 0; i < m_vdBlobXs.size(); i++)
        {
            blobcenter.setx(m_vdBlobXs[i]);
            blobcenter.sety(m_vdBlobYs[i]);

            MT_DrawArrow( blobcenter,
                          10.0,
                          MT_DEG2RAD*m_vdBlobOs[i],
                          MT_Blue,
                          1.0);
        }
    }

}

/**********************************************************************
 * GUI Frame Class
 *********************************************************************/

SimpleBWTrackerFrame::SimpleBWTrackerFrame(wxFrame* parent,
                               wxWindowID id,
                               const wxString& title, 
                               const wxPoint& pos, 
                               const wxSize& size,     
                               long style)
: MT_TrackerFrameBase(parent, id, title, pos, size, style)
{
    /* nothing else to do here */
}

void SimpleBWTrackerFrame::initUserData()
{
    initTrackerFrameData();
}

void SimpleBWTrackerFrame::initTracker()
{

    m_pSimpleBWTracker = new SimpleBWTracker(m_pCurrentFrame);
    m_pTracker = (MT_TrackerBase *) m_pSimpleBWTracker;

    /* note - do NOT call MT_TrackerBase::initTracker, which is
     * a placeholder function that sets m_pTracker to NULL! */
}

/**********************************************************************
 * GUI App Class
 *********************************************************************/

IMPLEMENT_APP(SimpleBWTrackerApp)
