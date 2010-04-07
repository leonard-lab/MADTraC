#ifndef MATRIX_H
#define MATRIX_H

/*
 *  Matrix.h
 *
 *  Created by Daniel Swain on 10/31/08.
 *
 *  Define Vector and Matrix types that are template based.
 *  That is, these are not dynamically allocated, rather they
 *  are classes with template<intN> data[N] already allocated.
 *  This should be faster and for small N are quite handy.
 *
 *  Currently the types are defined for most permutations of
 *  1, 2, and 4 rows/columns.  6x6 is also defined if _USE_MATRIX6X6
 *  is #define'd.
 *
 *  Cholesky decomposition and inversion are defined for
 *  symmetric positive definite matrices.
 *
 *  Several support functions (inner products, quadratic forms, etc)
 *  are also defined.
 * 
 */

#include <stdio.h>

#include "MT/MT_Core/support/mathsupport.h" // for random number generation and math.h

// uncomment to use 6x6 matrices
//#define _USE_MATRIX6X6

template<int N>class MT_Vector
{
protected:
  
public:
  
    double data[N];
  
    MT_Vector()
    {
    
        for(unsigned int i = 0; i < N; i++)
        {
            data[i] = 0;
        }
    
    };
  
    double& operator()(int i)
    {
    
        if(i < 0)
        {
            i = 0;
        }
    
        if(i >= N)
        {
            i = N-1;
        }
    
        return data[i];
    
    };
  
    void display(const char* NameString = "MT_Vector")
    {
    
        printf("========%s (transpose)======\n",NameString);
    
        printf("[");
    
        for(unsigned int i = 0; i < N; i++)
        {
            printf("\t\t%0+4.2f",data[i]);
        }
        printf("]\n");
    
        printf("================================\n");
    
    };
    
};

template<int N>
MT_Vector<N> operator+(const MT_Vector<N>& lhs, const MT_Vector<N>& rhs)
{
  
    MT_Vector<N> result;
  
    for(unsigned int i = 0; i < N; i++)
    {
        result.data[i] = lhs.data[i] + rhs.data[i];
    }
  
    return result;
  
}

template<int N>
MT_Vector<N> operator-(const MT_Vector<N>& lhs, const MT_Vector<N>& rhs)
{
  
    MT_Vector<N> result;
  
    for(unsigned int i = 0; i < N; i++)
    {
        result.data[i] = lhs.data[i] - rhs.data[i];
    }
  
    return result;
  
}

template<int N>
MT_Vector<N> operator*(const double lhs, const MT_Vector<N>& rhs)
{
  
    MT_Vector<N> result;
  
    for(unsigned int i = 0; i < N; i++)
    {
        result.data[i] = lhs*rhs.data[i];
    }
  
    return result;
}

template<int N>
MT_Vector<N> operator*(const MT_Vector<N>& lhs, const double rhs)
{
  
    return rhs*lhs;
  
}

template<int N>
double InnerProduct(const MT_Vector<N>& lhs, const MT_Vector<N>& rhs)
{
  
    double p = 0;
  
    for(unsigned int i = 0; i < N; i++)
    {
        p += lhs.data[i]*rhs.data[i];
    }
  
    return p;
  
}

template<int N>
double MT_VectorNormSquared(const MT_Vector<N>& v)
{
  
    double p = 0;
  
    for(unsigned int i = 0; i < N; i++)
    {
        p += v.data[i]*v.data[i];
    }
  
    return p;
  
}


#define MAT(a,b) a*C + b

template<int R,int C>
    class MT_Matrix
{
protected:
  
    int PositiveDefinite;
    int LowerTriangular;
  
public:

    double data[R*C];
  
    int nrows;
    int ncols;
  
    int isPositiveDefinite() const
    {
        return PositiveDefinite;
    };
    int isLowerTriangular() const
    {
        return LowerTriangular;
    };
    void markPositiveDefinite()
    {
        PositiveDefinite = 1;
    };
    void markLowerTriangular()
    {
        LowerTriangular = 1;
    };

    MT_Matrix()
    {
    
        PositiveDefinite = 0;
        nrows = R;
        ncols = C;
    
        for(unsigned int i = 0; i < R*C; i++)
        {
            data[i] = 0;
        }
    
    };
  
    void display(const char* NameString = "MT_Matrix")
    {
    
        printf("==============%s===========\n",NameString);
    
        for(unsigned int i = 0; i < R; i++)
        {
            printf("[ ");
            for(unsigned int j = 0; j < C; j++)
            {
                printf("\t\t%+4.2f",data[MAT(i,j)]);
            }
            printf("\t]\n");
        }
    
        printf("===============================\n");
    
    };
  
