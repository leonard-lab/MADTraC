#include "MT_Capture_Interfaces.h"

#include "MT/MT_Core/support/filesupport.h"


/*********************************************************************
 *
 * OpenCV File/AVI Iface
 *
 *********************************************************************/

MT_Cap_Iface_CV_File::~MT_Cap_Iface_CV_File()
{
    if(m_pCapture)
    {
        cvReleaseCapture(&m_pCapture);
    }
    if(m_pCurrentFrame)
    {
        cvReleaseImage(&m_pCurrentFrame);
    }
}

void MT_Cap_Iface_CV_File::doSafeInit()
{
    MT_Cap_Iface_Base::doSafeInit();
    m_pCapture = NULL;
}

bool MT_Cap_Iface_CV_File::initFromFile(const char* filename)
{
    fprintf(stdout, "Opening video file %s...\n", filename);

    m_pCapture = cvCreateFileCapture(filename);

    if(!m_pCapture)
    {
        fprintf(stderr, "Could not open video file %s\n", filename);
        m_Mode = MT_FC_MODE_OFF;
        return false;
    }

    m_iFrameHeight = (int) cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_FRAME_HEIGHT);
    m_iFrameWidth = (int) cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_FRAME_WIDTH);
    m_iNFrames = (int) cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_FRAME_COUNT);
    m_dFPS = cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_FPS);
    m_iCurrentFrameNumber = (int) cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);

    fprintf(stdout, 
            "Loaded file %s\n  With %d frames of size %dx%d at %lf FPS\n",
            filename,
            m_iNFrames,
            m_iFrameWidth,
            m_iFrameHeight,
            m_dFPS);
    
    m_Mode = MT_FC_MODE_AVI;

    /* have to defer setting of m_iNChannelsPerFrame until we have a frame */

    m_sTitle = std::string(filename);

    return true;

}

int MT_Cap_Iface_CV_File::setFrameNumber(int frame_index)
{
    /* doubles as a check for proper initialization */
    if(m_Mode == MT_FC_MODE_AVI)
    {
        cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES, frame_index);
        m_iCurrentFrameNumber = 
            (int) cvGetCaptureProperty(m_pCapture, CV_CAP_PROP_POS_FRAMES);
    }
    return m_iCurrentFrameNumber;
}

IplImage* MT_Cap_Iface_CV_File::getFrame(int frame_index)
{
    if(m_Mode != MT_FC_MODE_AVI)
    {
        /* indication that something is wrong, so bail */
        m_Mode = MT_FC_MODE_OFF;
        return NULL;
    }

    if(frame_index == MT_FC_NEXT_FRAME)
    {
        m_iCurrentFrameNumber++;
    }
    else
    {
        m_iCurrentFrameNumber = setFrameNumber(frame_index);
    }

    /* the capture owns the memory pointed to by the
       returned image, which could be somewhat volatile, so
       we should create our own local copy */
    IplImage* raw_frame = cvQueryFrame(m_pCapture);

    if(!m_pCurrentFrame)
    {
        m_pCurrentFrame = cvCloneImage(raw_frame);
        m_iNChannelsPerFrame = m_pCurrentFrame->nChannels;
    }
    else
    {
        cvCopy(raw_frame, m_pCurrentFrame);
    }

    return m_pCurrentFrame;

}


/*********************************************************************
 *
 * OpenCV Camera Iface
 *
 *********************************************************************/

const std::vector<std::string>MT_Cap_Iface_OpenCV_Camera::
listOfAvailableCameras(int maxCameras)
{
    std::vector<std::string> result;
    result.resize(0);

    for(int i = 0; i < maxCameras; i++)
    {
        std::stringstream ss;
        ss << "OpenCV Camera " << i;
        
        result.push_back(std::string(ss.str()));
    }

    return result;
}

void MT_Cap_Iface_OpenCV_Camera::doSafeInit()
{
    MT_Cap_Iface_Base::doSafeInit();
    m_pCapture = NULL;
}

