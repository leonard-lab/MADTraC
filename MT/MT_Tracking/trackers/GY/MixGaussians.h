#ifndef MIXGAUSSIANS_H
#define MIXGAUSSIANS_H

/*
 *  MixGaussians.h
 *  MADTraC
 *
 *  A class to describe a two-dimensional mixture of Gaussians
 *  that includes a method to fit the mixture to a black and
 *  white image using expectation maximisation.
 *
 *  Created by George Young on 6/10/09.
 *  Copyright 2009 Princeton University. All rights reserved.
 *
 */

#if defined(__APPLE__) || defined(MACOSX)
#include <OpenCV/OpenCV.h>
#else
#include <cv.h>
#include <highgui.h>
#endif

#include <cmath>

#include "GYBlobs.h"

#include "MT/MT_Core/primitives/Matrix.h"

#include <vector>
using namespace std;

class MixGaussians
{
protected:
    int m_iNumDists;
    std::vector<MT_Vector2> m_vMeans;
    std::vector<MT_Matrix2x2> m_vCovariances;
                
public:
    // Constructors
    MixGaussians();                                                                                                                     // Default constructor - initialises with no gaussians
    MixGaussians(int numdists, const CvRect& boundingbox);                                      // Constructor that evenly places distributions within a bounding box
                
    void AddDist(const MT_Vector2& newmean, const MT_Matrix2x2& newcovariance);       // Method to add a given distribution to a mixture model
                
    // Expectation maximisation algorithm to fit a mixture model to a given set of image data
    void EMMG(RawBlobPtr RawData, std::vector<int>& pixelalloc, int framenumber);

    // Methods to retrieve parameters
    int GetNumDists();
    void GetMeans(std::vector<MT_Vector2>& means);
    void GetCovariances(std::vector<MT_Matrix2x2>& covariances);
                
};
#endif                  // MIXGAUSSIANS_H
