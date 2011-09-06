#include "MT/MT_Tracking/trackers/DS/DSGYBlobber.h"
#include "MT/MT_Tracking/trackers/YA/YABlobber.h"

#define TEST_OUT(...) if(m_pTestFile){fprintf(m_pTestFile, __VA_ARGS__); fflush(m_pTestFile);}

void MT_DSGY_PaintYABlobIntoImage(const YABlob& blob,
                                  IplImage* image,
                                  CvScalar color)
{
    if(blob.sequence)
    {
        cvDrawContours(image,
                       blob.sequence,
                       color,
                       color,
                       -1,
                       CV_FILLED,
                       8);
    }
}


MT_DSGYBlobber::MT_DSGYBlobber(unsigned int num_obj)
    : m_iBlob_area_thresh_low(MT_DSGY_DEFAULT_AREA_THRESH_LOW),
      m_iBlob_area_thresh_high(MT_DSGY_DEFAULT_AREA_THRESH_HIGH),
      m_SearchArea(cvRect(0,0,0,0)),
      m_bHasHistory(false),      
      m_iNObj(num_obj),
      m_pTestFile(NULL)
{
    setNumberOfObjects(num_obj);
}

void MT_DSGYBlobber::setTestOut(FILE* fp)
{
    m_pTestFile = fp;
    m_Gaussians.setDebugFile(fp);
}

void MT_DSGYBlobber::setNumberOfObjects(unsigned int num_obj)
{
    m_iNObj = num_obj;
    m_bHasHistory = false;
    m_RawBlobData.resize(0);
    m_CurrentBlobs.resize(m_iNObj);
    m_OldBlobs.resize(m_iNObj);
}

void MT_DSGYBlobber::setSearchArea(CvRect new_search_area)
{
    m_SearchArea = new_search_area;
}

void MT_DSGYBlobber::resetSearchArea()
{
    m_SearchArea = cvRect(0, 0, 0, 0);
}


void MT_DSGYBlobber::setInitials(std::vector<double> x_c,
                              std::vector<double> y_c,
                              std::vector<double> xx,
                              std::vector<double> xy,
                              std::vector<double> yy)
{
    m_Gaussians.ClearDists();
    
    MT_Vector2 mean;
    MT_Matrix2x2 cov;
    for(unsigned int i = 0; i < x_c.size(); i++)
    {
        mean.data[0] = x_c[i];
        mean.data[1] = y_c[i];
        cov.data[0] = xx[i];
        cov.data[1] = xy[i];
        cov.data[2] = xy[i];
        cov.data[3] = yy[i];

        m_Gaussians.AddDist(mean, cov);

    }
    m_bHasHistory = true;
}



std::vector<GYBlob> MT_DSGYBlobber::findBlobs(IplImage* thresh_image,
                                           int num_to_find,
                                           int max_iters)
{
    TEST_OUT("============= MT_DSGYBlobber::findBlobs: =============\n");
    TEST_OUT("\tImage:  (%d x %d x %d)",
             thresh_image->width,
             thresh_image->height,
             thresh_image->nChannels);
    TEST_OUT(", %d blobs expected\n", num_to_find);        
    
    doBlobFinding(thresh_image);
    doSegmentation(num_to_find, max_iters);

    TEST_OUT("Found %d blobs:\n", (int) m_CurrentBlobs.size());
    if(m_pTestFile)
    {
        for(unsigned int i = 0; i < m_CurrentBlobs.size(); i++)
        {
            TEST_OUT("\t%d:  (x,y) = (%f, %f), orientation = %f,\n"
                     "\t\t axes = %f, %f\n"
                     "\t\t moments = %f, %f, %f\n",
                     i,
                     m_CurrentBlobs[i].m_dXCentre,
                     m_CurrentBlobs[i].m_dYCentre,
                     m_CurrentBlobs[i].m_dOrientation,
                     m_CurrentBlobs[i].m_dMinorAxis,
                     m_CurrentBlobs[i].m_dMajorAxis,
                     m_CurrentBlobs[i].m_dXXMoment,
                     m_CurrentBlobs[i].m_dXYMoment,
                     m_CurrentBlobs[i].m_dYYMoment
                );
        }
    }
    TEST_OUT(" -> Done\n");
    
    return m_CurrentBlobs;
}

