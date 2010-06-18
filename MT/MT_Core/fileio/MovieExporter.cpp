/*
 *  MT_MovieExporter.cpp
 *
 *  Created by Daniel Swain on 12/20/09.
 *
 */

#include "MovieExporter.h"

#include <sys/stat.h>

#ifndef MT_NO_OPENCV
static int MT_FOURCC = CV_FOURCC('M','J','P','G');
#endif

static const int MAX_FILENAME_LENGTH = 512;

#ifndef _WIN32
  static std::string path_sep = "/";
#else
  #include <direct.h>
  #define mkdir(a,b) _mkdir(a)
  static std::string path_sep = "\\";
#endif

static void mkdir_if_necessary(const char* dir_name)
{
  struct stat st;
  if(stat(dir_name, &st) == 0)
  {
    return;
  }
  else
  {
    mkdir(dir_name, 0777);
  }
}

MT_MovieExporter::MT_MovieExporter()
: m_METype(MT_ME_NONE),
  m_iFrameNumber(0),
  m_iSkip(0),
  m_iNumSinceLastFrame(0),
  m_sFilename(""),
  m_pVideoWriter(NULL)
{
}

MT_MovieExporter::~MT_MovieExporter()
{
    releaseVideoWriter();
}

void MT_MovieExporter::releaseVideoWriter()
{
#ifndef MT_NO_OPENCV    
    if(m_pVideoWriter)
    {
        cvReleaseVideoWriter(&m_pVideoWriter);
    }
#endif
}

void MT_MovieExporter::initForCvVideoWriter(const char* filename,
                                         double frames_per_second,
                                         unsigned int frame_width,
                                         unsigned int frame_height,
                                         unsigned int skip_frames)
{
    releaseVideoWriter();
#ifndef MT_NO_OPENCV    
    m_pVideoWriter = cvCreateVideoWriter(filename, 
                                         MT_FOURCC, 
                                         frames_per_second, 
                                         cvSize(frame_width, frame_height));
#endif    
    m_iSkip = skip_frames;
    m_sFilename = filename;
    m_METype = MT_ME_CV_VIDEO_WRITER;
}

void MT_MovieExporter::initForImageSequence(const char* path,
                                         const char* filename_formatter,
                                         unsigned int skip_frames)
{
    m_iSkip = skip_frames;
    m_METype = MT_ME_IMAGE_SEQUENCE;

    mkdir_if_necessary(path);

    m_sFilename = std::string(path) + path_sep + std::string(filename_formatter);
}

#ifndef MT_NO_OPENCV
void MT_MovieExporter::saveFrame(const IplImage* frame)
{
    m_iNumSinceLastFrame++;

    char _filename[MAX_FILENAME_LENGTH];

    if(m_iNumSinceLastFrame < m_iSkip)
    {
        return;
    }

    switch(m_METype)
    {
    case MT_ME_CV_VIDEO_WRITER:
#ifndef MT_NO_OPENCV        
        cvWriteFrame(m_pVideoWriter, frame);
#endif        
        break;
    case MT_ME_IMAGE_SEQUENCE:
        sprintf(_filename, m_sFilename.c_str(), m_iFrameNumber);
#ifndef MT_NO_OPENCV        
        cvSaveImage(_filename, frame);
#endif        
        break;
    case MT_ME_NONE:
    default:
        break;
    }
    m_iNumSinceLastFrame = 0;
    m_iFrameNumber++;
}
#endif /* !MT_NO_OPENCV */