    double& operator()(int i, int j)
    {
    
        if(i < 0)
        {
            i = 0;
        }
    
        if(i >= R)
        {
            i = R - 1;
        }
    
        if(j < 0)
        {
            j = 0;
        }
    
        if(j >= C)
        {
            j = C - 1;
        }
    
        //printf("MAT(i,j) = %d\n",MAT(i,j));
        return data[i*C + j];
    
    };
  
    MT_Matrix<R,C>& operator=(const MT_Matrix<R,C>& rhs)
        {
            if(&rhs == this)
            {
                return *this;
            }
        
            if(rhs.isPositiveDefinite())
            {
                markPositiveDefinite();
            }
    
            if(rhs.isLowerTriangular())
            {
                markLowerTriangular();
            }
    
            for(unsigned int i = 0; i < R*C; i++)
            {
                data[i] = rhs.data[i];
            }
    
            return *this;
        
        };
  
};

template<int R, int C>
    MT_Matrix<R,C> operator+(const MT_Matrix<R,C>& lhs, const MT_Matrix<R,C>& rhs)
{
  
    MT_Matrix<R,C> result;
  
    for(unsigned int i = 0; i < R; i++)
    {
        for(unsigned int j = 0; j < C; j++)
        {
            result.data[MAT(i,j)] = lhs.data[MAT(i,j)] + rhs.data[MAT(i,j)];
        }
    }
  
    return result;
  
};

template<int R, int C>
    MT_Matrix<R,C> operator-(const MT_Matrix<R,C>& lhs, const MT_Matrix<R,C>& rhs)
{
  
    MT_Matrix<R,C> result;
  
    for(unsigned int i = 0; i < R; i++)
    {
        for(unsigned int j = 0; j < C; j++)
        {
            result.data[MAT(i,j)] = lhs.data[MAT(i,j)] - rhs.data[MAT(i,j)];
        }
    }
  
    return result;
  
};

template<int R1, int C1, int C2>
    MT_Matrix<R1,C2> operator*(const MT_Matrix<R1,C1>& lhs, const MT_Matrix<C1,C2>& rhs)
{
  
    MT_Matrix<R1,C2> result;
  
    for(unsigned int i = 0; i < R1; i++)
    {
        for(unsigned int j = 0; j < C2; j++)
        {
            for(unsigned int k = 0; k < C1; k++)
            {
                result.data[i*C2 + j] = result.data[i*C2 + j] 
                    + lhs.data[i*C1 + k]*rhs.data[k*C2 + j];
            }
        }
    }
  
    return result;
  
}

template<int R, int C>
    MT_Matrix<R,C> operator*(MT_Matrix<R,C>& lhs, const double rhs)
{
  
    MT_Matrix<R,C> result;
  
    for(unsigned int i = 0; i < R*C; i++)
    {
        result.data[i] = rhs*lhs.data[i];
    }
  
    if(rhs > 0 && lhs.isPositiveDefinite())
    {
        result.markPositiveDefinite();
    }
  
    if(lhs.isLowerTriangular())
    {
        result.markLowerTriangular();
    }
  
    return result;

}

template<int R, int C>
    MT_Matrix<R,C> operator*(const double lhs, MT_Matrix<R,C>& rhs)
{
  
    return rhs*lhs;
  
}

template<int R>
MT_Matrix<R,R> IdentityMT_Matrix()
{
  
    MT_Matrix<R,R> I;
  
    for(unsigned int i = 0; i < R; i++)
    {
        I(i,i) = 1;
    }
  
    I.markPositiveDefinite();
    I.markLowerTriangular();
  
    return I;
}

template<int R, int C>
    MT_Matrix<C,R> Transpose(MT_Matrix<R,C>& M)
{
  
    MT_Matrix<C,R> MT;
  
    for(unsigned int i = 0; i < C; i++)
    {
        for(unsigned int j = 0; j < R; j++)
        {
            MT.data[i*C + j] = M.data[j*R + i];
        }
    }
  
    if(M.isPositiveDefinite())
    {
        MT.markPositiveDefinite();
    }
  
    return MT;
  
}

