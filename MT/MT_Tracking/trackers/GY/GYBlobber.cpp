#include "GYBlobber.h"

// Internal Functions for blob finding and segmentation
static double TraceContour(int start_x,
                           int start_y,
                           int first_x,
                           int first_y,
                           int width,
                           int height,
                           int* labelarray,
                           IplImage* frame);

static CvPoint GYTracer(IplImage* frame,
                        CvPoint currentpoint,
                        int* labelarray,
                        int first_x,
                        int first_y,
                        int width,
                        int height,
                        int startloc);

static int IndexMaxDiff(double* array1, int* array2, int arraylength);

static int IndexMaxDiff(int* array1, double* array2, int arraylength);

static int IndexMin(double* array, int arraylength);

static int ipow(int& a, int& n);


GYBlobber::GYBlobber(unsigned int num_obj)
    : m_iBlob_area_thresh_low(GY_DEFAULT_AREA_THRESH_LOW),
      m_iBlob_area_thresh_high(GY_DEFAULT_AREA_THRESH_HIGH),
      m_SearchArea(cvRect(0,0,0,0)),
      m_bHasHistory(false),      
      m_iNObj(num_obj)
{
    setNumberOfObjects(num_obj);
}

void GYBlobber::setNumberOfObjects(unsigned int num_obj)
{
    m_iNObj = num_obj;
    m_bHasHistory = false;
    m_RawBlobData.resize(m_iNObj);
    m_CurrentBlobs.resize(m_iNObj);
    m_OldBlobs.resize(m_iNObj);
}

void GYBlobber::setSearchArea(CvRect new_search_area)
{
    m_SearchArea = new_search_area;
}

void GYBlobber::resetSearchArea()
{
    m_SearchArea = cvRect(0, 0, 0, 0);
}

std::vector<GYBlob> GYBlobber::findBlobs(IplImage* thresh_image)
{
    doBlobFinding(thresh_image);
    doSegmentation();
    return m_CurrentBlobs;
}

void GYBlobber::doBlobFinding(IplImage* thresh_frame)
{
    int i, j;

    /* automatically set the search area to the whole frame if it
     * doesn't make sense. */
    if(m_SearchArea.width <= 0 ||
        m_SearchArea.height <= 0 ||
        m_SearchArea.x < 0 ||
        m_SearchArea.y < 0 ||
        m_SearchArea.x + m_SearchArea.width >= thresh_frame->width ||
        m_SearchArea.y + m_SearchArea.height >= thresh_frame->height)
    {
        m_SearchArea = cvRect(0, 0, thresh_frame->width, thresh_frame->height);
    }

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
            PixelValue = ((uchar*)(thresh_frame->imageData + thresh_frame->widthStep*(j + m_SearchArea.y)))[i + m_SearchArea.x];
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
                    perimeter = TraceContour(i, j, m_SearchArea.x, m_SearchArea.y, m_SearchArea.width, m_SearchArea.height, BlobNumbers, thresh_frame);

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
        resetSearchArea();
        doBlobFinding(thresh_frame);
    }

    delete[] BlobNumbers;       // release memory

}       // end function


/* This function takes the raw data from doBlobFinding and extracts the parameters of the individual
   blobs. It first calculates how many blobs are in each raw blob. For each raw blob that corresponds
   to a single blob, the parameters are directly measured. For each raw blob that corresponds to
   multiple blobs, an expectation maximisation algorithm is run to fit multiple ellipses to the pixel
   data. Then the individual blobs are extracted and their properties measured. */
void GYBlobber::doSegmentation()
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
        RawBlobSizes[i] = m_iNObj*(m_RawBlobData[i]->GetArea()/totalarea + m_RawBlobData[i]->GetPerimeter()/totalperimeter)/2.0;
        BlobSizes[i] = (int) (RawBlobSizes[i] + 0.5);
        numblobsfound += BlobSizes[i];
    }

    // If our estimates of the total number of blobs is incorrect (usually due to rounding effects), 
    // add or remove a blob from the most likely candidate raw blob
    while (numblobsfound < (int) m_iNObj)
    {
        index_max = IndexMaxDiff(RawBlobSizes, BlobSizes, numrawblobs);
        BlobSizes[index_max] += 1;
        numblobsfound += 1;             
    }
    while (numblobsfound > (int) m_iNObj)
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
            m_CurrentBlobs[currentblob].m_dXXMoment = m_RawBlobData[i]->GetXXMoment();
            m_CurrentBlobs[currentblob].m_dXYMoment = m_RawBlobData[i]->GetXYMoment();
            m_CurrentBlobs[currentblob].m_dYYMoment = m_RawBlobData[i]->GetYYMoment();            

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
                distances = new double[m_iNObj];
                int closestblob;
                MT_Vector2 GuessMean;
                MT_Matrix2x2 GuessCovariance;
                double sigma1, sigma2, phi, cp, sp;

                // Calculate the distances of each blob to the centre of the bounding box
                for (k = 0 ; k < (int) m_iNObj ; k++)
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
                    closestblob = IndexMin(distances, m_iNObj);
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
            FittedBlobs.EMMG(m_RawBlobData[i],
                             PixelAllocation,
                             0 /* was: m_iFrame_counter
                                * doesn't appear to be used by EMMG */);

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
                m_CurrentBlobs[currentblob].m_dXXMoment = ExtractedBlobs[k]->GetXXMoment();
                m_CurrentBlobs[currentblob].m_dXYMoment = ExtractedBlobs[k]->GetXYMoment();
                m_CurrentBlobs[currentblob].m_dYYMoment = ExtractedBlobs[k]->GetYYMoment();            
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

