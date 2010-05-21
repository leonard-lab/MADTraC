#ifndef GLSUPPORT_H
#define GLSUPPORT_H

/** @addtogroup MT_Core
 * @{ */

/** @file glSupport.h @short Basic OpenGL support and utility functions.
 *
 * Defines several useful functions for drawing basic OpenGL objects.
 *
 * Re-created DTS 8/10/09 using old glSupport.h and glArtist.h
 *
 */

// Carefully include OpenGL
#if defined(__APPLE__) || defined(MACOSX) && !(defined NO_GL)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#elif !(defined NO_GL)
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
//#include <GL/glut.h>
#endif

#include "MT/MT_Core/support/mathsupport.h"  // includes math.h
#include "MT/MT_Core/primitives/R3.h"           // several functions can take an MT_R3 location - also allows GLTranslate3fv(MT_R3)
#include "MT/MT_Core/primitives/Color.h"       // allows overload to glColor3fv(MT_Color)

enum MT_GLYPH
{
    MT_NULL = 0,
    MT_FISH,
    MT_ARROW,
    MT_ELLIPSE,
    MT_DISK,
    MT_CIRCLE,
    MT_DISK_HIGH_RES,
    MT_ARROW_NECK,
    MT_ARROW_HEAD,
    MT_SPHERE
};

/*
 * parameters passed to drawing functions to determine if the shape
 * should be filled.
 */
const bool MT_FILL = true;
const bool MT_NO_FILL = false;

/* Function to initialize the OpenGL lists.  Need this if you want to draw
    any of the predefined shapes we define here.  Omitted from documentation
    because it is called automatically. */
void MT_InitGLLists();

/** Function to translate in 3 dimensions based on a float pointer.  
    Does glTranslate3f(*v,*(v+1),*(v+2));  Handy to use with MT_R3. */
void MT_GLTranslatefv(const float* v);

/** Function to draw a "unit" arrow with length equal to scale. */
void MT_DrawUnitArrow(float scale);
/** Function to draw an arrow with fixed length and potentially fixed width.  If
 * fixed_width < 0, a width is chosen automatically. */
void MT_DrawArrow(float length, float fixed_width = -1.0);
/** Function to draw arrow at given location with given orientation, length, color and (potentially) width.  If fixed_width < 0, a width is chosen automatically. */
void MT_DrawArrow(const MT_R3& center, float length, float orientation, const MT_Color& color, float fixed_width = -1.0);
/** Function to draw a circle with given center, color, and radius (MT_R3 version). */
void MT_DrawCircle(const MT_R3& center, const MT_Color& color, double radius, bool fill = MT_NO_FILL);
/** Function to draw a circle with given center, color, and radius. */
void MT_DrawCircle(float xcenter, float ycenter, const MT_Color& color, double radius, bool fill = MT_NO_FILL);
/** Function to draw an ellipse with given center, axis lengths, orientation,
 * and color (MT_R3 version)*/
void MT_DrawEllipse(const MT_R3& center, float majoraxislength, float minoraxislength, float orientation, const MT_Color& color);

/** Function to draw an arrow made up of possibly dashed
 * lines (as opposed to a solid arrow). */
void MT_DrawLineArrow(double xfrom, double yfrom, double xto, double yto, const MT_Color& color, double head_size = 1.0, double head_at = 1.0, bool dashed = false);

/** Function to draw one of the glyphs defined by InitGLLists. */
void MT_GLDrawGlyph(unsigned int Glyph, const MT_Color& color, const MT_R3& center, double yaw, 
                 double size, double aspect = 1.0, double p1 = 0, double roll = 0, double pitch = 0);

/** Function to draw a line centered at MT_R3 center, with specified length, angle, and color. */
void MT_DrawLineCenterLengthAngleColor(const MT_R3& center, float length, float angle, const MT_Color& color);
/** Function to draw a line from one point to another, with the specified color. */
void MT_DrawLineFromTo(float xfrom, float yfrom, float xto, float yto, const MT_Color& color = MT_Red);
/** Function to draw a rectangle given the bottom left corner, width, and height, with given color */
void MT_DrawRectangle(float x, float y, float width, float height, const MT_Color& color = MT_Red);

/** Utility function to calculate a rectangle that preserves the original center and 
 * obeys the aspect ratio defined by ref_width/ref_height.  Fixes either the width
 * or height, depending upon the ratio.  The values are calculated based on
 * xmin, xmax, ymin, ymax and the results replace the original values.
 * @param ref_width Reference width to determine desired aspect ratio.
 * @param ref_height Reference height to determine desired aspect ratio.
 * @param xmin Minimum x value of input/output rectangle. 
 * @param xmax Maximum x value of input/output rectangle. 
 * @param ymin Minimum y value of input/output rectangle. 
 * @param ymax Maximum y value of input/output rectangle. 
 * @param expand_fraction Optional fraction to expand the result by.  Handy
 *      for calculating viewports with a fractional margin.  For example,
 *      to expand by 10% use 0.1.  0 corresponds to no expansion.  Can
 *      be < 1.
 */
void MT_CalculateAspectRectangle(double ref_width, double ref_height, double* xmin, double* xmax, double* ymin, double* ymax, double expand_fraction = 0);

/* @} */

#endif  // GLSUPPORT_H
