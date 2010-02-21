/*
 *  GYBlobs.cpp
 *  MADTraC
 *
 *  Blob classes for the use of GYSegmenter
 *
 *  Created by George Young on 5/10/09.
 *  Copyright 2009 Princeton University. All rights reserved.
 *
 */


#include "GYBlobs.h"

#include <math.h>

// Member functions for class GYBlob

GYBlob::GYBlob()
{
    m_dXCentre = m_dYCentre = m_dArea = m_dOrientation = m_dMajorAxis = m_dMinorAxis = 0.0;
}

GYBlob::GYBlob(double x, double y, double A, double phi, double major, double minor)
{
    m_dXCentre = x;
    m_dYCentre = y;
    m_dArea = A;
    m_dOrientation = phi;
    m_dMajorAxis = major;
    m_dMinorAxis = minor;
}



// Member functions for class GYRawBlob

GYRawBlob::GYRawBlob(int expectedpixels)
{
    m_vPixelList.reserve(expectedpixels);
    m_vPixelList.resize(0);
        
    m_iNumPixels = 0;
        
    m_dPerimeter = 0.0;
    m_dXCentre = 0.0;
    m_dYCentre = 0.0;
        
    m_bCalcXCentre = false;
    m_bCalcYCentre = false;
        
    m_BoundingBox = cvRect(0,0,0,0);
}

void GYRawBlob::AddPoint(CvPoint newpoint)
{
    m_vPixelList.push_back(newpoint);
    m_iNumPixels++;

    m_bCalcXCentre = false;
    m_bCalcYCentre = false;
        
    if (m_iNumPixels == 1)
    {
        m_BoundingBox.x = newpoint.x;
        m_BoundingBox.y = newpoint.y;
        m_BoundingBox.width = 1;
        m_BoundingBox.height = 1;
    }
    else
    {
        double temp_value = 0.0;
        if (newpoint.x < m_BoundingBox.x)
        {
            temp_value = m_BoundingBox.x;
            m_BoundingBox.x = newpoint.x;
            m_BoundingBox.width += temp_value - newpoint.x;
        }
        else if (newpoint.x > m_BoundingBox.x + m_BoundingBox.width - 1)
        {
            m_BoundingBox.width = newpoint.x - m_BoundingBox.x + 1;
        }
                
        if (newpoint.y < m_BoundingBox.y)
        {
            temp_value = m_BoundingBox.y;
            m_BoundingBox.y = newpoint.y;
            m_BoundingBox.height += temp_value - newpoint.y;
        }
        else if (newpoint.y > m_BoundingBox.y + m_BoundingBox.height - 1)
        {
            m_BoundingBox.height = newpoint.y - m_BoundingBox.y + 1;
        }
    }
}

void GYRawBlob::SetPerimeter(double p)
{
    m_dPerimeter = p;
}


CvRect GYRawBlob::GetBoundingBox()
{
    return m_BoundingBox;
}

int GYRawBlob::GetNumPixels()
{
    return m_iNumPixels;
}

double GYRawBlob::GetArea()
{
    return (double) m_iNumPixels;
}

double GYRawBlob::GetPerimeter()
{
    return m_dPerimeter;
}

double GYRawBlob::GetXCentre()
{
    if (m_iNumPixels == 0)
    {
        m_dXCentre = 0.0;
        m_bCalcXCentre = true;
    }
    if (!m_bCalcXCentre)
    {
        double tempXCentre = 0.0;
        int pixelnum;
        for (pixelnum = 0 ; pixelnum < m_iNumPixels ; pixelnum++)
        {
            tempXCentre += m_vPixelList[pixelnum].x;
        }

        m_dXCentre = tempXCentre/((double) m_iNumPixels);
        m_bCalcXCentre = true;
    }
        
    return m_dXCentre;
}

double GYRawBlob::GetYCentre()
{
    if (m_iNumPixels == 0)
    {
        m_dYCentre = 0.0;
        m_bCalcYCentre = true;
    }
    if (!m_bCalcYCentre)
    {
        double tempYCentre = 0.0;
        int pixelnum;
        for (pixelnum = 0 ; pixelnum < m_iNumPixels ; pixelnum++)
        {
            tempYCentre += m_vPixelList[pixelnum].y;
        }
                
        m_dYCentre = tempYCentre/((double) m_iNumPixels);
        m_bCalcYCentre = true;
    }
        
    return m_dYCentre;
}

double GYRawBlob::GetXXMoment()
{
    if (m_iNumPixels == 0)
    {
        return 0.0;
    }
    else
    {
        if(!m_bCalcXCentre)
        {
            double temp_val;
            temp_val = this->GetXCentre();
        }
                
        double XX = 0.0;
        int iter;
        for (iter = 0 ; iter < m_iNumPixels ; iter++)
        {
            XX += pow(m_vPixelList[iter].x - m_dXCentre, 2);
        }
                
        // Protect against zero moments (e.g. when all pixels have the same x value)
        if (XX < ((double) m_iNumPixels)/12.0)
        {
            XX = ((double) m_iNumPixels)/12.0;
        }
                
        return XX;
    }
}

double GYRawBlob::GetXYMoment()
{
    if (m_iNumPixels == 0)
    {
        return 0.0;
    }
    else
    {
        if(!m_bCalcXCentre)
        {
            double temp_val;
            temp_val = this->GetXCentre();
        }
        if(!m_bCalcYCentre)
        {
            double temp_val;
            temp_val = this->GetYCentre();
        }
                
        double XY = 0.0;
        int iter;
        for (iter = 0 ; iter < m_iNumPixels ; iter++)
        {
            XY += (m_vPixelList[iter].x - m_dXCentre)*(m_vPixelList[iter].y - m_dYCentre);
        }
                
        return XY;
    }   
}

double GYRawBlob::GetYYMoment()
{
    if (m_iNumPixels == 0)
    {
        return 0.0;
    }
    else
    {
        if(!m_bCalcYCentre)
        {
            double temp_val;
            temp_val = this->GetYCentre();
        }
                
        double YY = 0.0;
        int iter;
        for (iter = 0 ; iter < m_iNumPixels ; iter++)
        {
            YY += pow(m_vPixelList[iter].y - m_dYCentre, 2);
        }
                
        // Protect against zero moments (e.g. when all pixels have the same y value)
        if (YY < ((double) m_iNumPixels)/12.0)
        {
            YY = ((double) m_iNumPixels)/12.0;
        }
                
        return YY;
    }   
}

void GYRawBlob::GetPixelList(std::vector<CvPoint>& pixellist)
{
    if (pixellist.size() == (unsigned int) m_iNumPixels)
    {
        pixellist = m_vPixelList;
    }
    else
    {
        printf("Reserved vector has the wrong size\n");
    }   
}
