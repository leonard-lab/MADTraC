//-----------------Headers------------------------------------

// Standard headers
#include <stdio.h>
#include <math.h>

// Header for this class
#include "Color.h"

// Support for random functions
#include "mathsupport.h"


//-----------------Constructors-------------------------------

// Default constructor - random
MT_Color::MT_Color()
{
    data[0] = MT_frand(1.0f);
    data[1] = MT_frand(1.0f);
    data[2] = MT_frand(1.0f);
    data[3] = 1.0f;
}

MT_Color::MT_Color(float R, float G, float B)
{
    data[0] = MT_CLAMP(R,0.0f,1.0f);
    data[1] = MT_CLAMP(G,0.0f,1.0f);
    data[2] = MT_CLAMP(B,0.0f,1.0f);
    data[3] = 1.0f;
}

MT_Color::MT_Color(float R, float G, float B, float A)
{
    data[0] = MT_CLAMP(R,0.0f,1.0f);
    data[1] = MT_CLAMP(G,0.0f,1.0f);
    data[2] = MT_CLAMP(B,0.0f,1.0f);
    data[3] = MT_CLAMP(A,0.0f,1.0f);
}


//-----------------Operators----------------------------------

// Addition operator
MT_Color MT_Color::operator + (const MT_Color &v)
{
    float Ro = data[0] + v.R();
    float Go = data[1] + v.G();
    float Bo = data[2] + v.B();
    float Ao = data[3] + v.A();
    return MT_Color(Ro,Go,Bo,Ao);
}

// Unary addition
MT_Color &MT_Color::operator += (const MT_Color &v)
{
    *this = *this + v;
    return *this;
}

// Subtraction operator
MT_Color MT_Color::operator - (const MT_Color &v)
{
    float Ro = data[0] - v.R();
    float Go = data[1] - v.G();
    float Bo = data[2] - v.B();
    float Ao = data[3] - v.A();
    return MT_Color(Ro,Go,Bo,Ao);
}

// Unary subtraction
MT_Color &MT_Color::operator -= (const MT_Color &v)
{
    *this = *this - v;
    return *this;
}

// Multiplication operator
MT_Color MT_Color::operator * (double scalar)
{
    return MT_Color(scalar*R(),scalar*G(),scalar*B());
}

// Unary scalar multiplication
MT_Color &MT_Color::operator *= (double scalar)
{
    *this = (*this)*(scalar);
    return *this;
}

// Division operator
MT_Color MT_Color::operator / (double scalar)
{
    if(scalar != 0)
        return MT_Color(R()/scalar,G()/scalar,B()/scalar);
    else{
        fprintf(stderr,"Error:  Tried to divide vector .G().B()ero.\n");
        fprintf(stderr,"  Will divide .G() 0.000000001 instead.\n");
        return MT_Color(R()/0.000000001,G()/0.000000001,B()/0.000000001);
    }
}

// Unary scalar division
MT_Color &MT_Color::operator /= (double scalar)
{
    *this = (*this)/(scalar);
    return *this;
}


//-----------------Member Functions---------------------------

// Zero the values
void MT_Color::zero()
{
    data[0] = data[1] = data[2] = 0;
    data[3] = 1.0;
}

// 2-norm squared
double MT_Color::sqrnorm() const
{
    return R()*R() + G()*G() + B()*B();
}

// 2-norm
double MT_Color::norm() const
{
    return sqrt(R()*R() + G()*G() + B()*B());
}


const char* MT_Color::GetHexString()
{
    unsigned int r = 255*R();
    unsigned int g = 255*G();
    unsigned int b = 255*B();
    sprintf(hex_string, "%02X%02X%02X", r, g, b);
    hex_string[6] = '\0';
    return hex_string;
}

void MT_Color::SetHexValue(const char* hexstring)
{
    unsigned int r, g, b;
    r = g = b = 0;
    sscanf(hexstring, "%02X%02X%02X", &r, &g, &b);
    data[0] = ((double) r)/(255.0);
    data[1] = ((double) g)/(255.0);
    data[2] = ((double) b)/(255.0);
}


//-----------------Support Functions--------------------------

// Dot Product
double MT_dotproduct(MT_Color v1, MT_Color v2)
{
    return v1.R()*v2.R() + v1.G()*v2.G() + v1.B()*v2.B();
}

// Distance
double MT_distance(MT_Color v1, MT_Color v2)
{
    return sqrt((v1.R() - v2.R())*(v1.R() - v2.R())
                + (v1.G() - v2.G())*(v1.G() - v2.G())
                + (v1.B() - v2.B())*(v1.B() - v2.B()));
}

// Distance squared
double MT_distsquared(MT_Color v1, MT_Color v2)
{
    return (v1.R() - v2.R())*(v1.R() - v2.R())
        + (v1.G() - v2.G())*(v1.G() - v2.G())
        + (v1.B() - v2.B())*(v1.B() - v2.B());
}      
