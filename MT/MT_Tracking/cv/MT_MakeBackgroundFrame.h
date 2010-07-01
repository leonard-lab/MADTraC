#ifndef MT_MAKEBACKGROUNDFRAME_H
#define MT_MAKEBACKGROUNDFRAME_H

/*
 *  MT_MakeBackgroundFrame.h
 *
 *  Creates a background frame from a capture object by averaging
 *  NFramesToAverage random frames.  If the capture object is
 *  for a movie, the frames are between start and end frame.  If the
 *  capture object is for a camera, the code will wait until 
 *  StartFrame frames and capture frames randomly until
 *  NFramesToAverage are averaged, with the average rate of capture
 *  determined by (EndFrame - StartFrame)/NFramesToAverage.
 *
 *  Returns MT_MAKEBG_ERR (= 0) if there is a problem and MT_MAKEBG_OK (= 1)
 *  if there are no problems.
 *
 *  Created by Daniel Swain on 9/22/09 from original code by Yael Katz.
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
#include <ml.h>
#endif

#include "MT/MT_Tracking/capture/MT_Capture.h"

const int MT_MAKEBG_FIRST_FRAME = 0;
const int MT_MAKEBG_LAST_FRAME = -1;
const unsigned int MT_MAKEBG_DEFAULT_N_TO_AVG = 100;

const int MT_MAKEBG_MODE_STEP = 0;
const int MT_MAKEBG_MODE_AUTO = 1;

const bool MT_MAKEBG_OK = true;
const bool MT_MAKEBG_ERR = false;

const bool MT_MAKEBG_INPAINT = true;
const bool MT_MAKEBG_NO_INPAINT = false;

const unsigned int MT_MAKEBG_DEFAULT_RADIUS = 5;
const CvRect MT_MAKEBG_WHOLE_IMAGE = cvRect(0,0,0,0);

class MT_BackgroundFrameCreator
{
private:
    MT_Capture* m_pCapture;
    IplImage* m_pBackgroundFrame;
    IplImage* m_pBG;
    int m_iNFramesToAverage;
    int m_iStartFrame;
    int m_iEndFrame;
    int m_iStatus;
    double m_dorg_frame_index;
    bool m_bDoInpaint;
    int m_iInpaintRadius;
    CvRect m_InpaintROI;
    
public:
    MT_BackgroundFrameCreator(IplImage* BackgroundFrame, 
                              MT_Capture* Capture, 
                              int Mode = MT_MAKEBG_MODE_AUTO,
                              int NFramesToAverage = MT_MAKEBG_DEFAULT_N_TO_AVG,
                              bool inpaint = MT_MAKEBG_INPAINT,
                              int inpaintradius = MT_MAKEBG_DEFAULT_RADIUS,
                              const CvRect& inpaintroi = MT_MAKEBG_WHOLE_IMAGE,
                              int StartFrame = MT_MAKEBG_FIRST_FRAME,
                              int EndFrame = MT_MAKEBG_LAST_FRAME);
    ~MT_BackgroundFrameCreator();
    
    bool DoStep();
    void Finish();
    int GetStatus() const;
        
};

int MT_MakeBackgroundFrame(IplImage* BackgroundFrame, 
                           MT_Capture* Capture, 
                           int NFramesToAverage,
                           int StartFrame = MT_MAKEBG_FIRST_FRAME,
                           int EndFrame = MT_MAKEBG_LAST_FRAME);

#endif  // MT_MAKEBACKGROUNDFRAME_H
