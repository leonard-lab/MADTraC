/*
 *  Matrix.cpp
 *
 *  Created by Daniel Swain on 10/31/08.
 *
 */

#include "Matrix.h"


double MT_Vector1ToDouble(MT_Vector<1> v)
{
    return v.data[0];
}

double MT_Matrix1ToDouble(MT_Matrix<1,1> m)
{
    return m.data[0];
}

MT_Matrix4x4 PosDefInverse(MT_Matrix4x4& P)
{
  
    MT_Matrix4x4 Pinv = IdentityMT_Matrix<4>();
    MT_Matrix4x4 L, Linv;
  
    if(!P.isPositiveDefinite())
    {
        printf("MT_Matrix inverse of indefinite matrices is not yet implemented.  Returning identity.\n");
        return Pinv;
    }
  
    L = Cholesky(P);
  
    Linv = LowerTriangularInverse(L);

    Pinv = Transpose(Linv)*Linv;
  
    Pinv.markPositiveDefinite();
  
    return Pinv;
  
}

MT_Matrix4x4 LowerTriangularInverse(MT_Matrix4x4& L)
{
  
    unsigned int C = 4;
  
    MT_Matrix4x4 Linv = IdentityMT_Matrix<4>();
  
    if(!L.isLowerTriangular())
    {
        printf("Non-lower triangular matrix supplied to lower triangular matrix inversion function.  Returning identity.\n");
        return Linv;
    }
  
    double L11inv = 1.0/L.data[MAT(0,0)];
    double L22inv = 1.0/L.data[MAT(1,1)];
    double L33inv = 1.0/L.data[MAT(2,2)];
    double L44inv = 1.0/L.data[MAT(3,3)];
  
    // diagonal elements are just inverted
    Linv.data[MAT(0,0)] = L11inv;
    Linv.data[MAT(1,1)] = L22inv;
    Linv.data[MAT(2,2)] = L33inv;
    Linv.data[MAT(3,3)] = L44inv;
  
    // 1st subdiagonal
    Linv.data[MAT(1,0)] = -L.data[MAT(1,0)]*L11inv*L22inv;
    Linv.data[MAT(2,1)] = -L.data[MAT(2,1)]*L22inv*L33inv;
    Linv.data[MAT(3,2)] = -L.data[MAT(3,2)]*L33inv*L44inv;
  
    // 2nd subdiagonal
    Linv.data[MAT(2,0)] = (-L.data[MAT(1,1)]*L.data[MAT(2,0)]+L.data[MAT(1,0)]*L.data[MAT(2,1)])*L11inv*L22inv*L33inv;
    Linv.data[MAT(3,1)] = (-L.data[MAT(2,2)]*L.data[MAT(3,1)]+L.data[MAT(2,1)]*L.data[MAT(3,2)])*L22inv*L33inv*L44inv;
  
    // 3rd subdiagonal / lower left element
    Linv.data[MAT(3,0)] = (-L.data[MAT(1,1)]*L.data[MAT(2,2)]*L.data[MAT(3,0)] + L.data[MAT(1,0)]*L.data[MAT(2,2)]*L.data[MAT(3,1)] 
                           + L.data[MAT(1,1)]*L.data[MAT(2,0)]*L.data[MAT(3,2)] - L.data[MAT(1,0)]*L.data[MAT(2,1)]*L.data[MAT(3,2)])*L11inv*L22inv*L33inv*L44inv;
  
    Linv.markLowerTriangular();
  
    return Linv;
  
}

 

MT_Matrix2x2 Inverse2x2(MT_Matrix2x2& M)
{
  
    MT_Matrix2x2 result = IdentityMT_Matrix<2>();
  
    double detM = M.data[0]*M.data[3] - M.data[1]*M.data[2];
  
    if(detM == 0)
    {
        printf("2x2 MT_Matrix was singular, returning identity.\n");
        return result;
    }
  
    double detMinv = 1.0/detM;
  
    result.data[0] = M.data[3]*detMinv;
    result.data[1] = -M.data[1]*detMinv;
    result.data[2] = -M.data[2]*detMinv;
    result.data[3] = M.data[0]*detMinv;
  
    return result;
  
}

