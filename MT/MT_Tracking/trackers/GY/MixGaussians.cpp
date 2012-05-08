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

#define MAX_ITERS 1000

#define DEBUG_OUT(...) if(m_pDebugFile){printf(__VA_ARGS__);};

int intpow(int& a, int& n);
void calculate_axes_and_angle(double xx, double xy, double yy, double* M, double* m, double* o);

MixGaussians::MixGaussians()
{
    m_iNumDists = 0;

    m_vMeans.resize(0);
    m_vCovariances.resize(0);

    m_dMaxEccentricity = 0;
    m_dMaxDistance = -1;              
    m_dMaxSizePercentChange = -1;

    m_pDebugFile = NULL;
}

MixGaussians::MixGaussians(int numdists, const CvRect& boundingbox)
{
    m_iNumDists = numdists;
        
    m_vMeans.resize(numdists);
    m_vCovariances.resize(numdists);

    m_dMaxEccentricity = 0;
    m_dMaxDistance = -1;              
    m_dMaxSizePercentChange = -1;    

    CoverBox(numdists, boundingbox);

    m_pDebugFile = NULL;
}

void MixGaussians::CoverBox(int numdists, const CvRect& boundingbox)
{
    double x = (double) boundingbox.x;
    double y = (double) boundingbox.y;
    double width = (double) boundingbox.width;
    double height = (double) boundingbox.height;
        
    int i;
    MT_Vector2 temp_vector;
        
    /* Set the covariance matrices of each distribution to default values so they will approximately cover the bounding box*/
    MT_Matrix2x2 temp_matrix = IdentityMT_Matrix<2>();
    temp_matrix.data[0] = pow(width/2.0, 2);
    temp_matrix.data[3] = pow(height/((double) numdists*2), 2);
        
    /* Evenly space each distribution along the vertical line through the centre of the bounding box*/
    for (i=0 ; i < numdists ; i++)
    {
        temp_vector.data[0] = x + (width - 1)/2.0;
        temp_vector.data[1] = y + ((height - 1)*(i+1))/((double) numdists + 1);

        AddDist(temp_vector, temp_matrix);
    }
}

void MixGaussians::ClearDists()
{
    m_iNumDists = 0;
    m_vMeans.resize(0);
    m_vCovariances.resize(0);
}

void MixGaussians::AddDist(const MT_Vector2& newmean, const MT_Matrix2x2& newcovariance)
{
    m_iNumDists++;

    MT_Vector2 tempmean = newmean;
    m_vMeans.push_back(tempmean);
        
    MT_Matrix2x2 tempcov = newcovariance;
    m_vCovariances.push_back(tempcov);
        
}

int MixGaussians::GetNumDists()
{
    return m_iNumDists;
}

void MixGaussians::GetMeans(std::vector<MT_Vector2>& means)
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

