#include "UKF.h"
#include "OpenCVmath.h"

#ifndef MT_NO_OPENCV

static void generate_sigma_points(const CvMat* x_aug,
                                  const CvMat* P_aug,
                                  double c,
                                  CvMat* tmp_A,
                                  CvMat* tmp_Y,
                                  CvMat* X_sigma);
static void alloc_CvMat_if_necessary(CvMat** M,
                                     unsigned int rows,
                                     unsigned int cols);
static void alloc_CvMatHeader_if_necessary(CvMat** M,
                                           unsigned int rows,
                                           unsigned int cols);

static void alloc_CvMat_if_necessary(CvMat** M,
                                     unsigned int rows,
                                     unsigned int cols)
{
    if(*M)
    {
        if(((*M)->rows != rows) || ((*M)->cols != cols))
        {
            cvReleaseMat(M);
            *M = NULL;
        }
    }
    if(!(*M))
    {
        *M = cvCreateMat(rows, cols, CV_64FC1);
    }
}

static void alloc_CvMatHeader_if_necessary(CvMat** M,
                                           unsigned int rows,
                                           unsigned int cols)
{
    if(*M)
    {
        if(((*M)->rows != rows) || ((*M)->cols != cols))
        {
            cvReleaseMat(M);
            *M = NULL;
        }
    }
    if(!(*M))
    {
        *M = cvCreateMatHeader(rows, cols, CV_64FC1);
    }    
}

static void generate_sigma_points(const CvMat* x_aug,
                                  const CvMat* P_aug,
                                  double c,
                                  CvMat* tmp_A,
                                  CvMat* tmp_Y,
                                  CvMat* X_sigma)
{
    unsigned int n_aug = x_aug->rows;

    if(!P_aug || (P_aug->rows != n_aug) || (P_aug->cols != n_aug))
    {
        fprintf(stderr,
                "UKF Error:  P_aug is not the right size or is uninitialized in "
                "generate_sigma_points\n");
        return;
    }

    if(c <= 0)
    {
        fprintf(stderr,
                "UKF Error:  c must be > 0 in generate_sigma_points\n");
        return;
    }

    if(!tmp_A || (tmp_A->rows != n_aug) || (tmp_A->cols != n_aug))
    {
        fprintf(stderr,
                "UKF Error:  tmp_A is not the right size or is uninitialized "
                "in generate_sigma_points\n");
        return;
    }

    if(!tmp_Y || (tmp_Y->rows != n_aug) || (tmp_Y->cols != n_aug))
    {
        fprintf(stderr,
                "UKF Error:  tmp_Y is not the right size or is uninitialized "
                "in generate_sigma_points\n");
        return;
    }
    
    /* A = chol(P)' */
    MT_Cholesky(P_aug, tmp_A, MT_CHOLESKY_LOWER_TRIANGULAR);
    /* A = c*A = c*chol(P)' */
    cvScale(tmp_A, tmp_A, sqrt(c));

    /* Y = repmat(x, [1 numel(x)]) */
    cvRepeat(x_aug, tmp_Y);

    /* X = [x Y+A Y-A] */
    CvMat* B1, *B2, *B3;
    B1 = cvCreateMatHeader(n_aug, 1, CV_64FC1);
    B2 = cvCreateMatHeader(n_aug, n_aug, CV_64FC1);
    B3 = cvCreateMatHeader(n_aug, n_aug, CV_64FC1);
    cvGetSubRect(X_sigma, B1, cvRect(0, 0, 1, n_aug));
    cvGetSubRect(X_sigma, B2, cvRect(1, 0, n_aug, n_aug));
    cvGetSubRect(X_sigma, B3, cvRect(n_aug+1, 0, n_aug, n_aug));

    cvCopy(x_aug, B1);
    cvAdd(tmp_Y, tmp_A, B2);
    cvSub(tmp_Y, tmp_A, B3);

    cvReleaseMat(&B1);
    cvReleaseMat(&B2);
    cvReleaseMat(&B3);    
} 

