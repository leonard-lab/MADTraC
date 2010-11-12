/*
 *  GYSegmenter.cpp
 *  MADTraC
 *
 *  A class for locating elliptical-shaped blobs in an image
 *  using a mixture of Gaussians model when multiple blobs
 *  overlap. Based in part on code by Dan Swain and Yael
 *  Katz.
 *
 *  Created by George Young on 5/10/09.
 *
 */

#include "GYSegmenter.h"

#include "MT/MT_Core/support/mathsupport.h"
#include "MT/MT_Core/primitives/Matrix.h"
#include "MT/MT_Core/gl/glSupport.h"  // for blob drawing

GYBlobberParameters::GYBlobberParameters(int* val_thresh_low, 
                                     int* area_thresh_low, 
                                     int* area_thresh_high)
  : MT_DataGroup("Blobber Parameters")
{

    AddInt("Diff Thresh Low", val_thresh_low, MT_DATA_READWRITE, 0, 512);
    AddInt("Area Thresh Low", area_thresh_low, MT_DATA_READWRITE, 0);
    AddInt("Area Thresh High", area_thresh_high, MT_DATA_READWRITE, 0);

}

GYBlobberFrameGroup::GYBlobberFrameGroup(IplImage** diff_frame, IplImage** thresh_frame)
{

    StandardFrameGroupInit(2);

    m_vFrameNames[0] = "Diff Frame";
    m_vFrameNames[1] = "Threshold Frame";

    m_vFrames[0] = diff_frame;
    m_vFrames[1] = thresh_frame;

}

