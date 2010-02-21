#ifndef R3_H

/** @addtogroup MT_Support
 * @{ */

/** @file  R3.h 
 *
 * Implements of MT_R3, a 3 dimensional cartesian vector space 
 * that supports standard operations. Also defines several
 * support functions for 3D vectors. 
 *
 * Created by D. Swain 8/12/07
 * 
 */


/** @class MT_R3
 *
 * @brief 3-dimensional vector.
 *
 * A 3 dimensional cartesian vector space 
 * supporting standard vector operations.  The member data are
 * double precision floating point numbers that can
 * be accessed publicly - i.e. R.x, R.y, and R.z */
class MT_R3 {
private:
    mutable float fdata[3];
    mutable double ddata[3];

public:
    // Public data
    double x; /**< x coordinate */
    double y; /**< y coordinate */
    double z; /**< z coordinate */

    /** Default ctor sets x = y = z = 0 */
    MT_R3();
    /** Random ctor - generates x and y between -scale and scale
     * and sets z = 0. */
    MT_R3(double scale);
    /** Full specification ctor - sets x, y, and z.  
     * z defaults to zero. */
    MT_R3(double inx, double iny, double inz = 0);

    /** Addition operator.  Element-by-element addition is carried out. */
    MT_R3 operator + (const MT_R3 &v);
    /** Unary addition operator.  Element-by-element addition is carried out. */
    MT_R3 &operator += (const MT_R3 &v);
    /** Subtraction operator.  Element-by-element subtraction is carried out. */
    MT_R3 operator - (const MT_R3 &v);
    /** Unary subtraction operator.  Element-by-element subtraction 
     * is carried out. */
    MT_R3 &operator -= (const MT_R3 &v);
    /** Unary multiplication by a scalar operator.  Each element is
     * multiplied by the given scalar.
     *
     * Note: Non-unary multiplication operators are defined in the
     * file so that order can be accounted for.  */
    MT_R3 &operator *= (double scalar);
    /** Division by a scalar operator.  Each element is divided by the
     * given scalar.  If the scalar is zero, an error message is printed
     * to stderr and a very small number is substituted for scalar. */
    MT_R3 operator / (double scalar);
    /** Unary division by a scalar operator.  Each element is divided by the
     * given scalar.  If the scalar is zero, an error message is printed
     * to stderr and a very small number is substituted for scalar. */
    MT_R3 &operator /= (double scalar);
    /** Cast-to-float pointer operator.  Copies the values of x, y, and z
     * to a float array of length 3 and returns the address of the first
     * element.
     *
     * Example:
     * \code
     * MT_R3 r(1.0, -1.0, 0);
     * float* p = (float *) r;
     * printf("x = %f, y = %f, z = %f\n", p[0], p[1], p[2]);
     * \endcode
     * Would print "x = 1.0, y = -1.0, z = 0.0".
     *
     * Useful for e.g. glTranslate3fv. */
    operator float*() const { fdata[0] = x; fdata[1] = y; 
        fdata[2] = z; return &fdata[0]; };

    // Access functions (deprecated - x,y,z now public)
    double getx() const { return x; };          /**< x coordinate accessor */
    double gety() const { return y; };          /**< y coordinate accessor */
    double getz() const { return z; };          /**< z coordinate accessor */
    void setx(double in){ x = in; };    /** Set x coordinate       */
    void sety(double in){ y = in; };    /** Set y coordinate       */
    void setz(double in){ z = in; };    /** Set z coordinate       */

    /** Function to set x = y = z = 0 */
    void zero();
    /** Function to return the square of the Euclidean norm of the vector,
     * i.e. returns x^2 + y^2 + z^2 */
    double sqrnorm();
    /** Function to return the Euclidean norm of the vector, i.e.
     * returns sqrt(x^2 + y^2 + z^2) */
    double norm();

};

/** Multiplicaton-by-scalar operator for MT_R3 vectors.  Multiplies
 * each element by the scalar and returns the result.  
 * vector * scalar version. */
MT_R3 operator * (const MT_R3& lhs, double rhs);
/** Multiplicaton-by-scalar operator for MT_R3 vectors.  Multiplies
 * each element by the scalar and returns the result.  
 * scalar * vector version. */
MT_R3 operator * (double lhs, const MT_R3& rhs);


//--------------------Support Functions---------------

/** Function to calculate the dot product between two vectors.
 * Returns v1.x*v2.x + v1.y*v2.y + v1.z*v2.z  */
double MT_dotproduct(const MT_R3& v1, const MT_R3& v2);
/** Function to calculate the Euclidean distance between two
 * vectors.  
 * Returns sqrt( (v1.x - v2.x)^2 + (v1.y - v2.y)^2 + (v1.z - v2.z)^2).*/
double MT_R3distance(const MT_R3& v1, const MT_R3& v2);
/** Function to calculate the square of the Euclidean distance
 * between two vectors. 
 * Returns (v1.x - v2.x)^2 + (v1.y - v2.y)^2 + (v1.z - v2.z)^2. */
double MT_distsquared(const MT_R3& v1, const MT_R3& v2);

/* @} */

#define R3_H
#endif
