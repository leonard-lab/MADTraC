#include "MT/MT_Core/support/OpenCVmath.h"
#include "MT/MT_Core/support/UKF.h"

/* example of a state transformation function.  This can be as
 * arbitrarily complex as desired. The only limitation is that it
 * can't be a member function of a class... to pass configuration
 * parameters you'll need to use a global variable.  This could be
 * changed in the future, e.g. a UKF->parameters matrix or the
 * like... */
void f(const CvMat* x_k, const CvMat* u_k, const CvMat* v_k, CvMat* x_kplus1)
{
    /* note, not doing any size checking here... it probably should be
     * done, but if we set everything up properly it isn't necessary */

    CvMat* A = cvCreateMat(2, 2, CV_64FC1);

    cvSetReal2D(A, 0, 0, 1.0);
    cvSetReal2D(A, 0, 1, 0.1);
    cvSetReal2D(A, 1, 0, 0);
    cvSetReal2D(A, 1, 1, 0.9);
    /* A = [1.0 0.1
     *      0.0 0.9] */

    /* x1 = first element of x_k
     *  (note, grab this BEFORE the call to cvGEMM,
     *   it's possible that x_kplus1 and x_k are the same pointer,
     *   i.e. if you've called f(x, u, NULL, x) to set x to a new
     *   value  */
    double x1 = cvGetReal2D(x_k, 0, 0);

    /* does x_kplus1 = 1.0*A*x_k + 1.0*u_k */
    cvGEMM(A, x_k, 1.0, u_k, 1.0, x_kplus1);

    /* z1 = first element of x_kplus1 */
    double z1 = cvGetReal2D(x_kplus1, 0, 0);

    /* a simple dummy nonlinear transformation... sets
     * the first element of x_kplus1 = z1 + sin(x1) */
    cvSetReal2D(x_kplus1, 0, 0, z1 + sin(x1));

    /* if there is no noise, we can just set v_k to NULL */
    if(v_k)
    {
        /* additive noise here */
        cvAdd(x_kplus1, v_k, x_kplus1);
    }

    /* free memory allocated for the transition matrix.  this is
     * important b/c we call this function often - so if we don't
     * release the memory we'll create a memory leak */
    cvReleaseMat(&A);
}

/* example of a measurement function.  see comments for f */
void h(const CvMat* x_k, const CvMat* n_k, CvMat* z_k)
{
    /* z = first element of x_k */
    double z = cvGetReal2D(x_k, 0, 0);

    /* simple additive noise - can be skipped if n_k is NULL */
    if(n_k)
    {
        z += cvGetReal2D(n_k, 0, 0);
    }

    /* sets the first element of z_k to z - note that even though z_k
     * is a 1x1 matrix, we can't just set it like z_k = z */
    cvSetReal2D(z_k, 0, 0, z);

}

