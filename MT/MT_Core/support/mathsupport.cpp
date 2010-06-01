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

bool MT_isdigit(char c) {
    const char digits[] = "0123456789";
    for (int i = 0; i < strlen(digits); i++) {
        if (digits[i] == c) {
            return true;
        }
    }
    return false;
}

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

//------------------------------------------------------------
//    String analaysis functions
//------------------------------------------------------------

/* shorthand - makes the function return 0 if the character is past the end
 * of the string (assuming l = strlen(string)).  Mostly for readability. */
#define BAIL_IF_LAST_CHAR(ch) if(ch >= l) { return MT_FORMAT_STRING_INVALID; }

MT_FORMAT_STRING_TYPE MT_hasFormatString(const char* s, int* p_start, int* p_end)
{
    /* look for a %, p1 will be a pointer to the first appearance */
    const char* p1 = strchr(s, '%');
    int p = p1 - s; /* convert pointer to position */
    int l = strlen(s);

    /* a format specifier must begin with a % */
    if(!p1)
    {
        return MT_FORMAT_STRING_INVALID;
    }
    /* it also can't be the last position */
    if(p == l)
    {
        return MT_FORMAT_STRING_INVALID;
    }

    /* c will be the relative index from p (where the % is) */
    int c = 1;
    char n = s[p + c];

    /* the % can be followed by +, -, or a space to indicate
     * special formatting;  if this is true then we want to
     * move on to the next character */
    if(n == '-' || n == '+' || n == ' ')
    {
        c++;
        BAIL_IF_LAST_CHAR(p + c);
        n = s[p+c];
    }

    /* a digit indicates a width specifier, a . indicates moving on
     * to a precision specifier, and a * indicates a special width specifier */
    if( isdigit(n) || n == '.' || n == '*')
    {
        bool have_decimal = (n == '.');
        bool search = true;

        /* if this char is a *, the next one MUST either be
         * a . or moving on to the lenght / format specifier */
        if(n == '*')
        {
            BAIL_IF_LAST_CHAR(p+c);
            if(s[p + c] != '.')
            {
                search = false;
                c++;
            }
        }

        /* loop over characters until we find a non-digit, except
         * that we can allow a single decimal */
        while(1)
        {
            BAIL_IF_LAST_CHAR(p+c);
            n = s[p+c];

            /* numbers are OK, just keep going */
            if(!isdigit(n))
            {
                if(n == '.')
                {
                    /* we can only have one decimal */
                    if(have_decimal)
                    {
                        return MT_FORMAT_STRING_INVALID;
                    }
                    have_decimal = true;
                }
                else
                {   /* loop breaks here */
                    break;
                }
            }
            c++; /* next char */
        }
    }

    BAIL_IF_LAST_CHAR(p+c);
    n = s[p+c];

    if(n == '*')
    {
        /* if this is a * then the previous char can NOT have been
        * a number */
        if(isdigit(s[p + c - 1]))
        {
            return MT_FORMAT_STRING_INVALID;
        }
        c++;
    }

    BAIL_IF_LAST_CHAR(p+c);
    n = s[p+c];
    
    /* check if this is a length formatter */
    if(n == 'l' || n == 'L' || n == 'h')
    {
        c++; /* if it is, move to the next char */
    }

    BAIL_IF_LAST_CHAR(p+c);
    n = s[p+c];

    MT_FORMAT_STRING_TYPE ret_fmt;

    /* now finally check to see if this is a valid formatter */
    switch(s[p + c])
    {
        case 'c':
            ret_fmt = MT_FORMAT_STRING_CHAR;
            break;
        case 's':
            ret_fmt = MT_FORMAT_STRING_STRING;
            break;
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
        case 'p':
            ret_fmt = MT_FORMAT_STRING_INT;
            break;
        case 'f':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
            ret_fmt = MT_FORMAT_STRING_FLOAT;
            break;
        default:
            ret_fmt = MT_FORMAT_STRING_INVALID;
    }

    /* store the start and end positions if necessary */
    if(ret_fmt != MT_FORMAT_STRING_INVALID)
    {
        if(p_start)
        {
            *p_start = p;
        }
        if(p_end)
        {
            *p_end = p + c;
        }
    }

}
