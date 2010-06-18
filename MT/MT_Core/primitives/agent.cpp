//------------------------------------------------------------
//  agent.cpp - glswarm/robofish - D.Swain 8/17/07
//  
//  A class from which all the other particle models can be
//   derived.  Includes basic properties like position and
//   color, simulation methods like integration and kinematics,
//   and methods to draw the representation.
//
//------------------------------------------------------------


//-----------------Headers------------------------------------

// Standard headers
#include <stdio.h>

// #include "types.h"

// Carefully include OpenGL
#if defined(__APPLE__) || defined(MACOSX) && !(defined NO_GL)
#include <OpenGL/gl.h>
// #include <GLUT/glut.h>
#elif !(defined NO_GL)
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
// #include <GL/glut.h>
#endif

// Headers related to this project
#include "MT/MT_Core/support/mathsupport.h"
#include "agent.h"

#include <vector>
using namespace std;


//-----------------Constructors and Destructors---------------

/** Default constructor, zero initial conditions and random
    color. */
// Default Constructor - Zero ICs
MT_agent::MT_agent()
    : dp(), position(), myColor()
{
    common_init();
}

/** Randomized constructor - random ICs in range -scale to
    +scale, random color. */
// Random initialization (range -scale to +scale)
MT_agent::MT_agent(double scale) 
    : dp(), position(scale), myColor()
{
    common_init();
}

/** Constructor to set x, y, and z, random color.  */
// Initialize to specific values
MT_agent::MT_agent(double inx, double iny, double inz)
    : dp(), position(inx, iny, inz), myColor()
{
    common_init();
}

/** Destructor - deletes x, y, and z buffers. */
// Destructor - delete buffers
MT_agent::~MT_agent()
{
    delete xbuffer;
    delete ybuffer;
    delete zbuffer;
}


//-----------------Member Functions---------------------------

/** Function to execute common initializations for the
    various constructors. */
// Common initializations
void MT_agent::common_init()
{
    glyph = 1;
    dtheta = 0.0;
    theta = 0;
    dT = 1;
  
    size = MT_DEFAULT_SIZE;
    aspect = 1;
    units_per_pixel = 1;
  
    init_buffers(MT_DEFAULT_TAIL_LENGTH);
  
    SensingRadius = 0;
    BlindAngle = 0;
    NNeighbors = 0;  
}

/** Function to initialize the buffers to a given length. */
// Initialize buffers
void MT_agent::init_buffers(const int length)
{
    xbuffer = new MT_ringbuffer<double>(length);
    ybuffer = new MT_ringbuffer<double>(length);
    zbuffer = new MT_ringbuffer<double>(length);
}

/** Function to set the dx, dy, and dz values. */
// sets dx, dy, dz
void MT_agent::kinematics(double indx, double indy, double indz)
{
    dp.setx(indx);
    dp.sety(indy);
    dp.setz(indz);
}

/** Function to integrate the differential positions.  Simple
    forward Euler is used. */
// Integrate by adding in the differentials
void MT_agent::integrate()
{
    position += dp*dT;
    theta += dT*dtheta;
    dp.zero();
    dtheta = 0;
    xbuffer->push(x());
    ybuffer->push(y());
    zbuffer->push(z());
    NNeighbors = 0;
}

/** Function to display values for debugging. */
// debuging - spit out values
void MT_agent::spitup(const char* name) const
{
    printf("%s:  x = %f, y = %f, z = %f\n",name, x(), y(), z());
}


/** Prototype function to determine if this agent has
    neighbor pointed to by them.  Returns 1 if the
    sensing radius and blind angle = 0, and a 1 or 0 
    depending on the state and parameters.  Returns
    0 if the specified agent is us (no self-loops).
    Makes some attempt to be efficient - this algorithm
    can be a big time sink. */
int MT_agent::HasNeighbor(const MT_agent* them)
{
    // No self-loops, return 0
    if(this == them)
        return 0;

    // Return 1 right away if there is no sensing restriction
    if(!SensingRadius && !BlindAngle)
        return 1;
    else if(SensingRadius && !BlindAngle)
        return (DistanceTo(them) <= SensingRadius);
    else if(!SensingRadius && BlindAngle)
        return (fabs(RelAngle(them)) <= (MT_PI - 0.5*BlindAngle));
    else{
        if(DistanceTo(them) > SensingRadius)
            return 0;
        else
            return (fabs(RelAngle(them)) <= (MT_PI - 0.5*BlindAngle));
    }
}

/** Function to find the relative bearing to a neighbor. */
double MT_agent::RelAngle(const MT_agent* them)
{
    return MT_anglemod(atan2((this->y() - them->y()),
                          (this->x() - them->x())) - theta);
}

/** Function to find the distance from this particle to
    another particle. */
double MT_agent::DistanceTo(const MT_agent* them)
{
    return MT_R3distance(this->position,them->position);
}

double MT_agent::DistanceTo(const MT_LiteAgent* them)
{
    return MT_R3distance(this->position, them->position);
}

double MT_agent::DistanceTo(const MT_R3& there)
{
    return MT_R3distance(this->position,there);
}

/** Function to reset the agent within the specified bounds. */
void MT_agent::Reset(double xmin, double xmax, double ymin, double ymax)
{
    position.x = MT_frandr(xmin,xmax);
    position.y = MT_frandr(ymin,ymax);
    position.z = MT_frandr(0,0);
    xbuffer->reset();
    ybuffer->reset();
    zbuffer->reset();
}

std::vector<double> MT_agent::getXHistory(int length)
{
    if(length < 0 || length > (int) xbuffer->fullto)
    {
        length = xbuffer->fullto;
    }

    std::vector<double> r;
    
    for(int i = 0; i < length; i++)
    {
        r.push_back(xbuffer->atrel(i));
    }
    return r;
}

std::vector<double> MT_agent::getYHistory(int length)
{
    if(length < 0 || length > (int) ybuffer->fullto)
    {
        length = ybuffer->fullto;
    }

    std::vector<double> r;
    
    for(int i = 0; i < length; i++)
    {
        r.push_back(ybuffer->atrel(i));
    }
    return r;
}

std::vector<double> MT_agent::getZHistory(int length)
{
    if(length < 0 || length > (int) zbuffer->fullto)
    {
        length = zbuffer->fullto;
    }

    std::vector<double> r;
    
    for(int i = 0; i < length; i++)
    {
        r.push_back(zbuffer->atrel(i));
    }
    return r;
}