MT_Cap_Iface_OpenCV_Camera::MT_Cap_Iface_OpenCV_Camera()
{
    doSafeInit();
}

MT_Cap_Iface_OpenCV_Camera::~MT_Cap_Iface_OpenCV_Camera()
{
    if(m_pCapture)
    {
        cvReleaseCapture(&m_pCapture);
    }
    if(m_pCurrentFrame)
    {
        cvReleaseImage(&m_pCurrentFrame);
    }
}

bool MT_Cap_Iface_OpenCV_Camera::initCamera(int camNumber, int FW, int FH, bool ShowDialog, bool FlipH, bool FlipV)
{
	if(camNumber == MT_FC_NEXT_CAMERA)
	{
		camNumber = CV_CAP_ANY;
	}
    m_pCapture = cvCaptureFromCAM(camNumber);
    if(!m_pCapture)
    {
        fprintf(stderr, "Could not initialize OpenCV camera capture.\n");
        m_Mode = MT_FC_MODE_OFF;
        return false;
    }

    /* m_pCapture won't know the frame size until it grabs a frame */
    cvGrabFrame(m_pCapture);
    IplImage* tframe = cvRetrieveFrame(m_pCapture);
    m_iFrameHeight = tframe->height;
    m_iFrameWidth = tframe->width;

    m_sTitle = "OpenCV Camera Capture";

    return true;
}

IplImage* MT_Cap_Iface_OpenCV_Camera::getFrame(int frame_index) /* arg is ignored */
{
    cvGrabFrame(m_pCapture);
    IplImage* raw_frame = cvRetrieveFrame(m_pCapture);

    if(!m_pCurrentFrame)
    {
        m_pCurrentFrame = cvCreateImage(getFrameSize(), IPL_DEPTH_8U, 3);
        if(!m_pCurrentFrame)
        {
            fprintf(stderr, "Could not allocate camera frame.\n");
            return NULL;
        }
    }

    cvCopy(raw_frame, m_pCurrentFrame);

    return m_pCurrentFrame;
}


/*********************************************************************
 *
 * ARToolKit Camera Iface
 *
 *********************************************************************/

#ifdef MT_HAVE_ARTOOLKIT
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#endif

const std::vector<std::string>MT_Cap_Iface_ARToolKit_Camera::
listOfAvailableCameras(int maxCameras)
{
    std::vector<std::string> result;
    result.resize(0);

    /* not yet implemented */

    return result;
}

void MT_Cap_Iface_ARToolKit_Camera::doSafeInit()
{
    MT_Cap_Iface_Base::doSafeInit();
}

MT_Cap_Iface_ARToolKit_Camera::MT_Cap_Iface_ARToolKit_Camera()
{
    doSafeInit();
}

MT_Cap_Iface_ARToolKit_Camera::~MT_Cap_Iface_ARToolKit_Camera()
{
#ifdef MT_HAVE_ARTOOLKIT
    arVideoCapStop();
    arVideoClose();

    if(m_pCurrentFrame)
    {
        cvReleaseImage(&m_pCurrentFrame);
    }
    if(m_pRframe)
    {
        /* note we don't release m_pRawFrame b/c it points to
         * memory owned by the capture */
        cvReleaseImage(&m_pRframe);
        cvReleaseImage(&m_pGframe);
        cvReleaseImage(&m_pBframe);
        cvReleaseImage(&m_pAframe);
    }
#endif
}

