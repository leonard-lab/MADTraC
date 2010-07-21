/*
 *  BoundingBox.cpp
 *  glswarm
 *
 *  Created by Daniel Swain on 9/10/08.
 *
 */

#include "BoundingBox.h"
#include "MT/MT_Core/support/mathsupport.h"

MT_BoundingBox::MT_BoundingBox()
{
    xmin = xmax = ymin = ymax = zmin = zmax = 0; 
  
    initialized_xmin = initialized_xmax   
        = initialized_ymin 
        = initialized_ymax
        = initialized_zmin 
        = initialized_zmax 
        = false;
  
    lock_xmin = lock_xmax
        = lock_ymin
        = lock_ymax
        = lock_zmin
        = lock_zmax
        = false;
}

MT_BoundingBox::MT_BoundingBox(double setxmin, double setxmax, double setymin, double setymax, 
                         double setzmin, double setzmax)
{
    xmin = setxmin; xmax = setxmax; ymin = setymin; ymax = setymax;
    zmin = setzmin; zmax = setzmax;
    initialized_xmin = initialized_ymin = initialized_zmin = true;
    initialized_xmax = initialized_ymax = initialized_zmax = true;
    lock_xmin = lock_xmax = lock_ymin = lock_ymax = lock_zmin = lock_zmax = false;

}

void MT_BoundingBox::ShowX(double x)
{ 
    if( !lock_xmin )
    {
        if( initialized_xmin )
        {
            xmin = MT_MIN(x,xmin);
        } else {
            xmin = x;
        }
        initialized_xmin = true;
    }
  
    if( !lock_xmax )
    {
        if( initialized_xmax )
        {
            xmax = MT_MAX(x,xmax);
        } else {
            xmax = x;
        }
        initialized_xmax = true;
    }      
  
}

void MT_BoundingBox::ShowY(double y)
{ 
    if( !lock_ymin )
    {
        if( initialized_ymin )
        {
            ymin = MT_MIN(y,ymin);
        } else {
            ymin = y;
        }
        initialized_ymin = true;
    }
  
    if( !lock_ymax )
    {
        if( initialized_ymax )
        {
            ymax = MT_MAX(y,ymax);
        } else {
            ymax = y;
        }
        initialized_ymax = true;
    }      
  
}

void MT_BoundingBox::ShowZ(double z)
{ 
    if( !lock_zmin )
    {
        if( initialized_zmin )
        {
            zmin = MT_MIN(z,zmin);
        } else {
            zmin = z;
        }
        initialized_zmin = true;
    }
  
    if( !lock_zmax )
    {
        if( initialized_zmax )
        {
            zmax = MT_MAX(z,zmax);
        } else {
            zmax = z;
        }
        initialized_zmax = true;
    }      
  
}

void MT_BoundingBox::SetXMinAndLock(double x)
{
  
    xmin = x;
    lock_xmin = true;
  
}

void MT_BoundingBox::SetXMaxAndLock(double x)
{
  
    xmax = x;
    lock_xmax = true;
  
}

void MT_BoundingBox::SetYMinAndLock(double y)
{
  
    ymin = y;
    lock_ymin = true;
  
}

void MT_BoundingBox::SetYMaxAndLock(double y)
{
  
    ymax = y;
    lock_ymax = true;
  
}

void MT_BoundingBox::SetZMinAndLock(double z)
{
  
    zmin = z;
    lock_zmin = true;
  
}

void MT_BoundingBox::SetZMaxAndLock(double z)
{
  
    zmax = z;
    lock_zmax = true;
  
}