void MT_DSGYBlobber::doBlobFinding(IplImage* thresh_frame)
{
    int i, j;

    TEST_OUT("Blob Finding\n");
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
    TEST_OUT("\tSearch Area: (x, y, w, h) = (%d, %d, %d, %d)\n",
             m_SearchArea.x,
             m_SearchArea.y,
             m_SearchArea.width,
             m_SearchArea.height);

    /* Add every pixel to the raw blob data */
    TEST_OUT("\tAdding raw blob data\n");
    m_RawBlobData.resize(0);
    m_RawBlobData.push_back(RawBlobPtr(new GYRawBlob(300)));
    uchar p;
    for(j = 0; j < m_SearchArea.height; j++)
    {
        for(i = 0; i < m_SearchArea.width; i++)
        {
            p = ((uchar*)(thresh_frame->imageData
                          + thresh_frame->widthStep*(j + m_SearchArea.y)))[i + m_SearchArea.x];
            if(p == 255)
            {
                m_RawBlobData[0]->AddPoint(cvPoint(i + m_SearchArea.x,
                                                   j + m_SearchArea.y));
            }
        }
    }

    TEST_OUT("\tFound %d pixels in search area\n", m_RawBlobData[0]->GetNumPixels());
    
    // if there are no pixels in the search area, try resizing it
    if (m_RawBlobData[0]->GetNumPixels() == 0)
    {
        TEST_OUT("\tNo pixels found in the search area!")
            //m_bHasHistory = false;
        resetSearchArea();
        doBlobFinding(thresh_frame);
    }

    TEST_OUT(" -> Done\n");

}       // end function


/* This function takes the raw data from doBlobFinding and extracts the parameters of the individual
   blobs. It first calculates how many blobs are in each raw blob. For each raw blob that corresponds
   to a single blob, the parameters are directly measured. For each raw blob that corresponds to
   multiple blobs, an expectation maximisation algorithm is run to fit multiple ellipses to the pixel
   data. Then the individual blobs are extracted and their properties measured. */
