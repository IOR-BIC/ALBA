/*=========================================================================

 Program: MAF2
 Module: medOpMatrixVectorMath
 Authors: Nigel McFarlane
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Simple matrix and vector maths
//------------------------------------------------------------------------------

#include "medOpMatrixVectorMath.h"
#include <cmath>

#include <ostream>




//------------------------------------------------------------------------------
// Divide vector by homogeneous coord
void medOpMatrixVectorMath::DivideVectorByHomoCoord(double *a) const 
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    a[i] /= a[3] ;
  a[3] = 1.0 ;
}


//------------------------------------------------------------------------------
// Set vector to zero
void medOpMatrixVectorMath::SetVectorToZero(double *a) const
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    a[i] = 0.0 ;

  if (m_Homogeneous)
    a[3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Magnitude of vector.
double medOpMatrixVectorMath::MagnitudeOfVector(const double *a) const
//------------------------------------------------------------------------------
{
  int i ;
  double sumsq ;

  for (i = 0, sumsq = 0.0 ;  i < 3 ;  i++)
    sumsq += a[i]*a[i] ;
  sumsq = sqrt(sumsq) ;
  return sumsq ;
}




//------------------------------------------------------------------------------
// Normalize vector
void medOpMatrixVectorMath::NormalizeVector(double *a) const
//------------------------------------------------------------------------------
{
  double norm = MagnitudeOfVector(a) ;
  for (int i = 0 ;  i < 3 ;  i++)
    a[i] /= norm ;
}



//------------------------------------------------------------------------------
// Normalize vector
void medOpMatrixVectorMath::NormalizeVector(const double *a, double *b) const
//------------------------------------------------------------------------------
{
  double norm = MagnitudeOfVector(a) ;
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = a[i] / norm ;

  if (m_Homogeneous)
    b[3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Invert vector (multiply by -1)
void medOpMatrixVectorMath::InvertVector(double *a) const
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    a[i] *= -1.0 ;
}



//------------------------------------------------------------------------------
// Invert vector (multiply by -1)
void medOpMatrixVectorMath::InvertVector(const double *a, double *b) const
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = -a[i] ;

  if (m_Homogeneous)
    b[3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Multiply vector by scalar
void medOpMatrixVectorMath::MultiplyVectorByScalar(double s, const double *a, double *b) const
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = s*a[i] ;

  if (m_Homogeneous)
    b[3] = 1.0 ;
}


//------------------------------------------------------------------------------
// Divide vector by scalar
void medOpMatrixVectorMath::DivideVectorByScalar(double s, const double *a, double *b) const
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = a[i] / s ;

  if (m_Homogeneous)
    b[3] = 1.0 ;
}

//------------------------------------------------------------------------------
// Add vectors: a + b = c 
void medOpMatrixVectorMath::AddVectors(const double *a, const double *b, double *c) const
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    c[i] = a[i] + b[i] ;

  if (m_Homogeneous)
    c[3] = 1.0 ;
}

//------------------------------------------------------------------------------
// Subtract vectors: a - b = c 
void medOpMatrixVectorMath::SubtractVectors(const double *a, const double *b, double *c) const
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    c[i] = a[i] - b[i] ;

  if (m_Homogeneous)
    c[3] = 1.0 ;
}




//------------------------------------------------------------------------------
// Add multiple of vector: a + s*b = c
void medOpMatrixVectorMath::AddMultipleOfVector(const double *a, double s, double *b, double *c)
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    c[i] = a[i] + s*b[i] ;

  if (m_Homogeneous)
    c[3] = 1.0 ;
}


//------------------------------------------------------------------------------
// Subtract multiple of vector: a - s*b = c
void medOpMatrixVectorMath::SubtractMultipleOfVector(const double *a, double s, double *b, double *c)
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    c[i] = a[i] - s*b[i] ;

  if (m_Homogeneous)
    c[3] = 1.0 ;
}


//------------------------------------------------------------------------------
// Dot product
double medOpMatrixVectorMath::DotProduct(const double *a, const double *b) const
//------------------------------------------------------------------------------
{
  int i ;
  double dotprod ;

  for (i = 0, dotprod = 0.0 ;  i < 3 ;  i++)
    dotprod += a[i]*b[i] ;

  return dotprod ;
}


//------------------------------------------------------------------------------
// Vector Product a^b = c
void medOpMatrixVectorMath::VectorProduct(const double *a, const double *b, double *c) const
//------------------------------------------------------------------------------
{
  c[0] =   a[1]*b[2] - a[2]*b[1] ;
  c[1] = -(a[0]*b[2] - a[2]*b[0]) ;
  c[2] =   a[0]*b[1] - a[1]*b[0] ;

  if (m_Homogeneous)
    c[3] = 1.0 ;
}


//------------------------------------------------------------------------------
// Copy vector: b = a
void medOpMatrixVectorMath::CopyVector(const double *a, double *b) const
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = a[i] ;

  if (m_Homogeneous)
    b[3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Are vectors equal.
bool medOpMatrixVectorMath::Equals(const double *a, const double *b, double tol) const
//------------------------------------------------------------------------------
{
  bool eq0 = ((a[0] > b[0]-tol) && (a[0] < b[0]+tol)) ;
  bool eq1 = ((a[1] > b[1]-tol) && (a[1] < b[1]+tol)) ;
  bool eq2 = ((a[2] > b[2]-tol) && (a[2] < b[2]+tol)) ;
  return (eq0 && eq1 && eq2) ;
}



//------------------------------------------------------------------------------
// Calculate two arbitrary vectors v and w which are normal to the given vector u.
// u, v and w form a right handed coordinate system.
// The vectors are not normalised.
void medOpMatrixVectorMath::CalculateNormalsToU(const double *u,  double *v,  double *w) const
//------------------------------------------------------------------------------
{
  double x = u[0] ;
  double y = u[1] ;
  double z = u[2] ;

  // get component of axis direction with smallest magnitude
  int minComponent ;
  if ((fabs(x) <= fabs(y)) && (fabs(x) <= fabs(z)))
    minComponent = 0 ;
  else if ((fabs(y) <= fabs(x)) && (fabs(y) <= fabs(z)))
    minComponent = 1 ;
  else
    minComponent = 2 ;

  // create vector v which is normal to the u axis
  switch(minComponent){
    case 0:
      v[0] = -(y*y + z*z) ;
      v[1] = x*y ;
      v[2] = x*z ;
      break ;
    case 1:
      v[0] = y*x ;
      v[1] = -(x*x + z*z) ;
      v[2] = y*z ;
      break ;
    case 2:
      v[0] = z*x ;
      v[1] = z*y ;
      v[2] = -(x*x + y*y) ;
      break ;
  }

  // third vector is cross product w = u ^ v
  w[0] = u[1]*v[2] - u[2]*v[1] ;
  w[1] = u[2]*v[0] - u[0]*v[2] ;
  w[2] = u[0]*v[1] - u[1]*v[0] ;


  if (m_Homogeneous){
    v[3] = 1.0 ;
    w[3] = 1.0 ;
  }
}



//------------------------------------------------------------------------------
// Calculate two arbitrary vectors u and w which are normal to the given vector v.
// u, v and w form a right handed coordinate system.
// The vectors are not normalised.
void medOpMatrixVectorMath::CalculateNormalsToV(double *u,  const double *v,  double *w) const
//------------------------------------------------------------------------------
{
  double x = v[0] ;
  double y = v[1] ;
  double z = v[2] ;

  // get component of axis direction with smallest magnitude
  int minComponent ;
  if ((fabs(x) <= fabs(y)) && (fabs(x) <= fabs(z)))
    minComponent = 0 ;
  else if ((fabs(y) <= fabs(x)) && (fabs(y) <= fabs(z)))
    minComponent = 1 ;
  else
    minComponent = 2 ;

  // create vector u which is normal to the v axis
  switch(minComponent){
    case 0:
      u[0] = -(y*y + z*z) ;
      u[1] = x*y ;
      u[2] = x*z ;
      break ;
    case 1:
      u[0] = y*x ;
      u[1] = -(x*x + z*z) ;
      u[2] = y*z ;
      break ;
    case 2:
      u[0] = z*x ;
      u[1] = z*y ;
      u[2] = -(x*x + y*y) ;
      break ;
  }

  // third vector is cross product w = u ^ v
  w[0] = u[1]*v[2] - u[2]*v[1] ;
  w[1] = u[2]*v[0] - u[0]*v[2] ;
  w[2] = u[0]*v[1] - u[1]*v[0] ;


  if (m_Homogeneous){
    u[3] = 1.0 ;
    w[3] = 1.0 ;
  }
}



//------------------------------------------------------------------------------
// Calculate two arbitrary vectors u and v which are normal to the given vector w.
// u, v and w form a right handed coordinate system.
// The vectors are not normalised.
void medOpMatrixVectorMath::CalculateNormalsToW(double *u,  double *v,  const double *w) const
//------------------------------------------------------------------------------
{
  double x = w[0] ;
  double y = w[1] ;
  double z = w[2] ;

  // get component of axis direction with smallest magnitude
  int minComponent ;
  if ((fabs(x) <= fabs(y)) && (fabs(x) <= fabs(z)))
    minComponent = 0 ;
  else if ((fabs(y) <= fabs(x)) && (fabs(y) <= fabs(z)))
    minComponent = 1 ;
  else
    minComponent = 2 ;

  // create vector v which is normal to the w axis
  switch(minComponent){
    case 0:
      v[0] = -(y*y + z*z) ;
      v[1] = x*y ;
      v[2] = x*z ;
      break ;
    case 1:
      v[0] = y*x ;
      v[1] = -(x*x + z*z) ;
      v[2] = y*z ;
      break ;
    case 2:
      v[0] = z*x ;
      v[1] = z*y ;
      v[2] = -(x*x + y*y) ;
      break ;
  }

  // third vector is cross product u = v ^ w
  u[0] = v[1]*w[2] - v[2]*w[1] ;
  u[1] = v[2]*w[0] - v[0]*w[2] ;
  u[2] = v[0]*w[1] - v[1]*w[0] ;


  if (m_Homogeneous){
    u[3] = 1.0 ;
    v[3] = 1.0 ;
  }
}



//------------------------------------------------------------------------------
// Distance between two vectors
double medOpMatrixVectorMath::Distance(const double *a,  const double *b) const
//------------------------------------------------------------------------------
{
  double dx = a[0]-b[0] ;
  double dy = a[1]-b[1] ;
  double dz = a[2]-b[2] ;
  return sqrt(dx*dx + dy*dy + dz*dz) ;
}


//------------------------------------------------------------------------------
// Distance squared between two vectors
double medOpMatrixVectorMath::DistanceSquared(const double *a,  const double *b) const
//------------------------------------------------------------------------------
{
  double dx = a[0]-b[0] ;
  double dy = a[1]-b[1] ;
  double dz = a[2]-b[2] ;
  return (dx*dx + dy*dy + dz*dz) ;
}





//------------------------------------------------------------------------------
// Print vector
void medOpMatrixVectorMath::PrintVector(std::ostream& os, const double *a) const
//------------------------------------------------------------------------------
{
  if (m_Homogeneous){
    os << a[0] << "\t" << a[1] << "\t" << a[2] << "\t" << a[3] << std::endl ;
  }
  else{
    os << a[0] << "\t" << a[1] << "\t" << a[2] << std::endl ;
  }
}



//------------------------------------------------------------------------------
// Divide matrix by homogeneous coord
void medOpMatrixVectorMath::DivideMatrixByHomoCoord(double *A) const
//------------------------------------------------------------------------------
{
  for (int k = 0 ;  k < 15 ;  k++)
    A[k] /= A[15] ;
  A[15] = 1.0 ;
}


//------------------------------------------------------------------------------
// Set matrix to zero
void medOpMatrixVectorMath::SetMatrixToZero(double *A) const
//------------------------------------------------------------------------------
{
  if (m_Homogeneous){
    for (int k = 0 ;  k < 16 ;  k++)
      A[k] = 0.0 ;
    A[15] = 1.0 ;
  }
  else{
    for (int k = 0 ;  k < 9 ;  k++)
      A[k] = 0.0 ;
  }
}


//------------------------------------------------------------------------------
// Set matrix to identity
void medOpMatrixVectorMath::SetMatrixToIdentity(double *A) const
//------------------------------------------------------------------------------
{
  if (m_Homogeneous){
    for (int k = 0 ;  k < 16 ;  k++)
      A[k] = 0.0 ;
    A[0] = 1.0 ;
    A[5] = 1.0 ;
    A[10] = 1.0 ;
    A[15] = 1.0 ;
  }
  else{
    for (int k = 0 ;  k < 9 ;  k++)
      A[k] = 0.0 ;
    A[0] = 1.0 ;
    A[4] = 1.0 ;
    A[8] = 1.0 ;
  }
}



//------------------------------------------------------------------------------
// Set matrix row to vector
void medOpMatrixVectorMath::SetMatrixRowToVector(double *A, int rowId, double *u)
//------------------------------------------------------------------------------
{
  int k, ii ;

  if (m_Homogeneous){
    for (ii = 0, k = 4*rowId ;  ii < 3 ;  ii++, k++)
      A[k] = u[ii] ;
  }
  else{
    for (ii = 0, k = 3*rowId ;  ii < 3 ;  ii++, k++)
      A[k] = u[ii] ;
  }
}


//------------------------------------------------------------------------------
// Set matrix column to vector
void medOpMatrixVectorMath::SetMatrixColumnToVector(double *A, int colId, double *u)
//------------------------------------------------------------------------------
{
  int k, ii ;

  if (m_Homogeneous){
    for (ii = 0, k = colId ;  ii < 3 ;  ii++, k+=3)
      A[k] = u[ii] ;
  }
  else{
    for (ii = 0, k = colId ;  ii < 3 ;  ii++, k+=4)
      A[k] = u[ii] ;
  }
}



//------------------------------------------------------------------------------
// Set matrix rows to vectors
void medOpMatrixVectorMath::SetMatrixRowsToVectors(double *A, double *u, double *v, double *w)
//------------------------------------------------------------------------------
{
  SetMatrixRowToVector(A, 0, u) ;
  SetMatrixRowToVector(A, 1, v) ;
  SetMatrixRowToVector(A, 2, w) ;
}


//------------------------------------------------------------------------------
// Set matrix columns to vectors
void medOpMatrixVectorMath::SetMatrixColumnsToVectors(double *A, double *u, double *v, double *w)
//------------------------------------------------------------------------------
{
  SetMatrixColumnToVector(A, 0, u) ;
  SetMatrixColumnToVector(A, 1, v) ;
  SetMatrixColumnToVector(A, 2, w) ;
}


//------------------------------------------------------------------------------
// Multiply matrix by scalar
void medOpMatrixVectorMath::MultiplyMatrixByScalar(double s, const double *A, double *B) const
//------------------------------------------------------------------------------
{
  if (m_Homogeneous){
    for (int k = 0 ;  k < 15 ;  k++)
      B[k] = s*A[k] ;
    B[15] = 1.0 ;
  }
  else{
    for (int k = 0 ;  k < 9 ;  k++)
      B[k] = s*A[k] ;
  }
}



//------------------------------------------------------------------------------
// Divide matrix by scalar.
void medOpMatrixVectorMath::DivideMatrixByScalar(double s, const double *A, double *B) const
//------------------------------------------------------------------------------
{
  if (m_Homogeneous){
    for (int k = 0 ;  k < 15 ;  k++)
      B[k] = A[k]/s ;
    B[15] = 1.0 ;
  }
  else{
    for (int k = 0 ;  k < 9 ;  k++)
      B[k] = A[k]/s ;
  }
}    



//------------------------------------------------------------------------------
// Multiply matrix columns by scalars
// no. of scalars must equal no. of columns (3 or 4)
// This is useful for multiplying column eigenvectors by eigenvalues
void medOpMatrixVectorMath::MultiplyColumnsByScalars(const double *s, const double *A, double *B) const
//------------------------------------------------------------------------------
{
  int k = 0 ;

  if (m_Homogeneous){
    for (int j = 0 ;  j < 4 ;  j++)
      for (int i = 0 ;  i < 4 ;  i++, k++)
        B[k] = s[j]*A[k] ;
  }
  else{
    for (int j = 0 ;  j < 3 ;  j++)
      for (int i = 0 ;  i < 3 ;  i++, k++)
        B[k] = s[j]*A[k] ;
  }
}



//------------------------------------------------------------------------------
// Multiply matrix rows by scalars
// no. of scalars must equal no. of rows (3 or 4)
void medOpMatrixVectorMath::MultiplyRowsByScalars(const double *s, const double *A, double *B) const
//------------------------------------------------------------------------------
{
  int k = 0 ;
  if (m_Homogeneous){
    for (int j = 0 ;  j < 4 ;  j++)
      for (int i = 0 ;  i < 4 ;  i++, k++)
        B[k] = s[i]*A[k] ;
  }
  else{
    for (int j = 0 ;  j < 3 ;  j++)
      for (int i = 0 ;  i < 3 ;  i++, k++)
        B[k] = s[i]*A[k] ;
  }
}



//------------------------------------------------------------------------------
// Multiply vector by matrix
void medOpMatrixVectorMath::MultiplyMatrixByVector(const double *A, const double *v, double *Av) const
//------------------------------------------------------------------------------
{
  if (m_Homogeneous){
    Av[0] = A[0]*v[0] + A[4]*v[1] + A[8]*v[2] + A[12]*v[3] ;
    Av[1] = A[1]*v[0] + A[5]*v[1] + A[9]*v[2] + A[13]*v[3] ;
    Av[2] = A[2]*v[0] + A[6]*v[1] + A[10]*v[2] + A[14]*v[3] ;
    Av[3] = A[3]*v[0] + A[7]*v[1] + A[11]*v[2] + A[15]*v[3] ;
  }
  else{
    Av[0] = A[0]*v[0] + A[3]*v[1] + A[6]*v[2] ;
    Av[1] = A[1]*v[0] + A[4]*v[1] + A[7]*v[2] ;
    Av[2] = A[2]*v[0] + A[5]*v[1] + A[8]*v[2] ;
  }
}



//------------------------------------------------------------------------------
// Multiply matrix by matrix
void medOpMatrixVectorMath::MultiplyMatrixByMatrix(const double *A, const double *B, double *AB) const
//------------------------------------------------------------------------------
{
  if (m_Homogeneous){
    AB[0] =  A[0]*B[0] + A[4]*B[1] + A[8]*B[2] + A[12]*B[3] ;
    AB[1] =  A[1]*B[0] + A[5]*B[1] + A[9]*B[2] + A[13]*B[3] ;
    AB[2] =  A[2]*B[0] + A[6]*B[1] + A[10]*B[2] + A[14]*B[3] ;
    AB[3] =  A[3]*B[0] + A[7]*B[1] + A[11]*B[2] + A[15]*B[3] ;

    AB[4] =  A[0]*B[4] + A[4]*B[5] + A[8]*B[6] + A[12]*B[7] ;
    AB[5] =  A[1]*B[4] + A[5]*B[5] + A[9]*B[6] + A[13]*B[7] ;
    AB[6] =  A[2]*B[4] + A[6]*B[5] + A[10]*B[6] + A[14]*B[7] ;
    AB[7] =  A[3]*B[4] + A[7]*B[5] + A[11]*B[6] + A[15]*B[7] ;

    AB[8] =  A[0]*B[8] + A[4]*B[9] + A[8]*B[10] + A[12]*B[11] ;
    AB[9] =  A[1]*B[8] + A[5]*B[9] + A[9]*B[10] + A[13]*B[11] ;
    AB[10] = A[2]*B[8] + A[6]*B[9] + A[10]*B[10] + A[14]*B[11] ;
    AB[11] = A[3]*B[8] + A[7]*B[9] + A[11]*B[10] + A[15]*B[11] ;

    AB[12] = A[0]*B[12] + A[4]*B[13] + A[8]*B[14] + A[12]*B[15] ;
    AB[13] = A[1]*B[12] + A[5]*B[13] + A[9]*B[14] + A[13]*B[15] ;
    AB[14] = A[2]*B[12] + A[6]*B[13] + A[10]*B[14] + A[14]*B[15] ;
    AB[15] = A[3]*B[12] + A[7]*B[13] + A[11]*B[14] + A[15]*B[15] ;
  }
  else{
    AB[0] = A[0]*B[0] + A[3]*B[1] + A[6]*B[2] ;
    AB[1] = A[1]*B[0] + A[4]*B[1] + A[7]*B[2] ;
    AB[2] = A[2]*B[0] + A[5]*B[1] + A[8]*B[2] ;

    AB[3] = A[0]*B[3] + A[3]*B[4] + A[6]*B[5] ;
    AB[4] = A[1]*B[3] + A[4]*B[4] + A[7]*B[5] ;
    AB[5] = A[2]*B[3] + A[5]*B[4] + A[8]*B[5] ;

    AB[6] = A[0]*B[6] + A[3]*B[7] + A[6]*B[8] ;
    AB[7] = A[1]*B[6] + A[4]*B[7] + A[7]*B[8] ;
    AB[8] = A[2]*B[6] + A[5]*B[7] + A[8]*B[8] ;
  }
}



//------------------------------------------------------------------------------
// Transpose the matrix
void medOpMatrixVectorMath::Transpose(const double *A,  double *AT) const
//------------------------------------------------------------------------------
{
  int k = 0 ;
  double temp[16] ;

  if (m_Homogeneous){
    for (int j = 0 ;  j < 4 ;  j++)
      for (int i = 0 ;  i < 4 ;  i++, k++)
        temp[k] = A[j+4*i] ;
    for (k = 0 ;  k < 16 ;  k++)
      AT[k] = temp[k] ;
  }
  else{
    for (int j = 0 ;  j < 3 ;  j++)
      for (int i = 0 ;  i < 3 ;  i++, k++)
        temp[k] = A[j+3*i] ;
    for (k = 0 ;  k < 9 ;  k++)
      AT[k] = temp[k] ;
  }
}



//------------------------------------------------------------------------------
// Get pointer to column
double *medOpMatrixVectorMath::GetColumn(double *A, int col) const
//------------------------------------------------------------------------------
{
  if (m_Homogeneous)
    return A + 3*col ;
  else
    return A + 4*col ;
}


//------------------------------------------------------------------------------
// Print matrix
void medOpMatrixVectorMath::PrintMatrix(std::ostream& os, const double *A) const
//------------------------------------------------------------------------------
{
  if (m_Homogeneous){
    os << A[0] << "\t" << A[4] << "\t" << A[8] << "\t" << A[12] << std::endl ;
    os << A[1] << "\t" << A[5] << "\t" << A[9] << "\t" << A[13] << std::endl ;
    os << A[2] << "\t" << A[6] << "\t" << A[10] << "\t" << A[14] << std::endl ;
    os << A[3] << "\t" << A[7] << "\t" << A[11] << "\t" << A[15] << std::endl ;
    os << std::endl ;
  }
  else{
    os << A[0] << "\t" << A[3] << "\t" << A[6] << std::endl ;
    os << A[1] << "\t" << A[4] << "\t" << A[7] << std::endl ;
    os << A[2] << "\t" << A[5] << "\t" << A[8] << std::endl ;
    os << std::endl ;
  }
}



//------------------------------------------------------------------------------
// Transpose 2D array
void medOpMatrixVectorMath::Transpose2DArray(const double arr1[3][3], double arr2[3][3]) const
//------------------------------------------------------------------------------
{
//  double temp[4][4] ;

  if (m_Homogeneous){
    /*for (int i = 0 ;  i < 4 ;  i++)
      for (int j = 0 ;  j < 4 ;  j++)
        temp[i][j] = arr1[j][i] ;*/
    for (int i = 0 ;  i < 4 ;  i++)
      for (int j = 0 ;  j < 4 ;  j++)
        arr2[i][j] = arr1[j][i] ;
  }
  else{
    /*for (int i = 0 ;  i < 3 ;  i++)
      for (int j = 0 ;  j < 3 ;  j++)
        temp[i][j] = arr1[j][i] ;*/
    for (int i = 0 ;  i < 3 ;  i++)
      for (int j = 0 ;  j < 3 ;  j++)
        arr2[i][j] = arr1[j][i] ;
  }
}