double LowerTriangularDet(const MT_Matrix4x4& L)
{
  
    return L.data[0]*L.data[5]*L.data[10]*L.data[15];

}

double BivariateGaussianWithCov(const MT_Vector2& x, const MT_Vector2& mean, MT_Matrix2x2& cov)
{
      
    double detcov = cov.data[0]*cov.data[3] - cov.data[1]*cov.data[2];
    double normfac = 0.5/MT_PI;
  
    if(detcov <= 0)
    {
        printf("Invalid 2x2 Covariance MT_Matrix, Returning 1\n");
        return 1.0;
    }
  
    MT_Matrix2x2 invCov = Inverse2x2(cov);
    double exparg = -0.5*InnerProduct( (x - mean), invCov*(x - mean) );
  
    return normfac*exp(exparg)/sqrt(detcov);
  
}
  
double QuadvariateGaussianWithChol(const MT_Vector4& x, const MT_Vector4& mean, MT_Matrix4x4& L)
{
  
    double Ldet = LowerTriangularDet(L);
    double normfac = 0.25/MT_PI/MT_PI;
  
    if(Ldet <= 0)
    {
        //printf("Invalid 4x4 Covariance Cholesky Decomposition, Returning 1\n");
        return 1.0;
    }
  
    MT_Matrix4x4 Linv = LowerTriangularInverse(L);
  
    double exparg = -0.5*MT_VectorNormSquared( Linv*(x - mean) );
  
    return normfac*exp(exparg)/Ldet;
  
}

#ifdef _USE_MATRIX6X6
double LowerTriangularDet(const MT_Matrix6x6& L)
{
  
    return L.data[0]*L.data[7]*L.data[14]*L.data[21]*L.data[28]*L.data[35];
  
}

