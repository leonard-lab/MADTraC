#ifndef GYBLOBBER_H
#define GYBLOBBER_H

// OpenCV for underlying CV algorithms and structures
#ifdef MT_HAVE_OPENCV_FRAMEWORK
#include <OpenCV/OpenCV.h>
#else
#include <cv.h>
#include <highgui.h>
#endif

#include "MT/MT_Tracking/trackers/GY/GYBlobs.h"
#include "MT/MT_Tracking/trackers/GY/MixGaussians.h"

const int GY_DEFAULT_AREA_THRESH_LOW = 10;
const int GY_DEFAULT_AREA_THRESH_HIGH = 1000;

class GYBlobber
{
public:
    GYBlobber(unsigned int num_obj);

    void doBlobFinding(IplImage* thresh_image);
    void doSegmentation();

    std::vector<GYBlob> findBlobs(IplImage* thresh_image);

    int m_iBlob_area_thresh_low;
    int m_iBlob_area_thresh_high;

    void setNumberOfObjects(unsigned int num_obj);
    unsigned int getNumberOfObjects() const {return m_iNObj;};

    void setSearchArea(CvRect new_search_area);
    void resetSearchArea();

protected:
    
private:
    CvRect m_SearchArea;
    
    std::vector<RawBlobPtr> m_RawBlobData;
    std::vector<GYBlob> m_CurrentBlobs;
    std::vector<GYBlob> m_OldBlobs;

    bool m_bHasHistory;

    unsigned int m_iNObj;
    
};

#endif /* GYBLOBBER_H */    
