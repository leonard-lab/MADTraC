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

template<int N>class Vector
{
protected:
  
public:
  
    double data[N];
  
    Vector()
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
  
    void display(const char* NameString = "Vector")
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
Vector<N> operator+(const Vector<N>& lhs, const Vector<N>& rhs)
{
  
    Vector<N> result;
  
    for(unsigned int i = 0; i < N; i++)
    {
        result.data[i] = lhs.data[i] + rhs.data[i];
    }
  
    return result;
  
}

template<int N>
Vector<N> operator-(const Vector<N>& lhs, const Vector<N>& rhs)
{
  
    Vector<N> result;
  
    for(unsigned int i = 0; i < N; i++)
    {
        result.data[i] = lhs.data[i] - rhs.data[i];
    }
  
    return result;
  
}

template<int N>
Vector<N> operator*(const double lhs, const Vector<N>& rhs)
{
  
    Vector<N> result;
  
    for(unsigned int i = 0; i < N; i++)
    {
        result.data[i] = lhs*rhs.data[i];
    }
  
    return result;
}

template<int N>
Vector<N> operator*(const Vector<N>& lhs, const double rhs)
{
  
    return rhs*lhs;
  
}

template<int N>
double InnerProduct(const Vector<N>& lhs, const Vector<N>& rhs)
{
  
    double p = 0;
  
    for(unsigned int i = 0; i < N; i++)
    {
        p += lhs.data[i]*rhs.data[i];
    }
  
    return p;
  
}

template<int N>
double VectorNormSquared(const Vector<N>& v)
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
    class Matrix
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

    Matrix()
    {
    
        PositiveDefinite = 0;
        nrows = R;
        ncols = C;
    
        for(unsigned int i = 0; i < R*C; i++)
        {
            data[i] = 0;
        }
    
    };
  
    void display(const char* NameString = "Matrix")
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
  
    Matrix<R,C>& operator=(const Matrix<R,C>& rhs)
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
    Matrix<R,C> operator+(const Matrix<R,C>& lhs, const Matrix<R,C>& rhs)
{
  
    Matrix<R,C> result;
  
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
    Matrix<R,C> operator-(const Matrix<R,C>& lhs, const Matrix<R,C>& rhs)
{
  
    Matrix<R,C> result;
  
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
    Matrix<R1,C2> operator*(const Matrix<R1,C1>& lhs, const Matrix<C1,C2>& rhs)
{
  
    Matrix<R1,C2> result;
  
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
    Matrix<R,C> operator*(Matrix<R,C>& lhs, const double rhs)
{
  
    Matrix<R,C> result;
  
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
    Matrix<R,C> operator*(const double lhs, Matrix<R,C>& rhs)
{
  
    return rhs*lhs;
  
}

template<int R>
Matrix<R,R> IdentityMatrix()
{
  
    Matrix<R,R> I;
  
    for(unsigned int i = 0; i < R; i++)
    {
        I(i,i) = 1;
    }
  
    I.markPositiveDefinite();
    I.markLowerTriangular();
  
    return I;
}

template<int R, int C>
    Matrix<C,R> Transpose(Matrix<R,C>& M)
{
  
    Matrix<C,R> MT;
  
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
Matrix<R,R> Cholesky(Matrix<R,R>& P)
{
  
    Matrix<R,R> L;
  
    if(!P.isPositiveDefinite())
    {
        printf("Cholesky argument is not positive definite.  Returning identity.\n");
        L = IdentityMatrix<R>();
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
    Vector<R> operator*(const Matrix<R,C>& lhs, const Vector<C>& rhs)
{
  
    Vector<R> result;
  
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
double MatrixQuadratic(const Vector<R>& v, const Matrix<R,R> M)
{
  
    return InnerProduct(v,M*v);
  
}

template<int R>
double MatrixQuadratic(const Matrix<R,R> M, const Vector<R>& v)
{
  
    return InnerProduct(v,M*v);
  
}

template<int R>
Vector<R> RandNVector()
{
  
    Vector<R> result;
  
    for(unsigned int i = 0; i < R; i++)
    {
        result.data[i] = MT_randn();
    }
  
    return result;
  
}


template<int R>
Vector<R> ToVector(const Matrix<R,1> M)
{
  
    Vector<R> result;
  
    for(unsigned int i = 0; i < R; i++)
    {
        result.data[i] = M.data[i];
    }
  
    return result;
  
}

typedef Matrix<4,4> Matrix4x4;
typedef Matrix<4,2> Matrix4x2;
typedef Matrix<2,4> Matrix2x4;
typedef Matrix<2,2> Matrix2x2;
typedef Matrix<1,2> Matrix1x2;
typedef Matrix<2,1> Matrix2x1;
typedef Vector<4> Vector4;
typedef Vector<2> Vector2;

double Vector1ToDouble(Vector<1> v);
double Matrix1ToDouble(Matrix<1,1> m);

/* Computes the inverse of a positive definite 4x4 matrix by first finding
   thet Cholesky decomposition L s.t. P = L*L', then inverting L and returning
   Pinv = (Linv)'*Linv */
Matrix4x4 PosDefInverse(Matrix4x4& P);
/* Directly computes the inverse of a 4x4 lower triangular matrix (assumed
   to be nonsingular). */
Matrix4x4 LowerTriangularInverse(Matrix4x4& L);
/* Directly computes the inverse of a 2x2 matrix.  Prints a warning and 
   returns identity if matrix M is singular. */
Matrix2x2 Inverse2x2(Matrix2x2& M);

/* Computes the determinant of a lower triangular matrix - equal to the
   product of the diagonal elements. */
double LowerTriangularDet(const Matrix4x4& L);
/* Computes the probability of vector x drawn from a bivariate normal distribution with given mean
   and covariance. */
double BivariateGaussianWithCov(const Vector2& x, const Vector2& mean, Matrix2x2& cov);
/* Computes the probability of vector x drawn from a multiivariate normal distribution 
   over R^4 with given mean and covariance equal to L*L'. */
double QuadvariateGaussianWithChol(const Vector4& x, const Vector4& mean, Matrix4x4& L);

#ifdef _USE_MATRIX6X6
typedef Vector<6> Vector6;
typedef Matrix<6,6> Matrix6x6;
double LowerTriangularDet(const Matrix6x6& L);
/* Directly computes the inverse of a 6x6 lower triangular matrix (assumed
   to be nonsingular). */
Matrix6x6 LowerTriangularInverse(Matrix6x6& L);
/* Computes the probability of vector x drawn from a multiivariate normal distribution 
   over R^6 with given mean and covariance equal to L*L'. */
double MultivariateGaussianWithChol(const Vector6& x, const Vector6& mean, Matrix6x6& L);
#endif

#endif
