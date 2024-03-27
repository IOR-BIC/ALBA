/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAMatrixVectorMath
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/





#ifndef __vtkALBAMatrixVectorMath_h
#define __vtkALBAMatrixVectorMath_h

#include "albaConfigure.h"
#include "vtkObject.h"
#include <ostream>


//------------------------------------------------------------------------------
/// Simple matrix and vector arithmetic. \n
/// This is not itself a vector or matrix; it is only a set of useful methods, \n
/// and contains no internal data except a flag for homogeneous mode. \n\n
///
/// These methods are for vectors and matrices with dimensions 3x3 or 4x4 homogeneous. \n
/// Use SetHomogeneous() to set the homogeneous mode (default is off).\n
/// NB Many methods assume that the homogenous coord h = 1. \n\n
/// 
/// The matrix format is either column-major 1D array format, which is consistent with OpenGL:  \n
/// 0 3 6  or  0  4  8  12  \n
/// 1 4 7      1  5  9  13  \n
/// 2 5 8      2  6  10 14  \n
///            3  7  11 15  \n\n
///
/// or standard 2D array: A[3][3] or A[4][4]. \n
/// 4x4 matrices are assumed to be homegeneous. \n\n
///
/// Methods are provided to convert between 1D array and 2D array form. \n
/// 2D arrays are assumed to be in the familiar row-major form arr2D[row][col], which is consistent with vtkMath. \n\n
/// 
/// Matrix convention in this code: i is the row, j is the column, and k is the 1D array index. \n
/// The output matrix can be the same as the input unless stated otherwise. \n
//
// Modified: Nigel McFarlane 27.4.11
//------------------------------------------------------------------------------
class ALBA_EXPORT vtkALBAMatrixVectorMath : public vtkObject
{
public:
  static vtkALBAMatrixVectorMath *New();
  vtkTypeMacro(vtkALBAMatrixVectorMath, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) {} ;


  /// Set to false for non-homogeneous 3x3 (default), \n
  /// set to true for homogeneous 4x4
  void SetHomogeneous(bool homogeneous) {m_homogeneous = homogeneous ;}


  //----------------------------------------------------------------------------
  /// Vector methods
  //----------------------------------------------------------------------------

  /// Divide vector by homo coord
  void DivideVectorByHomoCoord(double *a) const ;  

  /// Set vector to zero
  void SetVectorToZero(double *a)  const ;

  /// Set vector
  void SetVector(double *a, double a0, double a1, double a2) const ;

  /// Magnitude of vector.
  double MagnitudeOfVector(const double *a) const ;

  /// Normalize vector
  void NormalizeVector(double *a) const ;                 

  /// Normalize vector
  void NormalizeVector(const double *a, double *b) const ; 

  /// Invert vector (multiply by -1)
  void InvertVector(double *a) const ;                 

  /// Invert vector (multiply by -1)
  void InvertVector(const double *a, double *b) const ;                 

  /// Multiply vector by scalar: s*a = b
  void MultiplyVectorByScalar(double s, const double *a, double *b) const ;   

  /// Divide vector by scalar: a/s = b
  void DivideVectorByScalar(double s, const double *a, double *b) const ;     

  /// Add vectors: a + b = c
  void AddVectors(const double *a, const double *b, double *c) const ;              

  /// Subtract vectors: a - b = c
  void SubtractVectors(const double *a, const double *b, double *c) const ;   

  /// Add multiple of vector: a + s*b = c
  void AddMultipleOfVector(const double *a, double s, const double *b, double *c) const ;

  /// Subtract multiple of vector: a - s*b = c
  void SubtractMultipleOfVector(const double *a, double s, const double *b, double *c) const ;

  /// Interpolate between vectors: (1-s)*a + s*b = c
  /// If s = 0, c = a
  /// If s = 1, c = b
  void InterpolateVectors(double s, const double *a, const double *b, double *c) const ;

  /// Dot product a.b
  double DotProduct(const double *a, const double *b) const ;   

  /// Vector Product a^b = c \n
  /// Output c cannot be the same as inputs a or b
  void VectorProduct(const double *a, const double *b, double *c) const ; 

  /// Copy vector: b = a
  void CopyVector(const double *a, double *b) const ; 

  /// Are vectors equal
  bool Equals(const double *a, const double *b, double tol) const ; 

  /// Calculate two arbitrary vectors v and w which are normal to the given vector u, \n
  /// where u, v and w form a right handed coordinate system. \n
  /// The vectors are not normalised.
  void CalculateNormalsToU(const double *u,  double *v,  double *w) const ;

