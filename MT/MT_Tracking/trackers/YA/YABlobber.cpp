/*
 *  YABlobber.cpp
 *
 *  Created by Daniel Swain on 9/22/09.
 *
 */

#include "YABlobber.h"
#include "MT/MT_Core/support/mathsupport.h"

YABlobber::YABlobber(bool UseBoundingBoxes)
{
    m_bUseBoundingBoxes = UseBoundingBoxes;
    m_BoundingBoxes.resize(0);
    m_blobs.resize(0);
}

std::vector<YABlob> YABlobber::FindBlobs(IplImage* BWFrame, 
                                         int MinBlobPerimeter, 
                                         int MinBlobArea,
                                         int MaxBlobPerimeter,
                                         int MaxBlobArea)
{
  
    double tA = MT_getTimeSec();
    double tB;
  
    m_blobs.resize(0);
    std::vector<double> perimeters;
    std::vector<double> areas;
    perimeters.resize(0);
    areas.resize(0);
  
    static CvMemStorage* mem_storage = cvCreateMemStorage(0);
    static CvSeq* contours = NULL;
  
    if(mem_storage)
    {
        cvClearMemStorage(mem_storage);
    }
  
    CvContourScanner scanner = cvStartFindContours(BWFrame,
                                                   mem_storage,
                                                   sizeof(CvContour),
                                                   CV_RETR_EXTERNAL,
                                                   CV_CHAIN_APPROX_SIMPLE);
  
    // Throw out contours that are too small and approximate rest with polygons 
    CvSeq* cs;
    int NFilteredContours = 0;
    double perimeter, area;
  
    tB = MT_getTimeSec();
    ////printf("Start Contour %f\n", tB-tA);
    tA = tB;
  
    while( (cs = cvFindNextContour( scanner )) != NULL){
    
        perimeter = cvContourPerimeter( cs );
        area = fabs(cvContourArea(cs));
    
        // Get rid of blobs that don't fit our criteria
        if (perimeter < MinBlobPerimeter)  /* blob filtering */
        {
            cvSubstituteContour( scanner, NULL );
        }
        else if((area == 0) || ((MinBlobArea != NO_AREA_THRESH) && (area < MinBlobArea)))
        {
            cvSubstituteContour(scanner, NULL);
        }
        else if((MaxBlobPerimeter != NO_MAX) && (perimeter > MaxBlobPerimeter))
        {
            cvSubstituteContour(scanner, NULL);
        }
        else if((MaxBlobArea != NO_MAX) && (area > MaxBlobArea))
        {
            cvSubstituteContour(scanner, NULL);
        } 
        else /* done filtering blobs */
        {
      
            // Store the perimeter and area for output
            perimeters.push_back(perimeter);
            areas.push_back(area);
      
            // Smooth its edges if large enough
            CvSeq* cs_new;
            // Polygonal approximation
            cs_new = cvApproxPoly(
                cs,
                sizeof(CvContour),
                mem_storage,
                CV_POLY_APPROX_DP,
                CVCONTOUR_APPROX_LEVEL,
                0
                );
            cvSubstituteContour( scanner, cs_new );
      
            NFilteredContours++;
        }
    }
    tB = MT_getTimeSec();
    //printf("Filter Contour %f\n", tB-tA);
    tA = tB;
  
    contours = cvEndFindContours( &scanner );
  
    // Some convenience variables
    const CvScalar CVX_WHITE = CV_RGB(0xff, 0xff, 0xff);
    const CvScalar CVX_BLACK = CV_RGB(0x00, 0x00, 0x00);
  
    // Paint the found regions back into the image
    cvZero( BWFrame ); // sets the array to zero
    IplImage* maskTemp = cvCreateImage( cvGetSize(BWFrame), IPL_DEPTH_8U, 1 ); 
    CvScalar cont_color = cvScalar( 255, 0, 0 );
  
    // CALC CENTRE OF MASS AND/OR BOUNDING RECTANGLES
    /*  if (num != NULL) {
        int N = *num, numFilled = 0, i = 0;*/
  
    CvMoments moments;
    CvScalar zfZero = cvScalar( 0.0 );
    double M00, M01, M10, M11, M02, M20, M12, M21, M30, M03;
    double mu11, mu20, mu02, mu12, mu21, mu30, mu03;
    double s;
    double orientangle;
    double qx, qy;
    double x, y;
    cvZero( maskTemp );
  
    int i;
    int numFilled = 0;
    double t0 = MT_getTimeSec();
    double tz;
    double ta = 0, tb = 0, tc = 0, td = 0, te = 0, tf = 0;
  
    tB = MT_getTimeSec();
    //printf("Middle %f\n", tB-tA);
    tA = tB;
    for (i = 0, cs = contours; cs != NULL; cs = cs->h_next, i++)
    {
    
        tz = MT_getTimeSec();
        // Only process up to *num of them
        cvDrawContours(
            maskTemp,
            cs,
            CVX_WHITE,
            CVX_WHITE,
            -1,
            CV_FILLED,
            8
            );
        ta += MT_getTimeSec() - tz;
        tz = MT_getTimeSec();
    
        // Update color
        cont_color.val[0] -= 5; 
    
        CvRect BB = cvBoundingRect(cs);
        int x1 = BB.x;
        int y1 = BB.y;
        int w = BB.width;
        int h = BB.height;
        
        BB.x = x1 - 0.5*w;
        BB.y = y1 - 0.5*w;
        BB.width = 2*w;
        BB.height = 2*w;
    
        cvSetImageROI(maskTemp, BB);
        /*cvDilate( maskTemp, maskTemp );
          cvErode( maskTemp, maskTemp );*/
        tb += MT_getTimeSec() - tz;
        tz = MT_getTimeSec();
    
        // Fill in the stats for each contour
        //if(centers != NULL && areas != NULL && orientAngle != NULL){
        cvMoments(cs, &moments, 1);
        tc += MT_getTimeSec() - tz;
        tz = MT_getTimeSec();
    
        // Spatial moments
        M00 = cvGetSpatialMoment(&moments, 0, 0);
        M10 = cvGetSpatialMoment(&moments, 1, 0);
        M01 = cvGetSpatialMoment(&moments, 0, 1); 
        M11 = cvGetSpatialMoment(&moments, 1, 1); 
        M20 = cvGetSpatialMoment(&moments, 2, 0);
        M02 = cvGetSpatialMoment(&moments, 0, 2);                               
        M12 = cvGetSpatialMoment(&moments, 1, 2);
        M21 = cvGetSpatialMoment(&moments, 2, 1);
        M30 = cvGetSpatialMoment(&moments, 3, 0);
        M03 = cvGetSpatialMoment(&moments, 0, 3);
        td += MT_getTimeSec() - tz;
        tz = MT_getTimeSec();
    
        // Center of each contour
        x = M10/M00;// + ((double) BB.x); // row/x  *indexed from top left, rows are x*
        y = M01/M00;// + ((double) BB.y); // column/y
        
        // Area and perimeter of each contour
        //areas[i] = M00;
        //perimeters[i] = lenstore[numCont-1-i];
    
        // Orientation of each contour
    
        // Central moments
        mu11 = M11/M00 - (M10/M00)*(M01/M00);
        mu20 = M20/M00 - (M10/M00)*(M10/M00);
        mu02 = M02/M00 - (M01/M00)*(M01/M00);
        mu12 = M12 - 2.0*(M01/M00)*M11 - (M10/M00)*M02 + 2.0*(M01/M00)*(M01/M00)*M10;
        mu21 = M21 - 2.0*(M10/M00)*M11 - (M01/M00)*M20 + 2.0*(M10/M00)*(M10/M00)*M01;
        mu30 = M30 - 3.0*(M10/M00)*M20 + 2.0*(M10/M00)*(M10/M00)*M10;
        mu03 = M03 - 3.0*(M01/M00)*M02 + 2.0*(M01/M00)*(M01/M00)*M01;
    
        orientangle =  180.0 - MT_RAD2DEG * 0.5*atan2( (mu11*2.0), (mu20-mu02) );
    
        qx = cos(MT_DEG2RAD*orientangle);
        qy = -sin(MT_DEG2RAD*orientangle);
    
        s = mu30*qx*qx*qx + 3.0*mu21*qx*qx*qy + 3.0*mu12*qx*qy*qy + mu03*qy*qy*qy;
    
        if (s > 0)      {
            orientangle += 180.0;
            qx = cos(MT_DEG2RAD*orientangle);
            qy = -sin(MT_DEG2RAD*orientangle);
        }
        //printf("%f, %f, %f\n", x, y, orientangle);
        
        // normalize orientation vectors
        /* why where you doing this?
           double mag = sqrt( orientVec[i][0]*orientVec[i][0] + orientVec[i][1]*orientVec[i][1] );
           orientVec[i][0] = orientVec[i][0]/mag;
           orientVec[i][1] = orientVec[i][1]/mag; */
    
        // Bounding rectangles around blobs
        if(m_bUseBoundingBoxes){
            m_BoundingBoxes.push_back(cvBoundingRect(cs));
        }
    
        te += MT_getTimeSec() - tz;
        tz = MT_getTimeSec();
    
        cvZero(maskTemp);
        cvResetImageROI(maskTemp);
        numFilled++;
    
    
        // Draw filled contours onto mask
        cvDrawContours(
            BWFrame,
            cs,
            cont_color,
            cont_color,
            -1,
            CV_FILLED,
            8               
            ); 
        tf += MT_getTimeSec() - tz;
        tz = MT_getTimeSec();
    
        ////printf("area %f, perimeter %f\n", areas[i], perimeters[i]);
        m_blobs.push_back(YABlob(perimeters[i], areas[i], x, y, orientangle, mu20, mu11, mu02, qx, qy));
    
        // end looping over contours
        //*num = numFilled;
    }
    //printf("ta = %f\n", ta);
    //printf("tb = %f\n", tb);
    //printf("tc = %f\n", tc);
    //printf("td = %f\n", td);
    //printf("te = %f\n", te);
    //printf("tf = %f\n", tf);
    //printf("dt = %f\n", MT_getTimeSec() - t0);
  
    //free(lenstore);
    cvReleaseImage( &maskTemp );  
    if( mem_storage != NULL ) cvClearMemStorage( mem_storage );
    if( contours != NULL ) cvClearSeq( contours );
  
    return m_blobs;
  
}