void MixGaussians::GetCovariances(std::vector<MT_Matrix2x2>& covariances)
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
void MixGaussians::EMMG(RawBlobPtr RawData, std::vector<int>& pixelalloc, int max_iters)
{

    /* The distribution assignment algorithm uses a bit pattern for
     * pixel assignment.  Therefore we need one bit for each
     * distribution and will correspondingly have problems if the
     * number of distributions is too large */
    if(m_iNumDists > 31)
    {
        fprintf(stderr, "MixGaussians::EMMG Error:  To many distributions!\n"
                "\tSee MixGaussians::EMMG code.\n");
        return;
    }
    
    // Retrieving relevant data
    int numpixels = RawData->GetNumPixels();
    std::vector<CvPoint> pixellist(numpixels);
    RawData->GetPixelList(pixellist);

    if(max_iters <= 0)
    {
        max_iters = MAX_ITERS;
    }
        
    // Check on the length of the pixelalloc vector
    if (pixelalloc.size() != (unsigned int) numpixels)
    {
        printf("Reserved pixel allocation vector has the wrong size\n");
        return;
    }
        
    // Arrays needed for the expectation maximisation algorithm and to
    // determine when to stop iterating
    double *pus;
    double *ps;
    double *pls;
    double *totalpls;
    double *totaldensity;
    double *oldxs;
    double *oldys;
    double *oldangles;
    double *oldms;
    double *oldMs;

    double *orig_xs = new double[m_iNumDists];
    double *orig_ys = new double[m_iNumDists];
    double *orig_Ms = new double[m_iNumDists];
    double *orig_ms = new double[m_iNumDists];
    
    double m, M, Mdiff, mdiff, o;
    for(int i = 0; i < m_iNumDists; i++)
    {
        orig_xs[i] = m_vMeans[i].data[0];
        orig_ys[i] = m_vMeans[i].data[1];
        calculate_axes_and_angle(m_vCovariances[i].data[0],
                                 m_vCovariances[i].data[1],
                                 m_vCovariances[i].data[3],
                                 &M, &m, &o);
        orig_Ms[i] = M;
        orig_ms[i] = m;
    }
    
    pus = new double[m_iNumDists*numpixels];        
    ps = new double[m_iNumDists*numpixels];
    pls = new double[m_iNumDists*numpixels];
    totalpls = new double[m_iNumDists];
    totaldensity = new double[numpixels];
    oldxs = new double[m_iNumDists];
    oldys = new double[m_iNumDists];
    oldangles = new double[m_iNumDists];
    oldMs = new double[m_iNumDists];
    oldms = new double[m_iNumDists];    
        
    // Temporary variables needed inside the main loop
    int i, j, k;
    double mu_x, mu_y, D, s_xx, s_yy, s_xy, dx, dy;
        
    MT_Matrix2x2 V, T;
    MT_Matrix2x1 P;
        
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
    while (maxchange > 1.0 && numiters < max_iters)
    {
        numiters++;

        // Measure the old parameters for comparison at the end of the loop
        for (i=0 ; i < m_iNumDists ; i++)
        {
            calculate_axes_and_angle(m_vCovariances[i].data[0],
                                     m_vCovariances[i].data[1],
                                     m_vCovariances[i].data[3],
                                     &M, &m, &o);
            oldMs[i] = M;
            oldms[i] = m;
            oldangles[i] = o;
            
            oldxs[i] = m_vMeans[i].data[0];
            oldys[i] = m_vMeans[i].data[1];
            //oldangles[i] = atan2(m_vCovariances[i].data[3] - m_vCovariances[i].data[0] + sqrt(pow(m_vCovariances[i].data[0] - m_vCovariances[i].data[3],2) + 4.0*pow(m_vCovariances[i].data[1],2)), 2*m_vCovariances[i].data[1]);
        }
                
        // Calculate the probability density for each pixel in each distribution
        for (i=0 ; i < m_iNumDists ; i++)
        {
            mu_x = m_vMeans[i].data[0];
            mu_y = m_vMeans[i].data[1];
            s_xx = m_vCovariances[i].data[0];
            s_xy = m_vCovariances[i].data[1];
            s_yy = m_vCovariances[i].data[3];
            D = s_xx*s_yy - s_xy*s_xy;
                        
            for (j=0 ; j < numpixels ; j++)
            {
                dx = (pixellist[j].x - mu_x);
                dy = (pixellist[j].y - mu_y);
                /* f is the bit in the exponent, this will be saved as
                   pu */
                double f = (s_yy*dx*dx + s_xx*dy*dy - 2.0*s_xy*dx*dy)/D;
                pus[i + j*m_iNumDists] = f;
                /* the probability that this pixel comes from the ith
                   distribution */
                ps[i + j*m_iNumDists] = (0.5/(M_PI*sqrt(D)))*exp(-0.5*f);
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
            
            calculate_axes_and_angle(m_vCovariances[i].data[0],
                                     m_vCovariances[i].data[1],
                                     m_vCovariances[i].data[3],
                                     &M,
                                     &m,
                                     &o);

            anglediff = fabs(o - oldangles[i]);
            //anglediff = abs(atan2(m_vCovariances[i].data[3] - m_vCovariances[i].data[0] + sqrt(pow(m_vCovariances[i].data[0] - m_vCovariances[i].data[3],2) + 4.0*pow(m_vCovariances[i].data[1],2)), 2*m_vCovariances[i].data[1]) - oldangles[i]);
            anglediff *= 57.2958;
            if (anglediff > maxdiff)
            {
                maxdiff = anglediff;
            }


            Mdiff = 100.0*fabs(M - oldMs[i]);
            mdiff = 100.0*fabs(m - oldms[i]);
            if(Mdiff > maxdiff)
            {
                maxdiff = Mdiff;
            }
            if(mdiff > maxdiff)
            {
                maxdiff = mdiff;
            }

            dx = m_vMeans[i].data[0] - orig_xs[i];
            dy = m_vMeans[i].data[1] - orig_ys[i];
            if(m_dMaxDistance > 0 && (dx*dx + dy*dy >= m_dMaxDistance))
            {
                if(m_pDebugFile){fprintf(m_pDebugFile, "Constrain distance\n");}
                double th = atan2(dy, dx);
                m_vMeans[i].data[0] = orig_xs[i] + m_dMaxDistance*cos(th);
                m_vMeans[i].data[1] = orig_ys[i] + m_dMaxDistance*sin(th);                
            }

            double area = M_PI*m*M;
            double old_area = M_PI*orig_Ms[i]*orig_ms[i];
            double eccentricity = M/m;
            if(eccentricity == 0)
            {
                eccentricity = 1.0;
            }
            bool const_e = (m_dMaxEccentricity > 1.0 &&
                                 (eccentricity > m_dMaxEccentricity));
            bool const_a = (m_dMaxSizePercentChange > 0 &&
                            ((fabs(area - old_area)/(old_area)) > 0.01*m_dMaxSizePercentChange));
            if(const_e || const_a)
            {
                if(const_e)
                {
                    if(m_pDebugFile){printf("Constrain eccentricity\n");}
                    eccentricity = m_dMaxEccentricity;
                    M = m*m_dMaxEccentricity;
                }
                if(const_a)
                {
                    if(m_pDebugFile){printf("Constrain area\n");}
                    area = old_area*(1.0 + 0.01*m_dMaxSizePercentChange);
                    m = sqrt(area/(M_PI*eccentricity));
                    M = m*eccentricity;
                }
                double c = cos(o);
                double s = sin(o);
                m_vCovariances[i].data[0] = 0.25*area*(c*c*M*M + s*s*m*m);
                m_vCovariances[i].data[1] = 0.25*area*(c*s*(M*M - m*m));
                m_vCovariances[i].data[2] = m_vCovariances[i].data[1];
                m_vCovariances[i].data[3] = s*s*M*M + c*c*m*m;
            }

            if(m_pDebugFile)
            {
                calculate_axes_and_angle(m_vCovariances[i].data[0],
                                         m_vCovariances[i].data[1],
                                         m_vCovariances[i].data[3],
                                         &M,
                                         &m,
                                         &o);
                
                fprintf(m_pDebugFile, "\t\tEMMG iter n = %d, i = %d, State: (x,y) = (%f, %f), "
                        "M = %f, m = %f, o = %f, s_xx = %f, s_yy = %f \n", numiters, i,
                        m_vMeans[i].data[0], m_vMeans[i].data[1], M, m, o,
                        m_vCovariances[i].data[0], m_vCovariances[i].data[1]);

                
                fprintf(m_pDebugFile, "\t\tEMMG iter n = %d, i = %d, Diffs:%f, %f, %f, %f, %f\n",
                        numiters, i, xdiff, ydiff, anglediff, Mdiff, mdiff);
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
        distributionnumber = -1;
        
        for(i=0 ; i < m_iNumDists ; i++)
        {
            // Determine which distribution has the maximum likelihood of containing the pixel
            if (pls[i + j*m_iNumDists] > maxlikelihood
                && ps[i + j*m_iNumDists] > 0.002)  // TODO: This is a
                                                   // magic number and
                                                   // should be
                                                   // replaced with
                                                   // something more meaningful
            {
                maxlikelihood = pls[i + j*m_iNumDists];
                distributionnumber = i;
            }
            // If the pixel lies within two standard deviations of the mean of any distribution, we include it
            else if (pus[i + j*m_iNumDists] < 4.0)
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
        if (distributionnumber >= 0 && distmembership[distributionnumber] == 0)
        {
            distmembership[distributionnumber] = 1;
            numberdists++;
        }

        // Generate an integer to represent the set of distributions that the pixel belongs to
        int p = 1;
        for(k = 0; k < m_iNumDists; k++)
        {
            if(distmembership[k] > 0)
            {
                pixelalloc[j] += p;
            }
            p *= 2;
        }
        /* for (k=0 ; k < numberdists ; k++)
         * {
         *     for (; distmembership[i] == 0 ; i++)
         *     {}
         *     pixelalloc[j] += i*intpow(m_iNumDists,k);
         *     distmembership[i] = 0;
         * } */

    }
        
    // Clear all temporary arrays
    delete[] pus;
    delete[] ps;
    delete[] pls;
    delete[] totalpls;
    delete[] totaldensity;
    delete[] oldxs;
    delete[] oldys;
    delete[] oldangles;
    delete[] oldMs;
    delete[] oldms;
    delete[] distmembership;

    delete[] orig_xs;
    delete[] orig_ys;
    delete[] orig_Ms;
    delete[] orig_ms;    
    
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

void calculate_axes_and_angle(double xx, double xy, double yy, double* M, double* m, double* o)
{
    double d = sqrt(4.0*xy*xy + (xx - yy)*(xx - yy));
    double a = pow(16.0*M_PI*M_PI*(xx*yy - xy*xy), 0.25);
    if(a == 0)
    {
        a = 1;
    }
    *M = sqrt((2*(xx + yy + d))/a);
    *m = sqrt((2*(xx + yy - d))/a);
    *o = -0.5*atan2(xy, (xx - yy));
}