MT_UKF_struct* MT_UKFInit(unsigned int n_states,
                          unsigned int n_meas,
                          double alpha,
                          double k,
                          double beta)
{
    MT_UKF_struct* pUKF = (MT_UKF_struct *)malloc(sizeof(MT_UKF_struct));

    pUKF->n = n_states;
    pUKF->m = n_meas;

    pUKF->have_current_measurement = 0;

    pUKF->n_aug = 0;
    pUKF->Q = NULL;
    pUKF->R = NULL;

    pUKF->alpha = alpha;
    pUKF->k = k;
    pUKF->beta = beta;
    pUKF->c = 0;
    pUKF->lambda = 0;
    
    pUKF->x = cvCreateMat(pUKF->n, 1, CV_64FC1);
    cvZero(pUKF->x);
    pUKF->z = cvCreateMat(pUKF->m, 1, CV_64FC1);
    cvZero(pUKF->z);
    pUKF->P = cvCreateMat(pUKF->n, pUKF->n, CV_64FC1);
    cvSetIdentity(pUKF->P);
    
    pUKF->Q = NULL;
    pUKF->R = NULL;    

    pUKF->Wm = NULL;
    pUKF->Wc = NULL;    

    pUKF->x_aug = NULL;

    pUKF->X_aug = NULL;    
    pUKF->P_aug = NULL;

    pUKF->X = NULL;    
    pUKF->Xv = NULL;
    pUKF->Xn = NULL;

    pUKF->x1 = NULL;
    pUKF->X1 = NULL;
    pUKF->P1 = NULL;
    pUKF->X2 = NULL;

    pUKF->z1 = NULL;
    pUKF->P2 = NULL;
    pUKF->Z = NULL;
    pUKF->Z2 = NULL;

    pUKF->P12 = NULL;
    pUKF->tP12 = NULL;
    
    pUKF->P2inv = NULL;

    pUKF->K = NULL;
    
    alloc_CvMatHeader_if_necessary(&(pUKF->thP), pUKF->n, pUKF->n);
    alloc_CvMatHeader_if_necessary(&(pUKF->thx), pUKF->n, 1);
    alloc_CvMatHeader_if_necessary(&(pUKF->thx2), pUKF->n, 1);
    
    return pUKF;
}

