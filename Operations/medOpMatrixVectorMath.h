/*=========================================================================

 Program: MAF2Medical
 Module: medOpMatrixVectorMath
 Authors: Nigel McFarlane
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpMatrixVectorMath_h
#define __medOpMatrixVectorMath_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include <ostream>


//------------------------------------------------------------------------------
/// Simple matrix and vector arithmetic. \n
/// This is not itself a vector or matrix; it is only a set of useful methods, \n
/// and contains no internal data except a flag for homogeneous mode. \n\n
///
/// These methods are for vectors and matrices with dimension 3 or 4x4 homogeneous. \n
/// Use SetHomogeneous() to set the homogeneous mode (default is off).\n
/// NB Many methods assume that the homogenous coord h = 1. \n\n
/// 
/// Matrices are in column-major 1D array format, which is consistent with OpenGL:  \n
/// 0 3 6  or  0  4  8  12  \n
/// 1 4 7      1  5  9  13  \n
/// 2 5 8      2  6  10 14  \n
///            3  7  11 15  \n\n
///
/// Methods are provided to convert between 1D array and 2D array form. \n
/// 2D arrays are assumed to be in the familiar row-major form arr2D[row][col], which is consistent with vtkMath. \n\n
/// 
/// Matrix convention in this code: i is the row, j is the column, and k is the 1D array index. \n
/// The output matrix can be the same as the input unless stated otherwise. \n
//------------------------------------------------------------------------------
class MAF_EXPORT medOpMatrixVectorMath{
public:
  /// Constructor.
  medOpMatrixVectorMath(bool homogeneous = false) : m_Homogeneous(homogeneous) {}

  /// Set to false for dims = 3, set to true for homogeneous dims = 4
  void SetHomogeneous(bool homogeneous) {m_Homogeneous = homogeneous ;}

  //----------------------------------------------------------------------------
  /// Vector methods
  //----------------------------------------------------------------------------

  /// Divide vector by homo coord
  void DivideVectorByHomoCoord(double *a) const ;  

  /// Set vector to zero
  void SetVectorToZero(double *a)  const ;

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
  void DivideVectorByScalar(double k, const double *a, double *b) const ;     

  /// Add vectors: a + b = c
  void AddVectors(const double *a, const double *b, double *c) const ;              

  /// Subtract vectors: a - b = c
  void SubtractVectors(const double *a, const double *b, double *c) const ;   

  /// Add multiple of vector: a + s*b = c
  void AddMultipleOfVector(const double *a, double s, double *b, double *c) ;

  /// Subtract multiple of vector: a - s*b = c
  void SubtractMultipleOfVector(const double *a, double s, double *b, double *c) ;

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

  /// print vector
  void PrintVector(std::ostream& os, const double *a) const ;    



  //----------------------------------------------------------------------------
  /// Matrix methods
  //----------------------------------------------------------------------------

  /// Divide matrix  by homo coord
  void DivideMatrixByHomoCoord(double *A) const ;  

  /// Set matrix to zero
  void SetMatrixToZero(double *A) const ;

  /// Set matrix to identity
  void SetMatrixToIdentity(double *A) const ;

  /// Set matrix row to vector
  void SetMatrixRowToVector(double *A, int rowId, double *u) ;

  /// Set matrix column to vector
  void SetMatrixColumnToVector(double *A, int colId, double *u) ;

  /// Set matrix rows to vectors
  void SetMatrixRowsToVectors(double *A, double *u, double *v, double *w) ;

  /// Set matrix columns to vectors
  void SetMatrixColumnsToVectors(double *A, double *u, double *v, double *w) ;

  /// Multiply matrix by scalar
  void MultiplyMatrixByScalar(double s, const double *A, double *B) const ;   

  /// Divide matrix by scalar.
  void DivideMatrixByScalar(double s, const double *A, double *B) const ;     

  /// Multiply matrix columns by scalars \n
  /// no. of scalars must equal no. of columns (3 or 4) \n
  /// This is useful for multiplying column eigenvectors by eigenvalues
  void MultiplyColumnsByScalars(const double *s, const double *A, double *B) const ;

  /// Multiply matrix rows by scalars \n
  /// no. of scalars must equal no. of rows (3 or 4)
  void MultiplyRowsByScalars(const double *s, const double *A, double *B) const ;

  /// Multiply vector by matrix
  void MultiplyMatrixByVector(const double *A, const double *v, double *Av) const ;

  /// Multiply matrix by matrix
  void MultiplyMatrixByMatrix(const double *A, const double *B, double *AB) const ;

  /// Transpose the matrix
  void Transpose(const double *A,  double *AT) const ;

  /// Get pointer to column
  double *GetColumn(double *A, int col) const ;

  /// print matrix
  void PrintMatrix(std::ostream& os, const double *A) const ;     


  //----------------------------------------------------------------------------
  /// Interface with 2D arrays
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

private:
  bool m_Homogeneous ;  ///< flag which switches homogeneous mode on or off

} ;



#endif

