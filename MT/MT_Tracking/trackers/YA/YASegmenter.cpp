#include "YASegmenter.h"
#include "MT/MT_Core/gl/glSupport.h"  // for blob drawing

#include <algorithm>  // for sort and unique

BlobberParameters::BlobberParameters(int* val_thresh_low, 
                                     int* val_thresh_high, 
                                     int* area_thresh_low, 
                                     int* area_thresh_high, 
                                     bool* testbool, 
                                     double* testdouble)
  : MT_DataGroup("Blobber Parameters")
{
  
    AddInt("Diff Thresh Low", val_thresh_low, MT_DATA_READWRITE, 0, 512);
    AddInt("Diff Thresh High", val_thresh_high, MT_DATA_READWRITE, 1, 512);
    AddInt("Area Thresh Low", area_thresh_low, MT_DATA_READWRITE, 0);
    AddInt("Area Thresh High", area_thresh_high, MT_DATA_READWRITE, 0);
    AddBool("Test Bool", testbool);
    AddDouble("Test Double", testdouble);
  
}

BlobberFrameGroup::BlobberFrameGroup(IplImage** diff_frame, IplImage** thresh_frame)
{
  
    StandardFrameGroupInit(2);

    m_vFrameNames[0] = "Diff Frame";
    m_vFrameNames[1] = "Threshold Frame";
  
    m_vFrames[0] = diff_frame;
    m_vFrames[1] = thresh_frame;
  
}

BlobInfoReport::BlobInfoReport(std::vector<int>* indexes, 
                               std::vector<double>* Xs, 
                               std::vector<double>* Ys, 
                               std::vector<double>* Areas, 
                               std::vector<double>* Orientations)
  : MT_DataReport("Blob Info")
{
  
    AddInt("Index", indexes);
    AddDouble("X [px]", Xs);
    AddDouble("Y [px]", Ys);
    AddDouble("Area [px]", Areas);
    AddDouble("Orientation [deg]", Orientations);
  
}

/*************************************************************/
# pragma mark -
# pragma mark Segmenter

Segmenter::Segmenter(IplImage* ProtoFrame)
  : m_Blobber(false)
{  
  
    Init(ProtoFrame);

}

void Segmenter::Init(IplImage* ProtoFrame)
{
  
    /* frame group initialization */
    m_pTrackerFrameGroup = new BlobberFrameGroup(&diff_frame, &thresh_frame);
  
    blob_val_thresh_low = RT_MIN_BLOB_VAL;
    blob_val_thresh_high = RT_MAX_BLOB_VAL;
    blob_area_thresh_low = RT_MIN_BLOB_SIZE;
    blob_area_thresh_high = RT_MAX_BLOB_SIZE;
  
    test_bool = true;
    test_double = 3.1415926;
  
    m_vDataGroups.resize(0);
    m_vDataGroups.push_back(new BlobberParameters(&blob_val_thresh_low, &blob_val_thresh_high, &blob_area_thresh_low, &blob_area_thresh_high, &test_bool, &test_double));
  
    BlobIndexes.resize(0);
    XBlobs.resize(0);
    YBlobs.resize(0);
    ABlobs.resize(0);
    OBlobs.resize(0);
  
    m_vDataReports.resize(0);
    m_vDataReports.push_back(new BlobInfoReport(&BlobIndexes, &XBlobs, &YBlobs, &ABlobs, &OBlobs));
                          
    BG_frame = 0;
    GS_frame = 0;
    diff_frame = 0;
    thresh_frame = 0;
    ROI_frame = 0;
      
    frame_counter = 0;
    NFound = 0;
  
    if(ProtoFrame)
    {
        train(ProtoFrame);
    }
    else
    {
        FrameWidth = 0;
        FrameHeight = 0;
    }
  
    //m_pBlobFile = NULL;
  
    m_YABlobs.resize(0);  
  
}

Segmenter::~Segmenter()
{
  
    //delete m_pBlobFile;
  
    if(BG_frame)
    {
        cvReleaseImage(&BG_frame);
        cvReleaseImage(&GS_frame);
        cvReleaseImage(&diff_frame);
        cvReleaseImage(&thresh_frame);
    }
    /* note ~TrackerCore will release ROI_frame */
}

void Segmenter::train(IplImage* frame)
{
  
    // TODO Should really check this against the ROI frame if it exists
  
    CvSize framesize = cvSize(frame->width,frame->height);
  
    FrameWidth = frame->width;
    FrameHeight = frame->height;  
  
    createFrames();
  
    if(frame->nChannels == 3)
    {
        cvCvtColor(frame, BG_frame, CV_BGR2GRAY);
    } else {
        cvCopy(frame, BG_frame);
    }
  
}