  /// Calculate two arbitrary vectors u and w which are normal to the given vector v, \n
  /// where u, v and w form a right handed coordinate system. \n
  /// The vectors are not normalised.
  void CalculateNormalsToV(double *u,  const double *v,  double *w) const ;

  /// Calculate two arbitrary vectors u and v which are normal to the given vector w, \n
  /// where u, v and w form a right handed coordinate system. \n
  /// The vectors are not normalised.
  void CalculateNormalsToW(double *u,  double *v,  const double *w) const ;

  /// Distance between two vectors
  double Distance(const double *a,  const double *b) const ;

  /// Distance squared between two vectors
  double DistanceSquared(const double *a,  const double *b) const ;

  /// Mean of two vectors
  void MeanVector(const double *a0, const double *a1, double *b) const ;

  /// Mean of three vectors
  void MeanVector(const double *a0, const double *a1, const double *a2, double *b) const ;

  /// Mean of four vectors
  void MeanVector(const double *a0, const double *a1, const double *a2, const double *a3, double *b) const ;

  /// Copy homo 4 vector to 3 vector. \n
   void CopyHomoVectorToVector(const double *aHomo, double *a) const ;

  /// Copy 3 vector to homo 4 vector
  void CopyVectorToHomoVector(const double *a, double *aHomo) const ;

  /// Rotate vector about x
  void RotateVectorAboutX(const double *a, double *b, double theta) const ;

  /// Rotate vector about y
  void RotateVectorAboutY(const double *a, double *b, double theta) const ;

  /// Rotate vector about z
  void RotateVectorAboutZ(const double *a, double *b, double theta) const ;

  /// Rotate vector about arbitrary axis w. \n
  /// NB this is inefficient if the rotation is to be repeated. \n
  /// Instead use SetMatrixToRotateAboutAxis() to get a reusable rotation matrix.
  void RotateVectorAboutAxis(const double *a, double *b, const double *w, double theta) const ;

  /// Find nearest point on line. \n
  /// Returns rsq and lambda, where rsq is the squared distance, and \n
  /// lambda is the position along the line segment.
  void FindNearestPointOnLine(const double x[3], const double p0[3], const double p1[3], double& rsq, double& lambda) ;

  /// print vector
  void PrintVector(std::ostream& os, const double *a) const ;    



  //----------------------------------------------------------------------------
  /// Matrix methods
  //----------------------------------------------------------------------------

  /// Copy matrix
  void CopyMatrix(const double *A, double *B) ;

  /// Copy matrix A[9] to homo matrix B[16]
  void CopyMatrixToHomoMatrix(const double *A, double *B) ;

  /// Copy matrix
  void CopyMatrix3x3(const double A[3][3], double B[3][3]) ;

  /// Copy matrix
  void CopyMatrix4x4(const double A[4][4], double B[4][4]) ;

  /// Divide matrix by homo coord
  void DivideMatrixByHomoCoord(double *A) const ;  

  /// Divide matrix by homo coord
  void DivideMatrixByHomoCoord(double A[4][4]) const ;  

  /// Set matrix to zero.
  void SetMatrixToZero(double *A) const ;

  /// Set matrix to zero
  void SetMatrixToZero3x3(double A[3][3]) const ;

  /// Set matrix to zero (except for homogeneous A[3][3] = 1)
  void SetMatrixToZero4x4(double A[4][4]) const ;

  /// Set matrix to identity
  void SetMatrixToIdentity(double *A) const ;

  /// Set matrix to identity
  void SetMatrixToIdentity3x3(double A[3][3]) const ;

  /// Set matrix to identity 
  void SetMatrixToIdentity4x4(double A[4][4]) const ;

  /// Set matrix to rotation about x
  void SetMatrixToRotateAboutX(double *A, double theta) const ;

  /// Set matrix to rotation about y
  void SetMatrixToRotateAboutY(double *A, double theta) const ;

  /// Set matrix to rotation about z
  void SetMatrixToRotateAboutZ(double *A, double theta) const ;

  /// Set matrix to rotation about arbitrary axis
  /// Rodrigues formula is Q = I + sW + (1-c)W^2
  void SetMatrixToRotateAboutAxis(double *Q, const double *w, double theta) const ;

  /// Set matrix to translate by t (homo only)
  void SetMatrixToTranslate(double *A, const double *t) const ;

