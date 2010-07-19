/*
 *  MT_Capture.cpp
 *
 *  Created by Daniel Swain on 12/22/07.
 *
 */

#include "MT_Capture.h"

#include "MT/MT_Core/support/mathsupport.h"  // for MT_getYYMMDDandHHMMSS to generate screenshot filenames

/* local shorthand */
#define SAFE_IFACE(iface) iface >= 0 && ((int) iface) < getNumInterfacesOpen()

/* ---------------------------- MT_Capture -------------------------------- */

/* static variable definitions */
MT_Cap_Iface_Table MT_Capture::s_IfaceTable;

/* MT_Capture ctors and dtors */

MT_Capture::MT_Capture(MT_Cap_Iface_Type default_type)
{
    doCommonInit(default_type);
}


MT_Capture::MT_Capture(const char* filename, MT_Cap_Iface_Type default_type)
{
    doCommonInit(default_type);

    if(filename)
    {
        initCaptureFromFile(filename);
    }
}

MT_Capture::MT_Capture(int FW, 
                       int FH, 
                       bool ShowDialog,
                       bool FlipH, 
                       bool FlipV,
                       MT_Cap_Iface_Type type,
					   int numInterface)
{
    doCommonInit(type);
  
    initCaptureFromCamera(FW, FH, ShowDialog, FlipH, FlipV, type, numInterface);
  
}

MT_Capture::~MT_Capture()
{
    for(unsigned int i = 0; i < m_vpInterfaces.size(); i = i+1)
    {
        delete m_vpInterfaces[i];
        m_vpInterfaces[i] = NULL;
    }
}
 
/* MT_Capture private member functions */

void MT_Capture::doCommonInit(MT_Cap_Iface_Type default_type)
{
    /* must build the interface table FIRST */
    buildInterfaceTable();

    if(default_type == MT_CAP_DEFAULT)
    {
        default_type = MT_CAP_DEFAULT_IFACE;
    }
    setDefaultInterface(default_type);

    m_vpInterfaces.resize(0);
    m_vIfaceTypes.resize(0);
}

void MT_Capture::buildInterfaceTable()
{
    static bool table_built = false;

    if(table_built)
    {
        return;
    }

    /* just local shorthand */
    typedef MT_Cap_Iface_Table_Entry TE_;

    /* strategy: build up table as default, then disable what won't work */
    s_IfaceTable[MT_CAP_CV_FILE] = TE_(MT_CAP_CV_FILE, true, "OpenCV File Capture (e.g. AVI)");
    s_IfaceTable[MT_CAP_CV_CAMERA] = TE_(MT_CAP_CV_CAMERA, true, "OpenCV Camera Capture");
    s_IfaceTable[MT_CAP_ARTK_CAMERA] = TE_(MT_CAP_ARTK_CAMERA, true, "ARToolKit Camera Capture");
    s_IfaceTable[MT_CAP_AVT_CAMERA] = TE_(MT_CAP_AVT_CAMERA, true, "AVT Camera Capture");

    /* the sentinel entry is special - it should be unavailable */
    s_IfaceTable[MT_CAP_sentinel] = TE_(MT_CAP_sentinel, false, "END OF TABLE");

    /* disable ARToolKit camera interface if it is unavailable
       (see header) */
#ifndef MT_HAVE_ARTOOLKIT
    s_IfaceTable[MT_CAP_ARTK_CAMERA].IsAvailable = false;
#endif

    /* disable AVT camera interface if it is unavailable
       (see header) */
#ifndef MT_HAVE_AVT
    s_IfaceTable[MT_CAP_AVT_CAMERA].IsAvailable = false;
#endif

    /* flag that we won't need to build the table next time */
    table_built = true;
}

/* MT_Capture public member functions */

MT_Cap_Iface_Type MT_Capture::setDefaultInterface(MT_Cap_Iface_Type requested_type)
{
    /* if the requested iface is unavailable... */
    if(!s_IfaceTable[requested_type].IsAvailable)
    {
        /* if the previous default is ALSO unavailable... */
        if(!s_IfaceTable[m_DefaultIface].IsAvailable)
        {
            /* take the first one that is (include sentinel so we can set
             NONE if we don't find a valid iface */
            for(MT_Cap_Iface_Type i = MT_CAP_CV_FILE; 
                    i < MT_CAP_sentinel;
                    i = MT_Cap_Iface_Type(i + 1))
            {
                if(s_IfaceTable[i].IsAvailable)
                {
                    m_DefaultIface = i;
                    break;
                }
            }
        }

        /* if we got all the way through the list, then we should set NONE */
        if(m_DefaultIface == MT_CAP_sentinel)
        {
            m_DefaultIface = MT_CAP_NONE;
        }
    }
    else  /* requested type is available, just use that */
    {
        m_DefaultIface = requested_type;
    }
    
    return m_DefaultIface;  /* return is whatever we ended up using */
}

MT_Cap_Iface_Type MT_Capture::getDefaultInterface() const
{
    return m_DefaultIface;
}

const char* MT_Capture::getDefaultInterfaceName() const
{
    return s_IfaceTable[m_DefaultIface].Name;
}

