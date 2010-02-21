#ifndef MT_TRACKERBASE_H
#define MT_TRACKERBASE_H

/*
 *  MT_TrackerBase.h
 *
 *  Created by Daniel Swain on 10/18/09.
 *
 */

// OpenCV for underlying CV algorithms and structures
#if defined(__APPLE__) || defined(MACOSX)
#include <OpenCV/OpenCV.h>
#else
#include <cv.h>
#include <highgui.h>
#endif

#include "MT/MT_Tracking/cv/MT_HungarianMatcher.h"

#include "MT/MT_Core/primitives/DataGroup.h"
#include "MT/MT_Core/primitives/BoundingBox.h"

#include "MT/MT_Core/fileio/ExperimentDataFile.h"

#include <vector>  /* for STL vector container */
#include <string>  /* for STL strings          */

/** @var const int MT_TB_NO_FLAGS
 * Indicates that there are no flags to be passed to a function.
 * @see MT_TrackerBase::doGLDrawing
 */
const int MT_TB_NO_FLAGS = -1;

#ifndef MT_HAVE_ROBOT_MODULE
const int MT_NO_ROBOT = -1;
#endif

/** @var const unsigned int MT_DEFAULT_STATE_6
 * Default state size for MT_TrackedObjectBase.  This corresponds to
 * a rigid body model of an object moving in two dimensions.  The state
 * is assumed to be [x y vx vy phi omega] where vx and vy are the x
 * and y components of velocity, phi is the orientation angle, and
 * omega is the rate of change of phi.
 */
const unsigned int MT_DEFAULT_STATE_6 = 6;
/** @var const unsigned int MT_DEFAULT_MEASUREMENT_3_XYPHI
 * Default measurement size for MT_TrackedObjectBase.  This corresponds
 * to a measurement of position (x and y) and orientation (phi).
 */
const unsigned int MT_DEFAULT_MEASUREMENT_3_XYPHI = 3;

/** @class MT_TrackerFrameGroup Container object to report images to GUI.
 *
 * The MT_TrackerFrameGroup object is used by MT_TrackerFrameBase to
 * determine which frames are available to view in the GUI.
 */
class MT_TrackerFrameGroup
{
protected:
    std::vector<IplImage**> m_vFrames;
    std::vector<string> m_vFrameNames;
    unsigned int m_iNFrames;

    void StandardFrameGroupInit(unsigned int n_frames);

public:
    /** Standard initializer resizes everything to zero length */
    MT_TrackerFrameGroup();
    virtual ~MT_TrackerFrameGroup(){};

    /** Returns a vector of the human-friendly names for the frames.
     * This is used to generate the entries in the "View" menu. */
    virtual std::vector<string> getFrameNames() const;
    /** Returns the frame given by the index.  The index order is the
     * same as the name order.  If the index is out of range, frame 0
     * is returned. */
    virtual IplImage* getFrame(unsigned int frame_index) const;
    /** Returns the number of frames contained in the group. */
    unsigned int getNumFrames() const {return m_iNFrames;};

};

class MT_TrackedObjectBase;

class MT_TrackedObjectsBase
{
protected:
    std::vector<MT_TrackedObjectBase> m_TrackedObjects;
    unsigned int m_iNumObjects;

public:
    MT_TrackedObjectsBase(unsigned int nobj = 0,
            unsigned int state_size = MT_DEFAULT_STATE_6,
            unsigned int measurement_size = MT_DEFAULT_MEASUREMENT_3_XYPHI);
    virtual ~MT_TrackedObjectsBase(){};

    virtual unsigned int getNumObjects() const {return m_iNumObjects;};

    virtual unsigned int getNumConsecutiveFrames(unsigned int i) const;
    virtual int getRobotIndex(unsigned int i) const;
    virtual bool getIsMoving(unsigned int i) const;

    virtual void setXY(unsigned int i, double x, double y);
    virtual void setOrientation(unsigned int i, double orientation);
    virtual double getX(unsigned int i) const;
    virtual double getY(unsigned int i) const;
    virtual double getOrientation(unsigned int i) const;

    virtual CvKalman* initKalmanFilter(unsigned int i, 
            unsigned int num_control_inputs = 0);

    virtual void setState(unsigned int i, double* state);
    virtual double* getState(unsigned int i) const;

    virtual void setMeasurement(unsigned int i, double* measurement);
    virtual double* getMeasurement(unsigned int i) const;

    virtual void setRobotIndex(unsigned int i, int new_index);

    virtual CvKalman* getKalmanFilterStruct(unsigned int i);

    virtual unsigned int getStateSize(unsigned int i) const;
    virtual unsigned int getMeasurementSize(unsigned int i) const;

};

class MT_TrackedObjectBase
{
protected:
    friend class MT_TrackedObjectsBase;
    unsigned int m_iNumConsecutiveFrames;
    int m_iRobotIndex;

    /* the state can be whatever it needs to be, but
     * the built-in functions assume the following:
     * State[0] = x position
     * State[1] = y position
     * State[2] = x velocity
     * State[3] = y velocity
     * State[4] = orientation cosine
     * State[5] = orientation sine
     */
    unsigned int m_iStateSize;
    unsigned int m_iMeasurementSize;
    double* m_pdState;
    double* m_pdMeasurement;

