#ifndef MT_CAPTURE_H
#define MT_CAPTURE_H

/** @addtogroup MT_Tracking
 * @{ */

/** @file
 *  MT_Capture.h
 *
 *  Attempts to provide a class frontend for frame capture.  Uses 
 *  OpenCV for capture from a movie file and ARToolkit for capture
 *  from a camera.  I developed this way because the ARToolkit
 *  interface to QuickTime includes functionality to bring up
 *  the camera setup dialog.  This is untested on systems other
 *  than OS X 10.5.
 * 
 *  On OS X this needs to link against the ARToolkit, OpenCV,
 *  QuickTime.framework, OpenGL.framework, and GLUT.framework.
 *
 *  Created by Daniel Swain on 12/22/07.
 *
 */

/* Camera capture options:  ARToolkit or OpenCV.  OpenCV now opens the
   QT video options dialog on OS X, but doesn't appear to allow changing
   video parameters (e.g. frame size), so I've left the option to build
   against the ARToolkit camera framework instead, which does allow video
   parameter settings but increases the executable size. */
 
#if defined(__APPLE__) || defined(MACOSX)
//#define CAMERA_CAPTURE_NONE
#define MT_CAMERA_CAPTURE_ARTOOLKIT
//#define CAMERA_CAPTURE_OPENCV

// KUT EDIT: With Windows, OpenCV is used by default.
#else
#define MT_CAMERA_CAPTURE_OPENCV
#endif

/* Define if using the AVT Guppy color camera.  Or any other camera in which
   we need to convert a single frame to color using the Bayer method. */
//#define MT_FC_DEBAYER

/* Define this to use threads to significantly speed up capture speed.
   After capturing the first frame, the program will spawn a new thread
   that gets the next frame in the background while continuing on with
   the current frame.  Currently this will NOT work with more than one
   AVI capture simultaneously.  (Note currently threading is not
   implemented for camera capture and hence threading is OK with one
   camera capture and one AVI capture concurrently). */
#define MT_FC_USE_THREADS

// Includes for OpenCV with Mac
#if defined(__APPLE__) || defined(MACOSX)
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

#include <map>
#include <vector>


// Defines to make capture options more readable
const int MT_FC_DEFAULT_CAM_PARAMS = -1;
const int MT_FC_DEFAULT_FW = -1;     /**< Default framewidth              */
const int MT_FC_DEFAULT_FH = -1;     /**< Default frameheight             */
const bool MT_FC_NODIALOG = false;   /**< Don't show video options dialog */
const bool MT_FC_SHOWDIALOG = true;  /**< Show video options dialog       */
const bool MT_FC_NOFLIP = false;     /**< Don't flip video                */
const bool MT_FC_FLIP = true;        /**< Flip video                      */

const int MT_FC_ERR = -1;

// Defined modes
typedef enum MT_FC_MODE_t
{
    MT_FC_MODE_CAM = 0,
    MT_FC_MODE_AVI,
    MT_FC_MODE_OFF
} MT_FC_MODE_t;


// call with constructor for no initialization
const char* const MT_FC_NO_INIT = NULL;

// get the next frame (default argument to getFrame)
const int MT_FC_NEXT_FRAME = -1;

/** work-in-progress below *********************************/

/* define MT_HAVE_ARTOOLKIT if ARToolKit is available
   for a camera interface */
#ifndef MT_NO_ARTOOLKIT
#define MT_HAVE_ARTOOLKIT
#endif

/* define MT_HAVE_AVT if the Allied Vision Tech (e.g. Guppy)
   drivers are available for a camera interface */
#define MT_HAVE_AVT

/* MT_HAVE_AVT should be automatically disabled if this isn't a
 * windows machine.  */
#ifndef WIN32
#undef MT_HAVE_AVT
#endif

/* for now we're not using ARTK on windows */
#ifdef WIN32
#undef MT_HAVE_ARTOOLKIT
#endif

