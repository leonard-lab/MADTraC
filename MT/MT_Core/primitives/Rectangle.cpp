/*
 *  MT_Rectangle.cpp
 *
 *  Created by Daniel Swain on 12/1/09.
 *
 */

#include "Rectangle.h"

#include "MT/MT_Core/support/mathsupport.h"

MT_Rectangle::MT_Rectangle()
  : xmin(0), xmax(0), ymin(0), ymax(0)
{
  
}

MT_Rectangle::MT_Rectangle(double _xmin, double _xmax, double _ymin, double _ymax)
  : xmin(MT_MIN(_xmin,_xmax)), xmax(MT_MAX(_xmin,_xmax)), ymin(MT_MIN(_ymin,_ymax)), ymax(MT_MAX(_ymin,_ymax))
{
}

MT_Rectangle& MT_Rectangle::operator = (const MT_Rectangle& rhs)
{
  
    if(this != &rhs)
    {
        xmin = rhs.xmin;
        xmax = rhs.xmax;
        ymin = rhs.ymin;
        ymax = rhs.ymax;
    }
  
    return *this;
  
}

MT_Rectangle& MT_Rectangle::operator += (const MT_Rectangle& rhs)
{
    xmin += rhs.xmin;
    xmax += rhs.xmax;
    ymin += rhs.ymin;
    ymax += rhs.ymax;
  
    return *this;
}

MT_Rectangle& MT_Rectangle::operator -= (const MT_Rectangle& rhs)
{
    xmin -= rhs.xmin;
    xmax -= rhs.xmax;
    ymin -= rhs.ymin;
    ymax -= rhs.ymax;
  
    return *this;
}

MT_Rectangle& MT_Rectangle::operator *= (double rhs)
{
    xmin *= rhs;
    xmax *= rhs;
    ymin *= rhs;
    ymax *= rhs;
  
    return *this;
}

MT_Rectangle& MT_Rectangle::operator /= (double rhs)
{
    if(rhs == 0)
    {
        rhs = 1E-10;
    }
  
    xmin /= rhs;
    xmax /= rhs;
    ymin /= rhs;
    ymax /= rhs;
  
    return *this;
}

const MT_Rectangle MT_Rectangle::operator + (const MT_Rectangle& other) const
{
    return (MT_Rectangle(*this) += other);
}

const MT_Rectangle MT_Rectangle::operator - (const MT_Rectangle& other) const
{
    return (MT_Rectangle(*this) -= other);
}

const MT_Rectangle MT_Rectangle::operator / (double scalar) const
{
    return (MT_Rectangle(*this) /= scalar);
}

bool MT_Rectangle::operator == (const MT_Rectangle& other) const
{
    return MT_IsEqual(xmin, other.xmin) && 
        MT_IsEqual(xmax, other.xmax) && 
        MT_IsEqual(ymin, other.ymin) && 
        MT_IsEqual(ymax, other.ymax);
}

bool MT_Rectangle::operator != (const MT_Rectangle& other) const
{
    return !(*this == other);
}

const MT_Rectangle operator * (const MT_Rectangle& v, double scalar)
{
    return MT_Rectangle(v.xmin*scalar, v.xmax*scalar, v.ymin*scalar, v.ymax*scalar);
}

const MT_Rectangle operator * (double scalar, const MT_Rectangle& v)
{
    return MT_Rectangle(v.xmin*scalar, v.xmax*scalar, v.ymin*scalar, v.ymax*scalar);
}

bool MT_Rectangle::Contains(const MT_Rectangle& v) const
{
    return (v.xmin >= xmin) && (v.xmax <= xmax) && (v.ymin >= ymin) && (v.ymax <= ymax);
}

bool MT_Rectangle::ContainedIn(const MT_Rectangle& v) const
{
    return v.Contains(*this);
}

void MT_Rectangle::Expand(double factor)
{
  
    double xcenter = 0.5*(xmin + xmax);
    double ycenter = 0.5*(ymin + ymax);
    double half_width = 0.5*(xmax - xmin);
    double half_height = 0.5*(ymax - ymin);
  
    xmin = xcenter - factor*half_width;
    xmax = xcenter + factor*half_width;
    ymin = ycenter - factor*half_height;
    ymax = ycenter + factor*half_height;
  
}

MT_Rectangle MT_RectangleFromBoundingBox(const MT_BoundingBox& bb)
{
    return MT_Rectangle(bb.xmin, bb.xmax, bb.ymin, bb.ymax);
}

MT_Rectangle MT_RectangleUnion(const MT_Rectangle& v1, const MT_Rectangle& v2)
{
    return MT_Rectangle(MT_MIN(v1.xmin, v2.xmin), MT_MAX(v1.xmax, v2.xmax), MT_MIN(v1.ymin, v2.ymin), MT_MAX(v1.ymax, v2.ymax));
}

MT_Rectangle MT_RectangleIntersection(const MT_Rectangle& v1, const MT_Rectangle& v2)
{
    return MT_Rectangle(MT_MAX(v1.xmin, v2.xmin), MT_MIN(v1.xmax, v2.xmax), MT_MAX(v1.ymin, v2.ymin), MT_MIN(v1.ymax, v2.ymax));
}

MT_RectangleHistory::MT_RectangleHistory(unsigned int max_length)
  : m_iMaxLength(max_length), m_vHistory(0) 
{
  
}

void MT_RectangleHistory::Push(MT_Rectangle viewport)
{
    m_vHistory.push_front(viewport);
    if(m_vHistory.size() >= m_iMaxLength)
    {
        /* discards the extras */
        m_vHistory.resize(m_iMaxLength);
    }
}

void MT_RectangleHistory::Pop()
{
    m_vHistory.pop_front();
}

MT_Rectangle MT_RectangleHistory::MostRecent() const
{
    if(m_vHistory.size() > 0)
    {
        return m_vHistory[0];
    }
    else
    {
        return MT_BlankRectangle;
    }
}

MT_Rectangle MT_RectangleHistory::AtHistory(unsigned int i) const
{
    if(i >= m_vHistory.size())
    {
        return MT_BlankRectangle;  
    }
    else
    {
        return m_vHistory[i];
    }
}


MT_PointHistory& MT_PointHistory::operator = (const MT_PointHistory& rhs)
{
    m_vXHistory = rhs.m_vXHistory;
    m_vYHistory = rhs.m_vYHistory;
    return *this;
}  


void MT_PointHistory::getMostRecentXY(double* x, double* y) const
{
    getXYIndexedFromStart(getSize() - 1, x, y);
}

void MT_PointHistory::getFirstXY(double* x, double* y) const
{
    getXYIndexedFromStart(0, x, y);
}

void MT_PointHistory::getXYIndexedFromStart(unsigned int i, double* x, double* y) const
{
    unsigned int size = getSize();
    if(size == 0 || i >= size || i < 0)
    {
        *x = 0;
        *y = 0;
    }
    else
    {
        *x = m_vXHistory[i];
        *y = m_vYHistory[i];
    }
}

void MT_PointHistory::getXYIndexedFromEnd(unsigned int i, double* x, double* y) const
{
    getXYIndexedFromStart(getSize() - i - 1, x, y);
}

MT_Rectangle MT_RectangleFromPointHistory(const MT_PointHistory& ph)
{
    if(ph.getSize() == 0)
    {
        return MT_Rectangle(0,0,0,0);
    }
    double x1, y1, x2, y2;
    ph.getMostRecentXY(&x2, &y2);
    ph.getFirstXY(&x1, &y1);
    return MT_Rectangle(x1, y1, x2, y2);
}
