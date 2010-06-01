#ifndef MATHSUPPORT_H

/** @addtogroup MT_Core
 * @{ */

/** @file  mathsupport.h - D.Swain 8/10/07
 * 
 *   Defines several mathematics, random number, and time-related
 *   functions.  Grouped as:
 *
 *    - Numerical constants
 *    - Arithmetic macros
 *    - Numerical limits
 *    - Random number generation
 *    - Floating point arithmetic utility functions
 *    - Time functions
 *    - String analysis
 *  
 */

// We shouldn't need to include math.h in other files if
//   this is included here
#include <math.h>
#include <stdlib.h>  /* has gnu abs() */
#include <stdio.h>   /* propagate printf, etc */
#include <limits>

// --------- Numerical Constants -----------------------------

/** @var const double MT_PI
 * Numerical constant ~= 3.141592653... = atan2(0,-1) */
const double MT_PI = atan2(0.0,-1.0);
/** @var const double MT_TWO_PI
 * Numerical constant 2*pi */
const double MT_TWO_PI = 2.0*MT_PI;
/** @var const double MT_RAD2DEG
 * Multiplicative constant to convert radians to degrees = 180/pi */
const double MT_RAD2DEG = 180.0/MT_PI;
/** @var const double MT_DEG2RAD
 * Multiplicative constant to convert degrees to radians = pi/180 */
const double MT_DEG2RAD = MT_PI/180.0;
/** @var const double MT_EPSILON
 * Numerical small-number constant.  Used, for example to judge
 * whether two floating-point numbers are equal.
 * @see MT_IsEqual */
const double MT_EPSILON = 1E-5;

/* not every system has M_PI */
#ifndef M_PI
#define M_PI MT_PI
#endif

// --------- Macros ------------------------------------------
/** @def MT_SGN(x)
 * Returns 1.0 if x >= 0 and -1.0 if x < 0 */
#define MT_SGN(x) ((x)>=(0)?(1.0):(-1.0))
/** @def MT_INTSGN(x)
 * Returns 1 if x >= 0 and -1 if x < 0 (int version of SGN) */
#define MT_INTSGN(x) ((x)>=(0)?(1):(-1))
/** @def MT_MIN(a,b)
 * Returns the minimum of a or b.  Won't clobber existing implementations. */
#ifndef MT_MIN
#define MT_MIN(a,b) ((a)>(b)?(b):(a))
#else
#define MT_MIN(a,b) MIN(a,b)
#endif
/** @def MT_MIN(a,b)
 * Returns the maximum of a or b.  Won't clobber existing implementations. */
#ifndef MT_MAX
#define MT_MAX(a,b) ((a)>(b)?(a):(b))
#else
#define MT_MAX(a,b) MAX(a,b)
#endif
/** @def MT_CLAMP(x, a, b)
 * "Clamp" function - returns x if a < x < b, a if x <= a, and b if x >= b. 
 * Won't clobber existing implementations.  */
#ifndef MT_CLAMP
#define MT_CLAMP(x, a, b) (((x) > (b)) ? (b) : (((x) < (a)) ? (a) : (x)))
#endif
/** @def MT_isnan(x)
 * Returns true if x is NaN and false if x is a valid number. */
#ifndef MT_isnan
#define MT_isnan(x) ((x) != (x))
#endif

/* ------ Numeric Limits -----------------------------------*/
/* VC++ defines these min and max macros, which we don't use and
 * cause problems below. */
#ifdef _WIN32
  #undef min
  #undef max
#endif

#ifndef isdigit
#define isdigit MT_isdigit
#endif

#include <limits> 
using namespace std;

bool MT_isdigit(char c);

/* maximum and minimum possible values of various numeric types */
/** @var const int MT_min_int 
 * Minimum system signed integer */
const int MT_min_int = numeric_limits<int>::min();
/** @var const int MT_max_int 
 * Maximum system signed integer */
const int MT_max_int = numeric_limits<int>::max();
/** @var const unsigned int MT_max_uint 
 * Maximum system unsigned integer */
/* The value of numeric_limits<unsigned int>::max() is -1 on my system !? */
const unsigned int MT_max_uint = numeric_limits<int>::max();
/** @var const long MT_max_long
 * Maximum system signed long integer */
const long MT_min_long = numeric_limits<long>::min();
/** @var const long MT_min_long
 * Minimum system signed long integer */
const long MT_max_long = numeric_limits<long>::max();
/** @var const char MT_min_char
 * Minimum system signed char */
const char MT_min_char = numeric_limits<char>::min();
/** @var const char MT_max_char
 * Maximum system signed char */
const char MT_max_char = numeric_limits<char>::max();
/** @var const unsigned char MT_max_uchar
 * Maximum system unsigned char */
const unsigned char MT_max_uchar = numeric_limits<unsigned char>::max();
/** @var const float MT_min_float
 * Minimum system single precision floating point number */
const float MT_min_float = -numeric_limits<float>::max();
/** @var const float MT_max_float
 * Maximum system single precision floating point number */
