#ifndef SEGMENTER_H
#define SEGMENTER_H

/*
 *  Segmenter.h
 *
 *  A Tracker class for tracking robots in a grayscale frame using
 *  background subtraction.
 *
 *  Original - DTS 12/31/07.
 *  Modified into Segmenter DTS 8/12/09
 *
 */

/* TODO this file will not compile - there are too many "old" pieces of code */

// OpenCV for underlying CV algorithms and structures
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

// Needed for Inspecta S.L.'s OpenCV-based 
//  blob tracking analysis
#include "MT/MT_Tracking/3rdparty/cvBlobsLib/BlobResult.h"

// Includes MT_CBlobGetXCenterOfMass etc. - custom COperadorBlob's
#include "MT/MT_Tracking/cv/MT_BlobExtras.h"

// #include "fileio/BlobFile.h"

#include "YABlobber.h"

#include "cv/TrackerCore.h"

// for STL vector & string
#include <vector>
#include <string>
using namespace std;

// no mask supplied to blob finder
#define RT_NOMASK NULL

// default parameters
#define RT_MIN_BLOB_VAL 40     // minimum bg diff value for blob
#define RT_MAX_BLOB_VAL 255    // max "" (set to max of 255)
#define RT_MIN_BLOB_SIZE 5     // minimum area of a blob
#define RT_MAX_BLOB_SIZE 4000  // maximum area of a blob

// compile-time options
// uncomment to get verbose reporting to stdout
// #define TRACKER_VERBOSE
// uncomment to get verbose reporting of match results to stdout
//#define TRACKER_VERBOSE_MATCHING

/*************************************************************/

class BlobberFrameGroup : public TrackerFrameGroup
{
    
public:
    BlobberFrameGroup(IplImage** diff_frame,
                      IplImage** thresh_frame);
    
};

class BlobberParameters : public MT_DataGroup
{
public: 
    BlobberParameters(int* val_thresh_low, 
                      int* val_thresh_high, 
                      int* area_thresh_low, 
                      int* area_thresh_high, 
                      bool* testbool, 
                      double* testdouble);
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

class Segmenter : public TrackerCore
{
    
protected:
    IplImage* org_frame;
    IplImage* BG_frame;
    IplImage* GS_frame;
    IplImage* diff_frame;
    IplImage* thresh_frame;
    IplImage* ROI_frame;
    
    int blob_val_thresh_low;
    int blob_val_thresh_high;
    int blob_area_thresh_low;
    int blob_area_thresh_high;
    
    bool test_bool;
    double test_double;
    
    int FrameWidth;
    int FrameHeight;
        
    CBlobResult blobs;
        
    int frame_counter;
    
    BlobFile* m_pBlobFile;
    
    std::vector<int> BlobIndexes;
    std::vector<double> XBlobs;
    std::vector<double> YBlobs;
    std::vector<double> ABlobs;
    std::vector<double> OBlobs;
    
    std::vector<YABlob> m_YABlobs;
    
    virtual void Init(IplImage* ProtoFrame);
    virtual void CreateFrames();
    
    /***************************************/
    // These functions split up the tasks of the tracker into easier-to-edit chunks
    //   They don't take input arguments or return anything, rather they work with
    //   the class's member data.  Regardless it's useful to think of these in the sense
    //   of what they take as "input" and what they produce as "output".
    
    /* DoImageProcessing takes the current frame and produces a black and white
       (i.e. binary) image that is set to 1 where there is high likelihood of a blob and
       set to 0 where there is low likelihood of a blob. */
    void DoImageProcessing();
    
    /* DoSegmentation takes the black and white image from DoImageProcessing and
       does blob detection.  These blobs are then filtered based on allowed sizes, etc. */
    void DoSegmentation();
    
    /* DoMeasurement does the data association step.  That is, it takes the blobs
       found in DoSegmentation and figures out which of the myTrackedObjects to assign
       the measurements of each blob to. */
    void DoMeasurement();
    
    /***************************************/ 
    YABlobber m_Blobber;
    void DoMeasurementWithYABlobs();
    
public:
    /* The constructor needs a prototype frame for frame sizes, etc */
    Segmenter(IplImage* ProtoFrame);
    /* The destructor needs to release all of the frames. */
    ~Segmenter();
    
    // Function for background training - sets the given frame as the background
    //   frame from which subtraction is computed
    void Train(IplImage* frame);
    
    /* Main workhorse function of the tracker that should get called externally
       during each time step with the most current frame.  */
    void DoTracking(IplImage* frame);
    
    // Functions to set various parameters
    //    void SetNExpected(int newNExpected);  // <- Not clear at this point how to do this nicely
    void SetDiffThreshLow(int newThresh);
    void SetAreaThreshLow(int newThresh);
    void SetAreaThreshHigh(int newThresh);
    void SetBlobFile(const char* blobfilename, const char* description);
    
    // Functions to query various parameters
    int GetDiffThreshLow();
    int GetAreaThreshLow();
    int GetAreaThreshHigh();
    
    // function to draw (with flag for drawing tracked objects and/or blobs)
    void glDraw(bool DrawBlobs);
    
    // functions to access the various frames    
    IplImage* GetBG_frame(){ return BG_frame;};
    IplImage* GetGS_frame(){ return GS_frame;};
    IplImage* Getdiff_frame(){ return diff_frame;};
    IplImage* Getthresh_frame(){ return thresh_frame;};
    
    virtual void DoGLDrawing(int flags = TC_NO_FLAGS);
    
};


#endif   // SEGMENTER_H
