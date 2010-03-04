#ifndef CCOLOR_H
#define CCOLOR_H

/** @addtogroup MT_Core */
/* @{ */

/** @file
 * cColor.h
 *
 * Defines class MT_Color - a vector-like color convenience class.
 *
 * Also defines several constant colors (e.g. MT_White) and a vector
 * of "primary" colors (not "primary" in the technical sense) and a
 * small handful of support functions.
 *
 * Created DTS 9/4/07
 *  + Added alpha support DTS 11/27/09
 *
 */

#include "MT/MT_Core/support/mathsupport.h" // random number faculties

/** @class MT_Color
 *
 * @brief Vector-like RGBA color class.
 *
 * A vector-like class for colors in RGBA space.  Each element 
 * (red, green, blue, and alpha) is a float point number 
 * between 0 and 1.  Most of the functions automatically 
 * clamp requested values between 0 and 1.  */
class MT_Color
{
protected:
    mutable float data[4];
    char hex_string[7];
    
public:
    /** The default ctor generates a random color with unit alpha.  The RGB
        components are drawn randomly between 0 and 1 and the alpha is set to 1.0. */
    MT_Color();
    /** The RGB ctor sets the RGB components as specified and
        sets the alpha value to 1.0.  Values are automatically
        clamped between 0 and 1. */
    MT_Color(float R, float G, float B);
    /** The RGBA ctor sets each component as specified.  Values
        are automatically clamped between 0 and 1. */
    MT_Color(float R, float G, float B, float alpha);
    
    // Operators
    /** + operator. Adds two colors element-by-element (including alpha).
        Result is automatically clamped. */
    MT_Color operator + (const MT_Color &v);
    /** += operator. Adds two colors element-by-element (including alpha).
        Result is automatically clamped. */
    MT_Color &operator += (const MT_Color &v);
    /** - operator. Subtracts rhs color element-by-element (including alpha)
        from lhs color.  Result is automatically clamped. */
    MT_Color operator - (const MT_Color &v);
    /** -= operator. Subtracts rhs color element-by-element (including alpha)
        from lhs color.  Result is automatically clamped. */
    MT_Color &operator -= (const MT_Color &v);
    /** * scalar operator.  Multiplies each element by the given scalar,
        leaving the alpha unchanged.  Result is automatically clamped. */
    MT_Color operator * (double scalar);
    /** *= scalar operator.  Multiplies each element by the given scalar,
        leaving the alpha unchanged.  Result is automatically clamped. */
    MT_Color &operator *= (double scalar);
    /** / scalar operator.  Divides each element by the given scalar,
        leaving the alpha unchanged.  Result is automatically clamped. 
        If the scalar is zero, a warning message is printed and
        division by a very small number is carried out. */
    MT_Color operator / (double scalar);
    /** /= scalar operator.  Divides each element by the given scalar,
        leaving the alpha unchanged.  Result is automatically clamped. 
        If the scalar is zero, a warning message is printed and
        division by a very small number is carried out. */
    MT_Color &operator /= (double scalar);
    /** float*() operator - returns a pointer to the data vector -
    
        Example:
        \code
        MT_Color myColor(1.0,0,0);
        float* p = (float *)myColor;
        printf("myColor = (%f, %f, %f, %f)\n", p[0], p[1], p[2], p[3])
        \endcode
        would print out "myColor = (1.0, 0.0, 0.0, 1.0)".
     
        This is handy e.g. for glColor. */
    operator float*() const { return &data[0]; };
    /** Parentheses operator - 3 or 4 arguments.  Allows you to set a new color.  Values
        are automatically clamped. */
    void operator ()(float inR, float inG, float inB, float inA = 1.0)
    { setR(inR); setG(inG); setB(inB); setA(inA);};
    /** Parentheses operator - 0 arguments.  Same as default ctor. 
        @see MT_Color() */
    void operator ()()
    { setR(MT_frand(1.0)); setG(MT_frand(1.0)); setB(MT_frand(1.0)); setA(1.0);};
    
    /** Function to get the Hex String corresponding to the RGB triplet.
        The result is a 6-character null-terminated string consisting of
        upper case hex letters and numbers, example red -> FF0000.  The
        value of the alpha channel is currently not used. */
    const char* GetHexString();
    /** Function to set the color by a 6-character hex string.  The alpha value
        is unchanged.
        @param hex_string Must be at least 6 characters, the first 6 of which
        are used to generate the RGB value.  Case insensitive. */
    void SetHexValue(const char* hex_string);

