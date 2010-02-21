#ifndef YABLOBBER_H
#define YABLOBBER_H

/*
 *  YABlobber.h
 *
 *  Efficient blobber designed by Yael Katz and Adrien De Froment.
 *
 *  Created by Daniel Swain on 9/22/09.
 *
 */

#if defined(__APPLE__) || defined(MACOSX)
#include <OpenCV/OpenCV.h>
#include <OpenCV/cvcompat.h>
#include <OpenCV/cvaux.h>
#include <OpenCV/cv.h>
#include <OpenCV/highgui.h>

#else
// Includes for OpenCV with Windows
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>
//#include <ml.h>
#endif
#include <vector>
using namespace std;

#define NO_BOUNDING_BOXES false
#define USE_BOUNDING_BOXES true

#define NO_MAX -1
#define NO_AREA_THRESH -1

// Approx.threshold for connected components - the bigger it is, the simpler the boundary is
#define CVCONTOUR_APPROX_LEVEL 0

class YABlob
{
public:
    YABlob(){ perimeter = area = COMx = COMy = orientation = XX = YY = QX = QY = 0;};
    YABlob(double p, double a, double x, double y, double o, double xx, double xy, double yy, double qx, double qy)
    {
        perimeter = p;
        area = a;
        COMx = x;
        COMy = y;
        orientation = o;
        XX = xx;
        XY = xy;
        YY = yy;
        QX = qx;
        QY = qy;
    };
    
    double perimeter;
    double area;
    double COMx;
    double COMy;
    double orientation;
    double XX;
    double XY;
    double YY;
    double QX;
    double QY; 
};

class YABlobber
{
protected:
    bool m_bUseBoundingBoxes;
    std::vector<CvRect> m_BoundingBoxes;
    std::vector<YABlob> m_blobs;
    
public:
    YABlobber(bool UseBoundingBoxes = NO_BOUNDING_BOXES);
    
    // TODO needs a return type
    std::vector<YABlob> FindBlobs(IplImage* BWFrame, 
                                  int MinBlobPerimeter, 
                                  int MinBlobArea = NO_AREA_THRESH,
                                  int MaxBlobPerimeter = NO_MAX,
                                  int MaxBlobArea = NO_MAX);
    
};

#endif // YABLOBBER_H
