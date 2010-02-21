//------------------------------------------------------------
//  R3.cpp - glswarm/robofish - D.Swain 8/12/07
//  
//  Implementation of a 3 dimensional cartesian vector space.
//   Supports standard operations.
//
//------------------------------------------------------------


//-----------------Headers------------------------------------

// Standard headers
#include <stdio.h>
#include <math.h>

// Header for this class
#include "R3.h"

// Support for random functions
#include "mathsupport.h"


//-----------------Constructors-------------------------------

// Default constructor - zero
MT_R3::MT_R3()
{
    zero();
}

// Single-input constructor - randomize
MT_R3::MT_R3(double scale)
{
    x = MT_FRND(scale);
    y = MT_FRND(scale);
    z = 0.0;
}

MT_R3::MT_R3(double inx, double iny, double inz)
{
    x = inx;
    y = iny;
    z = inz;
}


//-----------------Operators----------------------------------

// Addition operator
MT_R3 MT_R3::operator + (const MT_R3 &v)
{
    float xo = x + v.x;
    float yo = y + v.y;
    float zo = z + v.z;
    return MT_R3(xo,yo,zo);
}

// Unary addition
MT_R3 &MT_R3::operator += (const MT_R3 &v)
{
    *this = *this + v;
    return *this;
}

// Subtraction operator
MT_R3 MT_R3::operator - (const MT_R3 &v)
{
    float xo = x - v.x;
    float yo = y - v.y;
    float zo = z - v.z;
    return MT_R3(xo,yo,zo);
}

// Unary subtraction
MT_R3 &MT_R3::operator -= (const MT_R3 &v)
{
    *this = *this - v;
    return *this;
}

// Unary scalar multiplication
MT_R3 &MT_R3::operator *= (double scalar)
{
    *this = (*this)*(scalar);
    return *this;
}

// Division operator
MT_R3 MT_R3::operator / (double scalar)
{
    if(scalar != 0)
        return MT_R3(x/scalar,y/scalar,z/scalar);
    else{
        fprintf(stderr, "Error:  Tried to divide vector by zero.\n");
        fprintf(stderr, "  Will divide by 0.000000001 instead.\n");
        return MT_R3(x/0.000000001,y/0.000000001,z/0.000000001);
    }
}

// Unary scalar division
MT_R3 &MT_R3::operator /= (double scalar)
{
    *this = (*this)/(scalar);
    return *this;
}


//-----------------Member Functions---------------------------

// Zero the values
void MT_R3::zero()
{
    x = y = z = 0;
}

// 2-norm squared
double MT_R3::sqrnorm()
{
    return x*x + y*y + z*z;
}

// 2-norm
double MT_R3::norm()
{
    return sqrt(x*x + y*y + z*z);
}


//-----------------Support Functions--------------------------

// Dot Product
double MT_dotproduct(const MT_R3& v1, const MT_R3& v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

// Distance
double MT_R3distance(const MT_R3& v1, const MT_R3& v2)
{
    return sqrt((v1.x - v2.x)*(v1.x - v2.x)
                + (v1.y - v2.y)*(v1.y - v2.y)
                + (v1.z - v2.z)*(v1.z - v2.z));
}

// Distance squared
double MT_distsquared(const MT_R3& v1, const MT_R3& v2)
{
    return (v1.x - v2.x)*(v1.x - v2.x)
        + (v1.y - v2.y)*(v1.y - v2.y)
        + (v1.z - v2.z)*(v1.z - v2.z);
}      

MT_R3 operator * (const MT_R3& lhs, double rhs)
{
    return MT_R3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

MT_R3 operator * (double lhs, const MT_R3& rhs)
{
    return MT_R3(lhs*rhs.x, lhs*rhs.y, lhs*rhs.z);
}
