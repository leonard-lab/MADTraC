#ifndef MT_DSGYA_SEGMENTER_H
#define MT_DSGYA_SEGMENTER_H

#include <vector>

#include "MT/MT_Core/support/filesupport.h"
#include "MT/MT_Tracking/trackers/YA/YABlobber.h"
#include "MT/MT_Tracking/trackers/GY/GYBlobs.h"

class MT_DSGYA_Blob
{
public:
    MT_DSGYA_Blob();
    MT_DSGYA_Blob(double x,
               double y,
               double xx,
               double xy,
               double yy,
               double m,
               double M,
               double a,
               double o);
    MT_DSGYA_Blob(const YABlob& in_blob);
    MT_DSGYA_Blob(const GYBlob& in_blob);

    double m_dXCenter;
    double m_dYCenter;
    double m_dXXMoment;
    double m_dXYMoment;
    double m_dYYMoment;
    double m_dMinorAxis;
    double m_dMajorAxis;
    double m_dArea;
    double m_dOrientation;
    double m_dRhoContrib;
};

bool MT_writeDSGYABlobsToFile(const std::vector<MT_DSGYA_Blob>& blobs, const char* filename);
std::vector<MT_DSGYA_Blob> MT_readDSGYABlobsFromFile(const char* filename);

class MT_DSGYA_Segmenter
{
public:
    MT_DSGYA_Segmenter();
    ~MT_DSGYA_Segmenter();
    
    virtual std::vector<MT_DSGYA_Blob> segmentFirstFrame(const IplImage* I,
                                                      unsigned int num_objs);
    std::vector<MT_DSGYA_Blob> doSegmentation(const IplImage* I,
                                           const std::vector<MT_DSGYA_Blob>& in_blobs);

    std::vector<unsigned int> getAssignmentMatrix(unsigned int* rows = NULL,
                                                  unsigned int* cols = NULL)
        const
    {
        if(rows){*rows = m_iAssignmentRows;}
        if(cols){*cols = m_iAssignmentCols;}
        return m_viAssignmentMat;
    };
    std::vector<unsigned int> getAssignmentVector(unsigned int* rows = NULL,
                                                  unsigned int* cols = NULL)
        const
    {
        if(rows){*rows = m_iAssignmentRows;}
        if(cols){*cols = m_iAssignmentCols;}
        return m_viAssignmentVec;
    };    
    std::vector<YABlob> getInitialBlobs() const {return m_vInitBlobs;};
    
    unsigned int m_iMinBlobArea;
    unsigned int m_iMinBlobPerimeter;
    unsigned int m_iMaxBlobArea;
    unsigned int m_iMaxBlobPerimeter;
    double m_dOverlapFactor;

    void setDebugFile(FILE* file);
protected:
    virtual bool areAdjacent(MT_DSGYA_Blob* obj, const YABlob& blob);
    virtual bool areOverlapping(MT_DSGYA_Blob* obj, const YABlob& blob);
    virtual void usePrevious(MT_DSGYA_Blob* obj, unsigned int i);
    virtual std::vector<YABlob> filterFirstBlobs(const std::vector<YABlob>& in_blobs);

    std::vector<unsigned int> m_viAssignmentMat;
    std::vector<unsigned int> m_viAssignmentVec;    
    std::vector<YABlob> m_vInitBlobs;
    unsigned int m_iAssignmentRows;
    unsigned int m_iAssignmentCols;
    
private:
    unsigned int m_iFrameWidth;
    unsigned int m_iFrameHeight;
    IplImage* m_pBlobFrame;

    YABlobber m_YABlobber;

    FILE* m_pDebugFile;
};

#endif // MT_DSGYA_SEGMENTER_H