bool MT_Cap_Iface_ARToolKit_Camera::initCamera(int camNumber, int FW, int FH, bool ShowDialog, bool FlipH, bool FlipV)
{
#ifdef MT_HAVE_ARTOOLKIT
        // String to pass the grabber, contains various configuration options
    char vconf[256] = "\0";
    // temporary string used to build up vconf
    char cconf[64] = "\0";
  
    /*---- NOTE - The following options are for OS X / Quicktime only! ----*/
    // It would be great if someone made this more general, but this suits for now
  
    // override the default frame width
    if(FW > 0)
    {
        sprintf(cconf,"-width=%d ",FW);
        strcat(vconf,cconf);
    }
  
    // override the default frame height
    if(FH > 0)
    {
        sprintf(cconf,"-height=%d ",FH);
        strcat(vconf,cconf);
    }
  
    // override the default option (which is to show the config dialog)
    if(ShowDialog == MT_FC_NODIALOG)
    {
        sprintf(cconf,"-nodialog ");
        strcat(vconf,cconf);
    }
  
    // flip the frame horizontally
    if(FlipH == MT_FC_FLIP)
    {
        sprintf(cconf,"-fliph ");
        strcat(vconf,cconf);
    }
  
    // flip the frame vertically
    if(FlipV == MT_FC_FLIP)
    {
        sprintf(cconf,"-flipv ");
        strcat(vconf,cconf);
    }

    #ifdef MT_FC_DEBAYER
    sprintf(cconf, "-pixelformat=40");
    strcat(vconf, cconf);
    #endif

    /*--- Slightly modified from the ARToolkit examples ---*/
    /* open the video path */
    if( arVideoOpen( vconf ) < 0 )
    {
        fprintf(stderr, "Could not open ARToolKit camera interface. \n");
        return false;
    }
  
    /* find the size of the window */
    int t_framewidth, t_frameheight;
    if( arVideoInqSize(&t_framewidth, &t_frameheight) < 0 )
    {
        fprintf(stderr, "Could not obtain frame size - will not open interface.\n");
        return false;
    }
    else
    {
        m_iFrameWidth = t_framewidth;
        m_iFrameHeight = t_frameheight;
    }
  
    // set the capturing in motion
    arVideoCapStart();
    arVideoCapNext();

    /* The camera grabs an alpha frame, but we only keep R, G, B */
    m_iNChannelsPerFrame = 3;

    /* a normal color image has 4 channels in the raw frame for
     * ARTK - R, G, B, A */
    unsigned int raw_frame_channels = 4;

    /* A Bayer camera only has one raw channel */
#ifdef MT_FC_DEBAYER
    raw_frame_channels = 1;
#endif

    m_pRawFrame = cvCreateImage(getFrameSize(),
                                IPL_DEPTH_8U,
                                raw_frame_channels);
    m_pCurrentFrame = cvCreateImage(getFrameSize(), IPL_DEPTH_8U, 3);
    m_pRframe = cvCreateImage(getFrameSize(), IPL_DEPTH_8U, 1);
    m_pGframe = cvCreateImage(getFrameSize(), IPL_DEPTH_8U, 1);
    m_pBframe = cvCreateImage(getFrameSize(), IPL_DEPTH_8U, 1);
    m_pAframe = cvCreateImage(getFrameSize(), IPL_DEPTH_8U, 1);

    fprintf(stdout, 
            "Opened ARToolKit camera interface with image size %dx%d\n",
            m_iFrameWidth,
            m_iFrameHeight);

    m_Mode = MT_FC_MODE_CAM;

    return true;
#else /* MT_HAVE_ARTOOLKIT not defined */
    return false;
#endif
}

