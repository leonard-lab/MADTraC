#ifndef BLOBEXTRAS_H
#define BLOBEXTRAS_H

/*
 *  BlobExtras.h
 *
 *  Created by Daniel Swain on 8/12/09.
 *
 *  Contains a number of extra blob operators that I've defined.
 *
 */

// basic OpenCV and blob library
#if defined(__APPLE__) || defined(MACOSX)
#include <OpenCV/OpenCV.h>
#else
#include <cv.h>
#endif
#include "MT/MT_Tracking/3rdparty/cvBlobsLib/BlobResult.h"

// for MT_DEG2RAD and MT_RAD2DEG
#include "MT/MT_Core/support/mathsupport.h"

/* Class to calculate the ratio of the second order moments (related to
   eccentricity).   Returns either u20/u02 or u02/u20, which ever is > 1,
   where u20 and u02 are the second-order centralized moments.
 
   Based on the COperadorBlob class by Inspecta S.L.  Allows us to use all of the built-in
   classes in CBlobsLib.
 
   DTS 8/12/09 - Fixed from CBlobGetEccentricity */
class MT_CBlobGetMomentRatio : public COperadorBlob
{
public:
    double operator()(CBlob &blob) const
    {
        CvBox2D ellipse = blob.GetEllipse();
      
        // noncentralized moments
        double xcm = ellipse.center.x;
        double ycm = ellipse.center.y;
        double m00 = blob.Moment(0,0);
        double m20 = blob.Moment(2,0);
        double m02 = blob.Moment(0,2);
      
        // calculate centralized moments from noncentralized moments
        double u20 = m20 - xcm*xcm*m00;
        double u02 = m02 - ycm*ycm*m00;
      
        double r = u20/u02;
      
        if( r < 1.0 )
            return 1.0/r;
      
        return r;
    }
    const char *GetNom() const
    {
        return "MT_CBlobGetMomentRatio";
    }
};


/* Class to calculate the "true" orientation of a blob based on its skewness.
   Algorithm inspired by work by Adrian DeFroment.  This is designed for a fish-like
   object where the head should be fatter than the tail, therefore the distribution
   of pixels along the body axis should be skewed towards the tail. 
 
   Based on the COperadorBlob class by Inspecta S.L.  Allows us to use all of the built-in
   classes in CBlobsLib.
 
   DTS 7/2/09 - Created */
class MT_CBlobGetHeadOrientation : public COperadorBlob
{
public:
    ~MT_CBlobGetHeadOrientation(){};
    
    double operator()( CBlob &blob)
    {
        // we need the direction vector for the ellipse-fit orientation
        CvBox2D ellipse = blob.GetEllipse();
      
        // the center of mass
        double xcm = ellipse.center.x;
        double ycm = ellipse.center.y;
      
        double m00 = blob.Moment(0,0);
      
        double m10 = blob.Moment(1,0);
        double m01 = blob.Moment(0,1);
        double m11 = blob.Moment(1,1);
      
        double m20 = blob.Moment(2,0);
        double m02 = blob.Moment(0,2);
        double m12 = blob.Moment(1,2);
        double m21 = blob.Moment(2,1);
      
        double m30 = blob.Moment(3,0);
        double m03 = blob.Moment(0,3);
      
        double u20p = (m20/m00) - xcm*xcm;
        double u02p = (m02/m00) - ycm*ycm;
        double u11p = (m11/m00) - xcm*ycm;
      
        double myangle =  180.0 - MT_RAD2DEG*0.5*atan2( (2.0)*u11p, u20p-u02p );
      
        double u30 = m30 - 3.0*xcm*m20 + 2.0*xcm*xcm*m10;
        double u21 = m21 - 2.0*xcm*m11 - ycm*m20 + 2.0*xcm*xcm*m01;
        double u12 = m12 - 2.0*ycm*m11 - xcm*m02 + 2.0*ycm*ycm*m10;
        double u03 = m03 - 3*ycm*m02 + 2*ycm*ycm*m01;
        double qx = cos(MT_DEG2RAD*myangle);
        double qy = -sin(MT_DEG2RAD*myangle);
      
        double s = u30*qx*qx*qx + 3.0*u21*qx*qx*qy + 3.0*u12*qx*qy*qy + u03*qy*qy*qy;
      
        if(s > 0)
        {
            myangle += 180.0;
        }
      
        return myangle;
    }
    const char *GetNom() 
    {
        return "MT_CBlobGetHeadOrientation";
    }
};

/* Class to get the X center of mass of a blob.
   (NOTE that CBlobGetXCenter returns the center of a blob's bounding box!)
 
   Based on the COperadorBlob class by Inspecta S.L.  Allows us to use all of the built-in
   classes in CBlobsLib.

*/
class MT_CBlobGetXCenterOfMass : public COperadorBlob
{
public:
    ~MT_CBlobGetXCenterOfMass(){};
    
    double operator()( CBlob &blob)
    {
      
        // the center of mass
        CvBox2D ellipse = blob.GetEllipse();
        return ellipse.center.x;
      
    }
    const char *GetNom() 
    {
        return "MT_CBlobGetXCenterOfMass";
    }
};

/* Class to get the Y center of mass of a blob.
   (NOTE that CBlobGetYCenter returns the center of a blob's bounding box!)
 
   Based on the COperadorBlob class by Inspecta S.L.  Allows us to use all of the built-in
   classes in CBlobsLib.
 
*/
class MT_CBlobGetYCenterOfMass : public COperadorBlob
{
public:
    ~MT_CBlobGetYCenterOfMass(){};
    
    double operator()( CBlob &blob)
    {
      
        // the center of mass      
        CvBox2D ellipse = blob.GetEllipse();
        return ellipse.center.y;
      
    }
    const char *GetNom() 
    {
        return "MT_CBlobGetYCenterOfMass";
    }
};

#endif // BLOBEXTRAS_H
