/*
 *  MT_TrackerBase.cpp
 *
 *  Created by Daniel Swain on 10/18/09.
 *
 */

#include "MT_TrackerBase.h"
#include "MT/MT_Core/support/mathsupport.h"  /* for MT_getTimeSec() */

/* MT_TrackedObjectsBase safe access convenience macros */
#define SAFE_TO_RETURN(index, method, fail_value)       \
    if(index < 0 || index >= m_iNumObjects)             \
    {                                                   \
        return fail_value;                              \
    }                                                   \
    else                                                \
    {                                                   \
        return m_TrackedObjects[index].method;          \
    } /* end of SAFE_TO_RETURN macro */

#define SAFE_TO_METHOD(index, method)                   \
    if(index < 0 || index >= m_iNumObjects)             \
    {                                                   \
        return;                                         \
    }                                                   \
    else                                                \
    {                                                   \
        m_TrackedObjects[index].method;                 \
    } /* end of SAFE_TO_METHOD macro */


MT_TrackerFrameGroup::MT_TrackerFrameGroup()
{
    StandardFrameGroupInit(0);
}

void MT_TrackerFrameGroup::StandardFrameGroupInit(unsigned int n_frames)
{
    m_iNFrames = n_frames;
    m_vFrames.resize(n_frames, 0);
    m_vFrameNames.resize(n_frames, string("Unset name"));
}

std::vector<string> MT_TrackerFrameGroup::getFrameNames() const
{
    return m_vFrameNames;
}

IplImage* MT_TrackerFrameGroup::getFrame(unsigned int frame_index) const
{
    if(frame_index < 0 || frame_index >= m_iNFrames)
    {
        return *m_vFrames[0];
    }
    else
    {
        return *m_vFrames[frame_index];
    }
}

MT_TrackedObjectsBase::MT_TrackedObjectsBase(unsigned int nobj,
        unsigned int state_size,
        unsigned int measurement_size)
{
    m_TrackedObjects.resize(nobj,
            MT_TrackedObjectBase(state_size, measurement_size));

    m_iNumObjects = nobj;
}

unsigned int MT_TrackedObjectsBase::getNumConsecutiveFrames(unsigned int i) const
{
    SAFE_TO_RETURN(i, m_iNumConsecutiveFrames, 0); 
} 

int MT_TrackedObjectsBase::getRobotIndex(unsigned int i) const
{
    SAFE_TO_RETURN(i, m_iRobotIndex, MT_NO_ROBOT);
}

bool MT_TrackedObjectsBase::getIsMoving(unsigned int i) const
{
    SAFE_TO_RETURN(i, getIsMoving(), false);
}

void MT_TrackedObjectsBase::setRobotIndex(unsigned int i, int new_index)
{
    SAFE_TO_METHOD(i, m_iRobotIndex = new_index);
}

void MT_TrackedObjectsBase::setXY(unsigned int i, double x, double y)
{
    SAFE_TO_METHOD(i, setXY(x,y));
}

void MT_TrackedObjectsBase::setOrientation(unsigned int i, double orientation)
{
    SAFE_TO_METHOD(i, setOrientation(orientation));
}

double MT_TrackedObjectsBase::getX(unsigned int i) const
{
    SAFE_TO_RETURN(i, getX(), 0);
}

double MT_TrackedObjectsBase::getY(unsigned int i) const
{
    SAFE_TO_RETURN(i, getY(), 0);
}

double MT_TrackedObjectsBase::getOrientation(unsigned int i) const
{
    SAFE_TO_RETURN(i, getOrientation(), 0);
}

CvKalman* MT_TrackedObjectsBase::initKalmanFilter(unsigned int i,
        unsigned int num_control_inputs)
{
    SAFE_TO_RETURN(i, initKalmanFilter(num_control_inputs), NULL);
}

void MT_TrackedObjectsBase::setState(unsigned int i, double* state)
{
    SAFE_TO_METHOD(i, setState(state));
}

double* MT_TrackedObjectsBase::getState(unsigned int i) const
{
    SAFE_TO_RETURN(i, getState(), NULL);
}

void MT_TrackedObjectsBase::setMeasurement(unsigned int i, double* measurement)
{
    SAFE_TO_METHOD(i, setMeasurement(measurement));
}

double* MT_TrackedObjectsBase::getMeasurement(unsigned int i) const
{
    SAFE_TO_RETURN(i, getMeasurement(), NULL);
}

unsigned int MT_TrackedObjectsBase::getStateSize(unsigned int i) const
{
    SAFE_TO_RETURN(i, getStateSize(), 0);
}

unsigned int MT_TrackedObjectsBase::getMeasurementSize(unsigned int i) const
{
    SAFE_TO_RETURN(i, getMeasurementSize(), 0);
}

CvKalman* MT_TrackedObjectsBase::getKalmanFilterStruct(unsigned int i)
{
    SAFE_TO_RETURN(i, getKalmanFilterStruct(), NULL);
}