IplImage* MT_Cap_Iface_ARToolKit_Camera::getFrame(int frame_index) /* arg is ignored */
{
#ifdef MT_HAVE_ARTOOLKIT
    /* check for reasonable initialization */
    if(m_Mode != MT_FC_MODE_CAM)
    {
        return NULL;
    }

    /*--- Slightly modified from the ARToolkit examples ---*/

    ARUint8         *dataPtr;
    unsigned int wait_count = 0;
    /* grab a video frame */
    while( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        wait_count++;
    }
    /* capture the next video frame */
    arVideoCapNext();

    /*--- End ARToolkit setup from examples -------------*/

#ifndef MT_FC_DEBAYER
    cvSetImageData(m_pRawFrame, dataPtr, m_iFrameWidth*4);

#ifdef __APPLE__
    /* on OS X the default pixel format for ARTK is ARGB, but
     * everything else expects BGR */
    cvSplit(m_pRawFrame, m_pAframe, m_pRframe, m_pGframe, m_pBframe);
    cvMerge(m_pBframe, m_pGframe, m_pRframe, NULL, m_pCurrentFrame);
#else
    /* TODO */
#endif

#else  /* ifdef MT_FC_DEBAYER */

    cvSetImageData(m_pRawFrame, dataPtr, m_iFrameWidth);
    cvSubRS(m_pRawFrame, cvScalar(255), m_pRawFrame);
    cvCvtColor(m_pRawFrame, m_pCurrentFrame, CV_BayerBG2BGR);

#endif /* ifndef MT_FC_DEBAYER */

    return m_pCurrentFrame;

#else /* MT_HAVE_ARTOOLKIT not defined */
    return NULL;
#endif
}


/*********************************************************************
 *
 * AVT Camera Iface
 *
 *********************************************************************/

/* define to see extra information when initializing an AVT camera */
#define MT_AVT_VERBOSE

const std::vector<std::string>MT_Cap_Iface_AVT_Camera::
listOfAvailableCameras(int maxCameras)
{
    std::vector<std::string> result;
    result.resize(0);

#ifdef MT_HAVE_AVT    
    UINT32 err_code = FGInitModule(NULL);
	//If returns FCE_ALREADYOPENED, means that FireGrab has already been initialized
    if(err_code != FCE_NOERROR && err_code != FCE_ALREADYOPENED)
    {
        fprintf(stderr, "Could not open AVT Camera interface.  Error code: %ld\n", err_code);
        return result;
    }

    FGNODEINFO* node_info = new FGNODEINFO[maxCameras];
    UINT32 node_count;

    err_code = FGGetNodeList(node_info, maxCameras, &node_count);
    if(err_code != FCE_NOERROR)
    {
        fprintf(stderr, "Could not get AVT node list.  Error code: %ld\n", err_code);
		delete [] node_info;
        return result;
    }
    if(!node_count)
    {
        fprintf(stderr, "Failed to find any AVT nodes.\n");
		delete [] node_info;
        return result;
    }

    for (UINT32 i = 0; i < node_count; i++)
    {
        std::stringstream ss;
        ss << "AVT Camera " << node_info[i].Guid.High << node_info[i].Guid.Low;
        result.push_back(std::string(ss.str()));
    }
	delete [] node_info;
#endif
    
    return result;
}

void MT_Cap_Iface_AVT_Camera::doSafeInit()
{
    MT_Cap_Iface_Base::doSafeInit();
}

MT_Cap_Iface_AVT_Camera::MT_Cap_Iface_AVT_Camera()
{
    doSafeInit();
}

MT_Cap_Iface_AVT_Camera::~MT_Cap_Iface_AVT_Camera()
{
    if(m_pCurrentFrame)
    {
            cvReleaseImage(&m_pCurrentFrame);
    }

#ifdef MT_HAVE_AVT
    /* cleanup of the FireGrab module */
    m_Camera.StopDevice();
    m_Camera.CloseCapture();
    m_Camera.Disconnect();
    FGExitModule();
#endif
}