void MT_UKFCopyQR(MT_UKF_struct* pUKF, CvMat* Q, CvMat* R)
{
    pUKF->n_q = Q->rows;
    pUKF->n_r = R->rows;

    pUKF->n_aug = pUKF->n + pUKF->n_q + pUKF->n_r;

    /* allocate a new Q if necessary, then copy */
    alloc_CvMat_if_necessary(&(pUKF->Q), Q->rows, Q->cols);
    cvCopy(Q, pUKF->Q);
    
    /* allocate a new R if necessary, then copy */
    alloc_CvMat_if_necessary(&(pUKF->R), R->rows, R->cols);
    cvCopy(R, pUKF->R);

    alloc_CvMatHeader_if_necessary(&(pUKF->thQ), pUKF->n_q, pUKF->n_q);
    alloc_CvMatHeader_if_necessary(&(pUKF->thR), pUKF->n_r, pUKF->n_r);
    alloc_CvMatHeader_if_necessary(&(pUKF->thxv), pUKF->n_q, 1);
    alloc_CvMatHeader_if_necessary(&(pUKF->thxn), pUKF->n_r, 1);
    alloc_CvMatHeader_if_necessary(&(pUKF->thz), pUKF->n_r, 1);

    alloc_CvMat_if_necessary((&pUKF->z), pUKF->n_r, 1);

    /* allocate augmented state and P */
    alloc_CvMat_if_necessary(&(pUKF->x_aug), pUKF->n_aug, 1);
    cvZero(pUKF->x_aug);

    alloc_CvMat_if_necessary(&(pUKF->Wm), 1, 2*(pUKF->n_aug)+1);
    pUKF->lambda = (pUKF->alpha)*(pUKF->alpha)*
        ((double) (pUKF->n_aug) + pUKF->k) - pUKF->n_aug;
    double lambda = pUKF->lambda;
    pUKF->c = lambda + (double) (pUKF->n_aug);
    double c = pUKF->c;
    cvSet(pUKF->Wm, cvScalar(0.5/c));
    cvSetReal2D(pUKF->Wm, 0, 0, lambda/c);

    alloc_CvMat_if_necessary(&(pUKF->tWx), 2*(pUKF->n_aug)+1, pUKF->n);
    alloc_CvMat_if_necessary(&(pUKF->tWz), 2*(pUKF->n_aug)+1, pUKF->n_r);
    
    alloc_CvMat_if_necessary(&(pUKF->Wc), 2*(pUKF->n_aug)+1, 2*(pUKF->n_aug)+1);
    cvSetReal2D(pUKF->Wc,
                0,
                0,
                lambda/c + 1 - (pUKF->alpha)*(pUKF->alpha) + pUKF->beta);
    for(unsigned int i = 1; i < 2*(pUKF->n_aug)+1; i++)
    {
        cvSetReal2D(pUKF->Wc, i, i, 0.5/c);
    }

    alloc_CvMat_if_necessary(&(pUKF->P_aug), pUKF->n_aug, pUKF->n_aug);
    alloc_CvMat_if_necessary(&(pUKF->tPa), pUKF->n_aug, pUKF->n_aug);
    alloc_CvMat_if_necessary(&(pUKF->tPa2), pUKF->n_aug, pUKF->n_aug);
    
    alloc_CvMat_if_necessary(&(pUKF->X_aug), pUKF->n_aug, 2*(pUKF->n_aug)+1);
    cvZero(pUKF->P_aug);

    alloc_CvMat_if_necessary(&(pUKF->X), pUKF->n, 2*(pUKF->n_aug)+1);
    alloc_CvMat_if_necessary(&(pUKF->Xv), pUKF->n_q, 2*(pUKF->n_aug)+1);
    alloc_CvMat_if_necessary(&(pUKF->Xn), pUKF->n_r, 2*(pUKF->n_aug)+1);

    alloc_CvMat_if_necessary(&(pUKF->x1), pUKF->n, 1);
    alloc_CvMat_if_necessary(&(pUKF->X1), pUKF->n, 2*(pUKF->n_aug)+1);
    alloc_CvMat_if_necessary(&(pUKF->P1), pUKF->n, pUKF->n);
    alloc_CvMat_if_necessary(&(pUKF->X2), pUKF->n, 2*(pUKF->n_aug)+1);

    cvGetSubRect(pUKF->P_aug,
                 pUKF->thP,
                 cvRect(0, 0, pUKF->n, pUKF->n));
    cvGetSubRect(pUKF->P_aug,
                 pUKF->thQ,
                 cvRect(pUKF->n, pUKF->n, pUKF->n_q, pUKF->n_q));
    cvGetSubRect(pUKF->P_aug,
                 pUKF->thR,
                 cvRect(pUKF->n + pUKF->n_q, pUKF->n + pUKF->n_q,
                        pUKF->n_r, pUKF->n_r));
    
    cvCopy(pUKF->P, pUKF->thP);
    cvCopy(pUKF->Q, pUKF->thQ);
    cvCopy(pUKF->R, pUKF->thR);

    alloc_CvMat_if_necessary(&(pUKF->z1), pUKF->n_r, 1);
    alloc_CvMat_if_necessary(&(pUKF->Z2), pUKF->n_r, 2*(pUKF->n_aug)+1);
    alloc_CvMat_if_necessary(&(pUKF->Z), pUKF->n_r, 2*(pUKF->n_aug)+1);
    alloc_CvMat_if_necessary(&(pUKF->P2), pUKF->n_r, pUKF->n_r);

    alloc_CvMat_if_necessary(&(pUKF->P12), pUKF->n, pUKF->n_r);
    alloc_CvMat_if_necessary(&(pUKF->tP12), 2*(pUKF->n_aug)+1, pUKF->n_r);

    alloc_CvMat_if_necessary(&(pUKF->P2inv), pUKF->n_r, pUKF->n_r);
    alloc_CvMat_if_necessary(&(pUKF->K), pUKF->n, pUKF->n_r);

}

void MT_UKFSetState(MT_UKF_struct* pUKF, CvMat* x_set)
{
    if(!x_set || (x_set->rows != pUKF->n) || (x_set->cols != 1))
    {
        fprintf(stderr,
                "MT_UKFSetState Error:  x_set is the wrong "
                "size or not initialized\n");
        return;
    }

    cvCopy(x_set, pUKF->x);
}

void MT_UKFSetMeasurement(MT_UKF_struct* pUKF, CvMat* z_set)
{
    if(!z_set || (z_set->rows != pUKF->m) || (z_set->cols != 1))
    {
        fprintf(stderr,
                "MT_UKFSetMeasurement Error:  z_set is "
                "the wrong size or not initialized\n");
        return;
    }

    cvCopy(z_set, pUKF->z);
    pUKF->have_current_measurement = 1;
}

