#ifndef MT_TRACKERBASE_H
#define MT_TRACKERBASE_H

/** @addtogroup MT_Tracking
 * @{ */

/** @file
 *  MT_TrackerBase.h
 *
 *  @brief Base tracking classes for interfacing with MT.
 *
 *  By creating your tracker using derivatives of these classes, you
 *  can easily integrate them with the MT framework - providing, for
 *  example, a standardized GUI, methods for setting ROI (mask) and
 *  background frames, setting up output to data files, etc.  
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
 * MT_TrackerBase has a member pointer m_pTrackerFrameGroup, which you
 * can populate with frames that you want to be available in the
 * "veiw" menu.  See the documentation for MT_TrackerBase::doInit for
 * an example of usage.
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

    /** Add a frame to the group, at the end of the list. */
    virtual void pushFrame(IplImage** frame, std::string name)
    {
        m_vFrames.push_back(frame);
        m_vFrameNames.push_back(name);
        m_iNFrames++;
    }

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

/** @class MT_TrackerBase
 *
 * @brief Base tracker class for interface with MT framework.
 *
 * Create your tracker as a derivative of this class so that it will
 * interface nicely with the MT framework.  This is an extremely
 * flexible format and most of the functionality can be overridden.
 * Probably the most important functions to override are
 *  - constructor - For the variable initialization list.
 *  - doInit - To initialize your data.
 *  - createFrames - To allocate memory.
 *  - releaseFrames - To free memory upon destruction.
 *  - doGLDrawing - To do any OpenGL drawing you want to do during
 * each time step.
 *  - doTracking - The main workhorse tracking function.
 */
class MT_TrackerBase
{
protected:
    /** Frame managed by setROIImage.  Intended to be used as a
     * "mask" image, but this is not strictly necessary.  This frame is NOT
     * initialized by MT_TrackerBase - you need to allocate it in
     * createFrames.  */ 
    IplImage* ROI_frame;
    
    /** Frame managed by setBackgroundImage and doTrain.  Intended for
     * use as a background frame in background subtraction
     * algorithms.  This frame IS initialized by MT_TrackerBase as
     * long as you call MT_TrackerBase::createFrames. */
    IplImage* BG_frame;

    /** Width of a frame in pixels, gets set during calls to doTrain. */
    int FrameWidth;
    /** Height of a frame in pixels, gets set during calls to doTrain. */
    int FrameHeight;

    unsigned int NFound;

    /** MT_ExperimentDataFile managed by the tracker.
     * @see setDataFile
     * @see initDataFile
     * @see setNote
     * @see getNote */
    MT_ExperimentDataFile m_XDF;

    /** Data Groups managed by the tracker.  Typically these are
     * parameters that you want the user to be able to modify.
     *
     * Set this up during
     * initData and MT_TrackerFrameBase will automatically display
     * them in the "Tracker" menu.  They are also automatically
     * written to the data file.  See initData for an example of
     * usage. */
    std::vector<MT_DataGroup*> m_vDataGroups;
    /** Data Reports managed by the tracker.  Typically these are
     * lists of object positions, etc.
     *
     * Set this up during initData and MT_TrackerFrameBase will
     * automatically display them in the "Tracker" menu.  These are
     * not automatically written to the data file.  See
     * initData for an example of usage. */
    std::vector<MT_DataReport*> m_vDataReports;
    /** Load this with frames that you want to automatically be
     * available in the "Tracker" menu of MT_TrackerFrameBase.  See
     * initData for an example of usage.  Note that
     * MT_TrackerFrameBase will already have the current frame
     * (direct from the camera/avi) available. */
    MT_TrackerFrameGroup* m_pTrackerFrameGroup;

    MT_TrackedObjectsBase* m_pTrackedObjects;

    /** Function to release frames from memory (i.e. cvReleaseImage)
     * - called by dtor.  You
     * need to release any frames that were allocated by
     * createFrames.  BG_frame and ROI_frame are released if
     * MT_TrackerBase::releaseFrames is called. */
    virtual void releaseFrames();

    /** Function to create (or resize) frames based on current values of
       FrameWidth and FrameHeight - called by doTrain.  BG_frame is
       created if MT_TrackerBase::createFrames is called.  When this
       is called, the values of FrameWidth and FrameHeight should be
       up-to-date. */
    virtual void createFrames();