//------------------------------------------------------------------------------
// copy 2D array to matrix
void medOpMatrixVectorMath::Copy2DArrayToMatrix3x3(const double arr2d[3][3],  double *A) const
//------------------------------------------------------------------------------
{
  int k = 0 ;
  for (int j = 0 ;  j < 3 ;  j++)
    for (int i = 0 ;  i < 3 ;  i++, k++)
      A[k] = arr2d[i][j] ; 
}

//------------------------------------------------------------------------------
// copy 2D array to matrix
void medOpMatrixVectorMath::Copy2DArrayToMatrix4x4(const double arr2d[4][4],  double *A) const
//------------------------------------------------------------------------------
{
  int k = 0 ;
  for (int j = 0 ;  j < 4 ;  j++)
    for (int i = 0 ;  i < 4 ;  i++, k++)
      A[k] = arr2d[i][j] ; 
}

//------------------------------------------------------------------------------
// copy matrix to 2D array
void medOpMatrixVectorMath::CopyMatrixTo2DArray3x3(const double *A,  double arr2d[3][3]) const
//------------------------------------------------------------------------------
{
  int k = 0 ;
  for (int j = 0 ;  j < 3 ;  j++)
    for (int i = 0 ;  i < 3 ;  i++, k++)
      arr2d[i][j] = A[k] ; 
}

//------------------------------------------------------------------------------
// copy matrix to 2D array
void medOpMatrixVectorMath::CopyMatrixTo2DArray4x4(const double *A,  double arr2d[4][4]) const
//------------------------------------------------------------------------------
{
  int k = 0 ;
  for (int j = 0 ;  j < 4 ;  j++)
    for (int i = 0 ;  i < 4 ;  i++, k++)
      arr2d[i][j] = A[k] ; 
}




