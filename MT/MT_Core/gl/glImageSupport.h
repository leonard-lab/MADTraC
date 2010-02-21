#ifndef GLIMAGESUPPORT_H
#define GLIMAGESUPPORT_H

/*
 *  glImageSupport.h
 *
 *  Created by Daniel Swain on 8/10/09.
 *
 *  Contains functions to initialize and display an IplImage on a
 *  OpenGL texture canvas.  Also contains functions to save the current
 *  rendered OpenGL buffer to either a file or an IplImage.
 *
 */

// Carefully include OpenGL
#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
//#include <GL/glut.h>
#endif

// NOTE It may be possible to do this without linking against
//  OpenCV, we really just need a few functions to work with IplImages
#if defined(__APPLE__) || defined(MACOSX)
#include <OpenCV/OpenCV.h>
#else
#include <cxtypes.h>
#include <cv.h>
#include <highgui.h>
#endif

// may not be defined correctly in some implementations
#ifndef GL_BGR
#define GL_BGR GL_BGR_EXT
#endif

/* NOTE:  On older machines and some machines with integrated graphics
   drivers (especially if OpenGL is older than 2.0), textures
   can only be mapped with images whose sizes are powers of 2
   e.g. 64, 128, 256... If the image is not being displayed,
   try defining this and rebuilding.  This will pad the image
   to make it a power of 2, but keep the screen coordinates
   correct. 
   NOTE 2:  This also happens on even "modern" machines when using
   remote desktop software.  For example in Windows Remote Desktop
   it turns out that OpenGL compatibility is about 1.1 at best, no
   matter how good the machine's graphics card is. */
//#define NEED_POWERS_OF_2

/************ Texture / Display Support ********************/
/** Function to initialize an OpenGL texture for i.e. OpenCV image display */
void MT_TextureInitForCV(GLuint* texID);
/** Function to display an IplImage on an OpenGL texture */
void MT_DisplayCVImage(IplImage* image, float xmin, float xmax, float ymin, float ymax);

/************ GL Buffer -> Image (possibly save) support ***********/
// return constants
#define MT_SAVER_OK     0
#define MT_SAVER_ERROR  1
/** Function to save the current OpenGL buffer to filename - uses the OpenCV highgui
    to save the image. */
int MT_SaveGLBuffer(int width, int height, const char* filename);
/** Function to obtain the current OpenGL buffer rendered to an IplImage.  */
int MT_GLBufferToIplImage(int width, int height, IplImage* image);

#endif // GLIMAGESUPPORG_H