  /// Set matrix to transform corresponding to vector product by u
  /// A =  0  -uz  uy
  ///      uz  0  -ux
  ///     -uy  ux  0
  void SetMatrixToVectorProductTransform(double *A, const double *u) const ;

  /// Set matrix row to vector (columns 0-2 only)
  void SetMatrixRowToVector(double *A, int rowId, const double *u) const ;

  /// Set matrix row to vector
  void SetMatrixRowToVector3x3(double A[3][3], int rowId, const double *u) const ;

  /// Set matrix row to vector (columns 0-2 only)
  void SetMatrixRowToVector4x4(double A[4][4], int rowId, const double *u) const ;

  /// Set matrix column to vector (rows 0-2 only)
  void SetMatrixColumnToVector(double *A, int colId, const double *u) const ;

  /// Set matrix column to vector
  void SetMatrixColumnToVector3x3(double A[3][3], int colId, const double *u) const ;

  /// Set matrix column to vector (rows 0-2 only)
  void SetMatrixColumnToVector4x4(double A[4][4], int colId, const double *u) const ;

  /// Set matrix rows to vectors (rows and columns 0-2 only)
  void SetMatrixRowsToVectors(double *A, const double *u, const double *v, const double *w) const ;

  /// Set matrix rows to vectors (rows and columns 0-2 only)
  void SetMatrixRowsToVectors3x3(double A[3][3], const double *u, const double *v, const double *w) const ;

  /// Set matrix rows to vectors (rows and columns 0-2 only)
  void SetMatrixRowsToVectors4x4(double A[4][4], const double *u, const double *v, const double *w) const ;

  /// Set matrix columns to vectors (rows and columns 0-2 only)
  void SetMatrixColumnsToVectors(double *A, const double *u, const double *v, const double *w) const ;

  /// Set matrix columns to vectors (rows and columns 0-2 only)
  void SetMatrixColumnsToVectors3x3(double A[3][3], const double *u, const double *v, const double *w) const ;

  /// Set matrix columns to vectors (rows and columns 0-2 only)
  void SetMatrixColumnsToVectors4x4(double A[4][4], const double *u, const double *v, const double *w) const ;

  /// Multiply matrix by scalar. \n
  /// If the matrix is homogeneous, the right column is not multiplied, so that \n
  /// we preserve the scaling relation S(A)*x = A*S(x), \n
  /// where x is a vector and S() is the homo scaling operation. \n
  /// The homogeneous definition does not commute, \n
  /// so S(A)*x != S(A*x) and A*S(B) != S(A)*B.
  void MultiplyMatrixByScalar(double s, const double *A, double *B) const ;   

  /// Multiply matrix by scalar
  void MultiplyMatrixByScalar3x3(double s, const double A[3][3], double B[3][3]) const ;   

  /// Multiply matrix by scalar. \n
  /// The right column is not multiplied, so that \n
  /// we preserve the scaling relation S(A)*x = A*S(x), \n
  /// where x is a vector and S() is the homo scaling operation. \n
  /// The homogeneous definition does not commute, \n
  /// so S(A)*x != S(A*x) and A*S(B) != S(A)*B.
  void MultiplyMatrixByScalar4x4(double s, const double A[4][4], double B[3][3]) const ;   

  /// Divide matrix by scalar.
  /// If the matrix is homogeneous, the right column is not divided, so that \n
  /// we preserve the scaling relation S(A)*x = A*S(x), \n
  /// where x is a vector and S() is the homo scaling operation. \n
  /// The homogeneous definition does not commute, \n
  /// so S(A)*x != S(A*x) and A*S(B) != S(A)*B.
  void DivideMatrixByScalar(double s, const double *A, double *B) const ;     

  /// Divide matrix by scalar.
  void DivideMatrixByScalar3x3(double s, const double A[3][3], double B[3][3]) const ;     

  /// Divide matrix by scalar.
  /// The right column is not divided, so that \n
  /// we preserve the scaling relation S(A)*x = A*S(x), \n
  /// where x is a vector and S() is the homo scaling operation. \n
  /// The homogeneous definition does not commute, \n
  /// so S(A)*x != S(A*x) and A*S(B) != S(A)*B.
  void DivideMatrixByScalar4x4(double s, const double A[4][4], double B[4][4]) const ;     

  /// Multiply matrix columns by scalars \n
  /// no. of scalars must equal no. of columns (3 or 4) \n
  /// This is useful for multiplying column eigenvectors by eigenvalues
  void MultiplyColumnsByScalars(const double *s, const double *A, double *B) const ;

