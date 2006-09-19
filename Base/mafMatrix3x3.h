/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrix3x3.h,v $
  Language:  C++
  Date:      $Date: 2006-09-19 15:28:57 $
  Version:   $Revision: 1.6 $
  Authors:   Based on vtkMath code (www.vtk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafMatrix3x3_h
#define __mafMatrix3x3_h

#include "mafObject.h"
#include "mafTimeStamped.h"
#include <math.h>

typedef double (*mafMatrix3x3Elements)[3];


/** mafMatrix3x3 - Simple 3x3 Matrix.
  This class defines a simple 3x3 Matrix class, and some operators over it.
  This is typically used for internal algorithms, use mafMatrix for complex usage.
  @sa mafMatrix
*/
class MAF_EXPORT mafMatrix3x3: public mafObject, public mafTimeStamped
{
public:
  mafTypeMacro(mafMatrix3x3,mafObject);
  virtual void Print (std::ostream& os, const int indent=0) const;

  mafMatrix3x3();
  virtual ~mafMatrix3x3();

  mafMatrix3x3 &operator=(const mafMatrix3x3 &mat);
  mafMatrix3x3(mafMatrix3x3 &mat);

  /** copy the given matrix content */
  void DeepCopy(mafMatrix3x3 *mat);

  /** Multiply a vector by a 3x3 matrix.  The result is placed in out.*/
  static void MultiplyVector(const float A[3][3], const float in[3], 
                          float out[3]);
  /** Multiply a vector by a 3x3 matrix.  The result is placed in out.*/
  static void MultiplyVector(const double A[3][3], const double in[3], 
                          double out[3]);

  /** Multiply a vector by this matrix.  The result is placed in out.*/
  void MultiplyVector(const double in[3], double out[3]) {MultiplyVector(GetElements(),in,out);}

  /** Multiply one 3x3 matrix by another according to C = AB.*/
  static void Multiply(const double A[3][3], const double B[3][3], 
                          double C[3][3]);

  /** Multiply this matrix by another according to C = AB.*/
  static void Multiply(const mafMatrix3x3 &A,mafMatrix3x3 &B, mafMatrix3x3 &C) \
    {Multiply(A.GetElements(),B.GetElements(),C.GetElements());C.Modified();}
 
  mafMatrix3x3Elements GetElements() const {return (mafMatrix3x3Elements)m_Elements;}

  /** 
    Sets the element i,j in the matrix. Remember to call explicitly
    Modified when using this function. */
  void SetElement(int i, int j, double value) {GetElements()[i][j]=value;}

  /** Returns the element i,j from the matrix. */
  double GetElement(int i, int j) const {return GetElements()[i][j];}

  /** Get the given matrix versor. Static version. */
  static void GetVersor(int axis, const mafMatrix3x3 &matrix, double versor[3]);
  /** Get the given matrix versor*/
  void GetVersor(int axis, double versor[3]) {GetVersor(axis,*this,versor);}

  /** Set all of the elements to zero. Static version. */
  void Zero() { mafMatrix3x3::Zero(*GetElements()); Modified(); }
  /** Set all of the elements to zero. */  
  static void Zero(double elements[9]);

  /** Set equal to Identity matrix */
  void Identity() { mafMatrix3x3::Identity(GetElements()); Modified();}
  static void Identity(double A[3][3]);

  /** Transpose the 3x3 matrix.*/
  static void Transpose(const double A[3][3], double AT[3][3]);
  void Transpose() {Transpose(GetElements(),GetElements());}

  /** Invert the 3x3 matrix.*/
  static void Invert(const double A[3][3], double AI[3][3]);
  void Invert() {GetElements(),GetElements();}
  
  /**
   Orthogonalize a 3x3 matrix and put the result in B.  If matrix A
   has a negative determinant, then B will be a rotation plus a flip
   i.e. it will have a determinant of -1. */
  static void Orthogonalize(const double A[3][3], double B[3][3]);
  
  /** 
   Orthogonalize this matrix inplace.  If this matrix 
   has a negative determinant, then the result will be a
   rotation plus a flip i.e. it will have a determinant of -1. */
  void Orthogonalize() {Orthogonalize(GetElements(),GetElements());Modified();}

  /**
    Diagonalize a symmetric 3x3 matrix and return the eigenvalues in
    w and the eigenvectors in the columns of V.  The matrix V will 
    have a positive determinant, and the three eigenvectors will be
    aligned as closely as possible with the x, y, and z axes. */
  static void Diagonalize(const double A[3][3],double w[3],double V[3][3]);

  /** Return the determinant of a 3x3 matrix. */
  static double Determinant(double A[3][3]);

  /** Compute the determinant of the matrix and return it.*/
  double Determinant() {return Determinant(GetElements());}

  static inline double Determinant(const double c1[3], 
                                      const double c2[3], 
                                      const double c3[3]);

  static inline double Determinant(double a1, double a2, double a3, 
                                      double b1, double b2, double b3, 
                                      double c1, double c2, double c3);
  
  /**
    Convert a quaternion to a 3x3 rotation matrix.  The quaternion
    does not have to be normalized beforehand. */
  static void mmuQuaternionToMatrix(const double quat[4], double A[3][3]); 
  
  /**
    Convert a quaternion to a 3x3 rotation matrix.  The quaternion
    does not have to be normalized beforehand. */
  void mmuQuaternionToMatrix(const double quat[4]) {mmuQuaternionToMatrix(quat,GetElements());}

  /**
   Convert a 3x3 matrix into a quaternion.  This will provide the
   best possible answer even if the matrix is not a pure rotation matrix.
   The method used is that of B.K.P. Horn. */
  static void MatrixTommuQuaternion(const double A[3][3], double quat[4]);
  
  /**
   Convert a 3x3 matrix into a quaternion.  This will provide the
   best possible answer even if the matrix is not a pure rotation matrix.
   The method used is that of B.K.P. Horn. */
  void MatrixTommuQuaternion(double quat[4]) {MatrixTommuQuaternion(GetElements(),quat);}
  
  /**
    Perform singular value decomposition on a 3x3 matrix.  This is not
    done using a conventional SVD algorithm, instead it is done using
    Orthogonalize3x3 and Diagonalize3x3.  Both output matrices U and VT
    will have positive determinants, and the w values will be arranged
    such that the three rows of VT are aligned as closely as possible
    with the x, y, and z axes respectively.  If the determinant of A is
    negative, then the three w values will be negative. */                                            
  static void SingularValueDecomposition(const double A[3][3],
                                            double U[3][3], double w[3],
                                            double VT[3][3]);
                                            
  /** Perform singular value decomposition on a 3x3 matrix. See the static version. */ 
  void SingularValueDecomposition(double U[3][3], double w[3], double VT[3][3]) \
    {SingularValueDecomposition(GetElements(),U,w,VT);}

  /*
    Jacobi iteration for the solution of eigenvectors/eigenvalues of a 3x3
    real symmetric matrix. Square 3x3 matrix a; output eigenvalues in w;
    and output eigenvectors in v. Resulting eigenvalues/vectors are sorted
    in decreasing order; eigenvectors are normalized.*/
  static int Jacobi(const double A[3][3], double w[3], double v[3][3]);

  /**
    Jacobi iteration for the solution of eigenvectors/eigenvalues of this 3x3
    real symmetric matrix. Output eigenvalues in w; and output eigenvectors in v.
    Resulting eigenvalues/vectors are sorted in decreasing order; eigenvectors
    are normalized.*/
  int Jacobi(double w[3], double v[3][3]) {Jacobi(GetElements(),w,v);}

  /**
    JacobiN iteration for the solution of eigenvectors/eigenvalues of a nxn
    real symmetric matrix. Square nxn matrix a; size of matrix in n; output
    eigenvalues in w; and output eigenvectors in v. Resulting
    eigenvalues/vectors are sorted in decreasing order; eigenvectors are
    normalized.  w and v need to be allocated previously */
  static int JacobiN(double **a, int n, double *w, double **v);

  /**
    LU Factorization of a 3x3 matrix.  The diagonal elements are the
    multiplicative inverse of those in the standard LU factorization.*/
  static void LUFactor(double A[3][3], int index[3]);
  void LUFactor(int index[3]) {LUFactor(GetElements(),index);}

  /**
    LU back substitution for a 3x3 matrix.  The diagonal elements are the
    multiplicative inverse of those in the standard LU factorization.*/
  static void LUSolve(const double A[3][3], const int index[3], double x[3]);
  void LUSolve(const int index[3], double x[3]) \
    { LUSolve(GetElements(),index,x);}

  
  /** Useful constants. (double-precision version) */
  static double DegreesToRadians() {return 0.017453292519943295;};
  static double Pi() {return 3.1415926535897932384626;};
  static double RadiansToDegrees() {return 57.29577951308232;};

  /** bracket operator to access & write single elements */
  double *operator[](const unsigned int i) {return &(GetElements()[i][0]);}

  /** bracket operator to access single elements */
  const double *operator[](unsigned int i) const { return &(GetElements()[i][0]); }  
  
  /** Calculate the determinant of a 2x2 matrix: | a b | | c d | */
  static double Determinant2x2(double a, double b, double c, double d) {
    return (a * d - b * c);};
  /** Calculate the determinant of a 2x2 matrix with columns c1 and c2 */
  static double Determinant2x2(const double c1[2], const double c2[2]) {
    return (c1[0]*c2[1] - c2[0]*c1[1]);};

  /** Compute the norm of 3-vector (double-precision version).*/
  static double Norm(const double x[3]) {
    return sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);};
  
  static inline void Cross(const double x[3], const double y[3], double z[3]);
  static inline double Normalize(double x[3]);
  
protected:

  double m_Elements[3][3];  ///< internal representation
};