typedef enum
{
    MT_CAP_NONE = 0,      /* Just create a capture - do no
                             initialization                     */
    MT_CAP_DEFAULT,       /* Default type - see MT_Capture      */

    /* Group "file" interface types here (e.g. avi captures)    */
    MT_CAP_CV_FILE,       /* OpenCV File - i.e. AVI capture     */

    /* Group camera interface types here                        */
    MT_CAP_CV_CAMERA,     /* OpenCV Camera interface            */
    MT_CAP_ARTK_CAMERA,   /* ARToolKit Camera interface         */
    MT_CAP_AVT_CAMERA,    /* AVT Camera interace                */

    MT_CAP_sentinel       /* Sentinel entry - do not use, always
                             leave as the last entry            */
} MT_Cap_Iface_Type;

#define MT_CAP_FIRST 0

/* defining what "default" means on each system */
#if defined(WIN32) && defined(MT_HAVE_AVT)
    const MT_Cap_Iface_Type MT_CAP_DEFAULT_IFACE = MT_CAP_AVT_CAMERA;
#elif defined(MT_HAVE_ARTOOLKIT)
    const MT_Cap_Iface_Type MT_CAP_DEFAULT_IFACE = MT_CAP_ARTK_CAMERA;
#else
    const MT_Cap_Iface_Type MT_CAP_DEFAULT_IFACE = MT_CAP_CV_CAMERA;
#endif

#include "MT_Capture_Interfaces.h"

/*********************************************************************
 *
 * Interface Table Types
 *
 * The interface table is how the MT_Capture module keeps track of
 * what interfaces are available on a given system.
 *
 * @see MT_Cap_Iface_Type
 * @see MT_Capture::s_IfaceTable
 *
 *********************************************************************/

/* type for a single entry in the table */
typedef struct t_entry
{
    /* the type is a little redundant since this will also be the
     * key for the map, but this way the returned object encapsulates
     * all of the relevant information */
    MT_Cap_Iface_Type Type;
    /* tells us whether or not we can use this interface on this system */
    bool IsAvailable;
    /* human-readable name for this interface */
    const char* Name;

    /* default ctor should make it clear that this is an
     * uninitialized entry.  The table is actually an std::map,
     * which returns a default-constructed element when the
     * reference key is not found - this makes sure that the
     * returned object gives us some indication that an error
     * occurred.
     */
    t_entry() 
        : Type(MT_CAP_NONE), IsAvailable(false), Name("Key Match Error"){};

    /* "Normal" ctor to define a table entry */
    t_entry(MT_Cap_Iface_Type of_type, bool avail, const char* name) 
        : Type(of_type), IsAvailable(avail), Name(name){};

} MT_Cap_Iface_Table_Entry;

typedef t_entry MT_Cap_Iface_Table_Entry;

/* Just shorthand */
typedef std::map<MT_Cap_Iface_Type, MT_Cap_Iface_Table_Entry> MT_Cap_Iface_Table;
typedef std::vector<MT_Cap_Iface_Table_Entry> MT_Cap_Iface_Vector;

/* Forward declaration of Iface classes */
class MT_Cap_Iface_Base;

/** MT_Capture Rework for multiple interfaces.  
*
* In progress.  DTS 2/1/10.
*/
class MT_Capture
{
private:
    /* vector of interfaces */
    std::vector<MT_Cap_Iface_Base*> m_vpInterfaces;

    /* vector of interface types */
    std::vector<MT_Cap_Iface_Type> m_vIfaceTypes;
  
    /* Default inteface used for new camera interfaces */
    MT_Cap_Iface_Type m_DefaultIface;

    /* common initialization */
    void doCommonInit(MT_Cap_Iface_Type default_type = MT_CAP_CV_CAMERA);

    static MT_Cap_Iface_Table s_IfaceTable;