MT_TrackedObjectBase::MT_TrackedObjectBase(unsigned int state_size,
        unsigned int measurement_size)
    :m_pKalmanFilter(NULL)
{
    m_iNumConsecutiveFrames = 0;
    m_iRobotIndex = MT_NO_ROBOT;

    m_iStateSize = state_size;
    m_iMeasurementSize = measurement_size;

    m_pdState = (double *)calloc(state_size, sizeof(double));
    m_pdMeasurement = (double *)calloc(measurement_size, sizeof(double));
}

MT_TrackedObjectBase::MT_TrackedObjectBase(const MT_TrackedObjectBase& TO)
    : m_iNumConsecutiveFrames(0),
    m_iRobotIndex(MT_NO_ROBOT),
    m_iStateSize(TO.m_iStateSize),
    m_iMeasurementSize(TO.m_iMeasurementSize),
    m_pKalmanFilter(NULL)
{
    m_pdState = (double *)calloc(m_iStateSize, sizeof(double));
    m_pdMeasurement = (double *)calloc(m_iMeasurementSize, sizeof(double));
}

MT_TrackedObjectBase::~MT_TrackedObjectBase()
{
    if(m_iStateSize)
    {
        free(m_pdState);
    }
    if(m_iMeasurementSize)
    {
        free(m_pdMeasurement);
    }

    if(m_pKalmanFilter)
    {
        cvReleaseKalman(&m_pKalmanFilter);
    }
}

CvKalman* MT_TrackedObjectBase::initKalmanFilter(unsigned int num_control_inputs)
{
    if(m_pKalmanFilter)
    {
        cvReleaseKalman(&m_pKalmanFilter);
    }

    m_pKalmanFilter = cvCreateKalman(m_iStateSize, m_iMeasurementSize, num_control_inputs);

    return m_pKalmanFilter;
}

MT_TrackerBase::MT_TrackerBase(IplImage* ProtoFrame)
    : m_pTrackedObjects(NULL)
{
    doInit(ProtoFrame);
}

MT_TrackerBase::MT_TrackerBase()
{
    doInit(NULL);
}

MT_TrackerBase::~MT_TrackerBase()
{
  
    for(unsigned int i = 0; i < m_vDataGroups.size(); i++)
    {
        delete m_vDataGroups[i];
        m_vDataGroups[i] = NULL;
    }

	for(unsigned int i = 0; i < m_vDataReports.size(); i++)
	{
		delete m_vDataReports[i];
		m_vDataReports[i] = NULL;
	}
  
    if(m_pTrackerFrameGroup)
    {
        delete m_pTrackerFrameGroup;
    }

    if(m_pTrackedObjects)
    {
        delete m_pTrackedObjects;
    }
  
    releaseFrames();

}

void MT_TrackerBase::doInit(IplImage* ProtoFrame)
{
  
    m_SourceName = "Unknown";
    m_Note = "None";
    NFound = 0;

    m_vDataGroups.resize(0);
    m_pTrackerFrameGroup = NULL;
  
    BG_frame = NULL;
    ROI_frame = NULL;

    if(ProtoFrame)
    {
        doTrain(ProtoFrame);
    }
    else
    {
        FrameWidth = 0;
        FrameHeight = 0;
    }
  
}

void MT_TrackerBase::initDataFile()
{
    char datestring[10];
    char timestring[10];
    sprintf(datestring, "%02d/%02d/%02d", MT_getMonthMM(), MT_getDay(), MT_getYearYY());
    sprintf(timestring, "%02d:%02d:%02d", MT_getHour24(), MT_getMinute(), MT_getSecond());

    m_XDF.writeParameterToXML(MT_XDF_XML_VIDEOSOURCE_KEY, m_SourceName);
    m_XDF.writeParameterToXML(MT_XDF_XML_USERNOTE_KEY, m_Note.c_str());
    m_XDF.writeParameterToXML("Date", datestring);
    m_XDF.writeParameterToXML("Time", timestring);

    for(unsigned int i = 0; i < m_vDataGroups.size(); i++)
    {
        m_XDF.writeDataGroupToXML(m_vDataGroups[i]);
    }

}

void MT_TrackerBase::releaseFrames()
{
  
    if(BG_frame)
    {
        cvReleaseImage(&BG_frame);
    }
  
    if(ROI_frame)
    {
        cvReleaseImage(&ROI_frame);
    }
  
}

void MT_TrackerBase::doTrain(IplImage* frame)
{
  
    // TODO Should really check this against the ROI frame if it exists
  
    CvSize framesize = cvSize(frame->width,frame->height);
  
    FrameWidth = frame->width;
    FrameHeight = frame->height;  
  
    createFrames();
  
    if(frame->nChannels == 3)
    {
        cvCvtColor(frame, BG_frame, CV_BGR2GRAY);
    } else {
        cvCopy(frame, BG_frame);
    }
  
}

void MT_TrackerBase::createFrames()
{
  
    // TODO Should really check this against the ROI frame if it exists
  
    CvSize framesize = cvSize(FrameWidth, FrameHeight);

    if(BG_frame)
    {
        cvReleaseImage(&BG_frame);
    }
  
    BG_frame = cvCreateImage(framesize, IPL_DEPTH_8U, 1);

}