bool MT_Cap_Iface_AVT_Camera::initCamera(int camNumber, int FW, int FH, bool ShowDialog, bool FlipH, bool FlipV)
{
#ifdef MT_HAVE_AVT

	if(camNumber <= MT_FC_NEXT_CAMERA)
	{
		// this is a hack - it should know which is the next camera!
		camNumber = 0;
	}

    UINT32 err_code = FGInitModule(NULL);
	//If returns FCE_ALREADYOPENED, means that FireGrab has already been initialized
    if(err_code != FCE_NOERROR && err_code != FCE_ALREADYOPENED)
    {
        fprintf(stderr, "Could not open AVT Camera interface.  Error code: %ld\n", err_code);
        return false;
    }

    FGNODEINFO node_info[6];
    UINT32 node_count;

    err_code = FGGetNodeList(node_info, 6, &node_count);
    if(err_code != FCE_NOERROR)
    {
        fprintf(stderr, "Could not get AVT node list.  Error code: %ld\n", err_code);
        return false;
    }
    if(!node_count)
    {
        fprintf(stderr, "Failed to find any AVT nodes.\n");
        return false;
    }

	if(camNumber >= (int) node_count)
	{
		fprintf(stderr, "Camera %d is not present, there are only %d available.", camNumber, node_count);
		return false;
	}

    /*wxString cameras[16];
    for (UINT32 i = 0; i < node_count; i++)
    {
        char tmp[128];
        sprintf(tmp, "%i", node_info[i].Guid);
        cameras[i] = wxString(tmp);
    }
    MT_AVTCameraDialog* dlg = new MT_AVTCameraDialog(NULL, wxPoint(0, 0), cameras, node_count);
    bool success = dlg->Show();
    int cameraNum = dlg->GetCameraSelection();
    if (!success)
    {
        return false;
    }*/

	fprintf(stdout, "Trying to connect to camera with GUID %ld%ld\n",
		node_info[camNumber].Guid.High, node_info[camNumber].Guid.Low);

    err_code = m_Camera.Connect(&node_info[camNumber].Guid);
    if(err_code != FCE_NOERROR)
    {
		fprintf(stderr, "Could not connect to camera.  Error code: %ld\n", err_code);
        return false;
    }

#ifdef MT_AVT_VERBOSE
	UINT32 Result;
	FGPINFO PacketSize;
	Result=m_Camera.GetParameterInfo(FGP_PACKETSIZE,&PacketSize);
	printf("PacketSize is %i (%i...%i [%i]):\n ",PacketSize.IsValue, PacketSize.MinValue, PacketSize.MaxValue, PacketSize.Unit);
#endif

    UINT32 param_value;

	/* We need slow down the camera's frame rate so that the packet 
	   size is manageable for multiple cameras on the same bus.  Most
	   of our operations will run just fine at 30 FPS anyways. */
    m_Camera.GetParameter(FGP_IMAGEFORMAT, &param_value);
#ifdef MT_AVT_VERBOSE
	printf("ImageFormat is %d %d %d\n", IMGRES(param_value), IMGCOL(param_value), IMGRATE(param_value));
#endif
	m_Camera.SetParameter(FGP_IMAGEFORMAT, MAKEIMAGEFORMAT(IMGRES(param_value), IMGCOL(param_value), FR_30));

#ifdef MT_AVT_VERBOSE
    m_Camera.GetParameter(FGP_IMAGEFORMAT, &param_value);
	printf("ImageFormat is %d %d %d\n", IMGRES(param_value), IMGCOL(param_value), IMGRATE(param_value));
	Result=m_Camera.GetParameterInfo(FGP_PACKETSIZE,&PacketSize);
	printf("PacketSize is %i (%i...%i [%i]):\n ",PacketSize.IsValue, PacketSize.MinValue, PacketSize.MaxValue, PacketSize.Unit);
#endif

    err_code = m_Camera.OpenCapture();
    if(err_code != FCE_NOERROR)
    {
        fprintf(stderr, "Could not open capture DMA.  Error code: %ld\n", err_code);
        return false;
    }

    err_code = m_Camera.SetParameter(FGP_BURSTCOUNT, 1);
    if(err_code != FCE_NOERROR)
    {
        fprintf(stderr, "Could not set camera to one-shot mode.  Error code: %ld\n", err_code);
        return false;
    }

    err_code = m_Camera.StartDevice();
    if(err_code != FCE_NOERROR)
    {
        fprintf(stderr, "Could not start camera device.  Error code: %ld\n", err_code);
        return false;
    }

    m_Camera.GetParameter(FGP_XSIZE, &param_value);
    m_iFrameWidth = param_value;
    m_Camera.GetParameter(FGP_YSIZE, &param_value);
    m_iFrameHeight = param_value;

    //Camera does weird stuff with white-balancing. The factory default is often more reasonable though
    m_Camera.SetParameter(FGP_WHITEBALCB, PVAL_OFF);
    m_Camera.SetParameter(FGP_WHITEBALCR, PVAL_OFF);

    m_Camera.GetParameter(FGP_IMAGEFORMAT, &param_value);

    char name[256];
    m_Camera.GetDeviceName(name, 256);
    m_sTitle = std::string(name);

    fprintf(stdout, "Found camera \"%s\" with frame size %dx%d\n", 
            m_sTitle.c_str(), 
            m_iFrameWidth, 
            m_iFrameHeight);
    fprintf(stdout, "   Image format has resolution number %d, color format %d, and rate number %d.\n", 
            IMGRES(param_value), 
            IMGCOL(param_value), 
            IMGRATE(param_value));

    m_Mode = MT_FC_MODE_CAM;

    return true;
#else
    return false;
#endif
}

