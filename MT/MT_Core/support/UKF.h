#ifndef UKF_H
#define UKF_H

/** @addtogroup MT_Core
 * @{ */

/** @file  UKF.h - D.Swain 9/10/2010, Based on Matlab code by Darren Pais.
 *
 * Unscented Kalman Filter implementation using OpenCV matrices.
 *
 *  - Allocate and initialize with MT_UKFInit.
 *  - Set the Q and R matrices with MT_UKFCopyQR.
 *  - Set the initial condition with MT_UKFSetState.
 *  - Prediction step: MT_UKFPredict.
 *  - Set the measurement with MT_UKFSetMeasurement
 *  - Correct step: MT_UKFCorrect
 *  - The predicted state is MT_UKF_struct::x1
 *  - The estimated state is MT_UKF_struct::x
 *  - If MT_UKFSetMeasurement is not called between calls to
 *      MT_UKFCorrect, the predicted state (x1) is used
 *  - Function pointers are used for state and measurement
 *      transformations for greatest flexibility
 *
 * Basic usage is illustrated here.  For a more thorough (and
 * commented) example see test_UKF.  State transformation and
 * measurement functions are listed first, then basic usage of the
 * struct and its functions.     
 * @code
 * // State transformation function
 * void f(const CvMat* x_k, const CvMat* u_k, const CvMat* v_k, CvMat* x_kplus1)
 * {
 *     CvMat* A = cvCreateMat(2, 2, CV_64FC1);
 *  
 *     cvSetReal2D(A, 0, 0, 1.0);
 *     cvSetReal2D(A, 0, 1, 0.1);
 *     cvSetReal2D(A, 1, 0, 0);
 *     cvSetReal2D(A, 1, 1, 0.9);
 *     // A = [1.0 0.1
 *     //      0.0 0.9] 
 *  
 *     double x1 = cvGetReal2D(x_k, 0, 0);
 *
 *     // x_kplus1 = A*x_k + u_k
 *     cvGEMM(A, x_k, 1.0, u_k, 1.0, x_kplus1);
 *
 *     // simple nonlinearity
 *     double z1 = cvGetReal2D(x_kplus1, 0, 0);
 *     cvSetReal2D(x_kplus1, 0, 0, z1 + sin(x1));
 *
 *     // optional disturbance
 *     if(v_k)
 *     {
 *         cvAdd(x_kplus1, v_k, x_kplus1);
 *     }
 *  
 *     cvReleaseMat(&A);
 * }
 *  
 * // Measurement transformation function - simple (optional) additive noise
 * void h(const CvMat* x_k, const CvMat* n_k, CvMat* z_k)
 * {
 *     double z = cvGetReal2D(x_k, 0, 0);
 *     if(n_k)
 *     {
 *         z += cvGetReal2D(n_k, 0, 0);
 *     }
 *     cvSetReal2D(z_k, 0, 0, z);
 * }
 *
 * ...
 *
 * // basic usage: assuming you've set up Q, R, x0, z, u
 *
 * // initialization
 * MT_UKF_struct* UKF = MT_UKF_init(n, m);
 * MT_UKFCopyQR(UKF, Q, R);
 * MT_SetState(UKF, x0);
 *
 * // Prediction step
 * MT_UKFPredict(UKF, &f, &h, u);
 *
 * // could get predicted state with UKF->x1
 *
 * // Set the measurement
 * MT_UKFSetMeasurement(UKF, z);
 *
 * // Correction step
 * MT_UKFCorrect(UKF);
 *
 * // could get the state estimate with UKF->x
 *
 * // Suppose we miss a measurement.
 * MT_UKFPredict(UKF, &f, &h, u);
 * MT_UKFCorrect(UKF);
 * // now UKF->x is a copy of UKF->x1
 * 
 * @endcode
 * 
 */

#ifndef MT_NO_OPENCV

#if defined(MT_HAVE_OPENCV_FRAMEWORK)
#include <OpenCV/OpenCV.h>
#else
#include <cxtypes.h>
#include <cv.h>
#endif

