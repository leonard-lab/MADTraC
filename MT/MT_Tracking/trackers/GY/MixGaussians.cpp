/*
 *  MixGaussians.cpp
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

#include "MixGaussians.h"


int intpow(int& a, int& n);

MixGaussians::MixGaussians()
{
    m_iNumDists = 0;

    m_vMeans.resize(0);
    m_vCovariances.resize(0);
}

MixGaussians::MixGaussians(int numdists, const CvRect& boundingbox)
{
    m_iNumDists = numdists;
        
    m_vMeans.resize(numdists);
    m_vCovariances.resize(numdists);

    double x = (double) boundingbox.x;
    double y = (double) boundingbox.y;
    double width = (double) boundingbox.width;
    double height = (double) boundingbox.height;
        
    int i;
    Vector2 temp_vector;
        
    /* Set the covariance matrices of each distribution to default values so they will approximately cover the bounding box*/
    Matrix2x2 temp_matrix = IdentityMatrix<2>();
    temp_matrix.data[0] = pow(width/2.0, 2);
    temp_matrix.data[3] = pow(height/((double) numdists*2), 2);
        
    /* Evenly space each distribution along the vertical line through the centre of the bounding box*/
    for (i=0 ; i < numdists ; i++)
    {
        temp_vector.data[0] = x + (width - 1)/2.0;
        temp_vector.data[1] = y + ((height - 1)*(i+1))/((double) numdists + 1);
                
        m_vMeans[i] = temp_vector;
                
        m_vCovariances[i] = temp_matrix;
    }
}

void MixGaussians::AddDist(const Vector2& newmean, const Matrix2x2& newcovariance)
{
    m_iNumDists++;

    Vector2 tempmean = newmean;
    m_vMeans.push_back(tempmean);
        
    Matrix2x2 tempcov = newcovariance;
    m_vCovariances.push_back(tempcov);
        
}

int MixGaussians::GetNumDists()
{
    return m_iNumDists;
}

void MixGaussians::GetMeans(std::vector<Vector2>& means)
{
    if (means.size() == (unsigned int) m_iNumDists)
    {
        means = m_vMeans;
    }
    else
    {
        printf("Reserved means vector has the wrong size\n");
    }
}

void MixGaussians::GetCovariances(std::vector<Matrix2x2>& covariances)
{
    if (covariances.size() == (unsigned int) m_iNumDists)
    {
        covariances = m_vCovariances;
    }
    else
    {
        printf("Reserved covariances vector has the wrong size\n");
    }   
}


/* Main workhorse function for fitting the mixture of Gaussians to an image. The function
   takes as an input a pointer to a GYRawBlob (which has been found through a connected 
   component search). The GYRawBlob simplifies access to the needed data from the image
   since it contains a list of all the white pixels in the image. The function also takes 
   an integer vector as an input to store which distributions each pixel has been assigned 
   to.
 
   The algorithm followed is a standard expectation maximisation for a mixture of Gaussians
   with the exception that all distributions are assumed to have equal weight in the
   mixture. */