GYBlobInfoReport::GYBlobInfoReport(std::vector<int>* indexes, 
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


GYBlobberDrawingParameters::GYBlobberDrawingParameters(bool* draw_arrows, 
                                                   bool* draw_ellipses, 
                                                   bool* draw_search_rect,
                                                   double* arrow_length,
                                                   MT_Color* arrow_color,
                                                   MT_Color* ellipse_color)
  : MT_DataGroup("Blobber Drawing Parameters")
{
    AddBool("Draw Arrows", draw_arrows, MT_DATA_READWRITE);
    AddBool("Draw Ellipses", draw_ellipses, MT_DATA_READWRITE);
    AddBool("Draw Search Rectangle", draw_search_rect, MT_DATA_READWRITE);
    AddDouble("Arrow Length", arrow_length, MT_DATA_READWRITE);
    AddColor("Arrow Color", arrow_color, MT_DATA_READWRITE);
    AddColor("Ellipse Color", ellipse_color, MT_DATA_READWRITE);
}

// Internal Functions for blob finding and segmentation
static double TraceContour(int start_x, int start_y, int first_x, int first_y, int width, int height, int* labelarray, IplImage* frame);

static CvPoint GYTracer(IplImage* frame, CvPoint currentpoint, int* labelarray, int first_x, int first_y, int width, int height, int startloc);

static int IndexMaxDiff(double* array1, int* array2, int arraylength);

static int IndexMaxDiff(int* array1, double* array2, int arraylength);

static int IndexMin(double* array, int arraylength);

static int ipow(int& a, int& n);



GYSegmenter::GYSegmenter(IplImage* ProtoFrame)
{  
    doInit(ProtoFrame);
}

void GYSegmenter::doInit(IplImage* ProtoFrame)
{
    m_pTrackedObjects = NULL;

    m_pTrackerFrameGroup = new GYBlobberFrameGroup(&m_pDiff_frame, &m_pThresh_frame);

    m_pOrg_frame = 0;
    m_pBG_frame = 0;
    m_pGS_frame = 0;
    m_pDiff_frame = 0;
    m_pThresh_frame = 0;
    m_pROI_frame = 0;

    m_dFrameRate = 0.0;
    m_dAverageFrameRate = 0.0;

    m_iBlob_val_thresh = RT_MIN_BLOB_VAL;
    m_iBlob_area_thresh_low = RT_MIN_BLOB_SIZE;
    m_iBlob_area_thresh_high = RT_MAX_BLOB_SIZE;
    m_iNobj = RT_NUM_OBJECTS;

    m_bDrawArrows = true;
    m_bDrawEllipses = false;
    m_bDrawSearchRect = true;
    m_dArrowLength = 20.0;
    m_ArrowColor = MT_Green;
    m_EllipseColor = MT_Blue;

    m_vDataGroups.resize(0);
    m_vDataGroups.push_back(
        new GYBlobberParameters(
            &m_iBlob_val_thresh, 
            &m_iBlob_area_thresh_low, 
            &m_iBlob_area_thresh_high));
    m_vDataGroups.push_back(
        new GYBlobberDrawingParameters(
            &m_bDrawArrows, 
            &m_bDrawEllipses, 
            &m_bDrawSearchRect,
            &m_dArrowLength,
            &m_ArrowColor,
            &m_EllipseColor));

    BlobIndexes.resize(0);
    XBlobs.resize(0);
    YBlobs.resize(0);
    ABlobs.resize(0);
    OBlobs.resize(0);

    m_vDataReports.resize(0);
    m_vDataReports.push_back(new GYBlobInfoReport(&BlobIndexes, &XBlobs, &YBlobs, &ABlobs, &OBlobs));

    m_iFrame_counter = 0;

    doTrain(ProtoFrame);

    //m_pBlobFile = NULL;

    m_RawBlobData.resize(0);
    m_CurrentBlobs.resize(m_iNobj);
    m_OldBlobs.resize(m_iNobj);

    m_bHasHistory = false;

}       // end function

void GYSegmenter::initDataFile()
{
    m_XDF.addDataStream("X COM", "px.dat");
    m_XDF.addDataStream("Y COM", "py.dat");
    m_XDF.addDataStream("Area", "a.dat");
    m_XDF.addDataStream("Orientation", "o.dat");

    /* adds the parameter groups, etc */
    MT_TrackerBase::initDataFile();
}

void GYSegmenter::writeData()
{
    m_XDF.writeData("X COM", XBlobs);
    m_XDF.writeData("Y COM", YBlobs);
    m_XDF.writeData("Area", ABlobs);
    m_XDF.writeData("Orientation", OBlobs);
}

GYSegmenter::~GYSegmenter()
{
    /*if (m_pBlobFile)
      {
      delete m_pBlobFile;
      }*/

    if(m_pBG_frame)
    {
        /* NOTE we don't release m_pOrg_frame because it's a pointer
         * to a frame that the Capture owns. */
        cvReleaseImage(&m_pBG_frame);
        cvReleaseImage(&m_pGS_frame);
        cvReleaseImage(&m_pDiff_frame);
        cvReleaseImage(&m_pThresh_frame);
    }

    if(m_pROI_frame)
    {
        cvReleaseImage(&m_pROI_frame);
    }
}       // end function

void GYSegmenter::createFrames()
{

    CvSize framesize = cvSize(m_iFrameWidth, m_iFrameHeight);

    if(BG_frame)
    {
        cvReleaseImage(&m_pBG_frame);
        cvReleaseImage(&m_pGS_frame);
        cvReleaseImage(&m_pDiff_frame);
        cvReleaseImage(&m_pThresh_frame);
    }

    m_pBG_frame = cvCreateImage(framesize, IPL_DEPTH_8U, 1);
    cvSet(m_pBG_frame, cvRealScalar(0.0), NULL);
    m_pGS_frame = cvCreateImage(framesize, IPL_DEPTH_8U,1);
    cvSet(m_pGS_frame, cvRealScalar(0.0), NULL);
    m_pDiff_frame = cvCreateImage(framesize, IPL_DEPTH_8U,1);
    cvSet(m_pDiff_frame, cvRealScalar(0.0), NULL);
    m_pThresh_frame = cvCreateImage(framesize, IPL_DEPTH_8U,1);
    cvSet(m_pThresh_frame, cvRealScalar(0.0), NULL);

}

void GYSegmenter::doTrain(IplImage* frame)
{

    CvSize framesize = cvSize(frame->width,frame->height);

    if(m_pROI_frame)
    {
        if((m_pROI_frame->width != framesize.width) || (m_pROI_frame->height != framesize.height))
        {
            cvReleaseImage(&m_pROI_frame);
            m_pROI_frame = 0;
        }
    }

    m_iFrameWidth = frame->width;
    m_iFrameHeight = frame->height;

    m_SearchArea = cvRect(0, 0, m_iFrameWidth, m_iFrameHeight);

    createFrames();

    if(frame->nChannels == 3)
    {
        cvCvtColor(frame, m_pBG_frame, CV_BGR2GRAY);
    } else {
        cvCopy(frame, m_pBG_frame);
    }

}       // end function



void GYSegmenter::setBlobFile(const char* blobfilename, const char* description)
{
    /*if(m_pBlobFile)
      {
      delete m_pBlobFile;
      }*/

    //m_pBlobFile = new GYBlobFile(blobfilename, description, m_iFrameWidth, m_iFrameHeight);
}       // end function

void GYSegmenter::setNumObjects(int numobj)
{
    if(numobj <= 0)
    {
        numobj = 1;
    }

    m_iNobj = numobj;

    m_CurrentBlobs.resize(m_iNobj);
    m_OldBlobs.resize(m_iNobj);

    m_bHasHistory = false;
}       // end function


int GYSegmenter::getNumObjects()
{
    return m_iNobj;
}       // end function


int GYSegmenter::getNumFrames()
{
    return m_iFrame_counter;
}       // end function


double GYSegmenter::getFrameRate()
{
    return m_dFrameRate;
}       // end function


double GYSegmenter::getAveFrameRate()
{
    return m_dAverageFrameRate;
}       // end function


IplImage* GYSegmenter::getBG_frame()
{
    return m_pBG_frame;
}       // end function


IplImage* GYSegmenter::getGS_frame()
{
    return m_pGS_frame;
}       // end function


IplImage* GYSegmenter::getDiff_frame()
{
    return m_pDiff_frame;
}       // end function


IplImage* GYSegmenter::getThresh_frame()
{
    return m_pThresh_frame;
}       // end function


IplImage* GYSegmenter::getROI_frame()
{
    return m_pROI_frame;
}       // end function


void GYSegmenter::doImageProcessing()
{
    // Select the region to search for the raw blobs - the region containing the last
    // measured blob positions and up to 60 pixels in every direction. Note: the 60 
    // pixel value might not be appropriate for other applications
    if (m_bHasHistory)
    {
        // Find a bounding box around the current locations of all the blobs
        unsigned int i;
        double xmin, xmax, ymin, ymax;

        xmin = xmax = m_CurrentBlobs[0].m_dXCentre;
        ymin = ymax = m_CurrentBlobs[0].m_dYCentre;

        for (i = 1 ; i < m_CurrentBlobs.size() ; i++)
        {
            if (m_CurrentBlobs[i].m_dXCentre < xmin)
            {
                xmin = m_CurrentBlobs[i].m_dXCentre;
            }
            else if (m_CurrentBlobs[i].m_dXCentre > xmax)
            {
                xmax = m_CurrentBlobs[i].m_dXCentre;
            }
            if (m_CurrentBlobs[i].m_dYCentre < ymin)
            {
                ymin = m_CurrentBlobs[i].m_dYCentre;
            }
            else if (m_CurrentBlobs[i].m_dYCentre > ymax)
            {
                ymax = m_CurrentBlobs[i].m_dYCentre;
            }
        }               // end for (i = 1 ; i < m_CurrentBlobs.size() ; i++)

        // Increase the extents of the boundary by 60 pixels in each direction
        xmin -= 60;
        xmax += 60;
        ymin -= 60;
        ymax += 60;

        // Clip the search area to the bounds of the image
        int xmin_i, xmax_i, ymin_i, ymax_i;

        if (xmin < 0)
        {
            xmin_i = 0;
        }
        else
        {
            xmin_i = (int) xmin;
        }

        if (xmax > m_iFrameWidth - 1)
        {
            xmax_i = m_iFrameWidth - 1;
        }
        else
        {
            xmax_i = (int) xmax;
        }

        if (ymin < 0)
        {
            ymin_i = 0;
        }
        else
        {
            ymin_i = (int) ymin;
        }

        if (ymax > m_iFrameHeight - 1)
        {
            ymax_i = m_iFrameHeight - 1;
        }
        else
        {
            ymax_i = (int) ymax;
        }

        m_SearchArea.x = xmin_i;
        m_SearchArea.width = xmax_i - xmin_i + 1;
        m_SearchArea.y = ymin_i;
        m_SearchArea.height = ymax_i - ymin_i + 1;
    }           // end if (m_bHasHistory)
    else        // No currently known blobs, so set the search area to the whole image
    {
        m_SearchArea.x = 0;
        m_SearchArea.width = m_iFrameWidth;
        m_SearchArea.y = 0;
        m_SearchArea.height = m_iFrameHeight;
    }           // end else

    cvSetImageROI(m_pBG_frame, m_SearchArea);
    cvSetImageROI(m_pGS_frame, m_SearchArea);
    cvSetImageROI(m_pThresh_frame, m_SearchArea);
    cvSetImageROI(m_pDiff_frame, m_SearchArea);
    // Find regions that are darker than the background
    //   - first, mark pixels that are darker into a binary image
    //    (note we hijack the thresh_frame for the result, since it is temporary)
    cvCmp(m_pBG_frame,m_pGS_frame,m_pThresh_frame,CV_CMP_GT);
    /* //   - second, find the absolute value of the difference between the images
       cvAbsDiff(m_pBG_frame,m_pGS_frame,m_pDiff_frame); */
    //   - second, find the difference between the background and the frame. Areas of the
    //     frame that are darker than the background will noe be light. Areas of the frame
    //     that are lighter than the background will be black.
    cvSub(m_pBG_frame, m_pGS_frame, m_pDiff_frame);
    //   - then, AND with the binary image to keep the difference only for darker pixels
    cvAnd(m_pDiff_frame,m_pThresh_frame,m_pDiff_frame);

    if(m_pROI_frame)   // only if the ROI has been specified
    {
        //   - also AND with the ROI to make sure that blobs are only found inside the tank
        cvSetImageROI(m_pROI_frame, m_SearchArea);
        cvAnd(m_pDiff_frame,m_pROI_frame,m_pDiff_frame);
        cvResetImageROI(m_pROI_frame);
    }

    // Find the threshold of the difference
    cvThreshold(m_pDiff_frame, m_pThresh_frame, m_iBlob_val_thresh, 255.0, CV_THRESH_BINARY);

    cvResetImageROI(m_pBG_frame);
    cvResetImageROI(m_pGS_frame);
    cvResetImageROI(m_pThresh_frame);
    cvResetImageROI(m_pDiff_frame);
}       // end function


double GYSegmenter::updateFrameRate(double dt)
{

    if(dt > 0)
    {
        m_dFrameRate = 1.0/dt;
    }

    return m_dFrameRate;

}       // end function


void GYSegmenter::glDraw(bool DrawBlobs)
{

    MT_R3 blobcenter;

    if(m_bDrawArrows && m_bHasHistory)
    {
        for (unsigned int i = 0; i < m_CurrentBlobs.size(); i++)
        {

            blobcenter.setx(m_CurrentBlobs[i].m_dXCentre);
            blobcenter.sety(m_iFrameHeight - m_CurrentBlobs[i].m_dYCentre);
            blobcenter.setz( 0 );

            MT_DrawArrow( blobcenter,
                       m_dArrowLength,
                       MT_DEG2RAD*m_CurrentBlobs[i].m_dOrientation,
                       m_ArrowColor,
                       1.0 // fixed arrow width
                );    
        }
    }

    if(m_bDrawSearchRect && m_bHasHistory)
    {
        MT_DrawRectangle(m_SearchArea.x, m_iFrameHeight - m_SearchArea.y, m_SearchArea.width, -m_SearchArea.height);
    }

    if(m_pTrackedObjects)
    {
        for(unsigned int i = 0; i < (unsigned int) m_iNobj; i++)
        {
            blobcenter.setx(m_pTrackedObjects->getX(i));
            blobcenter.sety(m_iFrameHeight - m_pTrackedObjects->getY(i));
            blobcenter.setz(0);

            MT_DrawEllipse(blobcenter, 15.0, 15.0, 0, MT_Primaries[i % MT_NPrimaries]);
        }
    }

}       // end function

void GYSegmenter::doMatching()
{

    if(!m_pTrackedObjects) /* this should happen on the first iteration only */
    {

        m_pTrackedObjects = new MT_TrackedObjectsBase(m_iNobj);
        /* note these get deleted by the tracker base if != NULL */
        m_HungarianMatcher.doInit(m_iNobj);

        /* first time through just take the positions as initial */
        for(unsigned int i = 0; i < (unsigned int) m_iNobj; i++)
        {
            m_pTrackedObjects->setXY(i, XBlobs[i], YBlobs[i]);
        }
        return;
    }

    double dx;
    double dy;
    for(int i = 0; i < m_iNobj; i++)
    {
        for(int j = 0; j < m_iNobj; j++)
        { 
            dx = (XBlobs[i] - m_pTrackedObjects->getX(j));
            dy = (YBlobs[i] - m_pTrackedObjects->getY(j));
            m_HungarianMatcher.setValue(i,j,dx*dx + dy*dy);
        }
    }

    m_HungarianMatcher.doMatch(&m_viMatchAssignments);

    int j;
    double meas[3];
    for(int i = 0; i < m_iNobj; i++)
    {
        j = m_viMatchAssignments[i];
        m_pTrackedObjects->setXY(j, XBlobs[i], YBlobs[i]);

        meas[0] = XBlobs[i]; meas[1] = YBlobs[i]; meas[2] = OBlobs[i];
        m_pTrackedObjects->setMeasurement(j, meas);
    }

}

// Main Tracking Function - this is the main workhorse.
void GYSegmenter::doTracking(IplImage* frame)
{
    static double t_previous = MT_getTimeSec();
    static double t_start = MT_getTimeSec();

    double dt;
    double t_now = MT_getTimeSec();

    dt = t_now - t_previous;  /// TODO for an AVI, dt should be constant
    m_dT = dt;

    //dt = 0.04;
    t_previous = t_now;

    m_iFrame_counter++;

    // Keep a copy of the original frame pointer for display purposes
    m_pOrg_frame = frame;

    // Convert to grayscale
	if(frame->nChannels == 3)
	{
    	cvCvtColor(frame, m_pGS_frame, CV_BGR2GRAY);
	}
	else
	{
		cvCopy(frame, m_pGS_frame);
	}

    double t0 = MT_getTimeSec();

    // image operations to extract regions where fish are likely
    //  e.g. background substitution, thresholding
    doImageProcessing();

    double t1 = MT_getTimeSec();

    t0 = MT_getTimeSec();

    // Only track objects if there are some to find
    if (m_iNobj > 0)
    {
        doBlobFinding();
        //double t01 = MT_getTimeSec();
        doSegmentation();
        t1 = MT_getTimeSec();

        std::vector<double> d_vec(m_iNobj, 0);

        BlobIndexes.resize(m_iNobj);
        XBlobs = d_vec;  /* for some reason resize doesn't work here... but copying another vector of the right size does */
        YBlobs = d_vec;
        ABlobs = d_vec;
        OBlobs = d_vec;
        for(int i = 0; i < m_iNobj; i++)
        {
            BlobIndexes[i] = i;
            XBlobs[i] = m_CurrentBlobs[i].m_dXCentre;
            YBlobs[i] = m_CurrentBlobs[i].m_dYCentre;
            ABlobs[i] = m_CurrentBlobs[i].m_dArea;
            OBlobs[i] = m_CurrentBlobs[i].m_dOrientation;
        }
        doMatching();
    }
    writeData();

    if (t_now != t_start)
    {
        m_dAverageFrameRate = ((double) m_iFrame_counter)/(t_now - t_start);
    }

    updateFrameRate(dt);

    /*if(m_pBlobFile)
      {
      m_pBlobFile->WriteBlobs(m_iFrame_counter, dt, m_CurrentBlobs);
      }*/

    // Reset the history at random so the whole image is searched
    if (MT_frand(1.0) < 0.005)
    {
        m_bHasHistory = false;
    }
}       // end function


void GYSegmenter::doBlobFinding()
{
    int i, j;

    // Initialise the vector to contain the raw blob data
    std::vector<RawBlobPtr> FirstRawBlobs;
    FirstRawBlobs.resize(0);

    // Temporary variables for raw blob detection
    uchar PixelValue;
    bool InsideBlob = false;
    int MaxBlobNumber = 0;
    int PixelLabel,  TopPixelLabel, LeftPixelLabel;
    int *BlobNumbers;
    BlobNumbers = new int[m_SearchArea.width*m_SearchArea.height];
    for (i = 0 ; i < m_SearchArea.width ; i++)
    {
        for (j = 0 ; j < m_SearchArea.height ; j++)
        {
            BlobNumbers[i + j*m_SearchArea.width] = 0;
        }
    }

    // Find the raw blobs from the thresholded frame inside the search area
    for (j = 0 ; j < m_SearchArea.height ; j++)
    {
        for (i = 0 ; i < m_SearchArea.width ; i++)
        {
            PixelValue = ((uchar*)(m_pThresh_frame->imageData + m_pThresh_frame->widthStep*(j + m_SearchArea.y)))[i + m_SearchArea.x];
            PixelLabel = BlobNumbers[i + j*m_SearchArea.width];

            if (j == 0)
            {
                TopPixelLabel = 0;
            }
            else
            {
                TopPixelLabel = BlobNumbers[i + (j-1)*m_SearchArea.width];
            }

            if (i == 0)
            {
                LeftPixelLabel = 0;
            }
            else
            {
                LeftPixelLabel = BlobNumbers[i - 1 + j*m_SearchArea.width];
            }

            if (!InsideBlob)
            {
                if ((PixelValue == 255) && (PixelLabel == 0) && (TopPixelLabel <= 0)) // We have just found the start of a new contour
                {
                    MaxBlobNumber++;
                    BlobNumbers[i + j*m_SearchArea.width] = MaxBlobNumber;

                    RawBlobPtr rbp(new GYRawBlob(300));         // Make sure there is space for 300 pixels in the blob (NOTE: this number should be changed for different applications)
                    FirstRawBlobs.push_back(rbp);
                    FirstRawBlobs[MaxBlobNumber-1]->AddPoint(cvPoint(i + m_SearchArea.x, j + m_SearchArea.y));

                    double perimeter;
                    perimeter = TraceContour(i, j, m_SearchArea.x, m_SearchArea.y, m_SearchArea.width, m_SearchArea.height, BlobNumbers, m_pThresh_frame);

                    FirstRawBlobs[MaxBlobNumber-1]->SetPerimeter(perimeter);
                }
                else if ((PixelValue == 255) && (PixelLabel != 0))              // We have encountered a pixel on a labelled contour
                {
                    FirstRawBlobs[PixelLabel-1]->AddPoint(cvPoint(i + m_SearchArea.x, j + m_SearchArea.y));
                }
                else if (PixelValue == 255)             // We have encountered an unlabelled black pixel. The pixel to the left must be on the contour and labelled
                {
                    BlobNumbers[i + j*m_SearchArea.width] = LeftPixelLabel;
                    FirstRawBlobs[LeftPixelLabel-1]->AddPoint(cvPoint(i + m_SearchArea.x, j + m_SearchArea.y));
                    InsideBlob = true;
                }
                else if ((LeftPixelLabel > 0) && (PixelLabel != -1))    // We have encountered an internal white pixel. It should be black
                {
                    BlobNumbers[i + j*m_SearchArea.width] = LeftPixelLabel;
                    FirstRawBlobs[LeftPixelLabel-1]->AddPoint(cvPoint(i + m_SearchArea.x, j + m_SearchArea.y));
                    InsideBlob = true;
                }
            }           // end if (!InsideBlob)
            else        // We are inside a raw blob, so we can assume the pixel is black
            {
                if (PixelLabel != 0)    // We have reached the end of the raw blob
                {
                    FirstRawBlobs[PixelLabel-1]->AddPoint(cvPoint(i + m_SearchArea.x, j + m_SearchArea.y));
                    InsideBlob = false;
                }
                else
                {
                    BlobNumbers[i + j*m_SearchArea.width] = LeftPixelLabel;
                    FirstRawBlobs[LeftPixelLabel-1]->AddPoint(cvPoint(i + m_SearchArea.x, j + m_SearchArea.y));
                }
            }           // end else
        }               // end for (i = 0 ; i < m_SearchArea.width ; i++)
    }           // end for (j = 0 ; j < m_SearchArea.height ; j++)

    // Now filter the raw blobs according to the area thresholds
    m_RawBlobData.resize(0);
    for (j = 0 ; j < (int) FirstRawBlobs.size() ; j++)
    {
        if ((FirstRawBlobs[j]->GetNumPixels() >= m_iBlob_area_thresh_low) && (FirstRawBlobs[j]->GetNumPixels() <= m_iBlob_area_thresh_high))
        {
            m_RawBlobData.push_back(FirstRawBlobs[j]);
        }
    }

    // If we haven't found any blobs and we had a limited search area, reset the
    // search area and try again.
    if ((m_RawBlobData.size() == 0) && m_bHasHistory)
    {
        m_bHasHistory = false;
        doImageProcessing();
        doBlobFinding();
    }

    delete[] BlobNumbers;       // release memory

}       // end function


/* This function takes the raw data from doBlobFinding and extracts the parameters of the individual
   blobs. It first calculates how many blobs are in each raw blob. For each raw blob that corresponds
   to a single blob, the parameters are directly measured. For each raw blob that corresponds to
   multiple blobs, an expectation maximisation algorithm is run to fit multiple ellipses to the pixel
   data. Then the individual blobs are extracted and their properties measured. */
void GYSegmenter::doSegmentation()
{
    int i, j, k, index_max;

    // Determine how many blobs are in each raw blob by looking at the proportions of the
    // total area and perimeter in each blob
    int numrawblobs = m_RawBlobData.size();
    int numblobsfound = 0;

    double totalarea = 0.0;
    double totalperimeter = 0.0;

    double* RawBlobSizes = NULL;
    RawBlobSizes = new double[numrawblobs];

    int* BlobSizes = NULL;
    BlobSizes = new int[numrawblobs];

    for (i = 0 ; i < numrawblobs ; i++)
    {
        totalarea += m_RawBlobData[i]->GetArea();
        totalperimeter += m_RawBlobData[i]->GetPerimeter();
    }

    for (i = 0 ; i < numrawblobs ; i++)
    {
        RawBlobSizes[i] = m_iNobj*(m_RawBlobData[i]->GetArea()/totalarea + m_RawBlobData[i]->GetPerimeter()/totalperimeter)/2.0;
        BlobSizes[i] = (int) (RawBlobSizes[i] + 0.5);
        numblobsfound += BlobSizes[i];
    }

    // If our estimates of the total number of blobs is incorrect (usually due to rounding effects), 
    // add or remove a blob from the most likely candidate raw blob
    while (numblobsfound < m_iNobj)
    {
        index_max = IndexMaxDiff(RawBlobSizes, BlobSizes, numrawblobs);
        BlobSizes[index_max] += 1;
        numblobsfound += 1;             
    }
    while (numblobsfound > m_iNobj)
    {
        index_max = IndexMaxDiff(BlobSizes, RawBlobSizes, numrawblobs);
        BlobSizes[index_max] -= 1;
        numblobsfound -= 1;             
    }

    int currentblob = 0;
    /* int numpixels; */
    int numinrawblob;
    std::vector<CvPoint> PixelList;
    PixelList.resize(0);
    double XXMoment, YYMoment, XYMoment, Delta, A, theta, headfraction, slope;

    // Loop through the blobs to extract their parameters
    for (i = 0 ; i < numrawblobs ; i++)
    {
        // Here we have raw blobs that contain a single blob
        if (BlobSizes[i] == 1)
        {
            m_CurrentBlobs[currentblob].m_dArea = m_RawBlobData[i]->GetArea();
            m_CurrentBlobs[currentblob].m_dXCentre = m_RawBlobData[i]->GetXCentre();
            m_CurrentBlobs[currentblob].m_dYCentre = m_RawBlobData[i]->GetYCentre();

            // Calculating ellipse (semi) major and minor axes from the moments
            XXMoment = m_RawBlobData[i]->GetXXMoment();
            YYMoment = m_RawBlobData[i]->GetYYMoment();
            XYMoment = m_RawBlobData[i]->GetXYMoment();
            Delta = sqrt(4*pow(XYMoment, 2) + pow(XXMoment - YYMoment, 2));
            A = pow(16*pow(M_PI, 2)*(XXMoment*YYMoment - pow(XYMoment, 2)), 1.0/4.0);

            m_CurrentBlobs[currentblob].m_dMajorAxis = sqrt((2*(XXMoment + YYMoment + Delta))/A);
            m_CurrentBlobs[currentblob].m_dMinorAxis = sqrt((2*(XXMoment + YYMoment - Delta))/A);

            // Calculating the orientation angle from the moments. Note that after this
            // calculation, theta will be between -90 and 90
            theta = 1.0/2.0*atan2(2*XYMoment, XXMoment - YYMoment)*MT_RAD2DEG;

            // Estimate the correct orientation of the blob by assigning the side with the 
            // most pixels as the front. We determine the two 'sides' of the blob by 
            // drawing a line through the centroid, perpendicular to theta
            PixelList.resize(m_RawBlobData[i]->GetNumPixels());
            m_RawBlobData[i]->GetPixelList(PixelList);

            if (theta == 0)
            {
                headfraction = 0.0;
                for (j = 0; j < (int) PixelList.size() ; j++)
                {
                    if (PixelList[j].x > m_CurrentBlobs[currentblob].m_dXCentre)
                    {
                        headfraction += 1.0;
                    }
                }
                headfraction /= (double) PixelList.size();
            }           // end if (theta == 0)
            else
            {
                slope = tan((theta - 90.0)*MT_DEG2RAD);
                headfraction = 0.0;

                if (theta > 0)
                {
                    for (j = 0 ; j < (int) PixelList.size() ; j++)
                    {
                        if (PixelList[j].y > (slope*(PixelList[j].x - m_CurrentBlobs[currentblob].m_dXCentre) + m_CurrentBlobs[currentblob].m_dYCentre))
                        {
                            headfraction += 1.0;
                        }
                    }
                    headfraction /= (double) PixelList.size();
                }               // end if (theta > 0)
                else
                {
                    for (j = 0 ; j < (int) PixelList.size() ; j++)
                    {
                        if (PixelList[j].y < (slope*(PixelList[j].x - m_CurrentBlobs[currentblob].m_dXCentre) + m_CurrentBlobs[currentblob].m_dYCentre))
                        {
                            headfraction += 1.0;
                        }
                    }
                    headfraction /= (double) PixelList.size();
                }               // end else
            }           // end else

            // If headfraction is less than 0.5, we have the wrong orientation
            if (headfraction < 0.5)
            {
                theta = theta - 180.0;
            }
            // So now theta could be between -270 and 90

            if (theta < -180)
            {
                theta = theta + 360.0;
            }
            // Now we have theta between -180 and 180

            m_CurrentBlobs[currentblob].m_dOrientation = -theta;

            currentblob++;
        }               // end if (BlobSizes[i] == 1)
        else if (BlobSizes[i] > 0)              // Here we have raw blobs containing several blobs
        {
            numinrawblob = BlobSizes[i];
            MixGaussians FittedBlobs;   // Create a mixture of Gaussians model to estimate the individual blobs

            if (!m_bHasHistory)
            {
                // If we have no blob history, we make our initial estimate by distributing the mixture evenly
                // over the bounding box around the raw blob
                FittedBlobs = MixGaussians(numinrawblob, m_RawBlobData[i]->GetBoundingBox());
            }
            else
            {
                // If we have a history, we want to take the blobs that are closest to the raw blob as our estimates
                CvRect RawBoundingBox = m_RawBlobData[i]->GetBoundingBox();
                double xcentre = (double) RawBoundingBox.x + ((double) RawBoundingBox.width)/2.0 - 0.5;
                double ycentre = (double) RawBoundingBox.y + ((double) RawBoundingBox.height)/2.0 - 0.5;
                double maxdimension;
                if (RawBoundingBox.width > RawBoundingBox.height)
                {
                    maxdimension = (double) RawBoundingBox.width;
                }
                else
                {
                    maxdimension = (double) RawBoundingBox.height;
                }

                double* distances;
                distances = new double[m_iNobj];
                int closestblob;
                MT_Vector2 GuessMean;
                MT_Matrix2x2 GuessCovariance;
                double sigma1, sigma2, phi, cp, sp;

                // Calculate the distances of each blob to the centre of the bounding box
                for (k = 0 ; k < m_iNobj ; k++)
                {
                    distances[k] = sqrt(pow(xcentre - m_OldBlobs[k].m_dXCentre, 2) + pow(ycentre - m_OldBlobs[k].m_dYCentre, 2));

                    // For blobs outside the bounding box, we add additional weight to their distances
                    if((m_OldBlobs[k].m_dXCentre < RawBoundingBox.x) || (m_OldBlobs[k].m_dXCentre > (RawBoundingBox.x + RawBoundingBox.width - 1)) || (m_OldBlobs[k].m_dYCentre < RawBoundingBox.y) || (m_OldBlobs[k].m_dYCentre > (RawBoundingBox.y + RawBoundingBox.height - 1)))
                    {
                        distances[k] += maxdimension;
                    }
                }

                // Find the closest blob, calculate a mean and covariance matrix then add these to the mixture model
                for (k = 0 ; k < numinrawblob ; k++)
                {
                    closestblob = IndexMin(distances, m_iNobj);
                    GuessMean.data[0] = m_OldBlobs[closestblob].m_dXCentre;
                    GuessMean.data[1] = m_OldBlobs[closestblob].m_dYCentre;

                    // If GuessMean is outside the bounding box, shift it inside
                    if(GuessMean.data[0] < RawBoundingBox.x)
                    {
                        GuessMean.data[0] = (double) RawBoundingBox.x;
                    }
                    if(GuessMean.data[0] > RawBoundingBox.x + RawBoundingBox.width - 1)
                    {
                        GuessMean.data[0] = (double) (RawBoundingBox.x + RawBoundingBox.width - 1);
                    }
                    if(GuessMean.data[1] < RawBoundingBox.y)
                    {
                        GuessMean.data[1] = (double) RawBoundingBox.y;
                    }
                    if(GuessMean.data[1] > RawBoundingBox.y + RawBoundingBox.height - 1)
                    {
                        GuessMean.data[1] = (double) (RawBoundingBox.y + RawBoundingBox.height - 1);
                    }

                    // If the previous measured blob had a reasonable size, use it to generate the
                    // covariance matrix. Otherwise, use default values
                    if (m_OldBlobs[closestblob].m_dMajorAxis > 8)
                    {
                        sigma1 = pow(m_OldBlobs[closestblob].m_dMajorAxis/1.95, 2);
                    }
                    else
                    {
                        sigma1 = pow(8.0/1.95, 2);
                    }

                    if (m_OldBlobs[closestblob].m_dMinorAxis > 1)
                    {
                        sigma2 = pow(m_OldBlobs[closestblob].m_dMinorAxis/1.95, 2);
                    }
                    else
                    {
                        sigma2 = pow(1.0/1.95, 2);
                    }

                    phi = MT_DEG2RAD*m_OldBlobs[closestblob].m_dOrientation;
                    cp = cos(phi);
                    sp = -sin(phi);

                    GuessCovariance.data[0] = sigma1*cp*cp + sigma2*sp*sp;
                    GuessCovariance.data[1] = sp*cp*(sigma1 - sigma2);
                    GuessCovariance.data[2] = sp*cp*(sigma1 - sigma2);
                    GuessCovariance.data[3] = sigma1*sp*sp + sigma2*cp*cp;

                    FittedBlobs.AddDist(GuessMean, GuessCovariance);

                    // Make the distance very large so the next loop will find another blob
                    distances[closestblob] = 1E20;
                }               // end for (k = 0 ; k < numinrawblob ; k++)

                delete[] distances;             // release memory
            }           // end else

            /* Create a vector to hold the distribution allocations of each pixel in the raw blob.
               For a pixel assigned to distributions d_1, d_2, d_3, ..., the allocation number will
               be d_1 + n*d_2 + n^2*d_3 + ..., where n is the total number of distributions 
               (i.e. n = numinrawblob) */
            std::vector<int> PixelAllocation;
            PixelAllocation.resize(m_RawBlobData[i]->GetNumPixels());

            // Run the expectation maximisation algorithm
            FittedBlobs.EMMG(m_RawBlobData[i], PixelAllocation, m_iFrame_counter);

            // We now want a new vector of raw blobs for each extracted individual blob
            std::vector<RawBlobPtr> ExtractedBlobs;
            ExtractedBlobs.resize(0);
            for (k = 0 ; k < numinrawblob ; k++)
            {
                RawBlobPtr rbp(new GYRawBlob(m_RawBlobData[i]->GetNumPixels()));        // Make sure the new raw blobs have space for enough pixels - faster running at expense of more initial memory
                ExtractedBlobs.push_back(rbp);
            }

            // Run through the pixels from the original raw blob and assign them to their allocated new blobs
            PixelList.resize(m_RawBlobData[i]->GetNumPixels());
            m_RawBlobData[i]->GetPixelList(PixelList);
            for (k = 0 ; k < m_RawBlobData[i]->GetNumPixels() ; k++)
            {
                int allocated;
                int numdists = 1;
                int dist_number = PixelAllocation[k] % numinrawblob;

                ExtractedBlobs[dist_number]->AddPoint(PixelList[k]);
                allocated = dist_number;

                while (allocated != PixelAllocation[k])
                {
                    dist_number = ((PixelAllocation[k] - allocated)/(ipow(numinrawblob, numdists))) % numinrawblob;
                    ExtractedBlobs[dist_number]->AddPoint(PixelList[k]);
                    allocated += dist_number*ipow(numinrawblob, numdists);
                    numdists++;
                }
            }           // end for (k = 0 ; k < m_RawBlobData[i]->GetNumPixels() ; k++)

            // Loop through the new blobs and extract their parameters
            for (k = 0 ; k < numinrawblob ; k++)
            {
                m_CurrentBlobs[currentblob].m_dArea = ExtractedBlobs[k]->GetArea();
                m_CurrentBlobs[currentblob].m_dXCentre = ExtractedBlobs[k]->GetXCentre();
                m_CurrentBlobs[currentblob].m_dYCentre = ExtractedBlobs[k]->GetYCentre();

                // Calculating ellipse (semi) major and minor axes from the moments
                XXMoment = ExtractedBlobs[k]->GetXXMoment();
                YYMoment = ExtractedBlobs[k]->GetYYMoment();
                XYMoment = ExtractedBlobs[k]->GetXYMoment();
                Delta = sqrt(4*pow(XYMoment, 2) + pow(XXMoment - YYMoment, 2));
                A = pow(16*pow(M_PI, 2)*(XXMoment*YYMoment - pow(XYMoment, 2)), 0.25);

                if (A == 0)             // This will happen if all the pixels lie along a line
                {
                    A = 1;
                }

                m_CurrentBlobs[currentblob].m_dMajorAxis = sqrt((2*(XXMoment + YYMoment + Delta))/A);
                m_CurrentBlobs[currentblob].m_dMinorAxis = sqrt((2*(XXMoment + YYMoment - Delta))/A);

                // Calculating the orientation angle from the moments. Note that after this
                // calculation, theta will be between -90 and 90
                theta = 0.5*atan2(2*XYMoment, XXMoment - YYMoment)*MT_RAD2DEG;

                // Estimate the correct orientation of the blob by assigning the side with the 
                // most pixels as the front. We determine the two 'sides' of the blob by 
                // drawing a line through the centroid, perpendicular to theta
                PixelList.resize(ExtractedBlobs[k]->GetNumPixels());
                ExtractedBlobs[k]->GetPixelList(PixelList);

                if (theta == 0)
                {
                    headfraction = 0.0;
                    for (j = 0; j < (int) PixelList.size() ; j++)
                    {
                        if (PixelList[j].x > m_CurrentBlobs[currentblob].m_dXCentre)
                        {
                            headfraction += 1.0;
                        }
                    }
                    headfraction /= (double) PixelList.size();
                }               // end if (theta == 0)
                else
                {
                    slope = tan((theta - 90.0)*MT_DEG2RAD);
                    headfraction = 0.0;

                    if (theta > 0)
                    {
                        for (j = 0 ; j < (int) PixelList.size() ; j++)
                        {
                            if (PixelList[j].y > (slope*(PixelList[j].x - m_CurrentBlobs[currentblob].m_dXCentre) + m_CurrentBlobs[currentblob].m_dYCentre))
                            {
                                headfraction += 1.0;
                            }
                        }
                        headfraction /= (double) PixelList.size();
                    }           // end if (theta > 0)
                    else
                    {
                        for (j = 0 ; j < (int) PixelList.size() ; j++)
                        {
                            if (PixelList[j].y < (slope*(PixelList[j].x - m_CurrentBlobs[currentblob].m_dXCentre) + m_CurrentBlobs[currentblob].m_dYCentre))
                            {
                                headfraction += 1.0;
                            }
                        }
                        headfraction /= (double) PixelList.size();
                    }           // end else
                }               // end else

                // If headfraction is less than 0.5, we have the wrong orientation
                if (headfraction < 0.5)
                {
                    theta = theta - 180.0;
                }
                // So now theta could be between -270 and 90

                if (theta < -180)
                {
                    theta = theta + 360.0;
                }
                // Now we have theta between -180 and 180

                m_CurrentBlobs[currentblob].m_dOrientation = -theta;

                currentblob++;
            }           // end for (k = 0 ; k < numinrawblob ; k++)
        }               // end else if (BlobSizes[i] > 0)
    }           // end for (i = 0 ; i < numrawblobs ; i++)

    // Record the current set of blobs so we can make estimates next time (if needed)
    m_OldBlobs = m_CurrentBlobs;
    m_bHasHistory = true;

    // Cleanup
    if(numrawblobs)
    {
        delete[] RawBlobSizes;
        delete[] BlobSizes;
    }

}       // end function

MT_BoundingBox GYSegmenter::getObjectBoundingBox() const
{
    MT_BoundingBox r;
    for(int i = 0; i < m_iNobj; i++)
    {
        r.ShowX(m_CurrentBlobs[i].m_dXCentre);
        r.ShowY(m_iFrameHeight - m_CurrentBlobs[i].m_dYCentre);
    }
    return r;
}



// Contour tracing function following the algorithm given in Chang, Chen and Lu 2004 "A linear-time component-labeling
// algorithm using contour tracing technique", Computer Vision and Image Understanding, vol 93, pp 206-220
static double TraceContour(int start_x, int start_y, int first_x, int first_y, int width, int height, int* labelarray, IplImage* frame)
{
    double perimeter = 0.0;
    CvPoint startpoint = cvPoint(start_x + first_x, start_y + first_y);
    CvPoint secondpoint = GYTracer(frame, startpoint, labelarray, first_x, first_y, width, height, 7);
    CvPoint currentpoint = startpoint;
    CvPoint nextpoint = secondpoint;

    // If the second point is the same as the start point, we have an isolated pixel
    if ((secondpoint.x == startpoint.x) && (secondpoint.y == startpoint.y))
    {
        return 1.0;
    }
    else
    {
        int diffx, diffy;
        int startloc = 0;

        // Keep moving around the contour until the current point is the starting point and the next point is the second point
        do
        {
            perimeter += 1.0; // Increment the perimeter measurement

            // Determine the starting location for the next search. Adjacent pixels are numbered
            // in a clockwise fashion, with 0 being the pixel immediately to the right
            diffx = currentpoint.x - nextpoint.x;
            diffy = currentpoint.y - nextpoint.y;
            switch (diffy)
            {
            case 0:
                startloc = 4 - 2*diffx;
                break;
            case 1:
                startloc = 4 - diffx;
                break;
            case -1:
                startloc = ((diffx < 0) ? 7 : diffx);
                break;
            }           // end switch (diffy)

            currentpoint = nextpoint;
            // Run the tracer to find the next pixel on the contour
            nextpoint = GYTracer(frame, currentpoint, labelarray, first_x, first_y, width, height, startloc);
        }                       // end do
        while ((currentpoint.x != startpoint.x) || (currentpoint.y != startpoint.y) || (nextpoint.x != secondpoint.x) || (nextpoint.y != secondpoint.y));

        return perimeter;
    }           // end else
}       // end function


// Tracer function following the algorithm given in Chang, Chen and Lu 2004 "A linear-time component-labeling
// algorithm using contour tracing technique", Computer Vision and Image Understanding, vol 93, pp 206-220
static CvPoint GYTracer(IplImage* frame, CvPoint currentpoint, int* labelarray, int first_x, int first_y, int width, int height, int startloc)
{
    int i, j;
    int d = startloc;
    int pixelsvisited;
    uchar PixelValue = 0;
    CvPoint NextPoint;

    // Loop through the surrounding pixels
    for (pixelsvisited = 0 ; pixelsvisited < 8 ; pixelsvisited++)
    {
        /* Decode d into index shifts in x and y according to the following diagram
           -------------
           | 5 | 6 | 7 |
           -------------
           | 4 |   | 0 |
           -------------
           | 3 | 2 | 1 |
           -------------
           (note that lower pixels have greater y values) */

        if ((d % 4) == 0)
        {
            i = 1 - d/2;
            j = 0;
        }
        else if (d < 4)
        {
            i = 2 - d;
            j = 1;
        }
        else
        {
            i = d - 6;
            j = -1;
        }

        // If the pixel to search lies outside the search area, take its value to be zero
        if ((currentpoint.x + i < first_x) || (currentpoint.x + i >= first_x + width) || (currentpoint.y + j < first_y) || (currentpoint.y + j >= first_y + height))
        {
            PixelValue = 0;
            d = (d + 1) % 8;
        }
        else    // Otherwise, read the pixel value from the image
        {
            PixelValue = ((uchar*)(frame->imageData + frame->widthStep*(currentpoint.y + j)))[currentpoint.x + i];

            if (PixelValue == 0)        // If it is a white pixel, label it in the label array and increment d
            {
                labelarray[currentpoint.x + i - first_x + (currentpoint.y + j - first_y)*width] = -1;
                d = (d + 1) % 8;
            }
            else        // If it as a black pixel, label it the same as the current point and return its location
            {
                labelarray[currentpoint.x + i - first_x + (currentpoint.y + j - first_y)*width] = labelarray[currentpoint.x - first_x + (currentpoint.y - first_y)*width];
                NextPoint = cvPoint(currentpoint.x + i, currentpoint.y + j);
                return NextPoint;
            }
        }               // end else
    }           // end for (pixelsvisited = 0 ; pixelsvisited < 8 ; pixelsvisited++)

    return currentpoint;
}       // end function


/* Minor functions for finding indices for max and min values, and integer powers of integers */
static int IndexMaxDiff(double* array1, int* array2, int arraylength)
{
    int i;
    int index_max = 0;
    double currentdiff = array1[0] - (double) array2[0];

    for (i = 1 ; i < arraylength ; i++)
    {
        if ((array1[i] - (double) array2[i]) > currentdiff)
        {
            currentdiff = array1[i] - (double) array2[i];
            index_max = i;
        }
    }

    return index_max;
}       // end function


static int IndexMaxDiff(int* array1, double* array2, int arraylength)
{
    int i;
    int index_max = 0;
    double currentdiff = (double) array1[0] - array2[0];

    for (i = 1 ; i < arraylength ; i++)
    {
        if (((double) array1[i] - array2[i]) > currentdiff)
        {
            currentdiff = (double) array1[i] - array2[i];
            index_max = i;
        }
    }

    return index_max;
}       // end function


static int IndexMin(double* array, int arraylength)
{
    int i;
    int index_min = 0;
    double currentmin = array[0];

    for (i = 1 ; i < arraylength ; i++)
    {
        if (array[i] < currentmin)
        {
            currentmin = array[i];
            index_min = i;
        }
    }

    return index_min;
}       // end function


static int ipow(int& a, int& n)
{
    if (n == 0)
    {
        return 1;
    }
    else if (n > 0)
    {
        int val = 1;
        for (int i = 0; i < n ; i++)
        {
            val *= a;
        }
        return val;
    }
    else if ((a == 1) || ((a == 2) && (n == -1)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}       // end function