void MT_UKFPredict(MT_UKF_struct* pUKF,
                   MT_UKFStateTxfm f,
                   MT_UKFMeasTxfm h,
                   CvMat* u)
{
    MT_UKFAugment(pUKF);
    MT_UKFUTState(pUKF, u, f);
    MT_UKFUTMeas(pUKF, h);
}

void MT_UKFAugment(MT_UKF_struct* pUKF)
{
    if(!pUKF->Q || !pUKF->R || !pUKF->x_aug || !pUKF->P_aug)
    {
        fprintf(stderr,
                "MT_UKFAugment Error:  Q, R, x_aug, or P_aug "
                "not initialized.  Call MT_UKFCopyQR.\n");
    }

    cvGetSubRect(pUKF->P_aug,
                 pUKF->thP,
                 cvRect(0, 0, pUKF->n, pUKF->n));
    cvCopy(pUKF->P, pUKF->thP);

    cvZero(pUKF->x_aug);

    cvGetSubRect(pUKF->x_aug,
                 pUKF->thx,
                 cvRect(0, 0, 1, pUKF->n));
    cvCopy(pUKF->x, pUKF->thx);
    
    generate_sigma_points(pUKF->x_aug,
                          pUKF->P_aug,
                          pUKF->c,
                          pUKF->tPa,
                          pUKF->tPa2,
                          pUKF->X_aug);

    int w = 2*(pUKF->n_aug)+1;
    cvGetSubRect(pUKF->X_aug,
                 pUKF->X,
                 cvRect(0, 0, w, pUKF->n));
    cvGetSubRect(pUKF->X_aug,
                 pUKF->Xv,
                 cvRect(0, pUKF->n, w, pUKF->n_q));
    cvGetSubRect(pUKF->X_aug,
                 pUKF->Xn,
                 cvRect(0, pUKF->n+pUKF->n_q, w, pUKF->n_r));
}

void MT_UKFUTState(MT_UKF_struct* pUKF, CvMat* u, MT_UKFStateTxfm f)
{
    cvZero(pUKF->x1);
    cvZero(pUKF->X1);

    for(unsigned int i = 0; i < pUKF->X1->cols; i++)
    {
        cvGetSubRect(pUKF->X, pUKF->thx, cvRect(i, 0, 1, pUKF->n));
        cvGetSubRect(pUKF->Xv, pUKF->thxv, cvRect(i, 0, 1, pUKF->n_q));
        cvGetSubRect(pUKF->X1, pUKF->thx2, cvRect(i, 0, 1, pUKF->n));
        f(pUKF->thx, u, pUKF->thxv, pUKF->thx2);

        /* y=y+Wm(k)*Y(:,k); */
        cvAddWeighted(pUKF->x1,
                      1.0,
                      pUKF->thx2,
                      cvGetReal2D(pUKF->Wm, 0, i),
                      0,
                      pUKF->x1);
    }

    for(unsigned int i = 0; i < pUKF->X1->cols; i++)
    {
        cvGetSubRect(pUKF->X2, pUKF->thx, cvRect(i, 0, 1, pUKF->n));
        cvGetSubRect(pUKF->X1, pUKF->thx2, cvRect(i, 0, 1, pUKF->n));

        cvSub(pUKF->thx2, pUKF->x1, pUKF->thx);
    }

    MT_CVQuadraticMul(pUKF->X2, pUKF->Wc, pUKF->P1, true, pUKF->tWx);

}

void MT_UKFUTMeas(MT_UKF_struct* pUKF, MT_UKFMeasTxfm h)
{
    cvZero(pUKF->z1);
    cvZero(pUKF->Z2);

    for(unsigned int i = 0; i < pUKF->Z2->cols; i++)
    {
        cvGetSubRect(pUKF->Xn, pUKF->thxn, cvRect(i, 0, 1, pUKF->n_r));
        cvGetSubRect(pUKF->X1, pUKF->thx, cvRect(i, 0, 1, pUKF->n));
        cvGetSubRect(pUKF->Z, pUKF->thz, cvRect(i, 0, 1, pUKF->n_r));

        h(pUKF->thx, pUKF->thxn, pUKF->thz);

        cvAddWeighted(pUKF->z1,
                      1.0,
                      pUKF->thz,
                      cvGetReal2D(pUKF->Wm, 0, i),
                      0,
                      pUKF->z1);
    }

    for(unsigned int i = 0; i < pUKF->X1->cols; i++)
    {
        cvGetSubRect(pUKF->Z2, pUKF->thxn, cvRect(i, 0, 1, pUKF->n_r));
        cvGetSubRect(pUKF->Z, pUKF->thz, cvRect(i, 0, 1, pUKF->n_r));

        cvSub(pUKF->thz, pUKF->z1, pUKF->thxn);
    }

    MT_CVQuadraticMul(pUKF->Z2, pUKF->Wc, pUKF->P2, true, pUKF->tWz);
    
}

