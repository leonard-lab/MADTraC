#ifndef MT_CAPTURE_INTERFACES_H
#define MT_CAPTURE_INTERFACES_H

/** @addtogroup MT_Tracking
 * @{ */

/** @file
 *  MT_Capture_Intefaces.h
 * 
 *  Defines the capture interface classes used by MT_Capture
 *  to obtain images from files/cameras.  The base class is
 *  MT_Cap_Iface_Base and all interfaces are derived from that.
 *
 *  Created by D. Swain on 2/2/10
 *
 */

#include "MT_Capture.h"
#include "MT_AVTCameraDialog.h"

#include <map>
#include <string>



/*********************************************************************
 *
 * Interface Base Class
 *
 *********************************************************************/

class MT_Cap_Iface_Base
{
    protected:
        MT_FC_MODE_t m_Mode;

        int m_iFrameWidth;
        int m_iNFrames;
        int m_iNChannelsPerFrame;
        int m_iCurrentFrameNumber;
        int m_iFrameHeight;

        bool m_bEndOfCaptureFlag;

        IplImage* m_pCurrentFrame;
		IplImage* m_tmpGrayFrame;

        std::string m_sTitle;

        double m_dFPS;

        virtual void doSafeInit();

    public:
       /* the default ctor should just do a "safe" initialization,
           i.e. create an object that won't break everything if it's
           used accidentally, but also gives an indication that it's
           not a properly initialized object yet */
        MT_Cap_Iface_Base(){doSafeInit();};
        
        /* virtual dtor doesn't really do anything here, but makes sure
           that the interface compiles correctly */
        virtual ~MT_Cap_Iface_Base(){};

        /* virtual initialization functions -
           these are set up so that if you accidentally use a 
           file-oriented function in a camera-oriented interface,
           it will tell you that something is wrong but not break
           everything down. */
        virtual bool initFromFile(const char* filename)
            {doSafeInit(); return false;};
        virtual bool initCamera(int camNumber, int FrameWidth, int FrameHeight, bool ShowDialog, bool FlipH, bool FlipV)
            {doSafeInit(); return false;};

        /* this only makes sense for file-based captures
            - for file-based ifaces return the actually-set frame number
               (i.e. if you asked for x but got y, returns y)
            - for camera-based ifaces returns an error indicator
               (i.e. you asked to do something i can't do) */
        virtual int setFrameNumber(int frame_index)
            { return MT_FC_ERR; };

        /* return a pointer to the current frame, by default returns NULL */
        virtual IplImage* getFrame(int frame_index = MT_FC_NEXT_FRAME)
            { return NULL; };

        /* standard interface accessor functions.  These variables
           should get initialized and maintained in code, not in the
           accessor function calls. */
        MT_FC_MODE_t getMode() const {return m_Mode;};
        void saveFrame(const char* filename) const 
            {cvSaveImage(filename, m_pCurrentFrame);};
        CvSize getFrameSize() const {return cvSize(m_iFrameWidth, m_iFrameHeight);};
        int getFrameWidth() const {return m_iFrameWidth;};
        int getFrameHeight() const {return m_iFrameHeight;};
        const char* getTitle() const {return m_sTitle.c_str();};
        double getProgressFraction() const 
            {return ( (m_iNFrames > 0) ? 
                    ((double) m_iCurrentFrameNumber)/((double) m_iNFrames) :
                    0);};
        double getIsAtEnd() const {return m_bEndOfCaptureFlag;};
        int getNFrames() const {return m_iNFrames;};
        int getNChannels() const {return m_iNChannelsPerFrame;};
        int getFrameNumber() const {return m_iCurrentFrameNumber;};
        double getFPS() const {return m_dFPS;};
        int getFramePeriod_msec() const 
            {return ( (m_dFPS > 0) ? (int) (1000.0/m_dFPS) : -1);};

};

inline void MT_Cap_Iface_Base::doSafeInit()
{
    /* these variables should all get initialized */
    m_Mode = MT_FC_MODE_OFF;
    m_iFrameWidth = m_iFrameHeight = m_iNFrames 
        = m_iNChannelsPerFrame = m_iCurrentFrameNumber = MT_FC_ERR;
    m_dFPS = 0;
    m_bEndOfCaptureFlag = false;
    m_pCurrentFrame = NULL;
    m_sTitle = "Uninitialized Capture";
};

/*********************************************************************
 *
 * OpenCV File/AVI Capture Interface
 *
 *********************************************************************/

class MT_Cap_Iface_CV_File : public MT_Cap_Iface_Base
{
    private:
        CvCapture* m_pCapture;
    protected:
        void doSafeInit();
    public:
        MT_Cap_Iface_CV_File(){doSafeInit();};
        ~MT_Cap_Iface_CV_File();

        bool initFromFile(const char* filename);

        int setFrameNumber(int frame_index);

        IplImage* getFrame(int frame_index);
};


/*********************************************************************
 *
 * OpenCV Camera Interface
 *
 *********************************************************************/

class MT_Cap_Iface_OpenCV_Camera : public MT_Cap_Iface_Base
{
    private:
        CvCapture* m_pCapture;
    protected:
        void doSafeInit();
    public:
        MT_Cap_Iface_OpenCV_Camera();
        ~MT_Cap_Iface_OpenCV_Camera();

        bool initCamera(int camNumber, int FW, int FH, bool ShowDialog, bool FlipH, bool FlipV);
        IplImage* getFrame(int frame_index = MT_FC_NEXT_FRAME);

    static const std::vector<std::string> listOfAvailableCameras(int maxCameras);
};


/*********************************************************************
 *
 * ARToolKit Camera Interface
 *
 *********************************************************************/

class MT_Cap_Iface_ARToolKit_Camera : public MT_Cap_Iface_Base
{
    private:
        /* intermediate frames for rearranging the pixel format */
        IplImage* m_pRawFrame, *m_pRframe, *m_pGframe, *m_pBframe, *m_pAframe;
    protected:
        void doSafeInit();
    public:
        MT_Cap_Iface_ARToolKit_Camera();
        ~MT_Cap_Iface_ARToolKit_Camera();

        bool initCamera(int camNumber, int FW, int FH, bool ShowDialog, bool FlipH, bool FlipV);
        IplImage* getFrame(int frame_index = MT_FC_NEXT_FRAME);

    static const std::vector<std::string> listOfAvailableCameras(int maxCameras);
};


/*********************************************************************
 *
 * AVT Camera Interface
 *
 *********************************************************************/

#ifdef MT_HAVE_AVT
#include <FGCamera.h>
#endif

class MT_Cap_Iface_AVT_Camera : public MT_Cap_Iface_Base
{
    private:
#ifdef MT_HAVE_AVT
        CFGCamera m_Camera;
#endif
    protected:
        void doSafeInit();
    public:
        MT_Cap_Iface_AVT_Camera();
        ~MT_Cap_Iface_AVT_Camera();

        bool initCamera(int camNumber, int FW, int FH, bool ShowDialog, bool FlipH, bool FlipV);
        IplImage* getFrame(int frame_index = MT_FC_NEXT_FRAME);

    static const std::vector<std::string> listOfAvailableCameras(int maxCameras);
};

/* @} */

#endif /* MT_CAPTURE_INTERFACES_H */