template<int R>
MT_Matrix<R,R> Cholesky(MT_Matrix<R,R>& P)
{
  
    MT_Matrix<R,R> L;
  
    if(!P.isPositiveDefinite())
    {
        printf("Cholesky argument is not positive definite.  Returning identity.\n");
        L = IdentityMT_Matrix<R>();
        return L;
    }
  
    double p;
  
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
                p = P.data[i*R + i];
                for(int k = 0; k <= j-1; k++)
                {
                    p -= L.data[i*R+k]*L.data[i*R+k];
                }
                if( p < 0 ){     // NOTE these may not be numerically correct, but it is a safeguard
                    p = fabs(p);
                }
                if( p == 0 ){
                    p = 1e-6;
                }
                L.data[i*R+i] = sqrt(p);
            } else {
                p = P.data[i*R+j];
                for(int k = 0; k <= j-1; k++)
                {
                    p -= L.data[i*R+k]*L.data[j*R+k];
                }
                L.data[i*R+j] = p/L.data[j*R+j];
            }
      
        }
    }
  
    L.markLowerTriangular();
  
    return L;
  
}


template<int R, int C>
    MT_Vector<R> operator*(const MT_Matrix<R,C>& lhs, const MT_Vector<C>& rhs)
{
  
    MT_Vector<R> result;
  
    for(unsigned int i = 0; i < R; i++)
    {
    
        for(unsigned int j = 0; j < C; j++)
        {
      
            result.data[i] += lhs.data[i*C+j]*rhs.data[j];
      
        }
    
    }
  
    return result;
  
}

template<int R>
double MT_MatrixQuadratic(const MT_Vector<R>& v, const MT_Matrix<R,R> M)
{
  
    return InnerProduct(v,M*v);
  
}

template<int R>
double MT_MatrixQuadratic(const MT_Matrix<R,R> M, const MT_Vector<R>& v)
{
  
    return InnerProduct(v,M*v);
  
}

template<int R>
MT_Vector<R> RandNMT_Vector()
{
  
    MT_Vector<R> result;
  
    for(unsigned int i = 0; i < R; i++)
    {
        result.data[i] = MT_randn();
    }
  
    return result;
  
}


template<int R>
MT_Vector<R> ToMT_Vector(const MT_Matrix<R,1> M)
{
  
    MT_Vector<R> result;
  
    for(unsigned int i = 0; i < R; i++)
    {
        result.data[i] = M.data[i];
    }
  
    return result;
  
}

typedef MT_Matrix<4,4> MT_Matrix4x4;
typedef MT_Matrix<4,2> MT_Matrix4x2;
typedef MT_Matrix<2,4> MT_Matrix2x4;
typedef MT_Matrix<2,2> MT_Matrix2x2;
typedef MT_Matrix<1,2> MT_Matrix1x2;
typedef MT_Matrix<2,1> MT_Matrix2x1;
typedef MT_Vector<4> MT_Vector4;
typedef MT_Vector<2> MT_Vector2;

double MT_Vector1ToDouble(MT_Vector<1> v);
double MT_Matrix1ToDouble(MT_Matrix<1,1> m);

/* Computes the inverse of a positive definite 4x4 matrix by first finding
   thet Cholesky decomposition L s.t. P = L*L', then inverting L and returning
   Pinv = (Linv)'*Linv */
MT_Matrix4x4 PosDefInverse(MT_Matrix4x4& P);
/* Directly computes the inverse of a 4x4 lower triangular matrix (assumed
   to be nonsingular). */
MT_Matrix4x4 LowerTriangularInverse(MT_Matrix4x4& L);
/* Directly computes the inverse of a 2x2 matrix.  Prints a warning and 
   returns identity if matrix M is singular. */
MT_Matrix2x2 Inverse2x2(MT_Matrix2x2& M);

/* Computes the determinant of a lower triangular matrix - equal to the
   product of the diagonal elements. */
double LowerTriangularDet(const MT_Matrix4x4& L);
/* Computes the probability of vector x drawn from a bivariate normal distribution with given mean
   and covariance. */
double BivariateGaussianWithCov(const MT_Vector2& x, const MT_Vector2& mean, MT_Matrix2x2& cov);
/* Computes the probability of vector x drawn from a multiivariate normal distribution 
   over R^4 with given mean and covariance equal to L*L'. */
double QuadvariateGaussianWithChol(const MT_Vector4& x, const MT_Vector4& mean, MT_Matrix4x4& L);

#ifdef _USE_MATRIX6X6
typedef MT_Vector<6> MT_Vector6;
typedef MT_Matrix<6,6> MT_Matrix6x6;
double LowerTriangularDet(const MT_Matrix6x6& L);
/* Directly computes the inverse of a 6x6 lower triangular matrix (assumed
   to be nonsingular). */
MT_Matrix6x6 LowerTriangularInverse(MT_Matrix6x6& L);
/* Computes the probability of vector x drawn from a multiivariate normal distribution 
   over R^6 with given mean and covariance equal to L*L'. */
double MultivariateGaussianWithChol(const MT_Vector6& x, const MT_Vector6& mean, MT_Matrix6x6& L);
#endif

#endif