int main(int argc, char** argv)
{

    /* number of states */
    int n = 2;
    /* number of measurements */
    int m = 1;

    /* timing variables */
    double t = 0;
    double dt = 0.05;
    unsigned int Nt = 100;    

    /* create the UKF structure, which keeps track of persistent
     * variables and maintains allocated memory so we don't have to
     * re-allocate every time (which would be slow) */
    MT_UKF_struct* UKF = MT_UKFInit(n, m);

    /* allocate memory for "external" variables */
    CvMat* Q = cvCreateMat(n, n, CV_64FC1);
    CvMat* R = cvCreateMat(m, m, CV_64FC1);
    CvMat* x0 = cvCreateMat(n, 1, CV_64FC1);
    CvMat* z = cvCreateMat(m, 1, CV_64FC1);
    CvMat* u = cvCreateMat(n, 1, CV_64FC1);
    
    /* we're going to keep track of the actual value of x for
     * simulation purposes only */
    CvMat* x_act = cvCreateMat(n, 1, CV_64FC1);    

    /* set Q and R to identity matrices - they could really be any
     * positive definite matrix of the correct size */
    cvSetIdentity(Q);
    cvSetIdentity(R);

    /* initial conditions [0; 1] */
    cvZero(x0);
    cvSetReal2D(x0, 1, 0, 1.0);

    /* set the actual state value */
    cvCopy(x0, x_act);

    /* zero input at first */
    cvZero(u);

    /* file for test output */
    FILE* tf = fopen("test.dat", "w");

    /* copy the Q and R matrices to the UKF struct, this also does a
     * bit of other set up - including allocating most of the extra
     * persistent variables, since their sizes depend upon the sizes of Q
     * and R.  This way, we can call MT_UKFCopyQR at any time and resize
     * everything appropriately */
    MT_UKFCopyQR(UKF, Q, R);

    /* set the initial condition of the state */
    MT_UKFSetState(UKF, x0);

    /* the first time through, we'll do everything step-by-step just
     * illustrate.  Really we can do MT_UKFPredict, which is the same
     * as MT_UKFAugment, then MT_UKFUTState, then MT_UKFUTMeas */

    /* Augmentation step - calculates augmented state and sigma
     * points */
    MT_UKFAugment(UKF);

    /* Unscented transform of the state - notice we use a function
     * pointer to the transformation function f */
    MT_UKFUTState(UKF, u, &f);

    /* Unscented transformation of the measurement - similar to the
     * state version except we give a function pointer to h instead
     * of f */
    MT_UKFUTMeas(UKF, &h);

    /* calculate the actual state and the measurement due to the
     * actual state - this is for simulation purposes only - normally
     * x_act would be unknown and the mesurement would come from
     * hardware, etc. */
    f(x_act, u, NULL, x_act);
    h(x_act, NULL, z);

    /* Set the measurement - if this is skipped, (i.e. if a
     * measurement is missed) then the correction
     * step will use the predicted measurement value, which is
     * obtained by MT_UKFUTState (or MT_UKFPredict) */
    MT_UKFSetMeasurement(UKF, z);

    /* Correction step - updates the state estimate and P matrix */
    MT_UKFCorrect(UKF);

    /* output x0 and the first estimated state to our data file */
    MT_Display_CvMat(x0,     /* pointer to the matrix    */
                     NULL,   /* label (or NULL for none) */
                     tf);    /* pointer to file (omit for stdout) */
    MT_Display_CvMat(UKF->x, NULL, tf);

    /* the rest of the steps we'll do in a manner that better matches
     * "normal" usage */
    for(unsigned int i = 0; i < Nt-2; i++)  /* Nt-2 -> already have x0
                                               and first state */
    {
        /* update timing */
        t += dt;
        /* simple control signal */
        if(t > 0.2)
        {   /* u(1) = 1 if t > 0.2 */
            cvSetReal2D(u, 0, 0, 1.0);
        }
        if(t > 0.15)
        {   /* u(2) = -1 if t > 0.15 */
            cvSetReal2D(u, 1, 0, -1.0);
        }

        /* Predict step - based on current input */
        MT_UKFPredict(UKF, &f, &h, u);

        /* simulation to obtain current actual state and measurement
         * - note no noise or disturbance is used right now */
        f(x_act, u, NULL, x_act);
        h(x_act, NULL, z);

        /* Copy the measurement */
        MT_UKFSetMeasurement(UKF, z);

        /* Correction step */
        MT_UKFCorrect(UKF);

        /* output to file */
        MT_Display_CvMat(UKF->x, NULL, tf);
    }

    /* close the data file */
    fclose(tf);

    /* releases memory allocated by the UKF and frees the memory
     * allocated for the struct */
    MT_UKFFree(&UKF);

    /* free memory we've allocated for external variables */
    cvReleaseMat(&Q);
    cvReleaseMat(&R);
    cvReleaseMat(&u);
    cvReleaseMat(&x0);
    cvReleaseMat(&z);
    cvReleaseMat(&x_act);
                  
}