IplImage* MT_Cap_Iface_AVT_Camera::getFrame(int frame_index)
{
#ifdef MT_HAVE_AVT
    FGFRAME fg_frame;
    UINT32 err_code;

    m_Camera.StartDevice();
    err_code = m_Camera.GetFrame(&fg_frame, 1000);

    if(err_code != FCE_NOERROR)
    {
        fprintf(stderr, "Could not get frame from AVT camera.  Error code: %ld\n", err_code);
        return m_pCurrentFrame;
    }

	UINT32 param_value;
	m_Camera.GetParameter(FGP_IMAGEFORMAT, &param_value);
	int numChannels = IMGCOL(param_value) == CM_Y8 || IMGCOL(param_value) == CM_RGB8 || IMGCOL(param_value) == CM_RGB16 || IMGCOL(param_value) == CM_SRGB16 ? 3 : 1;

    /* TODO this is very inflexible */
    if(!m_pCurrentFrame)
    {
		m_pCurrentFrame = cvCreateImage(cvSize(m_iFrameWidth, m_iFrameHeight), IPL_DEPTH_8U, numChannels);
		m_tmpGrayFrame = cvCreateImage(cvSize(m_iFrameWidth, m_iFrameHeight), IPL_DEPTH_8U, 1);
		m_iNChannelsPerFrame = 1;
        if(!m_pCurrentFrame)
        {
            fprintf(stderr, "Error creating capture frame.\n");
            return NULL;
        }
    }

	/* copies the raw image data into the frame 
	 *   NOTE cvSetData (cvSetImageData) only assigns the pointer, it does not copy,
	 *     so that's not the appropriate method here (we want to avoid collisions
	 *     in the potentially volatile camera buffer) */
	if (IMGCOL(param_value) == CM_Y8 || IMGCOL(param_value) == CM_RGB8 || IMGCOL(param_value) == CM_RGB16 || IMGCOL(param_value) == CM_SRGB16)
	{
		for(int i = 0; i < m_iFrameWidth*m_iFrameHeight; i++)
		{
			//((uchar*)(m_pCurrentFrame->imageData))[i] = fg_frame.pData[i];
			((uchar*)(m_tmpGrayFrame->imageData))[i] = fg_frame.pData[i];
		}
		cvCvtColor(m_tmpGrayFrame, m_pCurrentFrame, CV_BayerRG2RGB);
	} 
	else
	{
		for(int i = 0; i < m_iFrameWidth*m_iFrameHeight; i++)
		{
			((uchar*)(m_pCurrentFrame->imageData))[i] = fg_frame.pData[i];
		}
	}

	/* release the memory to the camera */
    m_Camera.PutFrame(&fg_frame);

    return m_pCurrentFrame;
#else
    return NULL;
#endif
}