bool MT_TrackerBase::setROIImage(const char* ROIFilename,
                                 std::string* p_error_message)
{
    char message[512];
  
    ROI_frame = cvLoadImage(ROIFilename,CV_LOAD_IMAGE_GRAYSCALE);
    if(ROI_frame == 0)
    {
        sprintf(message,"Could not load ROI frame %s.",ROIFilename);
    } else {
        // need to make sure it's the right size
        if( (ROI_frame->width != FrameWidth) || (ROI_frame->height != FrameHeight) )
        {
            sprintf(message, "ROI Frame %s does not have the correct size.\n",ROIFilename);
            ROI_frame = 0;
        }
    }

    if(ROI_frame == 0)
    {
        if(p_error_message)
        {
            *p_error_message = message;
        }
        else
        {
            fprintf(stderr, "%s\n", message);
        }
        return false;
    }

    return true;
  
}

bool MT_TrackerBase::setBackgroundImage(const char* BackgroundFilename,
                                        std::string* p_error_message)
{
    char message[512];
  
    IplImage* tBG_frame = cvLoadImage(BackgroundFilename,CV_LOAD_IMAGE_GRAYSCALE);
    if(tBG_frame == 0)
    {
        sprintf(message, "Could not load background frame %s\n", BackgroundFilename);
    } else {
        // need to make sure it's the right size
        if( (tBG_frame->width != FrameWidth) || (tBG_frame->height != FrameHeight) )
        {
            sprintf(message, "ROI Frame %s does not have the correct size.\n", BackgroundFilename);
        }
    }

    if(tBG_frame == 0)
    {
        if(p_error_message)
        {
            *p_error_message = message;
        }
        else
        {
            fprintf(stderr, "%s\n", message);
        }
        return false;
    }
  
    // frame is ok, so re-train the tracker with this background
    //  note we don't need to actually assign the frame, this gets taken care of
    //   in the Train() function
    doTrain(tBG_frame);

    return true;
  
}

bool MT_TrackerBase::setDataFile(const char* DataFilename,
                                 std::string* p_error_message)
{
    char message[512];

    if(m_XDF.initAsNew(DataFilename, MT_XDF_OVERWRITE) == MT_XDF_ERROR)
    {
        sprintf(message, "Could not initialize data file %s.", DataFilename);
        if(p_error_message)
        {
            *p_error_message = message;
        }
        else
        {
            fprintf(stderr, "%s\n", message);
        }
        return false;
    }

    initDataFile();

    return true;

}

IplImage* MT_TrackerBase::getProcessedFrame(unsigned int frame_index) const
{
  
    if(!m_pTrackerFrameGroup || frame_index < 0 || frame_index >= m_pTrackerFrameGroup->getNumFrames())
    {
        return NULL;
    }
    else
    {
        return m_pTrackerFrameGroup->getFrame(frame_index);
    }
  
}

unsigned int MT_TrackerBase::getNumProcessedFrames() const
{
    if(!m_pTrackerFrameGroup)
    {
        return 0;
    }
    else
    {
        return m_pTrackerFrameGroup->getNumFrames();
    }
}


unsigned int MT_TrackerBase::getNumDataGroups() const
{
  
    return m_vDataGroups.size();
  
}

MT_DataGroup* MT_TrackerBase::getDataGroup(unsigned int i) const
{
  
    if(i < 0 || i >= getNumDataGroups())
    {
        return NULL;
    }
    else
    {
        return m_vDataGroups[i];
    }
  
}

unsigned int MT_TrackerBase::getNumDataReports() const
{
 
    return m_vDataReports.size();
  
}

MT_DataReport* MT_TrackerBase::getDataReport(unsigned int i) const
{
  
    if(i < 0 || i >= getNumDataReports())
    {
        return NULL;
    }
    else
    {
        return m_vDataReports[i];
    }
  
}

double MT_TrackerBase::getFrameRate(bool updaterate)
{
  
    static double t_previous = MT_getTimeSec();
    static double t_now = MT_getTimeSec();
    static double dt;
  
    if(updaterate)
    {
        t_now = MT_getTimeSec();
        dt = t_now - t_previous;
        t_previous = t_now;
    }
  
    if(dt == 0)
    {
        return 0;
    }
    else
    {
        return 1.0/dt;
    }

}

MT_BoundingBox MT_TrackerBase::getObjectBoundingBox() const
{
    return MT_BoundingBox(0,0,0,0);
}

void MT_TrackerBase::setNote(const char* note)
{
    m_Note = std::string(note);
    if(m_XDF.getStatus() == MT_XDF_OK)
    {
        m_XDF.writeParameterToXML(MT_XDF_XML_USERNOTE_KEY, m_Note.c_str());
    }
}

void MT_TrackerBase::getNote(std::string* note)
{
    std::string exist_note;
    if(m_XDF.getStatus() == MT_XDF_OK)
    {
        if(m_XDF.getParameterString(MT_XDF_XML_USERNOTE_KEY, &exist_note) == MT_XDF_OK)
        {
            m_Note = exist_note;
        }
    }

    *note = m_Note;
}