    /** Place to initialize data.  This should get called by the
     * ctor.  The argument ProtoFrame is taken to be a prototype
     * frame, representative of the size of normal frames passed to
     * doTracking.  MT_TrackerBase::doInit does the following.
     *  - Sets m_SourceName to "Unknown"
     *  - Sets m_Note to "None"
     *  - If ProtoFrame is non-null, calls doTrain(ProtoFrame)
     *  - If ProtoFrame is null, sets FrameWidth = FrameHeight = 0
     *
     * Example of usage:
     * @code
     * // ALWAYS call the base version first:
     * MT_TrackerBase::doInit(ProtoFrame);
     *
     * // Set up the tracker frame group.  Assuming these have been
     * //  declared as, e.g. IplImage* m_pDifferenceFrame (they can be
     * //  null at this point)
     * m_pTrackerFrameGroup = new MT_TrackerFrameGroup();
     * m_pTrackerFrameGroup->pushFrame(&m_pDifferenceFrame,
     *                                 "Background Difference Frame");
     * m_pTrackerFrameGroup->pushFrame(&m_pThresholdFrame,
     *                                 "Thresholded Frame");
     * // etc..
     *
     * // Set up parameter data groups (here we only use one, but
     * //   multiple groups can be added easily).  Assuming these
     * //   member variables exist and have the right type, etc.
     * MT_DataGroup* dg_params = new MT_DataGroup("Tracking Parameters");
     * dg_params->AddUInt("Difference Threshold", // name
     *                    &m_iThreshold,          // pointer to variable
     *                    MT_DATA_READWRITE,      // allow read-write access
     *                    0,                      // minimum value
     *                    255);                   // maximum value
     * dg_params->AddDouble("Min Blob Area", &m_dMinBlobArea);
     * // etc.. add more variables or create multiple groups
     * // stuff the groups into m_vDataGroups.
     * m_vDataGroups.resize(0);
     * m_vDataGroups.push_back(dg_params);
     * // etc...
     *
     * // Similar for data reports, except these should refer to
     * //   std::vector<some type>.
     * MT_DataReport* dr_blobs = new MT_DataReport("Blob Data");
     * dr_blobs->AddDouble("X [px]", &m_vdBlobXs);
     * dr_blobs->AddDouble("Y [px]", &m_vdBlobYs);
     * // etc...
     * m_vDataReports.resize(0);
     * m_vDataReports.push_back(dr_blobs);
     * @endcode */
    virtual void doInit(IplImage* ProtoFrame);

    /* QUARANTINED - these aren't necessary */
/*    virtual void doImageProcessing(){};
    virtual void doSegmentation(){};
    virtual void doMeasurement(){}; */

    /** This is just a suggestion of a method to write data to a
     * file.  You could override this and call it at the end of
     * doTracking at each time step. */
    virtual void writeData(){};

    /** Called if setDataFile is successful.
     * MT_TrackerBase::initDataFile does the following.
     *  - Writes m_SourceName
     *  - Writes m_Note
     *  - Writes a date and time stamp
     *  - Writes each data group in m_vDataGroups
     *  Note that this means the value of parameters at the time of
     * initialization are written to file.  You can write them
     * manually later if you want to reflect potentially changed
     * values, but in normal operation it's assumed that parameters
     * are set once before tracking. */
    virtual void initDataFile();

    /** Should be set to the name of the video source, e.g. the path
     * to the video or an indication that the source is a camera
     * feed.  Gets written to data file during initDataFile.
     * @see setSourceName */
    const char* m_SourceName;
    /** Note that can e.g. be written to a data file.  Gets written
     * to data file during initDataFile
     * @see setNote
     * @see getNote */
    std::string m_Note;

    std::vector<int> m_viMatchAssignments;
    MT_HungarianMatcher m_HungarianMatcher;

public:

    /** The default ctor should call doInit(NULL).  Use with caution. */
    MT_TrackerBase();
    /** This should be the main ctor that you use.  Pass a frame from
     * the camera/avi so that the various frames get initialized to
     * the right size.  Your ctor should consist of a list of
     * initializers and a call to doInit, e.g.
     * @code
     * myTracker::myTracker(IplImage* ProtoFrame)
     * : m_pSomePointer(NULL),
     *   m_dSomeNumber(0)
     * {
     *     doInit(ProtoFrame);
     * }
     * @endcode */
    MT_TrackerBase(IplImage* ProtoFrame);

