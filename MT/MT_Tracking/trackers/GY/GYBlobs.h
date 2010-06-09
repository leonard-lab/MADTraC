#ifndef GYBLOBS_H
#define GYBLOBS_H

/*
 *  GYBlobs.h
 *  MADTraC
 *
 *  Blob classes for the use of GYSegmenter
 *
 *  Created by George Young on 5/10/09.
 *  Copyright 2009 Princeton University. All rights reserved.
 *
 */

#if defined(__APPLE__) || defined(MACOSX)
#include <OpenCV/OpenCV.h>
#else
#include <cv.h>
#include <highgui.h>
#endif

#include <vector>

/* Depending on the compiler version, this header is in different places.*/
#ifdef __GNUC__

//#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNU_PATCHLEVEL__)
//#if GCC_VERSION < 40300 /* GCC < 4.3.0 */
#ifndef _WIN32
#include <tr1/memory>

#else /* GCC >= 4.3.0 */
#include <memory>
#endif
#else /* or e.g. VC++ */

#include <memory>

#endif

using namespace std;


/*********************************************************************************************/

class GYBlob
{
public:
    double m_dXCentre;
    double m_dYCentre;
    double m_dArea;
    double m_dOrientation;
    double m_dMajorAxis;
    double m_dMinorAxis;

    GYBlob();
    GYBlob(double x, double y, double A, double phi, double major, double minor);
};


class GYRawBlob
{
protected:
    std::vector<CvPoint> m_vPixelList;
                
    int m_iNumPixels;
                
    double m_dPerimeter;
    double m_dXCentre;
    double m_dYCentre;
                
    bool m_bCalcXCentre;
    bool m_bCalcYCentre;
                
    CvRect m_BoundingBox;
                
public:
    GYRawBlob(int expectedpixels);
                
    CvRect GetBoundingBox();
                
    double GetXCentre();
    double GetYCentre();
    int GetNumPixels();
    double GetArea();
    double GetPerimeter();
    double GetXXMoment();
    double GetXYMoment();
    double GetYYMoment();
    void GetPixelList(std::vector<CvPoint>& pixellist);
                
    void AddPoint(CvPoint newpoint);
    void SetPerimeter(double p);
};


typedef std::tr1::shared_ptr<GYRawBlob> RawBlobPtr;

#endif          // GYBLOBS_H
