#ifndef Viewport_H
#define Viewport_H

/** @addtogroup MT_Support
 * @{ */

/** @file  MT_Rectangle.h
 *  @short Classes to keep track of a two-dimensional rectangle.
 *
 *  Created by Daniel Swain on 12/1/09.
 *
 *  Defines the MT_Rectangle and MT_RectangleHistory classes,
 *  supporting functions and related constants.
 *
 */

#include "BoundingBox.h"

#include <deque>
#include <vector>

/** @class MT_Rectangle
 *
 * @brief Keeps track of two-dimensional rectangle defining a viewport.
 *
 * Contains four doubles - xmin, xmax, ymin, and ymax, defining 
 * a viewport rectangle.  The coordinates are public access, so
 * can be accessed simply by e.g. MT_Rectangle.xmin etc.
 *
 * Arithmetic operators are defined - e.g. addition, subtraction,
 * multiplication and division by a scalar.
 *
 * @see MT_RectangleFromBoundingBox
 * @see MT_RectangleUnion
 * @see MT_RectangleIntersection
 * @see MT_RectangleHistory
 *
 */
class MT_Rectangle
{
public:
    
    double xmin;  /**< x-coordinate of bottom left of rectangle. */
    double xmax;  /**< y-coordinate of bottom left of rectangle. */
    double ymin;  /**< x-coordinate of top right of rectangle.   */
    double ymax;  /**< y-coordinate of top right of rectangle.   */
    
    /** Default ctor sets values to zero. */
    MT_Rectangle();
    /** Ctor to set each of the values.  Automatically checks
     * to make sure xmin < xmax and ymin < ymax. */
    MT_Rectangle(double _xmin, double _xmax, double _ymin, double _ymax);
    
    MT_Rectangle& operator = (const MT_Rectangle& rhs);
    MT_Rectangle& operator += (const MT_Rectangle& rhs);
    MT_Rectangle& operator -= (const MT_Rectangle& rhs);
    MT_Rectangle& operator *= (double rhs);
    MT_Rectangle& operator /= (double rhs);
    const MT_Rectangle operator + (const MT_Rectangle& other) const;
    const MT_Rectangle operator - (const MT_Rectangle& other) const;
    const MT_Rectangle operator / (double scalar) const;

    /** Binary comparison operator.  Returns true if all of the individual
     * bounds are equal according to MT_IsEqual.
     * @see MT_IsEqual */
    bool operator == (const MT_Rectangle& other) const;
    /** Binary comparison operator.  Returns true if any of the individual
     * bounds are not equal according to MT_IsEqual.
     * @see MT_IsEqual */
    bool operator != (const MT_Rectangle& other) const;
    
    /** Returns true if the rectangle defined by v is contained
     * within the one defined by this viewport.
     * @see ContainedIn */
    bool Contains(const MT_Rectangle& v) const;
    /** Returns true if the rectangle defined this viewport is 
     * contained within the one defined by v.
     * @see Contains */
    bool ContainedIn(const MT_Rectangle& v) const;
    
    /** Expands the viewport by a percentage fractional factor.
     * The result has the same center but its width and height are
     * (1 + factor) times the current width and height. */
    void Expand(double factor);
    
    /** Returns the x-coordinate of the center of the rectangle. */
    double GetXCenter() const {return 0.5*(xmin + xmax);};
    /** Returns the y-coordinate of the center of the rectangle. */
    double GetYCenter() const {return 0.5*(ymin + ymax);};
    /** Returns the width of the rectangle. */ 
    double GetWidth() const {return xmax - xmin;};
    /** Returns the height of the rectangle. */ 
    double GetHeight() const {return ymax - ymin;};
    
};

const MT_Rectangle operator * (const MT_Rectangle& v, double scalar);
const MT_Rectangle operator * (double scalar, const MT_Rectangle& v);

/** Function to convert a bounding box to a viewport by copying
 * the bounds. */
MT_Rectangle MT_RectangleFromBoundingBox(const MT_BoundingBox& bb);
/** Function to find the union of two viewports.  */
MT_Rectangle MT_RectangleUnion(const MT_Rectangle& v1, const MT_Rectangle& v2);
/** Function to find the intersection of two viewports.  */
MT_Rectangle MT_RectangleIntersection(const MT_Rectangle& v1, const MT_Rectangle& v2);

/** @var const MT_Rectangle MT_BlankRectangle
 * A viewport with zero bounds.  Use for example as a default value. */
const MT_Rectangle MT_BlankRectangle = MT_Rectangle(0,0,0,0);

const unsigned int MT_DEFAULT_HISTORY_LENGTH = 25;

/** @class MT_RectangleHistory
 *
 * @brief A container list for recently used viewports.
 *
 * Can be used to store a fixed number of viewports.  The
 * default number is 25.  A bit like a FILO buffer, but
 * with a cap on the number of values stored.
 *
 */
class MT_RectangleHistory
{
protected:
    unsigned int m_iMaxLength;
    std::deque<MT_Rectangle> m_vHistory;
    
public:
    /** Ctor sets the maximum number of viewports to store and
     * clears the history. */
    MT_RectangleHistory(unsigned int max_length = MT_DEFAULT_HISTORY_LENGTH);
    
    /** Adds the given viewport to the front of the list.  That is,
     * this will be the value given when MostRecent is called. */
    void Push(MT_Rectangle viewport);

    /** Removes the most recent viewport from the list. */
    void Pop();
    
    /** Returns the most recent viewport on the list.  Does
     * not remove it from the list.
     * @see Pop */
    MT_Rectangle MostRecent() const;

    /** Returns the i'th most recent viewport in the list. */
    MT_Rectangle AtHistory(unsigned int i) const;
    
    /** Returns the number of viewports stored so far. 
     * Always less than the max_length specified initially. */
    unsigned int Size() const {return m_vHistory.size();};

    /** Clears the history. */
    void Clear(){m_vHistory.resize(0);};
    
};

class MT_PointHistory
{
private:
    std::vector<double> m_vXHistory;
    std::vector<double> m_vYHistory;
public:
MT_PointHistory():m_vXHistory(0), m_vYHistory(0){};

    MT_PointHistory& operator = (const MT_PointHistory& rhs);

    void pushXY(double x, double y){m_vXHistory.push_back(x); m_vYHistory.push_back(y);};

    void getMostRecentXY(double* x, double* y) const;
    void getFirstXY(double* x, double* y) const;
    void getXYIndexedFromStart(unsigned int i, double* x, double* y) const;
    void getXYIndexedFromEnd(unsigned int i, double* x, double* y) const;

    unsigned int getSize() const {return m_vXHistory.size();};

    void clearHistory(){m_vXHistory.resize(0); m_vYHistory.resize(0);};
};

MT_Rectangle MT_RectangleFromPointHistory(const MT_PointHistory& ph);

/* @} */

#endif // Viewport_H
