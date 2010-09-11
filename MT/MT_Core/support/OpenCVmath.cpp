#include "OpenCVmath.h"

#ifndef MT_NO_OPENCV

static bool check_size_type(const CvMat* src, const char* name)
{
    if(src->rows != src->cols)
    {
        fprintf(stderr, "MT_Cholesky Error:  %s must be a square matrix.\n",
                name);
        return false;
    }

    if(cvGetElemType(src) != CV_64FC1)
    {
        fprintf(stderr,
                "MT_Cholesky Error:  %s must be a single-channel "
                "matrix of doubles.\n",
                name);
        return false;
    }

    return true;
}

void MT_Display_CvMat(const CvMat* M, const char* name, FILE* f)
{
    if(name)
    {
        fprintf(f, "Matrix %s ", name);
        for(unsigned int i = 0; i < 35 - strlen(name); i++)
        {
            fprintf(f, "-");
        }
        fprintf(f, "\n");
    }

    if(!M && name)
    {
        fprintf(f, "   (uninitialized)\n");
        return;
    }
    
    for(unsigned int i = 0; i < M->rows; i++)
    {
        for(unsigned int j = 0; j < M->cols; j++)
        {
            fprintf(f, "%4.3f\t", cvGetReal2D(M, i, j));
        }
        fprintf(f, "\n");
    }
}

CvMat* MT_CreateCholeskyResult(const CvMat* src)
{
    if(!check_size_type(src, "Source"))
    {
        return NULL;
    }

    return cvCreateMat(src->rows, src->cols, CV_64FC1);

}

void MT_CVQuadraticMul(const CvMat* X,
                       const CvMat* W,
                       CvMat* dst,
                       bool transpose_X,
                       CvMat* tmp_prod)
{
    bool own_prod = (tmp_prod == NULL);

    if(own_prod)
    {
        tmp_prod = cvCreateMat(W->rows, X->cols, cvGetElemType(X));
    }

    cvGEMM(W, X, 1.0, NULL, 1.0, tmp_prod, transpose_X ? CV_GEMM_B_T : 0);
    cvGEMM(X, tmp_prod, 1.0, NULL, 1.0, dst, transpose_X ? 0 : CV_GEMM_A_T);

    if(own_prod)
    {
        cvReleaseMat(&tmp_prod);
    }
}

bool MT_Cholesky(const CvMat* src, CvMat* dst, unsigned int orientation)
{

    if(!check_size_type(src, "Input"))
    {
        return false;
    }

    if(!dst)
    {
        fprintf(stderr, "MT_Cholesky Error:  Output is not allocated.\n");
        return false;
    }
    else
    {
        if((dst->rows != src->rows) || (dst->cols != src->cols) ||
           (cvGetElemType(dst) != cvGetElemType(src)))
        {
            fprintf(stderr,
                    "MT_Cholesky Error:  Output matrix must be same size"
                    " and type as input.\n");
            return false;
        }
    }

    if((orientation != MT_CHOLESKY_UPPER_TRIANGULAR) &&
       (orientation != MT_CHOLESKY_LOWER_TRIANGULAR))
    {
        fprintf(stderr,
                "MT_Cholesky Error:  Orientation must be either "
                "MT_CHOLESKY_UPPER_TRIANGULAR (%d) or "
                "MT_CHOLESKY_LOWER_TRIANGULAR (%d).\n",
                MT_CHOLESKY_UPPER_TRIANGULAR,
                MT_CHOLESKY_LOWER_TRIANGULAR);
        return false;
    }

    double p;
    unsigned int R = src->rows;
    cvSet(dst, cvScalar(0));
    double* src_data = src->data.db;
    double* dst_data = dst->data.db;
  
    for(int i = 0; i < R; i++)
    {
        for(int j = 0; j < R; j++)
        {
            if(i < j)
            {
                continue;
            }
      
            p = 0;
      
            if(i==j)
            {
                p = src_data[i*R + i];
                for(int k = 0; k <= j-1; k++)
                {
                    p -= dst_data[i*R+k]*dst_data[i*R+k];
                }
                if( p < 0 ){     // NOTE these may not be numerically correct, but it is a safeguard
                    p = fabs(p);
                }
                if( p == 0 ){
                    p = 1e-6;
                }
                dst_data[i*R+i] = sqrt(p);
            } else {
                p = src_data[i*R+j];
                for(int k = 0; k <= j-1; k++)
                {
                    p -= dst_data[i*R+k]*dst_data[j*R+k];
                }
                dst_data[i*R+j] = p/dst_data[j*R+j];
            }
      
        }
    }

    if(orientation == MT_CHOLESKY_UPPER_TRIANGULAR)
    {
        cvTranspose(dst, dst);
    }
  
    return true;

}

#endif /* !MT_NO_OPENCV */
