#include "MT/MT_Core/support/OpenCVmath.h"

static void disp_mat(const CvMat* M, const char* name)
{
    printf("Matrix %s ", name);
    for(unsigned int i = 0; i < 35 - strlen(name); i++)
    {
        printf("-");
    }
    printf("\n");
    for(unsigned int i = 0; i < M->rows; i++)
    {
        for(unsigned int j = 0; j < M->cols; j++)
        {
            printf("%4.3f\t", cvGetReal2D(M, i, j));
        }
        printf("\n");
    }
}

int main(int argc, char** argv)
{
    
    CvMat* M = cvCreateMat(3, 3, CV_64FC1);

    cvSetIdentity(M);

    CvMat* L = MT_CreateCholeskyResult(M);

    MT_Cholesky(M, L);

    disp_mat(M, "M");
    disp_mat(L, "L = chol(M)");

    cvmSet(M, 1, 1, 4.0);
    cvmSet(M, 2, 2, 9.0);

    MT_Cholesky(M, L);

    disp_mat(M, "M");
    disp_mat(L, "L = chol(M)");

    cvmSet(M, 0, 1, 0.1);
    cvmSet(M, 0, 2, 0.1);
    cvmSet(M, 1, 0, 0.1);
    cvmSet(M, 1, 2, 0.1);
    cvmSet(M, 2, 0, 0.1);
    cvmSet(M, 2, 1, 0.1);

    MT_Cholesky(M, L);

    disp_mat(M, "M");
    disp_mat(L, "L = chol(M)");

    cvReleaseMat(&M);
    cvReleaseMat(&L);

    M = cvCreateMat(5, 5, CV_64FC1);
    cvSet(M, cvScalar(0.1));

    for(unsigned int i = 0; i < 5; i++)
    {
        cvmSet(M, i, i, (double) (i+1));
    }

    L = MT_CreateCholeskyResult(M);

    MT_Cholesky(M, L);

    disp_mat(M, "M");
    disp_mat(L, "L = chol(M)");

    cvReleaseMat(&M);
    cvReleaseMat(&L);


    CvMat* C = cvCreateMat(4, 4, CV_64FC1);
    CvMat* A = cvCreateMatHeader(2, 2, CV_64FC1);
    CvMat* B = cvCreateMatHeader(2, 2, CV_64FC1);

    cvGetSubRect(C, A, cvRect(0, 0, 2, 2));
    cvGetSubRect(C, B, cvRect(2, 2, 2, 2));

    cvSet(C, cvScalar(0));
    cvSet(A, cvScalar(1));
    cvSet(B, cvScalar(2));

    cvReleaseMat(&A);
    cvReleaseMat(&B);

    disp_mat(C, "C (composited)");

    cvReleaseMat(&C);    
        
}
