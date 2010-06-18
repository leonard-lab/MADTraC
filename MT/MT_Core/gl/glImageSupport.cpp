/*
 *  glImageSupport.cpp
 *
 *  Created by Daniel Swain on 8/10/09.
 *
 */

#include "glImageSupport.h"
#include <stdio.h>

GLuint g_texID;

#define MT_BYTE_ALIGNMENT 1

void MT_TextureInitForCV(GLuint* texID)
{

    // 1 means generate one texture
    glGenTextures(1,texID);
    g_texID = *texID;
  
    glBindTexture(GL_TEXTURE_2D, *texID);
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, MT_BYTE_ALIGNMENT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glDisable(GL_TEXTURE_2D); 
  
}

void MT_DisplayCVImage(IplImage* image, float xmin, float xmax, float ymin, float ymax)
{
#ifdef MT_NO_OPENCV
    return;
#else    

    glBindTexture(GL_TEXTURE_2D, g_texID);
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, MT_BYTE_ALIGNMENT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    // null images will cause memory access errors
    if(!image)
    {
        return;
    }
  
    GLenum format;

    switch(image->nChannels)
    {
    case 1:
        format = GL_LUMINANCE; break;
    case 3:
        format = GL_BGR; break;
    case 4:
        format = GL_RGBA; break;
    default:  /* unknown - try BGR */
        format = GL_BGR; break;
    }

    // This is the case on some older graphics cards
#ifdef NEED_POWERS_OF_2
    /* TODO:  We need to check against the maximum allowed
     * texture size on the current machine.  This can be
     * queried by
     *
     * GLint texSize;
     * glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
     *
     * It will require a nontrivial change to the below code.
     */
  
    // First, how big do we need?
    int tex_size = 64;  // 64 is the minimum allowed
    while(tex_size < MT_MAX(image->width,image->height))
    {
        tex_size *= 2;
    }

    // defining these statically means we don't have to do this every time, which
    //  saves a lot of time and memory
    IplImage* image2 = cvCreateImage(cvSize(tex_size,tex_size), IPL_DEPTH_8U, image->nChannels);
    CvMat image3;
    CvMat* image4;
  
    // here we copy into the bigger image, and set the extra space to black
    image4 = cvGetSubRect(image2, &image3, cvRect(0,tex_size - image->height,image->width,image->height));
    cvZero(image2);
    cvCopy(image, image4);
  
    // we paint the texture on an area bigger than the window so that the
    //  scaling is still correct (only the image should be in the window)
    xmin = ymin = 0;
    xmax = ymax = tex_size;
  
    // now we can paint the texture
    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 
        0, 
        GL_RGBA,
        image2->width,
        image2->height,
        0,
        format,
        GL_UNSIGNED_BYTE, 
        image2->imageData); 
#else
    // this is hopefully what happens on "modern" machines
    //glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 
                 0, 
                 GL_RGBA8,
                 image->width,
                 image->height,
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 image->imageData); 
#endif

    glBegin(GL_POLYGON);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(xmin, ymin);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(xmax, ymin);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(xmax, ymax);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(xmin, ymax);
    glEnd();
    glDisable(GL_TEXTURE_2D);

#ifdef NEED_POWERS_OF_2
    cvReleaseImage(&image2);
#endif

#endif /* !MT_NO_OPENCV */    
  
}


int MT_SaveGLBuffer(int width, int height, const char* filename)
{

#ifdef MT_NO_OPENCV
    return MT_SAVER_ERROR;
#else    
    // Save what is currently displayed into dump buffer
    IplImage *img=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U, 3);
    IplImage *img1=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U, 3);
  
    glReadPixels(0,0,width,height,
                 GL_RGB, GL_UNSIGNED_BYTE, img->imageData);
  
    cvConvertImage( img, img1, CV_CVTIMG_FLIP | CV_CVTIMG_SWAP_RB );
  
    cvSaveImage(filename,img1);
  
    cvReleaseImage(&img1);
    cvReleaseImage(&img);
  
    return MT_SAVER_OK;
#endif /* !MT_NO_OPENCV */  
}

int MT_GLBufferToIplImage(int width, int height, IplImage* image)
{
#ifdef MT_NO_OPENCV
    return MT_SAVER_ERROR;
#else  
    if(!image || image->width != width || image->height != height || image->nChannels != 3)
    {
        return MT_SAVER_ERROR;
    }
  
    IplImage* temp_image = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 3);
  
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE, temp_image->imageData);
    cvConvertImage(temp_image, image, CV_CVTIMG_FLIP | CV_CVTIMG_SWAP_RB);
  
    cvReleaseImage(&temp_image);
  
    return MT_SAVER_OK;
#endif /* !MT_NO_OPENCV */  
}