void Segmenter::createFrames()
{
  
    CvSize framesize = cvSize(FrameWidth, FrameHeight);
  
    if(BG_frame)
    {
        cvReleaseImage(&BG_frame);
        cvReleaseImage(&GS_frame);
        cvReleaseImage(&diff_frame);
        cvReleaseImage(&thresh_frame);
    }
  
    BG_frame = cvCreateImage(framesize, IPL_DEPTH_8U, 1);
    GS_frame = cvCreateImage(framesize, IPL_DEPTH_8U,1);
    diff_frame = cvCreateImage(framesize, IPL_DEPTH_8U,1);
    thresh_frame = cvCreateImage(framesize, IPL_DEPTH_8U,1);
  
}

void Segmenter::doMeasurement()
{
 
    // get the blob centroids and areas
    XBlobs = blobs.GetSTLResult( MT_CBlobGetXCenterOfMass() );
    YBlobs = blobs.GetSTLResult( MT_CBlobGetYCenterOfMass() );
    ABlobs = blobs.GetSTLResult( CBlobGetArea() );
    // translate from image coordinates
    for(unsigned int k = 0; k < NFound; k++)
    {
        YBlobs[k] = FrameHeight - YBlobs[k];
    }
  
}

void Segmenter::setBlobFile(const char* blobfilename, const char* description)
{
  
    /*if(!m_pBlobFile)
    {
        m_pBlobFile = new BlobFile(blobfilename, description, FrameWidth, FrameHeight);
    }*/
  
}

void Segmenter::setDiffThreshLow(int newThresh)
{
    blob_val_thresh_low = newThresh;
}

void Segmenter::setAreaThreshLow(int newThresh)
{
    blob_area_thresh_low = newThresh;
}

void Segmenter::setAreaThreshHigh(int newThresh)
{
    blob_area_thresh_high = newThresh;
}

int Segmenter::getDiffThreshLow()
{
    return blob_val_thresh_low;
}

int Segmenter::getAreaThreshLow()
{
    return blob_area_thresh_low;
}

int Segmenter::getAreaThreshHigh()
{
    return blob_area_thresh_high;
}

void Segmenter::doImageProcessing()
{
  
    // Find regions that are darker than the background
    //   - first, mark pixels that are darker into a binary image
    //    (note we hijack the thresh_frame for the result, since it is temporary)
    cvCmp(BG_frame,GS_frame,thresh_frame,CV_CMP_GT);
    //   - second, find the absolute value of the difference between the images
    cvAbsDiff(BG_frame,GS_frame,diff_frame);
    //   - then, AND with the binary image to keep the difference only for darker pixels
    cvAnd(diff_frame,thresh_frame,diff_frame);
  
    if(ROI_frame)   // only if the ROI has been specified
    {
        printf("Apply ROI frame\n");
        //   - also AND with the ROI to make sure that blobs are only found inside the tank
        cvAnd(diff_frame,ROI_frame,diff_frame);
    }
  
    // Find the threshold of the difference
    cvThreshold(diff_frame, thresh_frame, blob_val_thresh_low, 255.0, CV_THRESH_BINARY);
  
}

void Segmenter::doSegmentation()
{
  
    // Use the blob finding algorithm to find the blobs based on trhesholding
    //   the difference frame
    if(ROI_frame)
    {
        blobs = CBlobResult(thresh_frame, ROI_frame, 0);
    } else {
        blobs = CBlobResult(thresh_frame, RT_NOMASK, 0);
    }
  
#ifdef TRACKER_VERBOSE  
    for(unsigned int i = 0; i < blobs.GetNumBlobs(); i++)
    {
        printf("Unfiltered Blob %d, Area = %f, at (%f, %f) with orientation %f\n",i, 
               blobs.GetNumber( i, CBlobGetArea() ),
               blobs.GetNumber( i, CBlobGetXCenter() ), 
               blobs.GetNumber( i, CBlobGetYCenter() ),  
               blobs.GetNumber( i, MT_CBlobGetHeadOrientation() ) );
    }
#endif
  
    // Get rid of blobs that are either too big or too small
    blobs.Filter(blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, blob_area_thresh_low);
    blobs.Filter(blobs, B_INCLUDE, CBlobGetArea(), B_LESS, blob_area_thresh_high);
    // Certain conditions tend to generate a blob with this property - delete it
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetXCenter(), B_EQUAL, 0.5*((double) thresh_frame->width) + 0.5 );
  