void MT_UKFCorrect(MT_UKF_struct* pUKF)
{
    if(!pUKF->have_current_measurement)
    {
        cvCopy(pUKF->x1, pUKF->x);
        return;
    }

    pUKF->have_current_measurement = 0;
    
    /* P12 = X2*Wc*Z2' */
    /* tP12 = Wc*Z2' */
    cvGEMM(pUKF->Wc, pUKF->Z2, 1.0, NULL, 0, pUKF->tP12, CV_GEMM_B_T);
    /* P12 = X2*tP12 */
    cvMatMul(pUKF->X2, pUKF->tP12, pUKF->P12);

    /* K = P12*inv(P2) */
    cvInvert(pUKF->P2, pUKF->P2inv, CV_SVD_SYM); /* SVD method for
                                                  * symmetric PD
                                                  * matrices */
    cvMatMul(pUKF->P12, pUKF->P2inv, pUKF->K);

    /* x = x1 + K*(z - z1) */
    /* z1 = z - z1 */
    cvSub(pUKF->z, pUKF->z1, pUKF->z1);
    /* x = 1.0*(K*(z-z1 -> z1)) + 1.0*x1 */
    cvGEMM(pUKF->K,
           pUKF->z1,
           1.0,
           pUKF->x1,
           1.0,
           pUKF->x,
           0 /* no transposes */);

    /* P = P1 - P12*K' = -1.0*P12*K' + 1.0*P1 */
    cvGEMM(pUKF->P12,
           pUKF->K,
           -1.0,
           pUKF->P1,
           1.0,
           pUKF->P,
           CV_GEMM_B_T);
}

#define SAFE_RELEASE(M) if(M){cvReleaseMat(&M);};
#define UKF_SAFE_RELEASE(M) if((*pUKF)->M){cvReleaseMat(&((*pUKF)->M));};

void MT_UKFFree(MT_UKF_struct** pUKF)
{
    if(*pUKF)
    {
        UKF_SAFE_RELEASE(x);     
        UKF_SAFE_RELEASE(z);     
        UKF_SAFE_RELEASE(P);     
        UKF_SAFE_RELEASE(Q);     
        UKF_SAFE_RELEASE(R);     
        UKF_SAFE_RELEASE(Wm);    
        UKF_SAFE_RELEASE(Wc);    
        UKF_SAFE_RELEASE(x_aug); 
        UKF_SAFE_RELEASE(X_aug); 
        UKF_SAFE_RELEASE(P_aug); 
        UKF_SAFE_RELEASE(X);     
        UKF_SAFE_RELEASE(Xv);    
        UKF_SAFE_RELEASE(Xn);    
        UKF_SAFE_RELEASE(x1);    
        UKF_SAFE_RELEASE(X1);    
        UKF_SAFE_RELEASE(P1);    
        UKF_SAFE_RELEASE(X2);    
        UKF_SAFE_RELEASE(z1);    
        UKF_SAFE_RELEASE(P2);    
        UKF_SAFE_RELEASE(Z);     
        UKF_SAFE_RELEASE(Z2);    
        UKF_SAFE_RELEASE(P12);   
        UKF_SAFE_RELEASE(tP12);
        UKF_SAFE_RELEASE(P2inv); 
        UKF_SAFE_RELEASE(K);   
        UKF_SAFE_RELEASE(tPa); 
        UKF_SAFE_RELEASE(tPa2);
        UKF_SAFE_RELEASE(tWx);
        UKF_SAFE_RELEASE(tWz);         
        UKF_SAFE_RELEASE(thQ); 
        UKF_SAFE_RELEASE(thR); 
        UKF_SAFE_RELEASE(thP); 
        UKF_SAFE_RELEASE(thx); 
        UKF_SAFE_RELEASE(thx2);
        UKF_SAFE_RELEASE(thxv);
        UKF_SAFE_RELEASE(thxn);
        UKF_SAFE_RELEASE(thz); 
        
        free(*pUKF);
        *pUKF = NULL;
    }
}

#endif // !MT_NO_OPENCV