//------------------------------------------------------------------------------
inline double mafMatrix3x3::Determinant(double A[3][3])
//------------------------------------------------------------------------------
{
  return A[0][0]*A[1][1]*A[2][2] + A[1][0]*A[2][1]*A[0][2] + 
         A[2][0]*A[0][1]*A[1][2] - A[0][0]*A[2][1]*A[1][2] - 
         A[1][0]*A[0][1]*A[2][2] - A[2][0]*A[1][1]*A[0][2];
}
//------------------------------------------------------------------------------
inline double mafMatrix3x3::Determinant(const double c1[3], 
                                      const double c2[3], 
                                      const double c3[3])
//------------------------------------------------------------------------------
{
  return c1[0]*c2[1]*c3[2] + c2[0]*c3[1]*c1[2] + c3[0]*c1[1]*c2[2] -
         c1[0]*c3[1]*c2[2] - c2[0]*c1[1]*c3[2] - c3[0]*c2[1]*c1[2];
}
//------------------------------------------------------------------------------
inline double mafMatrix3x3::Determinant(double a1, double a2, double a3, 
                                      double b1, double b2, double b3, 
                                      double c1, double c2, double c3)
//------------------------------------------------------------------------------
{
    return ( a1 * mafMatrix3x3::Determinant2x2( b2, b3, c2, c3 )
           - b1 * mafMatrix3x3::Determinant2x2( a2, a3, c2, c3 )
           + c1 * mafMatrix3x3::Determinant2x2( a2, a3, b2, b3 ) );
}
//------------------------------------------------------------------------------
// Cross product of two 3-vectors. Result vector in z[3].
inline void mafMatrix3x3::Cross(const double x[3], const double y[3], double z[3])
//------------------------------------------------------------------------------
{
  double Zx = x[1]*y[2] - x[2]*y[1]; 
  double Zy = x[2]*y[0] - x[0]*y[2];
  double Zz = x[0]*y[1] - x[1]*y[0];
  z[0] = Zx; z[1] = Zy; z[2] = Zz; 
}
//------------------------------------------------------------------------------
inline double mafMatrix3x3::Normalize(double x[3])
//------------------------------------------------------------------------------
{
  double den; 
  if ( (den = mafMatrix3x3::Norm(x)) != 0.0 )
  {
    for (int i=0; i < 3; i++)
    {
      x[i] /= den;
    }
  }
  return den;
}

#endif 

