#ifndef OPENCVMATH_H
#define OPENCVMATH_H

/** @addtogroup MT_Core
 * @{ */

/** @file  OpenCVmath.h - D.Swain 9/1/2010
 *
 * OpenCV-based math support module.
 * 
 */

#ifndef MT_NO_OPENCV

#if defined(MT_HAVE_OPENCV_FRAMEWORK)
#include <OpenCV/OpenCV.h>
#else
#include <cxtypes.h>
#include <cv.h>
#endif

#include <stdio.h> /* for stdout, fprintf */

/** Prints a CvMat to the specified FILE stream (stdout by default).
 * If name is NULL, no header is displayed */
void MT_Display_CvMat(const CvMat* M,
                      const char* name,
                      FILE* f = stdout);

/** Computes a quadratic form, dst = X'*W*X.
 * @param X Outer matrix (or vector)
 * @param W Symmetric weight matrix
 * @param dst Destimation
 * @param transpose_X If true, X*W*X' is computed.
 * @param tmp_prod Pre-allocated temporary matrix for W*X (or W*X').
 * If NULL, a temporary matrix is allocated and deallocated
 * automatically. */
void MT_CVQuadraticMul(const CvMat* X,
                       const CvMat* W,
                       CvMat* dst,
                       bool transpose_X = false,
                       CvMat* tmp_prod = NULL);

/** Allocate a CvMat to hold a Cholesky decomposition result.  The
 * result has the same size as the src and is allocated as a matrix of
 * doubles. */
CvMat* MT_CreateCholeskyResult(const CvMat* src);

const unsigned int MT_CHOLESKY_UPPER_TRIANGULAR = 0;
const unsigned int MT_CHOLESKY_LOWER_TRIANGULAR = 1;

/** Perform a Cholesky decomposition.  If orientation is
 * MT_CHOLESKY_UPPER_TRIANGULAR (0, default), the result is an upper
 * triangular matrix and should match the result of typing chol(src)
 * in Matlab - transpose(dst)*dst = src.  If orientation is
 * MT_CHOLESKY_LOWER_TRIANGULAR (1), the result is lower triangular.
 * Because of the way the algorithm is calculated, the lower
 * triangular result is slightly more efficient.
 *
 * NOTE: It is assumed that the source matrix is symmetric and
 * positive definite.  No checking is done to ensure this!  In most
 * applications, this should be fine by construction.
 *
 * The source and destination matrices must be square
 * matrices of doubles (allocated with type CV_64FC1), otherwise false
 * will be returned and nothing will be done.
 *
 * If orientation is neither MT_CHOLESKY_UPPER_TRIANGULAR (0) nor
 * MT_CHOLESKY_LOWER_TRIANGULAR (1), false is returned.
 *
 * True is returned on success. */
bool MT_Cholesky(const CvMat* src,
                 CvMat* dst,
                 unsigned int orientation = MT_CHOLESKY_UPPER_TRIANGULAR);

#endif /* !MT_NO_OPENCV */

/* @} */

#endif /* OPENCVMATH_H */