void MixGaussians::EMMG(RawBlobPtr RawData, std::vector<int>& pixelalloc, int framenumber)
{
    // Retrieving relevant data
    int numpixels = RawData->GetNumPixels();
    std::vector<CvPoint> pixellist(numpixels);
    RawData->GetPixelList(pixellist);
        
    // Check on the length of the pixelalloc vector
    if (pixelalloc.size() != (unsigned int) numpixels)
    {
        printf("Reserved pixel allocation vector has the wrong size\n");
        return;
    }
        
    // Arrays needed for the expectation maximisation algorithm and to determine when to stop iterating
    double *ps;
    double *pls;
    double *totalpls;
    double *totaldensity;
    double *oldxs;
    double *oldys;
    double *oldangles;
        
    ps = new double[m_iNumDists*numpixels];
    pls = new double[m_iNumDists*numpixels];
    totalpls = new double[m_iNumDists];
    totaldensity = new double[numpixels];
    oldxs = new double[m_iNumDists];
    oldys = new double[m_iNumDists];
    oldangles = new double[m_iNumDists];
        
    // Temporary variables needed inside the main loop
    int i, j, k;
    double mu_x, mu_y, D;
        
    Matrix2x2 V, T;
    Matrix2x1 P;
        
    double xdiff, ydiff, anglediff, maxdiff;
    double maxchange = 10.0;
        
    double maxlikelihood;
    int distributionnumber = 0;
    int numberdists;
        
    bool *distmembership;
    distmembership = new bool[m_iNumDists];

    int numiters = 0;
    // maxchange measures the maximum amount that the distributions have varied, either in mean position or
    // in angle. We iterate until all means move by less than 1 pixel in either direction and all angles
    // shift by less than 1 degree.
    while (maxchange > 1.0)
    {
        numiters++;
        // Measure the old parameters for comparison at the end of the loop
        for (i=0 ; i < m_iNumDists ; i++)
        {
            oldxs[i] = m_vMeans[i].data[0];
            oldys[i] = m_vMeans[i].data[1];
            oldangles[i] = atan2(m_vCovariances[i].data[3] - m_vCovariances[i].data[0] + sqrt(pow(m_vCovariances[i].data[0] - m_vCovariances[i].data[3],2) + 4.0*pow(m_vCovariances[i].data[1],2)), 2*m_vCovariances[i].data[1]);
        }
                
        // Calculate the probability density for each pixel in each distribution
        for (i=0 ; i < m_iNumDists ; i++)
        {
            mu_x = m_vMeans[i].data[0];
            mu_y = m_vMeans[i].data[1];
            D = m_vCovariances[i].data[0]*m_vCovariances[i].data[3] - pow(m_vCovariances[i].data[1],2);
                        
            for (j=0 ; j < numpixels ; j++)
            {
                ps[i + j*m_iNumDists] = 1.0/(2.0*M_PI*sqrt(D))*exp(-1.0/(2.0*D)*(m_vCovariances[i].data[3]*pow(pixellist[j].x - mu_x,2) + m_vCovariances[i].data[0]*pow(pixellist[j].y - mu_y,2) - 2.0*m_vCovariances[i].data[1]*(pixellist[j].x - mu_x)*(pixellist[j].y - mu_y)));
            }
        }
                
        // Determine the total probability density at each pixel
        for (j=0 ; j < numpixels ; j++)
        {
            totaldensity[j] = 0;
            for (i=0 ; i < m_iNumDists ; i++)
            {
                totaldensity[j] += ps[i + j*m_iNumDists];
            }
                        
            if (totaldensity[j] == 0)
            {
                totaldensity[j] = 1.0;
            }
        }
                
        // Calculate the likelihood that each pixel belongs to each distribution
        for (i=0 ; i < m_iNumDists ; i++)
        {
            for (j=0 ; j < numpixels ; j++)
            {
                pls[i + j*m_iNumDists] = ps[i + j*m_iNumDists]/totaldensity[j];
            }
        }
                
        // Calculate the sum of the likelihoods of all pixels for each distribution
        for (i=0 ; i < m_iNumDists ; i++)
        {
            totalpls[i] = 0;
            for (j=0 ; j < numpixels ; j++)
            {
                totalpls[i] += pls[i + j*m_iNumDists];
            }
                        
            if (totalpls[i] == 0)
            {
                totalpls[i] = 1.0;
            }
        }
                
        // Update the estimate of the distribution means by taking a weighted average
        // using likelihood values for weights
        for (i=0 ; i < m_iNumDists ; i++)
        {
            m_vMeans[i].data[0] = 0.0;
            m_vMeans[i].data[1] = 0.0;
            for (j=0; j < numpixels ; j++)
            {
                m_vMeans[i].data[0] += pixellist[j].x*pls[i + j*m_iNumDists];
                m_vMeans[i].data[1] += pixellist[j].y*pls[i + j*m_iNumDists];
            }
            m_vMeans[i].data[0] /= totalpls[i];
            m_vMeans[i].data[1] /= totalpls[i];
        }
                
        // Update the estimate of the distribution covariances by taking a weighted
        // average using likelihood values for weights
        for (i=0 ; i < m_iNumDists ; i++)
        {
            V.data[0] = V.data[1] = V.data[2] = V.data[3] = 0;
            for (j=0 ; j < numpixels ; j++)
            {
                P.data[0] = pixellist[j].x - m_vMeans[i].data[0];
                P.data[1] = pixellist[j].y - m_vMeans[i].data[1];
                                
                T.data[0] = P.data[0]*P.data[0];
                T.data[1] = T.data[2] = P.data[0]*P.data[1];
                T.data[3] = P.data[1]*P.data[1];
                V = V + pls[i + j*m_iNumDists]*T;
            }
                        
            m_vCovariances[i].data[0] = V.data[0]/totalpls[i];
            m_vCovariances[i].data[1] = V.data[1]/totalpls[i];
            m_vCovariances[i].data[2] = V.data[2]/totalpls[i];
            m_vCovariances[i].data[3] = V.data[3]/totalpls[i];
                        
            // If we've shrunk the area of the distribution to zero, re-initialise it
            if ((m_vCovariances[i].data[0]*m_vCovariances[i].data[3] - pow(m_vCovariances[i].data[1], 2)) <= 0)
            {
                m_vCovariances[i].data[0] *= 2;
                m_vCovariances[i].data[1] *= 0.9;
                m_vCovariances[i].data[2] = m_vCovariances[i].data[1];
                m_vCovariances[i].data[3] *= 2;
            }
                                                
            // Ensure the matrix is symmetric
            if (m_vCovariances[i].data[1] != m_vCovariances[i].data[2])
            {
                m_vCovariances[i].data[2] = m_vCovariances[i].data[1];
            }
                        
            // Ensure that the diagonal entries are bounded away from 0
            if (m_vCovariances[i].data[0] < 1E-2)
            {
                m_vCovariances[i].data[0] = 0.5;
            }
            if (m_vCovariances[i].data[3] < 1E-2)
            {
                m_vCovariances[i].data[3] = 0.5;
            }
        }
                
        // Determine the maximum amount by which any distribution differs from the last iteration
        maxdiff = 0.0;
        for (i=0 ; i < m_iNumDists ; i++)
        {
            xdiff = abs(m_vMeans[i].data[0] - oldxs[i]);
            if (xdiff > maxdiff)
            {
                maxdiff = xdiff;
            }
                        
            ydiff = abs(m_vMeans[i].data[1] - oldys[i]);
            if (ydiff > maxdiff)
            {
                maxdiff = ydiff;
            }
                        
            anglediff = abs(atan2(m_vCovariances[i].data[3] - m_vCovariances[i].data[0] + sqrt(pow(m_vCovariances[i].data[0] - m_vCovariances[i].data[3],2) + 4.0*pow(m_vCovariances[i].data[1],2)), 2*m_vCovariances[i].data[1]) - oldangles[i]);
            if (anglediff > maxdiff)
            {
                maxdiff = anglediff;
            }
        }
                
        maxchange = maxdiff;
    }
        
    // Now determine which distribution(s) each pixel belongs to. We do this in two ways. First, each
    // pixel will belong to the distribution for which it has the greatest likelihood of membership.
    // However, since pixels could lie within multiple distributions, we also assign pixels to any
    // distribution for which it lies less than two standard deviations from the mean.
    for (j=0 ; j < numpixels ; j++)
    {
        pixelalloc[j] = 0;
                
        maxlikelihood = -1.0;
        numberdists = 0;
        for (i=0 ; i < m_iNumDists ; i++)
        {
            // Determine which distribution has the maximum likelihood of containing the pixel
            if (pls[i + j*m_iNumDists] > maxlikelihood)
            {
                maxlikelihood = pls[i + j*m_iNumDists];
                distributionnumber = i;
            }
                        
            // If the pixel lies within two standard deviations of the mean of any distribution, we include it
            if (ps[i + j*m_iNumDists] > 0.023)
            {
                distmembership[i] = 1;
                numberdists++;
            }
            else
            {
                distmembership[i] = 0;
            }
        }
                
        // If the pixel doesn't already belong to the distribution with the maximum likelihood, we include it
        if (distmembership[distributionnumber] == 0)
        {
            distmembership[distributionnumber] = 1;
            numberdists++;
        }
                
        // Generate an integer to represent the set of distributions that the pixel belongs to
        i = 0;
        for (k=0 ; k < numberdists ; k++)
        {
            for (; distmembership[i] == 0 ; i++)
            {}
            pixelalloc[j] += i*intpow(m_iNumDists,k);
            distmembership[i] = 0;
        }
    }
        
    // Clear all temporary arrays
    delete[] ps;
    delete[] pls;
    delete[] totalpls;
    delete[] totaldensity;
    delete[] oldxs;
    delete[] oldys;
    delete[] oldangles;
    delete[] distmembership;
}

int intpow(int& a, int& n)
{
    if (n == 0)
    {
        return 1;
    }
    else if (n > 0)
    {
        int val = 1;
        for (int i = 0; i < n ; i++)
        {
            val *= a;
        }
        return val;
    }
    else if ((a == 1) || ((a == 2) && (n == -1)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