const float MT_max_float = numeric_limits<float>::max(); 
/** @var const double MT_min_double
 * Minimum system double precision floating point number */
const double MT_min_double = -numeric_limits<double>::max();
/** @var const double MT_max_double
 * Maximum system double precision floating point number */
const double MT_max_double = numeric_limits<double>::max();

// --------- Random number functions ------------------------

// NOTE  in each of the following, "random" should be taken
//  to mean "pseudo-random" - in the sense that it isn't
//  really possible to have a computer generate a truly
//  random number, and that the generation algorithms here
//  are relatively crude (though fairly standard)

/** Seed the random number generator */
void MT_rand_init();

// NOTE  all random numbers are generated by manipulating the
//   output of MT_frand(1.0), MT_frand() generates random floats by
//   modulus and division by 1000000.0.  these numbers are
//   approximately uniformly distributed

/** Return a random float between -R and R */
float MT_FRND(float R);
/** Return a random float between fmin and fmax */
float MT_frandr(float fmin, float fmax);
/** Return a random float between 0 and 1  */
float MT_frand(float f);
/** Return a random integer between 0 and m - 1 */
int MT_randint(int m);

// NOTE  normally distributed random numbers here are
//  approximated by averaging 12 uniformly distributed
//  random numbers.  I've tested this by generating
//  large numbers of random numbers and comparing the
//  resulting histograms to Matlab's MT_randn().  The results
//  are indistinguishable.  Though one should NOTE that
//  there is NO chance of getting a number greater than 6
//  or less than -6 with this method.  In reality the
//  probability of such is VERY small.  There are methods
//  that doe not have this problem, but they are typically
//  computationally more expensive.

/** Return an approximately normally (Gaussian) distributed random number 
 * with mean zero and std dev 1 */
float MT_randn();
/** Return an approximately normally distributed random number with 
 * specified mean and std dev  */
float MT_randn(float mean, float std);

// --------- Angle functions -------------------------------
/** Return an equivalent angle (in radians) in the range -MT_PI to MT_PI */
double MT_anglemod(double theta);

/* --------- Floating point math functions -----------------*/
/** Compare two doubles - returns true if the absolute difference 
 * is within a fraction epsilon of x */
bool MT_IsEqual(double x, double y, double epsilon = MT_EPSILON);

// --------- Time functions --------------------------------
/** Return the system time in seconds, with roughly
 * one millisecond accuracy */
double MT_getTimeSec(void);
/** Get the last two digits of the current year. */
int MT_getYearYY(void);
/** Get the current month as an integer 1-12 */
int MT_getMonthMM(void);
/** Get the current day-of-month as an integer */
int MT_getDay(void);
/** Get the current hour as an integer 0-23 */
int MT_getHour24(void);
/** Get the current minute as an integer 0-59 */
int MT_getMinute(void);
/** Get the current second as an integer 0-59 */
int MT_getSecond(void);
/** Get the YYMMDD and HHMMSS strings. 
 * @param YYMMDD Pointer to a character array of length at least 6
 *              that will hold the resulting YYMMDD string.
 * @param HHMMSS Pointer to a character array of length at least 6
 *              that will hold the resulting HHMMSS string. 
 *
 * As an example, at 5:22:12 PM on December 2, 2009, this would set
 * HHMMSS to 172212 and YYMMDD to 091202.  Useful for generating
 * unique filenames.  NOTE that the function does not allocate the
 * strings, it assumes you have already done that.  */
void MT_getYYMMDDandHHMMSS(char* YYMMDD, char* HHMMSS);


/* --------- String analysis functions --------------------- */

/** Return values for MT_hasFormatString.
 * @see MT_hasFormatString
 */
typedef enum
{
    MT_FORMAT_STRING_INVALID = 0, /**< No valid format string found.     */
    MT_FORMAT_STRING_CHAR,        /**< Char (%c) format string found.    */
    MT_FORMAT_STRING_STRING,      /**< String (%s) format string found.  */
    MT_FORMAT_STRING_INT,         /**< Integer-type format string found. */
    MT_FORMAT_STRING_FLOAT        /**< Float-type format string found.   */
} MT_FORMAT_STRING_TYPE;

/** Checks a string to determine if it contains a valid
 * printf-style format string (e.g. %5.4f, %d, etc).  I've done my
 * best to check against any valid formatter.  Returns an enum 
 * (MT_FORMAT_STRING_TYPE) indicating the <i>type</i> of formatter found,
 * or 0 (MT_FORMAT_STRING_INVALID) if no valid format string is found.
 *
 * @param string_to_check Null-terminated string to check for format.
 * @param p_startof Integer to get the index of the start of the format
 *      string.  Ignored if NULL (default) or no valid format string is found.
 * @param p_endof Integer to get the index of the end of the format
 *      string.  Ignored if NULL (default) or no valid format string is found.
 */
MT_FORMAT_STRING_TYPE MT_hasFormatString(const char* string_to_check, 
        int* p_startof = NULL, 
        int* p_endof = NULL);

/* @} */

#define MATHSUPPORT_H
#endif