  /// Multiply matrix columns by scalars \n
  /// no. of scalars must equal 3 \n
  /// This is useful for multiplying column eigenvectors by eigenvalues
  void MultiplyColumnsByScalars3x3(const double *s, const double A[3][3], double B[3][3]) const ;

  /// Multiply matrix columns by scalars \n
  /// no. of scalars must equal 4 \n
  /// This is useful for multiplying column eigenvectors by eigenvalues
  void MultiplyColumnsByScalars4x4(const double *s, const double A[4][4], double B[4][4]) const ;

  /// Multiply matrix rows by scalars \n
  /// no. of scalars must equal no. of rows (3 or 4)
  void MultiplyRowsByScalars(const double *s, const double *A, double *B) const ;

  /// Multiply matrix rows by scalars \n
  /// no. of scalars must equal 3
  void MultiplyRowsByScalars3x3(const double *s, const double A[3][3], double B[3][3]) const ;

  /// Multiply matrix rows by scalars \n
  /// no. of scalars must equal 4
  void MultiplyRowsByScalars4x4(const double *s, const double A[4][4], double B[4][4]) const ;

  /// Multiply vector by matrix
  void MultiplyMatrixByVector(const double *A, const double *v, double *Av) const ;

  /// Multiply vector by matrix
  void MultiplyMatrixByVector3x3(const double A[3][3], const double *v, double *Av) const ;

  /// Multiply vector by matrix
  void MultiplyMatrixByVector4x4(const double A[4][4], const double *v, double *Av) const ;

  /// Multiply matrix by matrix
  void MultiplyMatrixByMatrix(const double *A, const double *B, double *AB) const ;

  /// Multiply matrix by matrix
  void MultiplyMatrixByMatrix3x3(const double A[3][3], const double B[3][3], double AB[3][3]) const ;

  /// Multiply matrix by matrix
  void MultiplyMatrixByMatrix4x4(const double A[4][4], const double B[4][4], double AB[4][4]) const ;

  /// Multiply 3 vector by homo matrix
  void MultiplyHomoMatrixBy3Vector(const double *A, const double v[3], double Av[3]) const ;

  /// Multiply 3 vector by homo matrix
  void MultiplyHomoMatrixBy3Vector(const double A[4][4], const double v[3], double Av[3]) const ;

  /// Transpose the matrix
  void Transpose(const double *A,  double *AT) const ;

  /// Transpose the matrix
  void Transpose3x3(const double A[3][3],  double AT[3][3]) const ;

  /// Transpose the matrix
  void Transpose4x4(const double A[4][4],  double AT[4][4]) const ;

  /// Get pointer to column
  double *GetColumn(double *A, int col) const ;

  /// print matrix
  void PrintMatrix(std::ostream& os, const double *A) const ;     

  /// print matrix
  void PrintMatrix3x3(std::ostream& os, const double A[3][3]) const ;     

  /// print matrix
  void PrintMatrix4x4(std::ostream& os, const double A[4][4]) const ;     


  //----------------------------------------------------------------------------
  /// Interface between 1D and 2D arrays
  //----------------------------------------------------------------------------

  /// Transpose 2D array
  void Transpose2DArray(const double arr1[3][3], double arr2[3][3]) const ;

  /// copy 2D array to matrix \n
  /// 2D array is in the form arr2D[row][col]
  void Copy2DArrayToMatrix3x3(const double arr2d[3][3],  double *A) const ;

  /// copy 2D array to matrix \n
  /// 2D array is in the form arr2D[row][col]
  void Copy2DArrayToMatrix4x4(const double arr2d[4][4],  double *A) const ;

  /// copy matrix to 2D array \n
  /// 2D array is in the form arr2D[row][col]
  void CopyMatrixTo2DArray3x3(const double *A,  double arr2d[3][3]) const ;

  /// copy matrix to 2D array \n
  /// 2D array is in the form arr2D[row][col]
  void CopyMatrixTo2DArray4x4(const double *A,  double arr2d[4][4]) const ;



  //----------------------------------------------------------------------------
  /// protected methods
  //----------------------------------------------------------------------------

protected:
  vtkALBAMatrixVectorMath() ;  ///< constructor
  ~vtkALBAMatrixVectorMath() ; ///< deconstructor

  bool m_homogeneous ;  ///< flag which switches homogeneous mode on or off

} ;



#endif