    CvKalman* m_pKalmanFilter;

public:
    MT_TrackedObjectBase(unsigned int state_size = MT_DEFAULT_STATE_6,
            unsigned int measurement_size = MT_DEFAULT_MEASUREMENT_3_XYPHI);

    /* copy constructor is important here - the copy will need to allocate
     * its own memory! */
    MT_TrackedObjectBase(const MT_TrackedObjectBase& TO);

    virtual ~MT_TrackedObjectBase();

    virtual CvKalman* initKalmanFilter(unsigned int num_control_inputs = 0);

    virtual unsigned int getStateSize() const {return m_iStateSize;};
    virtual unsigned int getMeasurementSize() const 
        { return m_iMeasurementSize; };

    CvKalman* getKalmanFilterStruct(){return m_pKalmanFilter;};

    virtual bool getIsMoving() const {return false;};

    virtual void setXY(double x, double y){m_pdState[0] = x; m_pdState[1] = y;};
    virtual void setOrientation(double orientation)
    { if(m_iStateSize >= 6) 
            {m_pdState[5] = cos(orientation); m_pdState[6] = sin(orientation);} };
    
    virtual void setState(double* state){memcpy(m_pdState, state, m_iStateSize*sizeof(double));};
    virtual void setMeasurement(double* measurement)
        {memcpy(m_pdMeasurement, measurement, m_iMeasurementSize*sizeof(double));};

    virtual double getX() const {return m_pdState[0];};
    virtual double getY() const {return m_pdState[1];};

    /** Returns atan2(state[5],state[4]) if the state size is at least 6,
     * otherwise returns zero */
    virtual double getOrientation() const 
    { if(m_iStateSize > 6) {return atan2(m_pdState[5], m_pdState[4]);} else {return 0;} };

    virtual double* getState() const {return m_pdState;};
    virtual double* getMeasurement() const {return m_pdMeasurement;};

};

class MT_TrackerBase
{
protected:
    IplImage* ROI_frame;
    IplImage* BG_frame;

    int FrameWidth;
    int FrameHeight;
    unsigned int NFound;

    MT_ExperimentDataFile m_XDF;

    std::vector<MT_DataGroup*> m_vDataGroups;
    std::vector<MT_DataReport*> m_vDataReports;
    MT_TrackerFrameGroup* m_pTrackerFrameGroup;

    MT_TrackedObjectsBase* m_pTrackedObjects;

    /* Function to release frames from memory - called by dtor */
    virtual void releaseFrames();

    /* Function to create (or resize) frames based on current values of
       FrameWidth and FrameHeight - called by Train */
    virtual void createFrames();

    /* common initialization */
    virtual void doInit(IplImage* ProtoFrame);

    virtual void doImageProcessing(){};
    virtual void doSegmentation(){};
    virtual void doMeasurement(){};

    virtual void writeData(){};

    virtual void initDataFile();

    const char* m_SourceName;
    std::string m_Note;

    std::vector<int> m_viMatchAssignments;
    MT_HungarianMatcher m_HungarianMatcher;

public:

    MT_TrackerBase();
    MT_TrackerBase(IplImage* ProtoFrame);

    virtual ~MT_TrackerBase();

    /* Function to set the BG_frame - should call CreateFrames to make sure
       frames are right size */
    virtual void doTrain(IplImage* frame);

    /* Function to set the ROI image from a given file
       It's OK if this is never called - then the whole image is used
       This frame needs to be the same size as the ProtoFrame was */
    virtual bool setROIImage(const char* ROIFilename, std::string* p_error_message = NULL);

    /* Function to set the background image from a given file
       It's OK if this is never called - then the first frame is used
       (or a later frame is specified by the user)
       Needs to match the size of the ProtoFrame */
    virtual bool setBackgroundImage(const char* BackgroundFilename, std::string* p_error_message = NULL);

    /* Function to set data file - doesn't do anything here - provided
       for user interface - user needs to write their own function here */
    virtual bool setDataFile(const char* DataFilename, std::string* p_error_message = NULL);

    /* Main workhorse function of the tracker that should get called externally
       during each time step with the most current frame.  */
    virtual void doTracking(IplImage* frame){};

    /* Function to draw tracker status */
    virtual void doGLDrawing(int flags = MT_TB_NO_FLAGS){};

    /* Function to get a frame for viewing */
    virtual IplImage* getProcessedFrame(unsigned int frame_index) const;
    virtual unsigned int getNumProcessedFrames() const;

    /* parameter group handling */
    unsigned int getNumDataGroups() const;
    MT_DataGroup* getDataGroup(unsigned int i) const;

    /* Data reports */
    unsigned int getNumDataReports() const;
    MT_DataReport* getDataReport(unsigned int i) const;

    /* function to get frame group */
    virtual MT_TrackerFrameGroup* getFrameGroup() const {return m_pTrackerFrameGroup;};

    /* function to get the frame rate:
       frame rate gets updated every time you call this function with
       the argument true. */
    virtual double getFrameRate(bool updaterate = true);

    /* function to query how many objects were found in the frame */
    virtual unsigned int getNFound() const {return NFound;};

    virtual MT_TrackedObjectsBase* getTrackedObjects() const {return m_pTrackedObjects;};

    virtual MT_BoundingBox getObjectBoundingBox() const;

    virtual void setSourceName(const char* name){m_SourceName = name;};
    virtual void setNote(const char* note);
    virtual void getNote(std::string* note);

};

#endif // MT_TRACKERBASE_H
