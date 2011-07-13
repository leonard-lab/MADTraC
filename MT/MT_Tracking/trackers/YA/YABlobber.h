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

#ifdef MT_HAVE_OPENCV_FRAMEWORK
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
    YABlob(){
        perimeter = area = COMx = COMy = orientation = XX = YY = QX = QY = 0;
        major_axis = minor_axis = 0;
        sequence = NULL; sequence_storage = NULL;
    };
    YABlob(double p, double a, double x, double y, double o, double xx, double xy, double yy, double qx, double qy, double m, double M)
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
        minor_axis = m;
        major_axis = M;
        sequence = NULL;
        sequence_storage = NULL;
    };

    YABlob(const YABlob& in_blob)
        : perimeter(in_blob.perimeter),
          area(in_blob.area),
          COMx(in_blob.COMx),
          COMy(in_blob.COMy),
          orientation(in_blob.orientation),
          XX(in_blob.XX),
          XY(in_blob.XY),
          YY(in_blob.YY),
          QX(in_blob.QX),
          QY(in_blob.QY),
          minor_axis(in_blob.minor_axis),
          major_axis(in_blob.major_axis),          
          sequence(NULL),
          sequence_storage(NULL)
    {
        copySequence(in_blob.sequence);
    }

    YABlob& operator=(const YABlob& in_blob)
    {
        if(this != &in_blob)
        {
            area = in_blob.area;
            COMx = in_blob.COMx;
            COMy = in_blob.COMy;
            orientation = in_blob.orientation;
            XX = in_blob.XX;
            XY = in_blob.XY;
            YY = in_blob.YY;
            QX = in_blob.QX;
            QY = in_blob.QY;
            minor_axis = in_blob.minor_axis;
            major_axis = in_blob.major_axis;            
            sequence = NULL;
            sequence_storage = NULL;
            copySequence(in_blob.sequence);
        }
        return *this;
    }
    
    ~YABlob(){
        if(sequence)
        {
            cvClearSeq(sequence);
        }
        
        if(sequence_storage)
        {
            cvReleaseMemStorage(&sequence_storage);
        }
    };

    void copySequence(const CvSeq* seq){
        if(!seq)
        {
            return;
        }
        
        if(sequence)
        {
            cvClearSeq(sequence);
        }
        if(sequence_storage)
        {
            cvReleaseMemStorage(&sequence_storage);
        }
        sequence_storage = cvCreateMemStorage(0);
        sequence = cvCloneSeq(seq, sequence_storage);
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
    double minor_axis;
    double major_axis;    

    CvSeq* sequence;
    CvMemStorage* sequence_storage;
};

class YABlobber
{
protected:
    bool m_bUseBoundingBoxes;
    std::vector<CvRect> m_BoundingBoxes;
    std::vector<YABlob> m_blobs;
    
public:
    bool m_bCopySequences;
    
    YABlobber(bool UseBoundingBoxes = NO_BOUNDING_BOXES);
    
    // TODO needs a return type
    std::vector<YABlob> FindBlobs(IplImage* BWFrame, 
                                  int MinBlobPerimeter, 
                                  int MinBlobArea = NO_AREA_THRESH,
                                  int MaxBlobPerimeter = NO_MAX,
                                  int MaxBlobArea = NO_MAX);
    
};

#endif // YABLOBBER_H
