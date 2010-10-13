#include "GSThresholder.h"

MT_SparseBinaryImage::MT_SparseBinaryImage(IplImage* from_image)
{
    if(!from_image)
    {
        fprintf(stderr, "MT_SparseBinaryImage Error:  Null input image.\n");
        return;
    }

    if(from_image->nChannels != 1 || from_image->depth != IPL_DEPTH_8U)
    {
        fprintf(stderr, "MT_SparseBinaryImage Error:  Image must be a single-channel image with depth IPL_DEPTH_8U.\n");
        return;
    }

    m_uiWidth = from_image->width;
    m_uiHeight = from_image->height;

    unsigned long n_pix = ((unsigned long) m_uiWidth)*((unsigned long) m_uiHeight);

    m_vulOnIndexes.resize(0);

    for(unsigned long i = 0; i < n_pix; i++)
    {
        uchar v = ((uchar *)from_image->imageData)[i];
        if(v > 0)
        {
            m_vulOnIndexes.push_back(i);
        }
    }
    
}

IplImage* MT_SparseBinaryImage::allocImage()
{
    return cvCreateImage(cvSize(m_uiWidth, m_uiHeight), IPL_DEPTH_8U, 1);
}

bool MT_SparseBinaryImage::convertToIplImage(IplImage* dest, unsigned char off_val, unsigned char on_val)
{
    if(!dest)
    {
        fprintf(stderr, "MT_SparseBinaryImage Error:  Null input image.\n");
        return false;
    }

    if(dest->nChannels != 1 || dest->depth != IPL_DEPTH_8U)
    {
        fprintf(stderr, "MT_SparseBinaryImage Error:  Image must be a single-channel image with depth IPL_DEPTH_8U.\n");
        return false;
    }

    if(dest->width != (int) m_uiWidth || dest->height != (int) m_uiHeight)
    {
        fprintf(stderr, "MT_SparseBinaryImage Error:  Image sizes do not match.");
        return false;
    }

    /* unsigned long n_pix = ((unsigned long) m_uiWidth)*((unsigned
     * long) m_uiHeight); */

    cvSet(dest, cvScalar(off_val));
    for(unsigned long i = 0; i < m_vulOnIndexes.size(); i++)
    {
        ((uchar *)dest->imageData)[m_vulOnIndexes[i]] = on_val;
    }

    return true;
    
}

MT_GSThresholder::MT_GSThresholder(IplImage* bgImage)
    : m_pGSFrame(NULL),
      m_pDiffFrame(NULL),
      m_pThreshFrame(NULL)
{
    setSharedBackground(bgImage);
}

MT_GSThresholder::~MT_GSThresholder()
{
    /* Safely allocate space for the frames we'll need */
    if(m_pGSFrame)
    {
        cvReleaseImage(&m_pGSFrame);
    }
    if(m_pDiffFrame)
    {
        cvReleaseImage(&m_pDiffFrame);
    }
    if(m_pThreshFrame)
    {
        cvReleaseImage(&m_pThreshFrame);
    }

}

bool MT_GSThresholder::setSharedBackground(IplImage* bgImage)
{

    if(!bgImage)
    {
        fprintf(stderr, "MT_GSThresholder Error:  Background image is NULL.\n");
        return false;
    }

    if(bgImage->nChannels != 1)
    {
        fprintf(stderr, "MT_GSThresholder Error:  Background must be a grayscale image.\n");
        return false;
    }

    m_pBGFrame = bgImage;
    
    /* just for convenience in the following */
    CvSize framesize = cvSize(bgImage->width, bgImage->height);

    /* Safely allocate space for the frames we'll need */
    if(m_pGSFrame)
    {
        cvReleaseImage(&m_pGSFrame);
    }
    m_pGSFrame = cvCreateImage(framesize, IPL_DEPTH_8U, 1);

    if(m_pDiffFrame)
    {
        cvReleaseImage(&m_pDiffFrame);
    }
    m_pDiffFrame = cvCreateImage(framesize, IPL_DEPTH_8U, 1);

    if(m_pThreshFrame)
    {
        cvReleaseImage(&m_pThreshFrame);
    }
    m_pThreshFrame = cvCreateImage(framesize, IPL_DEPTH_8U, 1);

    return true;
    
}

MT_SparseBinaryImage MT_GSThresholder::threshToBinary(IplImage* curr_frame,
                                                 unsigned int thresh,
                                                 IplImage* mask_frame)
{
    doThresholding(curr_frame, thresh, mask_frame);
    return MT_SparseBinaryImage(m_pThreshFrame);
}

void MT_GSThresholder::doThresholding(IplImage* curr_frame,
                                   unsigned int thresh,
                                   IplImage* mask_frame)
{

    /* Convert frame to grayscale, if necessary */
    if(curr_frame->nChannels == 3)
    {
        cvCvtColor(curr_frame, m_pGSFrame, CV_BGR2GRAY);
    }
    else
    {
        cvCopy(curr_frame, m_pGSFrame);
    }
    
    /* perform sign-aware background subtraction */
    cvCmp(m_pBGFrame, m_pGSFrame, m_pThreshFrame, CV_CMP_GT);
    cvSub(m_pBGFrame, m_pGSFrame, m_pDiffFrame);
    cvAnd(m_pDiffFrame, m_pThreshFrame, m_pDiffFrame);

    /* apply the ROI mask if available */
    if(mask_frame)
    {
        cvAnd(m_pDiffFrame, mask_frame, m_pDiffFrame);
    }

    /* threshold */
    cvThreshold(m_pDiffFrame,
                m_pThreshFrame,
                thresh,
                255.0,
                CV_THRESH_BINARY);
    
}