const MT_Cap_Iface_Vector MT_Capture::getAvailableInterfaces() const
{
    MT_Cap_Iface_Vector result;

    for(MT_Cap_Iface_Type i = MT_CAP_CV_FILE; 
            i < MT_CAP_sentinel;
            i = MT_Cap_Iface_Type(i + 1))
    {
        if(s_IfaceTable[i].IsAvailable)
        {
            result.push_back(s_IfaceTable[i]);
        }
    }
    return result;
}

bool MT_Capture::initCaptureFromFile(const char* filename)
{

    MT_Cap_Iface_CV_File* new_iface = new MT_Cap_Iface_CV_File();

    if(!new_iface->initFromFile(filename))
    {
        delete new_iface;
        return false;
    }
    
    m_vpInterfaces.push_back(new_iface);
    m_vIfaceTypes.push_back(MT_CAP_CV_FILE);

    return true;

}

bool MT_Capture::initCaptureFromCamera(int FW, 
                                       int FH, 
                                       bool ShowDialog,
                                       bool FlipH, 
                                       bool FlipV,
                                       MT_Cap_Iface_Type iface_type,
									   int numInterface)
{
    
    /* see MT_Cap_Iface_Type definition - all camera interfaces
       need to be MT_CAP_CV_CAMERA or higher */
    if(iface_type != MT_CAP_DEFAULT && iface_type < MT_CAP_CV_CAMERA)
    {
        fprintf(stderr, 
                "Interface type %s is invalid for camera initialization.\n", 
                s_IfaceTable[iface_type].Name);
        return false;
    }

    if(iface_type == MT_CAP_DEFAULT)
    {
        iface_type = m_DefaultIface;
        fprintf(stdout, 
                "Using default interface %s\n", 
                s_IfaceTable[iface_type].Name);
    }

    if(!s_IfaceTable[iface_type].IsAvailable)
    {
        fprintf(stderr,
                "Interface type %s is unavailable.\n",
                s_IfaceTable[iface_type].Name);
        return false;
    }

    MT_Cap_Iface_Base* new_iface = NULL;

    switch(iface_type)
    {
        case MT_CAP_ARTK_CAMERA:
            new_iface = new MT_Cap_Iface_ARToolKit_Camera();
            break;
        case MT_CAP_AVT_CAMERA:
            new_iface = new MT_Cap_Iface_AVT_Camera();
            break;
        case MT_CAP_CV_CAMERA:
        default:
            new_iface = new MT_Cap_Iface_OpenCV_Camera();
            iface_type = MT_CAP_CV_CAMERA;
            break;
    }

    /* this shouldn't occur, but make sure */
    if(!new_iface)
    {
        return false;
    }

    if(!new_iface->initCamera(FW, FH, ShowDialog, FlipH, FlipV, numInterface))
    {
        delete new_iface;
        return false;
    }

    m_vpInterfaces.push_back(new_iface);
    m_vIfaceTypes.push_back(iface_type);
  
    return true;
}

MT_FC_MODE_t MT_Capture::getMode(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getMode();
    }
    else
    {
        return MT_FC_MODE_OFF;
    }
}

int MT_Capture::getNFrames(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getNFrames();
    }
    else
    {
        return MT_FC_ERR;
    }
}

int MT_Capture::getNChannels(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getNChannels();
    }
    else
    {
        return MT_FC_ERR;
    }
}

int MT_Capture::getFrameNumber(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getFrameNumber();
    }
    else
    {
        return MT_FC_ERR;
    }
}

int MT_Capture::setFrameNumber(int frame_index, unsigned int iface)
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->setFrameNumber(frame_index);
    }
    else
    {
        return MT_FC_ERR;
    }
}

IplImage* MT_Capture::getFrame(int frame_index, unsigned int iface)
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getFrame(frame_index);
    }
    else
    {
        return NULL;
    }
}

double MT_Capture::getFPS(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getFPS();
    }
    else
    {
        return 0;
    }
}

int MT_Capture::getFramePeriod_msec(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getFramePeriod_msec();
    }
    else
    {
        return 0;
    }
}

CvSize MT_Capture::getFrameSize(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getFrameSize();
    }
    else
    {
        return cvSize(0,0);
    }
}

int MT_Capture::getFrameWidth(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getFrameWidth();
    }
    else
    {
        return MT_FC_ERR;
    }
}

int MT_Capture::getFrameHeight(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getFrameHeight();
    }
    else
    {
        return MT_FC_ERR;
    }
}

const char* MT_Capture::getTitle(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getTitle();
    }
    else
    {
        return "Error.";
    }
}

void MT_Capture::saveFrame(const char* filename, unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        m_vpInterfaces[iface]->saveFrame(filename);
    }
}

double MT_Capture::getProgressFraction(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getProgressFraction();
    }
    else
    {
        return 0.0;
    }
}

bool MT_Capture::getIsAtEnd(unsigned int iface) const
{
    if(SAFE_IFACE(iface))
    {
        return m_vpInterfaces[iface]->getIsAtEnd();
    }
    else
    {
        return false; /* don't return true - this can cause the program
                         to think it's at the end of a file and should quit */
    }
}
