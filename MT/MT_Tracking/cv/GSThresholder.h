#ifndef GSTHRESHOLDER_H
#define GSTHRESHOLDER_H

#include <vector>

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
#include <ml.h>
#endif

class MT_SparseBinaryImage
{
public:
    MT_SparseBinaryImage(IplImage* from_image);

    IplImage* allocImage();
    bool convertToIplImage(IplImage* dest,
                           unsigned char off_val = 0,
                           unsigned char on_val = 255);
    
private:
    std::vector<unsigned long> m_vulOnIndexes;
    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
};

class MT_GSThresholder
{
public:
    MT_GSThresholder(IplImage* bgImage);
    ~MT_GSThresholder();

    bool setSharedBackground(IplImage* bgImage);
    
    MT_SparseBinaryImage threshToBinary(IplImage* curr_frame,
                                     unsigned int thresh,
                                     IplImage* mask_frame = NULL);

    void doThresholding(IplImage* curr_frame,
                        unsigned int thresh,
                        IplImage* mask_frame = NULL);

    IplImage* getGSFrame(){return m_pGSFrame;};
    IplImage* getDiffFrame(){return m_pDiffFrame;};
    IplImage* getThreshFrame(){return m_pThreshFrame;};
    
private:
    IplImage* m_pBGFrame;      /* Background frame - SHARED */
    IplImage* m_pGSFrame;      /* Grayscale version of current frame */
    IplImage* m_pDiffFrame;    /* Background subtracted frame */
    IplImage* m_pThreshFrame;  /* Thresholded frame */

};

#endif /* GSTHRESHOLDER_H */
