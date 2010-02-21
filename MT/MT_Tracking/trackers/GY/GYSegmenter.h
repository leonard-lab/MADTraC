#ifndef GYSEGMENTER_H
#define GYSEGMENTER_H

/*
 *  GYSegmenter.h
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

// OpenCV for underlying CV algorithms and structures
#if defined(__APPLE__) || defined(MACOSX)
#include <OpenCV/OpenCV.h>
#else
#include <cv.h>
#include <highgui.h>
#endif

// Needed for Inspecta S.L.'s OpenCV-based 
//  blob tracking analysis
#include "MT/MT_Tracking/3rdparty/cvBlobsLib/BlobResult.h"

// Includes MT_CBlobGetXCenterOfMass etc. - custom COperadorBlob's
#include "MT/MT_Tracking/cv/MT_BlobExtras.h"

#include "MT/MT_Core/primitives/Matrix.h"
#include "MT/MT_Tracking/base/MT_TrackerBase.h"

#include "GYBlobs.h"
#include "MixGaussians.h"

// for STL vector
#include <vector>
using namespace std;

// no mask supplied to blob finder
#define RT_NOMASK NULL

// default parameters
#define RT_MIN_BLOB_VAL 60     // minimum bg diff value for blob
#define RT_MIN_BLOB_SIZE 5     // minimum area of a blob
#define RT_MAX_BLOB_SIZE 4000  // maximum area of a blob
#define RT_NUM_OBJECTS 10          // number of objects to track

#define NO_BOUNDING_BOXES false
#define USE_BOUNDING_BOXES true

#define NO_MAX -1
#define NO_AREA_THRESH -1

// Approx.threshold for connected components - the bigger it is, the simpler the boundary is
#define CVCONTOUR_APPROX_LEVEL 0


/*********************************************************************************************/

class BlobberFrameGroup : public MT_TrackerFrameGroup
{

public:
    BlobberFrameGroup(IplImage** diff_frame,
                      IplImage** thresh_frame);

};

class BlobberParameters : public MT_DataGroup
{
public: 
    BlobberParameters(int* val_thresh_low, 
                      int* area_thresh_low, 
                      int* area_thresh_high);
};

class BlobInfoReport : public MT_DataReport
{
public:
    BlobInfoReport(std::vector<int>* indexes, 
                   std::vector<double>* Xs, 
                   std::vector<double>* Ys, 
                   std::vector<double>* Areas, 
                   std::vector<double>* Orientations);

};

class BlobberDrawingParameters : public MT_DataGroup
{
public:
    BlobberDrawingParameters(bool* draw_arrows, 
                             bool* draw_ellipses, 
                             bool* draw_search_rect,
                             double* arrow_length,
                             MT_Color* arrow_color,
                             MT_Color* ellipse_color);
};


class GYSegmenter : public MT_TrackerBase
{

protected:
    IplImage* m_pOrg_frame;
    IplImage* m_pBG_frame;
    IplImage* m_pGS_frame;
    IplImage* m_pDiff_frame;
    IplImage* m_pThresh_frame;
    IplImage* m_pROI_frame;

    int m_iBlob_val_thresh;
    int m_iBlob_area_thresh_low;
    int m_iBlob_area_thresh_high;

    int m_iFrameWidth;
    int m_iFrameHeight;

    double m_dFrameRate;
    double m_dAverageFrameRate;
    double m_dT;

    int m_iNobj;

    int m_iFrame_counter;

    bool m_bDrawArrows;
    bool m_bDrawEllipses;
    bool m_bDrawSearchRect;
    double m_dArrowLength;
    MT_Color m_ArrowColor;
    MT_Color m_EllipseColor;

    //GYBlobFile* m_pBlobFile;

    CvRect m_SearchArea;

    std::vector<RawBlobPtr> m_RawBlobData;

    std::vector<GYBlob> m_CurrentBlobs;
    std::vector<GYBlob> m_OldBlobs;

    bool m_bHasHistory;

    void doImageProcessing();
    void doBlobFinding();
    void doSegmentation();

    double updateFrameRate(double dt);

    virtual void doInit(IplImage* ProtoFrame);
    virtual void createFrames();

    virtual void initDataFile();
    virtual void writeData();

    std::vector<int> BlobIndexes;
    std::vector<double> XBlobs;
    std::vector<double> YBlobs;
    std::vector<double> ABlobs;
    std::vector<double> OBlobs;

    void doMatching();

public:
    GYSegmenter(IplImage* ProtoFrame);

    virtual ~GYSegmenter();

    void doTrain(IplImage* frame);

    void doTracking(IplImage* frame);

    void setNumObjects(int numobj);
    void setBlobFile(const char* BlobFilename, const char* description);

    int getNumObjects();
    int getNumFrames();

    double getFrameRate();
    double getAveFrameRate();

    void glDraw(bool DrawBlobs);

    IplImage* getBG_frame();
    IplImage* getGS_frame();
    IplImage* getDiff_frame();
    IplImage* getThresh_frame();
    IplImage* getROI_frame();

    virtual void doGLDrawing(int flags = MT_TB_NO_FLAGS){glDraw(flags > 0);};

    virtual MT_BoundingBox getObjectBoundingBox() const;
};



#endif          // GYSEGMENTER_H
