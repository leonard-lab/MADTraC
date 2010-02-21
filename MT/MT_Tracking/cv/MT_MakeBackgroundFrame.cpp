/*
 *  MT_MakeBackgroundFrame.cpp
 *
 */

#include "MT_MakeBackgroundFrame.h"

#include "MT/MT_Core/support/mathsupport.h"

static void do_inpainting(IplImage* frame, int radius, const CvRect& roi);

MT_BackgroundFrameCreator::MT_BackgroundFrameCreator(IplImage* BackgroundFrame, 
                                                     MT_Capture* Capture, 
                                                     int Mode,
                                                     int NFramesToAverage,
                                                     bool inpaint,
                                                     int inpaintradius,
                                                     const CvRect& inpaintroi,
                                                     int StartFrame,
                                                     int EndFrame)
{
    m_pCapture = Capture;
    m_pBackgroundFrame = BackgroundFrame;
    m_iNFramesToAverage = NFramesToAverage;
    m_iStartFrame = StartFrame;
    m_iEndFrame = EndFrame;
  
    m_bDoInpaint = inpaint;
    m_iInpaintRadius = inpaintradius;
  
    if(inpaintroi.width == 0 || inpaintroi.height == 0)
    {
        m_InpaintROI = cvRect(0, 0, BackgroundFrame->width, BackgroundFrame->height);
    }
    else
    {
        m_InpaintROI = inpaintroi;
    }
  
    // check to make sure the capture is OK
    if(!Capture || (Capture->getMode() == MT_FC_MODE_OFF))
    {
        // not OK
        m_iStatus = MT_MAKEBG_ERR;
        return;
    }
  
    // for movie mode, make sure the frame numbers are OK
    if(Capture->getMode() == MT_FC_MODE_AVI)
    {
        // can't average more frames than are in the movie!
        if(m_iNFramesToAverage > Capture->getNFrames())
        {
            m_iNFramesToAverage = Capture->getNFrames();
        }
        // set LAST_FRAME to right value
        if(m_iEndFrame == MT_MAKEBG_LAST_FRAME)
        {
            m_iEndFrame = Capture->getNFrames();
        }
        // if we're asking for more frames than are in the span
        if((m_iEndFrame - m_iStartFrame) < m_iNFramesToAverage)
        {
            m_iNFramesToAverage = m_iEndFrame - m_iStartFrame;
        } 
    }
  
    // we're going to create a background frame that is 32bits depth
    //  so the averaging goes smoothly.  we'll convert it later to match
    //  the format provided by BackgroundFrame
    m_pBG = cvCreateImage( Capture->getFrameSize(), IPL_DEPTH_32F, Capture->getNChannels());
    // make sure the frame is zeroed initially
    cvZero(m_pBG);
  
    if(Capture->getMode() == MT_FC_MODE_AVI)
    {
        m_dorg_frame_index = Capture->getFrameNumber();
    }
  
    m_iStatus = MT_MAKEBG_OK;
    if(Mode != MT_MAKEBG_MODE_STEP)
    {
    
        for(int i = 0; i < NFramesToAverage; i++)
        {
            DoStep();
        }
    
        Finish();
    
    }
  
}

bool MT_BackgroundFrameCreator::DoStep()
{
  
  
    if(m_iStatus == MT_MAKEBG_ERR)
    {
        return false;
    }
  
    IplImage* Frame;  // this memory is managed by the capture object
  
    if(m_pCapture->getMode() == MT_FC_MODE_AVI)
    {
        // generate a random frame index
        double frame_index = floor(MT_frandr(m_iStartFrame, m_iEndFrame));

        // get the frame
        Frame = m_pCapture->getFrame(frame_index);
    
        if(m_bDoInpaint)
        {
            do_inpainting(Frame, m_iInpaintRadius, m_InpaintROI);
        }
      
        // accumulate this frame into the BG frame (divide at end)
        cvAcc(Frame, m_pBG);
    }
  
    if(m_pCapture->getMode() == MT_FC_MODE_CAM)
    {
        /* for now all we'll do is get consecutive frames */
        Frame = m_pCapture->getFrame();

        if(m_bDoInpaint)
        {
            do_inpainting(Frame, m_iInpaintRadius, m_InpaintROI);
        }
      
        // accumulate this frame into the BG frame (divide at end)
        cvAcc(Frame, m_pBG);
    }
  
    return true;
  
}
   
void MT_BackgroundFrameCreator::Finish()
{
  
    // divide by the number of frames averaged and store in the real background
    cvConvertScale(m_pBG, m_pBackgroundFrame, ((double)(1.0/m_iNFramesToAverage)));

    if(m_bDoInpaint)
    {
        do_inpainting(m_pBackgroundFrame, m_iInpaintRadius, m_InpaintROI);
    }
  
    if(m_pCapture->getMode() == MT_FC_MODE_AVI)
    {
        // done!  clean up and return
        m_pCapture->setFrameNumber(m_dorg_frame_index);
    }
    
}

MT_BackgroundFrameCreator::~MT_BackgroundFrameCreator()
{
  
    cvReleaseImage(&m_pBG);
  
}

int MT_BackgroundFrameCreator::GetStatus() const
{
  
    return m_iStatus;
  
}

int MT_MakeBackgroundFrame(IplImage* BackgroundFrame, 
                           MT_Capture* Capture, 
                           int NFramesToAverage,
                           int StartFrame,
                           int EndFrame)
{

    MT_BackgroundFrameCreator* MakeBG = new MT_BackgroundFrameCreator(BackgroundFrame,
                                                                      Capture,
                                                                      MT_MAKEBG_MODE_AUTO,
                                                                      NFramesToAverage,
                                                                      StartFrame,
                                                                      EndFrame);
    int r = MakeBG->GetStatus();
  
    delete MakeBG;
  
    return r;
  
}

static void do_inpainting(IplImage* frame, int radius, const CvRect& roi)
{
    IplImage* g = cvCreateImage(cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1);
    IplImage* m = cvCreateImage(cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1);
    cvZero(m);

    cvCvtColor(frame, g, CV_RGB2GRAY);

    cvSetImageROI(g, roi);
    cvSetImageROI(m, roi);

    double min, max;

    cvMinMaxLoc(g, &min, &max);

    cvThreshold(g, m, 0.4*(max + min), 255, CV_THRESH_BINARY);

    cvResetImageROI(m);

    cvInpaint(frame, m, frame, radius, CV_INPAINT_TELEA);

    cvReleaseImage(&g);
    cvReleaseImage(&m);
}