    static void buildInterfaceTable();
  
public:   
    /* default ctor initializes with no interfaces, option to
     set default interface type */
    MT_Capture(MT_Cap_Iface_Type default_type = MT_CAP_DEFAULT);

    /* ctor with a filename initializes with a single interface
       pointing to a single CV movie file */
    MT_Capture(const char* filename, MT_Cap_Iface_Type default_type = MT_CAP_DEFAULT);
  
    /* ctor with camera parameters initializes to 
       OpenCV camera interface */
    MT_Capture(int FW,   /* pass MT_FC_DEFAULT_CAM_PARAMS for all defaults
                            note this function declaration can't have default
                            parameters for FW b/c MT_Capture() would be
                            ambiguous */
               int FH = MT_FC_DEFAULT_FH, 
               bool ShowDialog = MT_FC_SHOWDIALOG,
               bool FlipH = MT_FC_NOFLIP,
               bool FlipV = MT_FC_NOFLIP,
               MT_Cap_Iface_Type type = MT_CAP_DEFAULT);
  
    /* Destructor */
    ~MT_Capture();

    MT_Cap_Iface_Type setDefaultInterface(MT_Cap_Iface_Type type = MT_CAP_DEFAULT);
    MT_Cap_Iface_Type getDefaultInterface() const;
    const char* getDefaultInterfaceName() const;

    const MT_Cap_Iface_Vector getAvailableInterfaces() const;
  
    /* initialize a capture using a file on the next availabe interface */
    bool initCaptureFromFile(const char* filename);

    /* initialize a capture using a camera on the next availabe interface */
    bool initCaptureFromCamera(int FW = MT_FC_DEFAULT_FW, 
                               int FH = MT_FC_DEFAULT_FH, 
                               bool ShowDialog = MT_FC_SHOWDIALOG,
                               bool FlipH = MT_FC_NOFLIP,
                               bool FlipV = MT_FC_NOFLIP,
                               MT_Cap_Iface_Type type = MT_CAP_DEFAULT);
   
    MT_Cap_Iface_Type getInterface(unsigned int iface_num) const;
    const char* getInterfaceName(unsigned int iface_num);

    int getNumInterfacesOpen() const {return m_vpInterfaces.size();};

    MT_FC_MODE_t getMode(unsigned int iface = MT_CAP_FIRST) const;
    int getNFrames(unsigned int iface = MT_CAP_FIRST) const;
    int getNChannels(unsigned int iface = MT_CAP_FIRST) const;
    int getFrameNumber(unsigned int iface = MT_CAP_FIRST) const;
    int setFrameNumber(int frame_index, unsigned int iface = MT_CAP_FIRST);
  
    // Poll for, and return, a new frame
    IplImage* getFrame(int frame_index = MT_FC_NEXT_FRAME, 
            unsigned int iface = MT_CAP_FIRST);
  
    // poll the frame rate
    double getFPS(unsigned int iface = MT_CAP_FIRST) const;
    // get the frame period in msec
    int getFramePeriod_msec(unsigned int iface = MT_CAP_FIRST) const;
  
    // return the size of the frame in a CvSize structure
    CvSize getFrameSize(unsigned int iface = MT_CAP_FIRST) const;
  
    // Query the size of the frame (in pixels)
    int getFrameWidth(unsigned int iface = MT_CAP_FIRST) const;
    int getFrameHeight(unsigned int iface = MT_CAP_FIRST) const;
  
    // Get some information - either the filename or camera
    const char* getTitle(unsigned int iface = MT_CAP_FIRST) const;
  
    // provide an interface to save the current frame
    void saveFrame(const char* filename, unsigned int iface = MT_CAP_FIRST) const;

    double getProgressFraction(unsigned int iface = MT_CAP_FIRST) const;
    bool getIsAtEnd(unsigned int iface = MT_CAP_FIRST) const;
  
};

/** @} */

#endif  // MT_CAPTURE_H