/** Unscented Kalman Filter Structure. */
typedef struct MT_UKF_struct
{
    unsigned int n; /**< number of state elements */
    unsigned int m; /**< number of measurements */

    unsigned int n_q; /**< num of process noise inputs, i.e. size(Q) */
    unsigned int n_r; /**< num of meas unoise inputs, i.e. size(R) */

    unsigned int n_aug; /**< num augmented states = n + n_q + n_r */

    unsigned int have_current_measurement;  /**< Set to 1 by
                                             * MT_UKFSetMeasurement,
                                             * set to 0 during
                                             * MT_UKFCorrect.  If 0,
                                             * MT_UKFCorrect uses the
                                             * current prediction for
                                             * the state estimate
                                             * (i.e. when a
                                             * measurement is missed) */

    double alpha;  /**< Parameter.  Default 1e-3 */
    double k;      /**< Parameter.  Default 0 */
    double beta;   /**< Parameter.  Default 2.0 */
    
    double c;      /* calculated */
    double lambda; /* calculated */

    CvMat* x;      /**< Most recent state estimate.  n x 1 */
    CvMat* z;      /**< Measurement vector. Use MT_UKFSetMeasurement
                    * to set this. n_r x 1 */
    
    CvMat* P;      /**< State cross-covariance matrix.  Initialized to
                    * identity.  n x n */
    CvMat* Q;      /**< Disturbance covariance matrix.  Set using
                    * MT_UKFCopyQR. n_q x n_q */
    CvMat* R;      /**< Measurement noise covariance matrix.  Set
                    * using MT_UKFCopyQR.  n_r x n_r */

    CvMat* x1;     /**< Most recent predicted state estimate.  n x 1
                    * */
    CvMat* K;      /**< Most recent calculated gain matrix.  n x n_r */    

    /* Undocumented members:  all of these get calculated in the code. */
    CvMat* Wm;     /* 1 x 2*n_aug + 1 */
    CvMat* Wc;     /* 2*n_aug+1 x 2*n_aug+1 */

    CvMat* x_aug;  /* n_aug x 1 */

    CvMat* X_aug;  /* n_aug x 2*n_aug+1 */
    CvMat* P_aug;  /* n_aug x n_aug */

    CvMat* X;      /* n x 2*n_aug+1 */
    CvMat* Xv;     /* n_q x 2*n_aug+1 */
    CvMat* Xn;     /* n_r x 2*n_aug+1 */

    CvMat* X1;     /* n x 2*n_aug+1 */
    CvMat* P1;     /* n x n */
    CvMat* X2;     /* n x 2*n_aug+1 */

    CvMat* z1;     /* n_r x 1 */
    CvMat* P2;     /* n_r x n_r */
    CvMat* Z;      /* n_r x 2*n_aug+1 */
    CvMat* Z2;     /* n_r x 2*n_aug+1 */

    CvMat* P12;    /* n x n_r */
    CvMat* tP12;   /* intermediate, 2*n_aug+1 x n_r */

    CvMat* P2inv;  /* intermediate, n_r x n_r */

    CvMat* tPa; /* volatile, size of P_aug */
    CvMat* tPa2;/* volatile, size of P_aug */
    CvMat* tWx; /* volatile, 2*n_aug+1 x n */
    CvMat* tWz; /* volatile, 2*n_aug+1 x n_r */

    CvMat* thQ;  /* volatile header, n_q x n_q */
    CvMat* thR;  /* volatile header, n_r x n_r */
    CvMat* thP;  /* volatile header, n x n */
    CvMat* thx;  /* volatile header, n x 1 */
    CvMat* thx2; /* volatile header, n x 1 */
    CvMat* thxv; /* volatile header, n_q x 1 */
    CvMat* thxn; /* volatile header, n_r x 1 */
    CvMat* thz;  /* volatile header, n_r x 1 */

} MT_UKF_struct;

/** Function pointer type for state transformation.
 * i.e x_{k+1} = f(x_k, u_k, v_k).  x_k and x_kplus1 can be shared,
 * as long as your implementation accounts for this.
 * @param x_k Current state
 * @param u_k Input vector
 * @param v_k Disturbance vector
 * @param x_kplus1 Result */
typedef void(* MT_UKFStateTxfm)(const CvMat* x_k,
                                const CvMat* u_k,
                                const CvMat* v_k,
                                CvMat* x_kplus1);
/** Function pointer type for measurement transformation.
 * i.e. z_k = h(x_k, n_k)
 * @param x_k Current state
 * @param n_k Noise vector
 * @param z_k Result */
typedef void(* MT_UKFMeasTxfm)(const CvMat* x_k, const CvMat* n_k, CvMat* z_k);

/** Copy disturbance and measurement noise covariance matrices.  Also
 * allocates other matrices whose size depends on the size of Q and
 * R.  The matrices are copied, so further changes to Q and R will
 * have no effect until this function is called again.  Subsequent
 * calls will re-allocate matrices as necessary - i.e. if the size of
 * Q or R changes.  */
void MT_UKFCopyQR(MT_UKF_struct* pUKF, CvMat* Q, CvMat* R);

/** Force setting the state of the UKF.  For example, initial
 * conditions.  Checks to make sure the size of the state is
 * correct. */
void MT_UKFSetState(MT_UKF_struct* pUKF, CvMat* x_set);

/** Set the measurement vector.  Also sets have_current_measurement
 * to 1.  If this is not called between calls to MT_UKFCorrect, the
 * estimated state (x1) is used and the covariance is not updated. */
void MT_UKFSetMeasurement(MT_UKF_struct* pUKF, CvMat* z_set);

/** Prediction step.  Shortcut for
 * @code
 * MT_UKFAugment(pUKF);
 * MT_UKFUTState(pUKF, u, f);
 * MT_UKFUTMeas(pUKF, h);
 * @endcode
 */
void MT_UKFPredict(MT_UKF_struct* pUKF,
                   MT_UKFStateTxfm f,
                   MT_UKFMeasTxfm h,
                   CvMat* u = NULL);

/** Correction step.  If have_current_measurement is 0 (i.e. if
 * MT_UKFSetMeasurement was not called), uses the value last
 * predicted by MT_UKFUTState.  Sets have_current_measurement to 0. */
void MT_UKFCorrect(MT_UKF_struct* pUKF);

/** Generate the augmented state and sigma points. */
void MT_UKFAugment(MT_UKF_struct* pUKF);
/** Unscented state transformation. */
void MT_UKFUTState(MT_UKF_struct* pUKF, CvMat* u, MT_UKFStateTxfm f);
/** Unscented measurement transformation. */
void MT_UKFUTMeas(MT_UKF_struct* pUKF, MT_UKFMeasTxfm h);

/** Creates an MT_UKF_struct and returns a pointer to the created
 * struct. Allocates as many of the matrices as possible, though most
 * are allocated by MT_UKFCopyQR. */
MT_UKF_struct* MT_UKFInit(unsigned int n_states,
                          unsigned int n_meas,
                          double alpha = 0.001,
                          double k = 0,
                          double beta = 2.0);

/** Deallocate a MT_UKF_struct.  Safely calls cvReleaseMat on all of
 * the matrices pointed to by MT_UKF_struct then frees the struct. */
void MT_UKFFree(MT_UKF_struct** pUKF);

#endif /* !MT_NO_OPENCV */

/** @} */

#endif /* UKF_H */
