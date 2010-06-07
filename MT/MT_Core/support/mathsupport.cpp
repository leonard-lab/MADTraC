//------------------------------------------------------------
//  mathsupport.cpp - glswarm/robofish - D.Swain 8/10/07
//  
//  Various numerical routines that don't fit nicely in any
//    other modules.  Including:
//      - Random Number Generation
//      - Mathematics for dealing with angles
//      - Basic macros (defined in the header)
//      - Timebase generation
//------------------------------------------------------------

#include <stdlib.h>     // for rand()
#include <string.h>     // for strlen()
#include <stdio.h>      // for sprintf()

#ifdef _WIN32
#include <time.h>     // windows timing
#include <windows.h>
#else
#include <time.h>     /* linux timing */
#include <sys/time.h> // posix timing
#endif

// Header for this module
#include "mathsupport.h"

//------------------------------------------------------------
//    Random Number Functions
//------------------------------------------------------------

/** Function to generate a random float in the range
    -R to +R. */
float MT_FRND(float R)
{
    return (2*fabs(R)*MT_frand(1.0) -fabs(R));
}

/** Function to generate a random float in the range
    fmin to fmax */
float MT_frandr(float fmin, float fmax)
{
    return (MT_MIN(fmin,fmax) + 
            (MT_MAX(fmin,fmax) - MT_MIN(fmin,fmax))*MT_frand(1.0));
}

/** Function to generate a random float in the range 0
    to f. */
#ifndef XSCREENSAVER_MODE
float MT_frand(float f)
{
#ifdef _WIN32
    return f*((rand() % 1000) / 1000.0);
#else  
    return f*((rand() % 1000000) / 1000000.0);
#endif
}
#endif

/** Function to initialize the random number generator.*/
void MT_rand_init()
{
    srand( time(NULL) );
}

/** function to return a random integer between 0 and m-1 */
int MT_randint(int m)
{
  
    return rand() % m;
  
}

/** Function to approximate a random number drawn from
    a normal distribution with zero mean and unit standard
    deviation.  Approximates by summing 12 random numbers
    from a uniform distribution. */
float MT_randn()
{
    float result = 0;
    for(int i = 0; i < 12; i++)
        result += MT_FRND(0.5);
    return result;
}

/** Function to approximate a random number drawn from
    a normal distribution with specified mean and standard
    deviation.  Uses a simple linear transformation on a
    normal distribution.  See MT_randn() for method. */
float MT_randn(float mean, float std)
{
    return std*MT_randn() + mean;
}

//------------------------------------------------------------
//    Angle Functions
//------------------------------------------------------------

/** Function to restrict an angle to the range
    -pi to +pi */
double MT_anglemod(double theta)
{
    // First reduce it to within 2pi of the origin
    theta = fmod(theta,2.0*MT_PI);

    // Then wrap it into the +/- pi range
    if(theta > MT_PI)                // for positive angles
        return theta - 2.0*MT_PI;
    else if(theta < -1.0*MT_PI)      // for negative angles
        return theta + 2.0*MT_PI;
    else
        return theta;               // these angles are just right
}

/*------------------------------------------------------------
  Floating Point Arithmetic Functions
  ------------------------------------------------------------*/
bool MT_IsEqual(double x, double y, double epsilon)
{
    return fabs(x - y) <= epsilon*fabs(x);
}

//------------------------------------------------------------
//    Time Functions
//------------------------------------------------------------

/** Function to get the system time in seconds
    with ~millisecond accuracy.  */
double MT_getTimeSec(void)
{

#ifdef _WIN32
    // Windows has to of course use this non-posix method
    //   from Windows' time.h
    return GetTickCount()/1000.0;
#else
    // Everyone else uses a posix method
    timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec/1000000.0;
#endif

}

/** Function to get info on the time and date.  POSIX only for now. see
    http://www.cs.utah.edu/dept/old/texinfo/glibc-manual-0.02/library_19.html
    and 
    http://www.cplusplus.com/reference/clibrary/ctime/localtime.html */
struct tm* getTimeStruct()
{
    time_t rawtime;  
    time ( &rawtime );
    return localtime ( &rawtime );
}

/** Function to get the last 2 digits of the year.  Only POSIX
    is currently implemented.  
    @ See getTimeStruct
*/
int MT_getYearYY(void)
{ 
    struct tm* timeinfo = getTimeStruct();
    return (timeinfo->tm_year % 100);
}

int MT_getMonthMM(void)
{
    struct tm* timeinfo = getTimeStruct();
    return (timeinfo->tm_mon + 1);
}

int MT_getDay(void)
{
    struct tm* timeinfo = getTimeStruct();
    return (timeinfo->tm_mday);
}

int MT_getHour24(void)
{
    struct tm* timeinfo = getTimeStruct();
    return (timeinfo->tm_hour);
}

int MT_getMinute(void)
{
    struct tm* timeinfo = getTimeStruct();
    return timeinfo->tm_min;
}

int MT_getSecond(void)
{
    struct tm* timeinfo = getTimeStruct();
    return timeinfo->tm_sec;
}

void MT_getYYMMDDandHHMMSS(char* YYMMDD, char* HHMMSS)
{
    struct tm* timeinfo = getTimeStruct();
    
    sprintf(YYMMDD,"%02d%02d%02d", (timeinfo->tm_year % 100) ,(timeinfo->tm_mon + 1),timeinfo->tm_mday);
  
    sprintf(HHMMSS,"%02d%02d%02d",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  
}