void MT_DSGYBlobber::doSegmentation(int num_to_find, int max_iters)
{
    TEST_OUT("EMMG\n");
    
    int j, k;

    int currentblob = 0;
    /* int numpixels; */
    int numinrawblob;
    std::vector<CvPoint> PixelList;
    PixelList.resize(0);
    double XXMoment, YYMoment, XYMoment, Delta, A, theta, headfraction, slope;

    numinrawblob = num_to_find;
    
    TEST_OUT("\tInitialize Distributions\n");
    if(!m_bHasHistory)
    {
        TEST_OUT("\tInitializing with uniform blobs\n");
        //m_Gaussians = MixGaussians(num_to_find, m_SearchArea);
        m_Gaussians.CoverBox(num_to_find, m_SearchArea);
    }
    else
    {
        TEST_OUT("\tAlready have %d distributions\n", m_Gaussians.GetNumDists());
        if(num_to_find > m_Gaussians.GetNumDists())
        {
            TEST_OUT("\tAdding extra distributions\n");
            m_Gaussians.CoverBox(num_to_find - m_Gaussians.GetNumDists(),
                                 m_SearchArea);
        }
    }


   //////////////////////////////////////////////////////////////////////

    /* Create a vector to hold the distribution allocations of each pixel in the raw blob.
       For a pixel assigned to distributions d_1, d_2, d_3, ..., the allocation number will
       be d_1 + n*d_2 + n^2*d_3 + ..., where n is the total number of distributions 
       (i.e. n = numinrawblob) */
    TEST_OUT("\tInitialize Allocations\n");
    std::vector<int> PixelAllocation;
    PixelAllocation.resize(m_RawBlobData[0]->GetNumPixels());

    // Run the expectation maximisation algorithm
    TEST_OUT("\tRunning EMMG algorithm\n");
    /*m_Gaussians.m_dMaxEccentricity = 1.1;*/
    /*m_Gaussians.m_dMaxDistance = 10.0;*/
    /*m_Gaussians.m_dMaxSizePercentChange = 10.0;*/
    m_Gaussians.EMMG(m_RawBlobData[0],
                     PixelAllocation,
                     max_iters);

   //////////////////////////////////////////////////////////////////////

    // We now want a new vector of raw blobs for each extracted
    // individual blob
    TEST_OUT("\tExtracting blob information\n");
    std::vector<RawBlobPtr> ExtractedBlobs;
    ExtractedBlobs.resize(0);
    for (k = 0 ; k < numinrawblob ; k++)
    {
        RawBlobPtr rbp(new GYRawBlob(m_RawBlobData[0]->GetNumPixels()));        // Make sure the new raw blobs have space for enough pixels - faster running at expense of more initial memory
        ExtractedBlobs.push_back(rbp);
    }

   //////////////////////////////////////////////////////////////////////

    // Run through the pixels from the original raw blob and assign them to their allocated new blobs
    PixelList.resize(m_RawBlobData[0]->GetNumPixels());
    m_RawBlobData[0]->GetPixelList(PixelList);
    for (k = 0 ; k < m_RawBlobData[0]->GetNumPixels() ; k++)
    {
        int dists = PixelAllocation[k];
        int p = 1;
        for(int i = 0; i < numinrawblob; i++)
        {
            if(dists & p)
            {
                ExtractedBlobs[i]->AddPoint(PixelList[k]);
            }
            p *= 2;
        }
        
        /* int allocated;
         * int numdists = 1;
         * int dist_number = PixelAllocation[k] % numinrawblob;
         * if(dist_number < 0)
         * {
         *     continue;
         * }
         * 
         * ExtractedBlobs[dist_number]->AddPoint(PixelList[k]);
         * allocated = dist_number;
         * 
         * while (allocated != PixelAllocation[k])
         * {
         *     dist_number = ((PixelAllocation[k] - allocated)/(ipow(numinrawblob, numdists))) % numinrawblob;
         *     ExtractedBlobs[dist_number]->AddPoint(PixelList[k]);
         *     allocated += dist_number*ipow(numinrawblob, numdists);
         *     numdists++;
         * } */
    }           // end for (k = 0 ; k < m_RawBlobData[i]->GetNumPixels() ; k++)



   //////////////////////////////////////////////////////////////////////


    // Loop through the new blobs and extract their parameters
    for (k = 0 ; k < numinrawblob ; k++)
    {
        m_CurrentBlobs[k].m_dArea = ExtractedBlobs[k]->GetArea();
        m_CurrentBlobs[k].m_dXCentre = ExtractedBlobs[k]->GetXCentre();
        m_CurrentBlobs[k].m_dYCentre = ExtractedBlobs[k]->GetYCentre();
        m_CurrentBlobs[k].m_dXXMoment = ExtractedBlobs[k]->GetXXMoment();
        m_CurrentBlobs[k].m_dXYMoment = ExtractedBlobs[k]->GetXYMoment();
        m_CurrentBlobs[k].m_dYYMoment = ExtractedBlobs[k]->GetYYMoment();            

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

        double M, m;
        M = sqrt((2*(XXMoment + YYMoment + Delta))/A);
        m = sqrt((2*(XXMoment + YYMoment - Delta))/A);        
        m_CurrentBlobs[k].m_dMajorAxis = M;
        m_CurrentBlobs[k].m_dMinorAxis = m;

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
                if (PixelList[j].x > m_CurrentBlobs[k].m_dXCentre)
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
                    if (PixelList[j].y > (slope*(PixelList[j].x - m_CurrentBlobs[k].m_dXCentre) + m_CurrentBlobs[k].m_dYCentre))
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
                    if (PixelList[j].y < (slope*(PixelList[j].x - m_CurrentBlobs[k].m_dXCentre) + m_CurrentBlobs[k].m_dYCentre))
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

        m_CurrentBlobs[k].m_dOrientation = -theta;

        currentblob++;
    }           // end for (k = 0 ; k < numinrawblob ; k++)

    // Record the current set of blobs so we can make estimates next time (if needed)
    m_OldBlobs = m_CurrentBlobs;
    //m_bHasHistory = true;

    TEST_OUT(" -> Done\n");

    // Cleanup
    /*if(numrawblobs)
    {
        delete[] RawBlobSizes;
        delete[] BlobSizes;
        }*/
}       // end function

