#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

/** @addtogroup MT_Support
 * @{ */

/** @file
 *  BoundingBox.h
 *
 *  Defines the MT_BoundingBox class
 *
 *  Created by Daniel Swain on 9/10/08.
 *  
 */

/** @class MT_BoundingBox
 *
 * @brief Updatable bounding box for 2-or-3-dimensional data.
 *
 * A simple class for keeping track of the extents of
 * 2-or-3-dimensional spatial data.  After creation
 * just call the ShowX, ShowY, ShowZ functions to
 * update the box.  You can also force X or Y limits
 * by calling the SetXMinAndLock, etc, functions. 
 * The data elements are not encapsulated - i.e.
 * you can access (or set) the limits by just using
 * MT_BoundingBox.xmin, etc.  Note that setting
 * MT_BoundingBox.xmin = some value is not the same as
 * calling MT_BoundingBox.SetXMinAndLock(some value) since
 * the value can be modified by further calls to ShowX */
class MT_BoundingBox
{
private:
    
    bool initialized_xmin;
    bool initialized_xmax;
    bool initialized_ymin;
    bool initialized_ymax;
    bool initialized_zmin;
    bool initialized_zmax;
    
    bool lock_xmin;
    bool lock_xmax;
    bool lock_ymin;
    bool lock_ymax;
    bool lock_zmin;
    bool lock_zmax;
    
public:
    
    double xmin;  /**< Minimum x value seen. */
    double xmax;  /**< Maximum x value seen. */
    double ymin;  /**< Minimum y value seen. */
    double ymax;  /**< Maximum y value seen. */
    double zmin;  /**< Minimum z value seen. */
    double zmax;  /**< Maximum z value seen. */
    
    /** Default ctor sets all limits to zero and prepares the class
     * to receive subsequent calls to ShowX, ShowY, and ShowZ. */
    MT_BoundingBox();
    
    /** Ctor to set limits (defaults z limits to zero) and prepare
     * to receive subsequent calls to ShowX, ShowY, and ShowZ. */
    MT_BoundingBox(double setxmin, double setxmax, double setymin, double setymax, 
                double setzmin = 0,
                double setzmax = 0);
    
    /** Show the MT_BoundingBox an x value.  If the value is less than
     * xmin and ShowXMinAndLock has not been called, the xmin value
     * is updated.  Likewise for xmax and ShowXMaxAndLock. */
    void ShowX(double x);  
    /** Show the MT_BoundingBox a y value.  If the value is less than
     * ymin and ShowYMinAndLock has not been called, the ymin value
     * is updated.  Likewise for ymax and ShowYMaxAndLock. */
    void ShowY(double y); 
    /** Show the MT_BoundingBox a z value.  If the value is less than
     * zmin and ShowZMinAndLock has not been called, the zmin value
     * is updated.  Likewise for zmax and ShowZMaxAndLock. */
    void ShowZ(double z);
    
    /** Forces xmin to the given value and locks it so that it
     * will not be updated by subsequent calls to ShowX. */
    void SetXMinAndLock(double x);
    /** Forces xmax to the given value and locks it so that it
     * will not be updated by subsequent calls to ShowX. */
    void SetXMaxAndLock(double x);
    /** Forces ymin to the given value and locks it so that it
     * will not be updated by subsequent calls to ShowY. */
    void SetYMinAndLock(double y);
    /** Forces ymax to the given value and locks it so that it
     * will not be updated by subsequent calls to ShowY. */
    void SetYMaxAndLock(double y);
    /** Forces zmin to the given value and locks it so that it
     * will not be updated by subsequent calls to ShowZ. */
    void SetZMinAndLock(double z);
    /** Forces zmax to the given value and locks it so that it
     * will not be updated by subsequent calls to ShowZ. */
    void SetZMaxAndLock(double z);
    
};

/* @} */

#endif  // BOUNDINGBOX_H