    float R() const { return data[0]; };          /**< Red component accessor   */
    float G() const { return data[1]; };          /**< Green component accessor */
    float B() const { return data[2]; };          /**< Blue component accessor  */
    float A() const { return data[3]; };          /**< Alpha component accessor */
    /** Set red component. Automatically clamped between 0 and 1.      */
    void setR(double in){ data[0] = MT_CLAMP(in,0,1.0); }; 
    /** Set green component. Automatically clamped between 0 and 1.    */
    void setG(double in){ data[1] = MT_CLAMP(in,0,1.0); }; 
    /** Set blue component. Automatically clamped between 0 and 1.     */
    void setB(double in){ data[2] = MT_CLAMP(in,0,1.0); }; 
    /** Set alpha component. Automatically clamped between 0 and 1.    */
    void setA(double in){ data[3] = MT_CLAMP(in,0,1.0); }; 
    
    /** Function to set each color component to zero and alpha to 1.0. */
    void zero();
    /** Function to calculate the squared value of the norm of the color 
        (ignoring the alpha value).  I.e. the result = R^2 + G^2 + B^2 */
    double sqrnorm() const;   
    /** Function to calculate the value of the norm of the color 
        (ignoring the alpha value).  I.e. the result = sqrt(R^2 + G^2 + B^2) */
    double norm() const;
    
};

/** @var const MT_Color MT_LightBlue
    A light-blue color (219, 224, 255) */
const MT_Color MT_LightBlue = MT_Color(219.0/255.0,224.0/255.0,1.0);
/** @var const MT_Color MT_White
    White color (255, 255, 255) */
const MT_Color MT_White = MT_Color(1.0,1.0,1.0);
/** @var const MT_Color MT_Black
    Black color (0, 0, 0) */
const MT_Color MT_Black = MT_Color(0,0,0);
/** @var const MT_Color MT_Red
    Red color (255, 0, 0) */
const MT_Color MT_Red = MT_Color(1.0,0,0);
/** @var const MT_Color MT_Green
    Green color (0, 255, 0) */
const MT_Color MT_Green = MT_Color(0,1.0,0);
/** @var const MT_Color MT_Blue
    Blue color (0, 0, 255) */
const MT_Color MT_Blue = MT_Color(0,0,1.0);
/** @var const MT_Color MT_Brown
    Brown color (153, 102, 255) */
const MT_Color MT_Brown = MT_Color(153.0/255.0,102.0/255.0,51.0/255.0);
/** @var const MT_Color MT_Magenta
    Magenta color (255, 0, 255) */
const MT_Color MT_Magenta = MT_Color(1.0,0,1.0);
/** @var const MT_Color MT_Orange
    Orange color (255, 128, 0) */
const MT_Color MT_Orange = MT_Color(1.0,0.5,0);
/** @var const MT_Color MT_Purple
    Purple color (128, 0, 128) */
const MT_Color MT_Purple = MT_Color(0.5,0,0.5);
/** @var const MT_Color MT_Yellow
    Yellow color (255, 255, 0) */
const MT_Color MT_Yellow = MT_Color(1.0,1.0,0);
/** @var const MT_Color MT_Cyan
    Cyan color (0, 255, 255) */
const MT_Color MT_Cyan = MT_Color(0,1.0,1.0);
/** @var const MT_Color MT_LightGray
    Light gray color (204, 204, 204) */
const MT_Color MT_LightGray = MT_Color(0.8, 0.8, 0.8);
/** @var const MT_Color MT_Gray
    Gray color (128, 128, 128) */
const MT_Color MT_Gray = MT_Color(0.5, 0.5, 0.5);

/** @var const unsigned int MT_NPrimaries
 * Number of primary colors in MT_Primaries array. */
const unsigned int MT_NPrimaries = 8;
/** @var const MT_Color MT_Primaries[]
 * List of "primary" colors.  Useful for generating "random" colors that
 * are guaranteed to cover a range of color space. */
const MT_Color MT_Primaries[MT_NPrimaries] = {MT_Black, MT_White, MT_Red, MT_Green, MT_Blue, MT_Brown, MT_Orange, MT_Purple};

/*--------------------Support Functions---------------*/

/** Function to calculate the dot product between two colors, 
 * ignoring the alpha channels.
 * Result = v1.R*v2.R + v1.G*v2.G + v1.B*v2.B */
double MT_dotproduct(MT_Color v1, MT_Color v2);
/** Function to calculate the "distance" between two colors,
 * ignoring the alpha channels and treating the RGB components
 * as a vector in 3D space. */
double MT_distance(MT_Color v1, MT_Color v2);
/** Function to calculate the squared "distance" between two colors,
 * ignoring the alpha channels and treating the RGB components
 * as a vector in 3D space. */
double MT_distsquared(MT_Color v1, MT_Color v2);

/* @} */

#endif  // CCOLOR_H