#ifdef TRACKER_VERBOSE  
    for(unsigned int i = 0; i < blobs.GetNumBlobs(); i++)
    {
        printf("Filtered Blob %d, Area = %f, at (%f, %f) with orientation %f\n",i, 
               blobs.GetNumber( i, CBlobGetArea() ),
               blobs.GetNumber( i, CBlobGetXCenter() ), 
               blobs.GetNumber( i, CBlobGetYCenter() ),  
               blobs.GetNumber( i, MT_CBlobGetHeadOrientation() ) );
    }
#endif
  
    // How many blobs did we find on first pass?
    NFound = blobs.GetNumBlobs();  

}

void Segmenter::doMeasurementWithYABlobs()
{
  
    BlobIndexes.resize(NFound);
    XBlobs.resize(NFound);
    YBlobs.resize(NFound);
    ABlobs.resize(NFound);
    OBlobs.resize(NFound);
  
    for(unsigned int i = 0; i < NFound; i++)
    {
        BlobIndexes[i] = i;
        XBlobs[i] = m_YABlobs[i].COMx;
        YBlobs[i] = m_YABlobs[i].COMy;
        ABlobs[i] = m_YABlobs[i].area;
        OBlobs[i] = m_YABlobs[i].orientation;
    }
  
}

// Main Tracking Function - this is the main workhorse.
void Segmenter::doTracking(IplImage* frame)
{
    //printf("thresh low %d\n area low %d\n bool %d\n double %f\n", blob_val_thresh_low, blob_area_thresh_low, test_bool, test_double);
  
    static double t_previous = MT_getTimeSec();
  
    double dt;
    double t_now = MT_getTimeSec();
  
    dt = t_now - t_previous;  /// TODO for an AVI, dt should be constant
  
    //dt = 0.04;
    t_previous = t_now;
  
    frame_counter++;
  
    // Keep a copy of the original frame pointer for display purposes
    org_frame = frame;
    
    // Convert to grayscale if necessary
	if(frame->nChannels == 3)
    {
        cvCvtColor(frame, GS_frame, CV_BGR2GRAY);
    } else {
        cvCopy(frame, GS_frame);
    }
  
    double t0 = MT_getTimeSec();

    // image operations to extract regions where fish are likely
    //  e.g. background substitution, thresholding
    doImageProcessing();
  
    // image segmentation - i.e. blob detection
    //DoSegmentation();  
    // data association and assignment
    //DoMeasurement();
  
    double t1 = MT_getTimeSec();
    //printf("Image Processing %f\n", t1-t0);
  
    t0 = MT_getTimeSec();

    m_YABlobs = m_Blobber.FindBlobs(thresh_frame, 10, blob_area_thresh_low);
    t1 = MT_getTimeSec();
    //printf("Blobbing %f\n", t1-t0);
    NFound = m_YABlobs.size();
    doMeasurementWithYABlobs();

    /*if(m_pBlobFile)
    {
        m_pBlobFile->WriteBlobs(frame_counter, dt, blobs);
    }*/
    
}

void Segmenter::glDraw(bool DrawBlobs)
{
  
    MT_R3 blobcenter;
  
    if(DrawBlobs)
    {
        /*   for(int i = 0; i < blobs.GetNumBlobs(); i++)
             {
      
             blobcenter.setx( blobs.GetNumber(i, MT_CBlobGetXCenterOfMass()));
             blobcenter.sety( FrameHeight - blobs.GetNumber(i, MT_CBlobGetYCenterOfMass()));
             blobcenter.setz( 0 );
      
             DrawArrow( blobcenter,
             1.5*blobs.GetNumber(i, CBlobGetMajorAxisLength()),
             MT_DEG2RAD*blobs.GetNumber(i, MT_CBlobGetHeadOrientation()),
             MT_Green,
             1.0 // fixed arrow width
             );    
             }*/
    
        for(int i = 0; i < m_YABlobs.size(); i++)
        {
      
            blobcenter.setx(m_YABlobs[i].COMx);
            blobcenter.sety(FrameHeight - m_YABlobs[i].COMy);
            blobcenter.setz( 0 );
			fprintf(stdout, "I'm drawing to (%f, %f)\n", m_YABlobs[i].COMx, FrameHeight - m_YABlobs[i].COMy);
            MT_DrawArrow( MT_R3(3, 3, 0)/*blobcenter*/,
                       250/*1.5*m_YABlobs[i].area*/,
                       MT_DEG2RAD*m_YABlobs[i].orientation,
                       MT_Red,
                       1.0 // fixed arrow width
                );    
        }
    }
  
}

void Segmenter::doGLDrawing(int flags)
{
  
    glDraw(flags > 0);
  
}