    /** The MT_TrackerBase dtor deletes:
     * - All entries in m_vDataGroups
     * - All entries in m_vDataReports
     * - m_pTrackerFrameGroup
     * - Calls releaseFrames() */
    virtual ~MT_TrackerBase();

    /** Uses the provided frame to set FrameWidth and FrameHeight,
     * calls createFrames, and sets BG_frame as a grayscale copy of frame. */
    virtual void doTrain(IplImage* frame);

    /** Load the ROI_frame from a file.  The size is checked against
     * FrameWidth and FrameHeight.  If an error occurs, a message is written
     * to p_error_message (if non-null).  Returns true on success, false on error. */
    virtual bool setROIImage(const char* ROIFilename, std::string* p_error_message = NULL);

    /** Load the BG_frame from a file.  The size is checked against
     * FrameWidth and FrameHeight.  If an error occurs, a message is
     * written to p_error_message (if non-null).  If successful, calls doTrain with
     * the loaded image and returns true.  Returns false on failure. */
    virtual bool setBackgroundImage(const char* BackgroundFilename, std::string* p_error_message = NULL);

    /** Attempts to initialize a datafile with the given filename as m_XDF.
     * If there is an error false is returned and if p_error_message
     * is non-null, an error message is set.  If successful,
     * initDataFile is called and true is returned. */
    virtual bool setDataFile(const char* DataFilename,
                             std::string* p_error_message = NULL);

    /** This should be the main workhorse function of your tracker.
     * When integrated with MT_TrackerFrameBase, this gets called
     * each time step. */
    virtual void doTracking(IplImage* frame){};

    /** Use this function to do any drawing that you want to do.
     * When integrated with MT_TrackerFrameBase, this gets called
     * during each draw cycle.  The integer argument is optional and
     * can be used (or not used) however you want. */
    virtual void doGLDrawing(int flags = MT_TB_NO_FLAGS){};

    /** Returns the frame_index'th frame from m_pTrackerFrameGroup,
     * or NULL if this is not possible.  */
    virtual IplImage* getProcessedFrame(unsigned int frame_index) const;
    /** Returns the number of frames in m_pTrackerFrameGroup, or zero
     * if it has not been initialized. */
    virtual unsigned int getNumProcessedFrames() const;

    /** Returns the number of data groups in m_vDataGroups. */
    unsigned int getNumDataGroups() const;
    /** Returns a pointer to the i'th data group in m_vDataGroups, or
     * NULL if that's not possible. */
    MT_DataGroup* getDataGroup(unsigned int i) const;

    /** Returns the number of data reports in m_vDataReports. */
    unsigned int getNumDataReports() const;
    /** Returns a poitner to the i'th data report in m_vDataReports,
     * or NULL if that's not possible. */
    MT_DataReport* getDataReport(unsigned int i) const;

    /** Returns m_pTrackerFrameGroup. */
    virtual MT_TrackerFrameGroup* getFrameGroup() const
        {return m_pTrackerFrameGroup;};

    /** Update and return the frame rate (in frames per second).
     * If updaterate is false, then the rate is not updated.  Note
     * the rate is only accurate if you call this once per frame with
     * the true argument. */
    virtual double getFrameRate(bool updaterate = true);

    /* function to query how many objects were found in the frame */
    virtual unsigned int getNFound() const {return NFound;};

    virtual MT_TrackedObjectsBase* getTrackedObjects() const {return m_pTrackedObjects;};

    /** Function to determine the bounding box of objects being
     * tracked.  MT_TrackerBase::getObjectBoundingBox returns an
     * empty bounding box (MT_BoundingBox(0,0,0,0)).  Can be used
     * e.g. for auto-zooming. */
    virtual MT_BoundingBox getObjectBoundingBox() const;

    /** Sets m_SourceName.  */
    virtual void setSourceName(const char* name){m_SourceName = name;};
    /** Sets m_Note */
    virtual void setNote(const char* note);
    /** Returns m_Note, possibly checking m_XDF for differences. */
    virtual void getNote(std::string* note);

};

/** @} */

#endif // MT_TRACKERBASE_H