MT_Matrix6x6 LowerTriangularInverse(MT_Matrix6x6& L)
{
  
    unsigned int C = 6;
  
    MT_Matrix6x6 Linv = IdentityMT_Matrix<6>();
  
    if(!L.isLowerTriangular())
    {
        printf("Non-lower triangular matrix supplied to lower triangular matrix inversion function.  Returning identity.\n");
        return Linv;
    }
  
    double L11inv = 1.0/L.data[MAT(0,0)];
    double L22inv = 1.0/L.data[MAT(1,1)];
    double L33inv = 1.0/L.data[MAT(2,2)];
    double L44inv = 1.0/L.data[MAT(3,3)];
    double L55inv = 1.0/L.data[MAT(4,4)];
    double L66inv = 1.0/L.data[MAT(5,5)];
  
    double L11 = L.data[MAT(0,0)];
    double L21 = L.data[MAT(1,0)];
    double L22 = L.data[MAT(1,1)];
    double L31 = L.data[MAT(2,0)];
    double L32 = L.data[MAT(2,1)];
    double L33 = L.data[MAT(2,2)];
    double L41 = L.data[MAT(3,0)];
    double L42 = L.data[MAT(3,1)];
    double L43 = L.data[MAT(3,2)];
    double L44 = L.data[MAT(3,3)];
    double L51 = L.data[MAT(4,0)];
    double L52 = L.data[MAT(4,1)];
    double L53 = L.data[MAT(4,2)];
    double L54 = L.data[MAT(4,3)];
    double L55 = L.data[MAT(4,4)];
    double L61 = L.data[MAT(5,0)];
    double L62 = L.data[MAT(5,1)];
    double L63 = L.data[MAT(5,2)];
    double L64 = L.data[MAT(5,3)];
    double L65 = L.data[MAT(5,4)];
    double L66 = L.data[MAT(5,5)];
  
    // diagonal elements are just inverted
    Linv.data[MAT(0,0)] = L11inv;
    Linv.data[MAT(1,1)] = L22inv;
    Linv.data[MAT(2,2)] = L33inv;
    Linv.data[MAT(3,3)] = L44inv;
    Linv.data[MAT(4,4)] = L55inv;
    Linv.data[MAT(5,5)] = L66inv;
  
    // 1st subdiagonal
    Linv.data[MAT(1,0)] = -L21*L11inv*L22inv;
    Linv.data[MAT(2,1)] = -L32*L22inv*L33inv;
    Linv.data[MAT(3,2)] = -L43*L33inv*L44inv;
    Linv.data[MAT(4,3)] = -L54*L44inv*L55inv;
    Linv.data[MAT(5,4)] = -L65*L55inv*L66inv;
  
    // 2nd subdiagonal
    Linv.data[MAT(2,0)] = (-L22*L31+L21*L32)*L11inv*L22inv*L33inv;
    Linv.data[MAT(3,1)] = (-L33*L42+L32*L43)*L22inv*L33inv*L44inv;
    Linv.data[MAT(4,2)] = (-L44*L53+L43*L54)*L33inv*L44inv*L55inv;
    Linv.data[MAT(5,3)] = (-L55*L64+L54*L65)*L44inv*L55inv*L66inv;
  
    // 3rd subdiagonal
    Linv.data[MAT(3,0)] = (-L22*L33*L41 + L21*L33*L42 + L22*L31*L43 - L21*L32*L43)*L11inv*L22inv*L33inv*L44inv;
    Linv.data[MAT(4,1)] = (-L33*L44*L52 + L32*L44*L53 + L33*L42*L54 - L32*L43*L54)*L22inv*L33inv*L44inv*L55inv;
    Linv.data[MAT(5,2)] = (-L44*L55*L63 + L43*L55*L64 + L44*L53*L65 - L43*L54*L65)*L33inv*L44inv*L55inv*L66inv;
  
    // 4th subdiagonal
    Linv.data[MAT(4,0)] = (L22*((-L33)*L44*L51 + L31*L44*L53 + 
                                L33*L41*L54 - L31*L43*L54) + 
                           L21*(L33*L44*L52 - L32*L44*L53 - 
                                L33*L42*L54 + L32*L43*L54))*L11inv*L22inv*L33inv*L44inv*L55inv;
    Linv.data[MAT(5,1)] = (L33*((-L44)*L55*L62 + L42*L55*L64 + L44*L52*L65 - L42*L54*L65) + 
                           L32*(L44*L55*L63 - L43*L55*L64 - L44*L53*L65 + L43*L54*L65))*L22inv*L33inv*L44inv*L55inv*L66inv;
  
    // 5th subdiagonal
    Linv.data[MAT(5,0)] = (L21*(L33*(L44*L55*L62 - L42*L55*L64 - L44*L52*L65 + L42*L54*L65) +                         
                                L32*((-L44)*L55*L63 + L43*L55*L64 + L44*L53*L65 - L43*L54*L65)) + 
                           L22*(L33*((-L44)*L55*L61 + L41*L55*L64 + L44*L51*L65 - 
                                     L41*L54*L65) + 
                                L31*(L44*L55*L63 - L43*L55*L64 - L44*L53*L65 + 
                                     L43*L54*L65)))*L11inv*L22inv*L33inv*L44inv*L55inv*L66inv;
  
    Linv.markLowerTriangular();
  
    return Linv;
  
}

double MultivariateGaussianWithChol(const MT_Vector6& x, const MT_Vector6& mean, MT_Matrix6x6& L)
{
  
    double Ldet = LowerTriangularDet(L);
    double normfac = 0.125/MT_PI/MT_PI/MT_PI;
  
    if(Ldet <= 0)
    {
        //printf("Invalid 4x4 Covariance Cholesky Decomposition, Returning 1\n");
        return 1.0;
    }
  
    MT_Matrix6x6 Linv = LowerTriangularInverse(L);
  
    double exparg = -0.5*MT_VectorNormSquared( Linv*(x - mean) );
  
    return normfac*exp(exparg)/Ldet;
  
}
#endif // _USE_MATRIX6X6
