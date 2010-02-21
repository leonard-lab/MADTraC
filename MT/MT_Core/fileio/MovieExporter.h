#ifndef MT_MovieExporter_H
#define MT_MovieExporter_H

/*
 *  MT_MovieExporter.h
 *
 *  Created by Daniel Swain on 12/20/09.
 *  
 */

#ifdef __APPLE__
  #include <OpenCV/OpenCV.h>
#else
  #include <cv.h>
  #include <cvaux.h>
  #include <highgui.h>
  #include <ml.h>
#endif

#include <string>

#include "MT/MT_Core/gl/glImageSupport.h"

const char* const MT_ME_DEFAULT_FRAME_FILENAME_FORMAT = "frame%06d.jpg";

typedef enum _exporter_type
{
    MT_ME_NONE,
    MT_ME_CV_VIDEO_WRITER,
    MT_ME_IMAGE_SEQUENCE,
} MT_ME_Type;

class MT_MovieExporter
{
private:
    MT_ME_Type m_METype; 
    unsigned int m_iFrameNumber;
    unsigned int m_iSkip;
    unsigned int m_iNumSinceLastFrame;
    std::string m_sFilename;

    CvVideoWriter* m_pVideoWriter;

    void releaseVideoWriter();

public:
    MT_MovieExporter();
    ~MT_MovieExporter();

    void initForCvVideoWriter(const char* filename,
                              double frames_per_second,
                              unsigned int frame_width,
                              unsigned int frame_height,
                              unsigned int skip_frames = 0);
    void initForImageSequence(const char* path,
                              const char* filename_formatter = MT_ME_DEFAULT_FRAME_FILENAME_FORMAT,
                              unsigned int skip_frames = 0);

    void saveFrame(const IplImage* frame);

    bool getIsActive() const {return m_METype != MT_ME_NONE;};
    void deactivate(){m_METype = MT_ME_NONE; releaseVideoWriter();};
};

#endif // MT_MovieExporter_H
