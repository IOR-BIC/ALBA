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





#include "vtkObjectFactory.h"
#include "vtkObject.h"
#include "vtkALBAMatrixVectorMath.h"
#include <cmath>
#include <ostream>
#include "assert.h"




//------------------------------------------------------------------------------
// standard macros
vtkStandardNewMacro(vtkALBAMatrixVectorMath);
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
vtkALBAMatrixVectorMath::vtkALBAMatrixVectorMath() : m_homogeneous(false)
{
}


//------------------------------------------------------------------------------
// Destructor
vtkALBAMatrixVectorMath::~vtkALBAMatrixVectorMath()
  //------------------------------------------------------------------------------
{
}




//------------------------------------------------------------------------------
// Divide vector by homogeneous coord
void vtkALBAMatrixVectorMath::DivideVectorByHomoCoord(double *a) const 
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    a[i] /= a[3] ;
  a[3] = 1.0 ;
}


//------------------------------------------------------------------------------
// Set vector to zero
void vtkALBAMatrixVectorMath::SetVectorToZero(double *a) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    a[i] = 0.0 ;

  if (m_homogeneous)
    a[3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Set vector
void vtkALBAMatrixVectorMath::SetVector(double *a, double a0, double a1, double a2) const
  //------------------------------------------------------------------------------
{
  a[0] = a0 ;
  a[1] = a1 ;
  a[2] = a2 ;
 
  if (m_homogeneous)
    a[3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Magnitude of vector.
double vtkALBAMatrixVectorMath::MagnitudeOfVector(const double *a) const
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
void vtkALBAMatrixVectorMath::NormalizeVector(double *a) const
  //------------------------------------------------------------------------------
{
  double norm = MagnitudeOfVector(a) ;
  for (int i = 0 ;  i < 3 ;  i++)
    a[i] /= norm ;
}



//------------------------------------------------------------------------------
// Normalize vector
void vtkALBAMatrixVectorMath::NormalizeVector(const double *a, double *b) const
  //------------------------------------------------------------------------------
{
  double norm = MagnitudeOfVector(a) ;
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = a[i] / norm ;

  if (m_homogeneous)
    b[3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Invert vector (multiply by -1)
void vtkALBAMatrixVectorMath::InvertVector(double *a) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    a[i] *= -1.0 ;
}



//------------------------------------------------------------------------------
// Invert vector (multiply by -1)
void vtkALBAMatrixVectorMath::InvertVector(const double *a, double *b) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = -a[i] ;

  if (m_homogeneous)
    b[3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Multiply vector by scalar
void vtkALBAMatrixVectorMath::MultiplyVectorByScalar(double s, const double *a, double *b) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = s*a[i] ;

  if (m_homogeneous)
    b[3] = 1.0 ;
}


//------------------------------------------------------------------------------
// Divide vector by scalar
void vtkALBAMatrixVectorMath::DivideVectorByScalar(double s, const double *a, double *b) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = a[i] / s ;

  if (m_homogeneous)
    b[3] = 1.0 ;
}

//------------------------------------------------------------------------------
// Add vectors: a + b = c 
void vtkALBAMatrixVectorMath::AddVectors(const double *a, const double *b, double *c) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    c[i] = a[i] + b[i] ;

  if (m_homogeneous)
    c[3] = 1.0 ;
}

//------------------------------------------------------------------------------
// Subtract vectors: a - b = c 
void vtkALBAMatrixVectorMath::SubtractVectors(const double *a, const double *b, double *c) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    c[i] = a[i] - b[i] ;

  if (m_homogeneous)
    c[3] = 1.0 ;
}




//------------------------------------------------------------------------------
// Add multiple of vector: a + s*b = c
void vtkALBAMatrixVectorMath::AddMultipleOfVector(const double *a, double s, const double *b, double *c) const 
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    c[i] = a[i] + s*b[i] ;

  if (m_homogeneous)
    c[3] = 1.0 ;
}


//------------------------------------------------------------------------------
// Subtract multiple of vector: a - s*b = c
void vtkALBAMatrixVectorMath::SubtractMultipleOfVector(const double *a, double s, const double *b, double *c) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    c[i] = a[i] - s*b[i] ;

  if (m_homogeneous)
    c[3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Interpolate between vectors: (1-s)*a + s*b = c
// If s = 0, c = a
// If s = 1, c = b
void vtkALBAMatrixVectorMath::InterpolateVectors(double s, const double *a, const double *b, double *c) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    c[i] = a[i] + s*(b[i]-a[i]) ;

  if (m_homogeneous)
    c[3] = 1.0 ;
}


//------------------------------------------------------------------------------
// Dot product
double vtkALBAMatrixVectorMath::DotProduct(const double *a, const double *b) const
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
void vtkALBAMatrixVectorMath::VectorProduct(const double *a, const double *b, double *c) const
  //------------------------------------------------------------------------------
{
  c[0] =   a[1]*b[2] - a[2]*b[1] ;
  c[1] = -(a[0]*b[2] - a[2]*b[0]) ;
  c[2] =   a[0]*b[1] - a[1]*b[0] ;

  if (m_homogeneous)
    c[3] = 1.0 ;
}


//------------------------------------------------------------------------------
// Copy vector: b = a
void vtkALBAMatrixVectorMath::CopyVector(const double *a, double *b) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = a[i] ;

  if (m_homogeneous)
    b[3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Are vectors equal.
bool vtkALBAMatrixVectorMath::Equals(const double *a, const double *b, double tol) const
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
void vtkALBAMatrixVectorMath::CalculateNormalsToU(const double *u,  double *v,  double *w) const
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


  if (m_homogeneous){
    v[3] = 1.0 ;
    w[3] = 1.0 ;
  }
}



//------------------------------------------------------------------------------
// Calculate two arbitrary vectors u and w which are normal to the given vector v.
// u, v and w form a right handed coordinate system.
// The vectors are not normalised.
void vtkALBAMatrixVectorMath::CalculateNormalsToV(double *u,  const double *v,  double *w) const
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


  if (m_homogeneous){
    u[3] = 1.0 ;
    w[3] = 1.0 ;
  }
}



//------------------------------------------------------------------------------
// Calculate two arbitrary vectors u and v which are normal to the given vector w.
// u, v and w form a right handed coordinate system.
// The vectors are not normalised.
void vtkALBAMatrixVectorMath::CalculateNormalsToW(double *u,  double *v,  const double *w) const
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


  if (m_homogeneous){
    u[3] = 1.0 ;
    v[3] = 1.0 ;
  }
}



//------------------------------------------------------------------------------
// Distance between two vectors
double vtkALBAMatrixVectorMath::Distance(const double *a,  const double *b) const
  //------------------------------------------------------------------------------
{
  double dx = a[0]-b[0] ;
  double dy = a[1]-b[1] ;
  double dz = a[2]-b[2] ;
  return sqrt(dx*dx + dy*dy + dz*dz) ;
}


//------------------------------------------------------------------------------
// Distance squared between two vectors
double vtkALBAMatrixVectorMath::DistanceSquared(const double *a,  const double *b) const
  //------------------------------------------------------------------------------
{
  double dx = a[0]-b[0] ;
  double dy = a[1]-b[1] ;
  double dz = a[2]-b[2] ;
  return (dx*dx + dy*dy + dz*dz) ;
}


//------------------------------------------------------------------------------
// Mean of two vectors
void vtkALBAMatrixVectorMath::MeanVector(const double *a0, const double *a1, double *b) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = (a0[i] + a1[i]) / 2.0 ;
}


//------------------------------------------------------------------------------
// Mean of three vectors
void vtkALBAMatrixVectorMath::MeanVector(const double *a0, const double *a1, const double *a2, double *b) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = (a0[i] + a1[i] + a2[i]) / 3.0 ;
}


//------------------------------------------------------------------------------
// Mean of four vectors
void vtkALBAMatrixVectorMath::MeanVector(const double *a0, const double *a1, const double *a2, const double *a3, double *b) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    b[i] = (a0[i] + a1[i] + a2[i] + a3[i]) / 4.0 ;
}



//------------------------------------------------------------------------------
// Copy homo 4 vector to 3 vector
void vtkALBAMatrixVectorMath::CopyHomoVectorToVector(const double *aHomo, double *a) const
  //------------------------------------------------------------------------------
{
  if (aHomo[3] == 1.0){
    a[0] = aHomo[0] ;
    a[1] = aHomo[1] ;
    a[2] = aHomo[2] ;
  }
  else{
    a[0] = aHomo[0] / aHomo[3] ;
    a[1] = aHomo[1] / aHomo[3] ;
    a[2] = aHomo[2] / aHomo[3] ;
  }
}


//------------------------------------------------------------------------------
// Copy 3 vector to homo 4 vector
void vtkALBAMatrixVectorMath::CopyVectorToHomoVector(const double *a, double *aHomo) const
  //------------------------------------------------------------------------------
{
  aHomo[0] = a[0] ;
  aHomo[1] = a[1] ;
  aHomo[2] = a[2] ;
  aHomo[3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Print vector
void vtkALBAMatrixVectorMath::PrintVector(std::ostream& os, const double *a) const
  //------------------------------------------------------------------------------
{
  if (m_homogeneous){
    os << a[0] << "\t" << a[1] << "\t" << a[2] << "\t" << a[3] << std::endl ;
  }
  else{
    os << a[0] << "\t" << a[1] << "\t" << a[2] << std::endl ;
  }
}






//------------------------------------------------------------------------------
// Copy matrix
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::CopyMatrix(const double *A, double *B)
{
  if (m_homogeneous){
    for (int k = 0 ;  k < 16 ;  k++)
      B[k] = A[k] ;
  }
  else{
    for (int k = 0 ;  k < 9 ;  k++)
      B[k] = A[k] ;
  }
}



//------------------------------------------------------------------------------
// Copy matrix A[9] to homo matrix B[16]
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::CopyMatrixToHomoMatrix(const double *A, double *B)
{
  bool hsave = m_homogeneous ;
  SetHomogeneous(true) ;
  SetMatrixToZero(B) ;

  int kb[9] = {0,1,2,4,5,6,8,9,10} ;
  for (int ka = 0 ;  ka < 9 ;  ka++)
    B[kb[ka]] = A[ka] ;

  SetHomogeneous(hsave) ;
}



//------------------------------------------------------------------------------
// Copy matrix
void vtkALBAMatrixVectorMath::CopyMatrix3x3(const double A[3][3], double B[3][3])
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    for (int j = 0 ;  j < 3 ;  j++)
      B[i][j] = A[i][j] ;
}


//------------------------------------------------------------------------------
// Copy matrix
void vtkALBAMatrixVectorMath::CopyMatrix4x4(const double A[4][4], double B[4][4])
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 4 ;  i++)
    for (int j = 0 ;  j < 4 ;  j++)
      B[i][j] = A[i][j] ;
}



//------------------------------------------------------------------------------
// Divide matrix by homogeneous coord
void vtkALBAMatrixVectorMath::DivideMatrixByHomoCoord(double *A) const
  //------------------------------------------------------------------------------
{
  for (int k = 0 ;  k < 15 ;  k++)
    A[k] /= A[15] ;
  A[15] = 1.0 ;
}



//------------------------------------------------------------------------------
// Divide matrix by homogeneous coord
void vtkALBAMatrixVectorMath::DivideMatrixByHomoCoord(double A[4][4]) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 4 ;  i++)
    for (int j = 0 ;  j < 4 ; j++)
      A[i][j] /= A[3][3] ;
  A[3][3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Set matrix to zero
void vtkALBAMatrixVectorMath::SetMatrixToZero(double *A) const
  //------------------------------------------------------------------------------
{
  if (m_homogeneous){
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
// Set matrix to zero
void vtkALBAMatrixVectorMath::SetMatrixToZero3x3(double A[3][3]) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    for (int j = 0 ;  j < 3 ;  j++)
      A[i][j] = 0.0 ;
}



//------------------------------------------------------------------------------
// Set matrix to zero (except for homogeneous A[3][3] = 1)
void vtkALBAMatrixVectorMath::SetMatrixToZero4x4(double A[4][4]) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 4 ;  i++)
    for (int j = 0 ;  j < 4 ;  j++)
      A[i][j] = 0.0 ;
  A[3][3] = 1.0 ;
}



//------------------------------------------------------------------------------
// Set matrix to identity
void vtkALBAMatrixVectorMath::SetMatrixToIdentity(double *A) const
  //------------------------------------------------------------------------------
{
  if (m_homogeneous){
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
// Set matrix to identity
void vtkALBAMatrixVectorMath::SetMatrixToIdentity3x3(double A[3][3]) const
  //------------------------------------------------------------------------------
{
  SetMatrixToZero3x3(A) ;
  for (int i = 0 ;  i < 3 ;  i++)
    A[i][i] = 1.0 ;
}



//------------------------------------------------------------------------------
// Set matrix to identity 
void vtkALBAMatrixVectorMath::SetMatrixToIdentity4x4(double A[4][4]) const
  //------------------------------------------------------------------------------
{
  SetMatrixToZero4x4(A) ;
  for (int i = 0 ;  i < 4 ;  i++)
    A[i][i] = 1.0 ;
}



//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::SetMatrixToRotateAboutX(double *A, double theta) const
  //------------------------------------------------------------------------------
{
  SetMatrixToIdentity(A) ;
  double s = (double)sin(theta);
  double c = (double)cos(theta);

  if (m_homogeneous){
    A[5] = c;
    A[9] = -s;
    A[6] = s;
    A[10] = c;
  }
  else{
    A[4] = c;
    A[7] = -s;
    A[5] = s;
    A[8] = c;
  }
}



//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::SetMatrixToRotateAboutY(double *A, double theta) const
  //------------------------------------------------------------------------------
{
  SetMatrixToIdentity(A) ;
  double s = (double)sin(theta);
  double c = (double)cos(theta);

  if (m_homogeneous){
    A[0] = c;
    A[8] = s;
    A[2] = -s;
    A[10] = c;
  }
  else{
    A[0] = c;
    A[6] = s;
    A[2] = -s;
    A[8] = c;
  }
}



//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::SetMatrixToRotateAboutZ(double *A, double theta) const
  //------------------------------------------------------------------------------
{
  SetMatrixToIdentity(A) ;
  double s = (double)sin(theta);
  double c = (double)cos(theta);

  if (m_homogeneous){
    A[0] = c;
    A[4] = -s;
    A[1] = s;
    A[5] = c;
  }
  else{
    A[0] = c;
    A[3] = -s;
    A[1] = s;
    A[4] = c;
  }
}



//------------------------------------------------------------------------------
// Set matrix to rotation about arbitrary axis
// Rodrigues formula is Q = I + sW + (1-c)W^2
void vtkALBAMatrixVectorMath::SetMatrixToRotateAboutAxis(double *Q, const double *w, double theta) const
  //------------------------------------------------------------------------------
{
  SetMatrixToIdentity(Q) ;
  double s = sin(theta) ;
  double c = cos(theta) ;

  // Get W matrix, equivalent to vector product with w.
  double W[16], Wsq[16] ;
  SetMatrixToVectorProductTransform(W, w) ;
  MultiplyMatrixByMatrix(W, W, Wsq) ; // W squared

  // Construct Q using Rodrigues formula
  if (m_homogeneous){
    for (int i = 0 ;  i < 11;  i++)
      Q[i] += s*W[i] + (1.0-c)*Wsq[i] ;
  }
  else{
    for (int i = 0 ;  i < 9;  i++)
      Q[i] += s*W[i] + (1.0-c)*Wsq[i] ;
  }
}



//------------------------------------------------------------------------------
// Set matrix to translate by t (homo only)
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::SetMatrixToTranslate(double *A, const double *t) const
{
  SetMatrixToIdentity(A) ;

  if (m_homogeneous){
    A[12] = t[0] ;
    A[13] = t[1] ;
    A[14] = t[2] ;
  }
}



//------------------------------------------------------------------------------
// Set matrix to transform corresponding to vector product by u
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::SetMatrixToVectorProductTransform(double *A, const double *u) const
{
  SetMatrixToZero(A) ;

  if (m_homogeneous){
    A[1] = u[2] ;
    A[2] = -u[1] ;
    A[4] = -u[2] ;
    A[6] = u[0] ;
    A[8] = u[1] ;
    A[9] = -u[0] ;
  }
  else{
    A[1] = u[2] ;
    A[2] = -u[1] ;
    A[3] = -u[2] ;
    A[5] = u[0] ;
    A[6] = u[1] ;
    A[7] = -u[0] ;
  }
}



//------------------------------------------------------------------------------
// Set matrix row to vector (columns 0-2 only) 
void vtkALBAMatrixVectorMath::SetMatrixRowToVector(double *A, int rowId, const double *u) const
  //------------------------------------------------------------------------------
{
  int k, j ;

  if (m_homogeneous){
    for (j = 0, k = rowId ;  j < 3 ;  j++, k+=3)
      A[k] = u[j] ;
  }
  else{
    for (j = 0, k = rowId ;  j < 3 ;  j++, k+=4)
      A[k] = u[j] ;
  }
}




//------------------------------------------------------------------------------
// Set matrix row to vector
void vtkALBAMatrixVectorMath::SetMatrixRowToVector3x3(double A[3][3], int rowId, const double *u) const
  //------------------------------------------------------------------------------
{
  for (int j = 0 ;  j < 3 ;  j++)
    A[rowId][j] = u[j] ;
}



//------------------------------------------------------------------------------
// Set matrix row  to vector (columns 0-2 only)
void vtkALBAMatrixVectorMath::SetMatrixRowToVector4x4(double A[4][4], int rowId, const double *u) const
  //------------------------------------------------------------------------------
{
  for (int j = 0 ;  j < 3 ;  j++)
    A[rowId][j] = u[j] ;
}




//------------------------------------------------------------------------------
// Set matrix column to vector (rows 0-2 only)
void vtkALBAMatrixVectorMath::SetMatrixColumnToVector(double *A, int colId, const double *u) const
  //------------------------------------------------------------------------------
{
  int k, i ;

  if (m_homogeneous){
    for (i = 0, k = 4*colId ;  i < 3 ;  i++, k++)
      A[k] = u[i] ;
  }
  else{
    for (i = 0, k = 3*colId ;  i < 3 ;  i++, k++)
      A[k] = u[i] ;
  }
}



//------------------------------------------------------------------------------
// Set matrix column to vector
void vtkALBAMatrixVectorMath::SetMatrixColumnToVector3x3(double A[3][3], int colId, const double *u) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    A[i][colId] = u[i] ;
}



//------------------------------------------------------------------------------
// Set matrix column to vector (rows 0-2 only)
void vtkALBAMatrixVectorMath::SetMatrixColumnToVector4x4(double A[4][4], int colId, const double *u) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    A[i][colId] = u[i] ;
}




//------------------------------------------------------------------------------
// Set matrix rows to vectors (columns and rows 0-2 only)
void vtkALBAMatrixVectorMath::SetMatrixRowsToVectors(double *A, const double *u, const double *v, const double *w) const
  //------------------------------------------------------------------------------
{
  SetMatrixRowToVector(A, 0, u) ;
  SetMatrixRowToVector(A, 1, v) ;
  SetMatrixRowToVector(A, 2, w) ;
}




//------------------------------------------------------------------------------
// Set matrix rows to vectors (columns and rows 0-2 only)
void vtkALBAMatrixVectorMath::SetMatrixRowsToVectors3x3(double A[3][3], const double *u, const double *v, const double *w) const
  //------------------------------------------------------------------------------
{
  SetMatrixRowToVector3x3(A, 0, u) ;
  SetMatrixRowToVector3x3(A, 1, v) ;
  SetMatrixRowToVector3x3(A, 2, w) ;
}



//------------------------------------------------------------------------------
// Set matrix rows to vectors (columns and rows 0-2 only)
void vtkALBAMatrixVectorMath::SetMatrixRowsToVectors4x4(double A[4][4], const double *u, const double *v, const double *w) const
  //------------------------------------------------------------------------------
{
  SetMatrixRowToVector4x4(A, 0, u) ;
  SetMatrixRowToVector4x4(A, 1, v) ;
  SetMatrixRowToVector4x4(A, 2, w) ;
}

//------------------------------------------------------------------------------
// Set matrix columns to vectors (columns and rows 0-2 only)
void vtkALBAMatrixVectorMath::SetMatrixColumnsToVectors(double *A, const double *u, const double *v, const double *w) const
  //------------------------------------------------------------------------------
{
  SetMatrixColumnToVector(A, 0, u) ;
  SetMatrixColumnToVector(A, 1, v) ;
  SetMatrixColumnToVector(A, 2, w) ;
}


//------------------------------------------------------------------------------
// Set matrix columns to vectors (columns and rows 0-2 only)
void vtkALBAMatrixVectorMath::SetMatrixColumnsToVectors3x3(double A[3][3], const double *u, const double *v, const double *w) const
  //------------------------------------------------------------------------------
{
  SetMatrixColumnToVector3x3(A, 0, u) ;
  SetMatrixColumnToVector3x3(A, 1, v) ;
  SetMatrixColumnToVector3x3(A, 2, w) ;
}


//------------------------------------------------------------------------------
// Set matrix columns to vectors (columns and rows 0-2 only)
void vtkALBAMatrixVectorMath::SetMatrixColumnsToVectors4x4(double A[4][4], const double *u, const double *v, const double *w) const
  //------------------------------------------------------------------------------
{
  SetMatrixColumnToVector4x4(A, 0, u) ;
  SetMatrixColumnToVector4x4(A, 1, v) ;
  SetMatrixColumnToVector4x4(A, 2, w) ;
}



//------------------------------------------------------------------------------
// Multiply matrix by scalar.
// If the matrix is homogeneous, the right column is not multiplied, so that
// we preserve the scaling relation S(A)*x = A*S(x), 
// where x is a vector and S() is the homo scaling operation.
// The homogeneous definition does not commute, 
// so S(A)*x != S(A*x) and A*S(B) != S(A)*B.
void vtkALBAMatrixVectorMath::MultiplyMatrixByScalar(double s, const double *A, double *B) const
  //------------------------------------------------------------------------------
{
  if (m_homogeneous){
    for (int k = 0 ;  k < 12 ;  k++)
      B[k] = s*A[k] ;
  }
  else{
    for (int k = 0 ;  k < 9 ;  k++)
      B[k] = s*A[k] ;
  }
}



//------------------------------------------------------------------------------
// Multiply matrix by scalar
void vtkALBAMatrixVectorMath::MultiplyMatrixByScalar3x3(double s, const double A[3][3], double B[3][3]) const   
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    for (int j = 0 ;  j < 3 ;  j++)
      B[i][j] = A[i][j] * s ;
}



//------------------------------------------------------------------------------
// Multiply matrix by scalar
// If the matrix is homogeneous, the right column is not multiplied, so that
// we preserve the scaling relation S(A)*x = A*S(x), 
// where x is a vector and S() is the homo scaling operation.
// The homogeneous definition does not commute, 
// so S(A)*x != S(A*x) and A*S(B) != S(A)*B.
void vtkALBAMatrixVectorMath::MultiplyMatrixByScalar4x4(double s, const double A[4][4], double B[3][3]) const   
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 4 ;  i++)
    for (int j = 0 ;  j < 3 ;  j++)
      B[i][j] = A[i][j] * s ;
}



//------------------------------------------------------------------------------
// Divide matrix by scalar.
// If the matrix is homogeneous, the right column is not divided, so that
// we preserve the scaling relation S(A)*x = A*S(x), 
// where x is a vector and S() is the homo scaling operation.
// The homogeneous definition does not commute, 
// so S(A)*x != S(A*x) and A*S(B) != S(A)*B.
void vtkALBAMatrixVectorMath::DivideMatrixByScalar(double s, const double *A, double *B) const
  //------------------------------------------------------------------------------
{
  if (m_homogeneous){
    for (int k = 0 ;  k < 12 ;  k++)
      B[k] = A[k]/s ;
  }
  else{
    for (int k = 0 ;  k < 9 ;  k++)
      B[k] = A[k]/s ;
  }
}    



//------------------------------------------------------------------------------
// Divide matrix by scalar.
void vtkALBAMatrixVectorMath::DivideMatrixByScalar3x3(double s, const double A[3][3], double B[3][3]) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    for (int j = 0 ;  j < 3 ;  j++)
      B[i][j] = A[i][j] / s ;
}    



//------------------------------------------------------------------------------
// Divide matrix by scalar.
// If the matrix is homogeneous, the right column is not divided, so that
// we preserve the scaling relation S(A)*x = A*S(x), 
// where x is a vector and S() is the homo scaling operation.
// The homogeneous definition does not commute, 
// so S(A)*x != S(A*x) and A*S(B) != S(A)*B.
void vtkALBAMatrixVectorMath::DivideMatrixByScalar4x4(double s, const double A[4][4], double B[4][4]) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 4 ;  i++)
    for (int j = 0 ;  j < 3 ;  j++)
      B[i][j] = A[i][j] / s ;
}



//------------------------------------------------------------------------------
// Multiply matrix columns by scalars
// no. of scalars must equal no. of columns (3 or 4)
// This is useful for multiplying column eigenvectors by eigenvalues
void vtkALBAMatrixVectorMath::MultiplyColumnsByScalars(const double *s, const double *A, double *B) const
  //------------------------------------------------------------------------------
{
  int k = 0 ;

  if (m_homogeneous){
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
// Multiply matrix columns by scalars
// no. of scalars must equal 3
// This is useful for multiplying column eigenvectors by eigenvalues
void vtkALBAMatrixVectorMath::MultiplyColumnsByScalars3x3(const double *s, const double A[3][3], double B[3][3]) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++){
    for (int j = 0 ;  j < 3 ;  j++){
      B[i][j] = s[j]*A[i][j] ;
    }
  }
}


//------------------------------------------------------------------------------
// Multiply matrix columns by scalars
// no. of scalars must equal 4
// This is useful for multiplying column eigenvectors by eigenvalues
void vtkALBAMatrixVectorMath::MultiplyColumnsByScalars4x4(const double *s, const double A[4][4], double B[4][4]) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 4 ;  i++){
    for (int j = 0 ;  j < 4 ;  j++){
      B[i][j] = s[j]*A[i][j] ;
    }
  }
}



//------------------------------------------------------------------------------
// Multiply matrix rows by scalars
// no. of scalars must equal no. of rows (3 or 4)
void vtkALBAMatrixVectorMath::MultiplyRowsByScalars(const double *s, const double *A, double *B) const
  //------------------------------------------------------------------------------
{
  int k = 0 ;
  if (m_homogeneous){
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
// Multiply matrix rows by scalars
// no. of scalars must equal 3
void vtkALBAMatrixVectorMath::MultiplyRowsByScalars3x3(const double *s, const double A[3][3], double B[3][3]) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    for (int j = 0 ;  j < 3 ;  j++)
      B[i][j] = s[i]*A[i][j] ;
}


//------------------------------------------------------------------------------
// Multiply matrix rows by scalars
// no. of scalars must equal 4
void vtkALBAMatrixVectorMath::MultiplyRowsByScalars4x4(const double *s, const double A[4][4], double B[4][4]) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 4 ;  i++)
    for (int j = 0 ;  j < 4 ;  j++)
      B[i][j] = s[i]*A[i][j] ;
}


//------------------------------------------------------------------------------
// Multiply vector by matrix
void vtkALBAMatrixVectorMath::MultiplyMatrixByVector(const double *A, const double *v, double *Av) const
  //------------------------------------------------------------------------------
{
  if (m_homogeneous){
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
// Multiply vector by matrix
void vtkALBAMatrixVectorMath::MultiplyMatrixByVector3x3(const double A[3][3], const double *v, double *Av) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    Av[i] = 0.0 ;

  for (int i = 0 ;  i < 3 ;  i++)
    for (int j = 0 ;  j < 3 ;  j++)
      Av[i] += A[i][j] * v[j] ;
}




//------------------------------------------------------------------------------
// Multiply vector by matrix
void vtkALBAMatrixVectorMath::MultiplyMatrixByVector4x4(const double A[4][4], const double *v, double *Av) const
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 4 ;  i++)
    Av[i] = 0.0 ;

  for (int i = 0 ;  i < 4 ;  i++)
    for (int j = 0 ;  j < 4 ;  j++)
      Av[i] += A[i][j] * v[j] ;
}




//------------------------------------------------------------------------------
// Multiply matrix by matrix
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::MultiplyMatrixByMatrix(const double *A, const double *B, double *AB) const
{
  if ((AB == A) || (AB == B)){
    // output can't be same as input
    assert(false) ;
  }

  if (m_homogeneous){
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
// Multiply matrix by matrix
void vtkALBAMatrixVectorMath::MultiplyMatrixByMatrix3x3(const double A[3][3], const double B[3][3], double AB[3][3]) const
  //------------------------------------------------------------------------------
{
  int jj ;

  for (int i = 0 ;  i < 3 ;  i++)
    for (int j = 0 ;  j < 3 ;  j++)
      for (jj = 0, AB[i][j] = 0.0 ;  jj < 3 ;  jj++)
        AB[i][j] += A[i][jj]*B[jj][j] ;
}



//------------------------------------------------------------------------------
// Multiply matrix by matrix
void vtkALBAMatrixVectorMath::MultiplyMatrixByMatrix4x4(const double A[4][4], const double B[4][4], double AB[4][4]) const
  //------------------------------------------------------------------------------
{
  int jj ;

  for (int i = 0 ;  i < 4 ;  i++)
    for (int j = 0 ;  j < 4 ;  j++)
      for (jj = 0, AB[i][j] = 0.0 ;  jj < 4 ;  jj++)
        AB[i][j] += A[i][jj]*B[jj][j] ;
}




//------------------------------------------------------------------------------
// Multiply 3 vector by homo 4x4 matrix
void vtkALBAMatrixVectorMath::MultiplyHomoMatrixBy3Vector(const double *A, const double v[3], double Av[3]) const
  //------------------------------------------------------------------------------
{
  double vhomo[4], Avhomo[4] ;
  CopyVectorToHomoVector(v, vhomo) ;
  MultiplyMatrixByVector(A, vhomo, Avhomo) ;
  CopyHomoVectorToVector(Avhomo, Av) ;
}


//------------------------------------------------------------------------------
// Multiply 3 vector by homo 4x4 matrix
void vtkALBAMatrixVectorMath::MultiplyHomoMatrixBy3Vector(const double A[4][4], const double v[3], double Av[3]) const
  //------------------------------------------------------------------------------
{
  double vhomo[4], Avhomo[4] ;
  CopyVectorToHomoVector(v, vhomo) ;
  MultiplyMatrixByVector4x4(A, vhomo, Avhomo) ;
  CopyHomoVectorToVector(Avhomo, Av) ;
}





//------------------------------------------------------------------------------
// Rotate vector about x
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::RotateVectorAboutX(const double *a, double *b, double theta) const
{
  double A[16] ;
  SetMatrixToRotateAboutX(A, theta) ;
  MultiplyMatrixByVector(A, a, b) ;
}


//------------------------------------------------------------------------------
// Rotate vector about y
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::RotateVectorAboutY(const double *a, double *b, double theta) const
{
  double A[16] ;
  SetMatrixToRotateAboutY(A, theta) ;
  MultiplyMatrixByVector(A, a, b) ;

}


//------------------------------------------------------------------------------
// Rotate vector about z
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::RotateVectorAboutZ(const double *a, double *b, double theta) const
{
  double A[16] ;
  SetMatrixToRotateAboutZ(A, theta) ;
  MultiplyMatrixByVector(A, a, b) ;

}


//------------------------------------------------------------------------------
// Rotate vector about arbitrary axis w
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::RotateVectorAboutAxis(const double *a, double *b, const double *w, double theta) const
{
  double Q[16] ;
  SetMatrixToRotateAboutAxis(Q, w, theta) ;
  MultiplyMatrixByVector(Q, a, b) ;
}



//------------------------------------------------------------------------------
// Find nearest point on line. \n
// Returns rsq and lambda, where rsq is the squared distance, and \n
// lambda is the position along the line segment.
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::FindNearestPointOnLine(const double x[3], const double p0[3], const double p1[3], double& rsq, double& lambda)
{
  bool saveHomo = m_homogeneous ;
  SetHomogeneous(false) ;

  // unit vector along line
  double u[3] ;
  SubtractVectors(p1, p0, u) ;
  NormalizeVector(u) ;

  double dx[3] ;
  SubtractVectors(x, p0, dx) ;
  double m1 = MagnitudeOfVector(dx) ; // dist x to p0
  double m2 = DotProduct(dx,u) ;      // dist dx.u to p0
  rsq = m1*m1 - m2*m2 ;

  double m3 = Distance(p1,p0) ; // dist p1 to p0
  lambda = m2/m3 ;

  SetHomogeneous(saveHomo) ;
}




//------------------------------------------------------------------------------
// Transpose the matrix
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::Transpose(const double *A,  double *AT) const
{
  int k = 0 ;
  double temp[16] ;

  if (m_homogeneous){
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
// Transpose the matrix
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::Transpose3x3(const double A[3][3],  double AT[3][3]) const
{
  for (int i = 0 ;  i < 3 ;  i++)
    for (int j = 0 ;  j < 3 ;  j++)
      AT[i][j] = A[j][i] ;
}


//------------------------------------------------------------------------------
// Transpose the matrix
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::Transpose4x4(const double A[4][4],  double AT[4][4]) const
{
  for (int i = 0 ;  i < 4 ;  i++)
    for (int j = 0 ;  j < 4 ;  j++)
      AT[i][j] = A[j][i] ;

}



//------------------------------------------------------------------------------
// Get pointer to column
//------------------------------------------------------------------------------
double *vtkALBAMatrixVectorMath::GetColumn(double *A, int col) const
{
  if (m_homogeneous)
    return A + 3*col ;
  else
    return A + 4*col ;
}


//------------------------------------------------------------------------------
// Print matrix
//------------------------------------------------------------------------------
void vtkALBAMatrixVectorMath::PrintMatrix(std::ostream& os, const double *A) const
{
  if (m_homogeneous){
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
// print matrix
void vtkALBAMatrixVectorMath::PrintMatrix3x3(std::ostream& os, const double A[3][3]) const     
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++){
    for (int j = 0 ;  j < 3 ;  j++)
      os << A[i][j] << "\t" ;
    os << std::endl ;
  }
  os << std::endl ;
}



//------------------------------------------------------------------------------
// print matrix
void vtkALBAMatrixVectorMath::PrintMatrix4x4(std::ostream& os, const double A[4][4]) const     
  //------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 4 ;  i++){
    for (int j = 0 ;  j < 4 ;  j++)
      os << A[i][j] << "\t" ;
    os << std::endl ;
  }
  os << std::endl ;
}



//------------------------------------------------------------------------------
// Transpose 2D array
void vtkALBAMatrixVectorMath::Transpose2DArray(const double arr1[3][3], double arr2[3][3]) const
  //------------------------------------------------------------------------------
{
  double temp[4][4] ;

  if (m_homogeneous){
    for (int i = 0 ;  i < 4 ;  i++)
      for (int j = 0 ;  j < 4 ;  j++)
        temp[i][j] = arr1[j][i] ;
    for (int i = 0 ;  i < 4 ;  i++)
      for (int j = 0 ;  j < 4 ;  j++)
        arr2[i][j] = temp[j][i] ;
  }
  else{
    for (int i = 0 ;  i < 3 ;  i++)
      for (int j = 0 ;  j < 3 ;  j++)
        temp[i][j] = arr1[j][i] ;
    for (int i = 0 ;  i < 3 ;  i++)
      for (int j = 0 ;  j < 3 ;  j++)
        arr2[i][j] = temp[j][i] ;
  }
}


//------------------------------------------------------------------------------
// copy 2D array to matrix
void vtkALBAMatrixVectorMath::Copy2DArrayToMatrix3x3(const double arr2d[3][3],  double *A) const
  //------------------------------------------------------------------------------
{
  int k = 0 ;
  for (int j = 0 ;  j < 3 ;  j++)
    for (int i = 0 ;  i < 3 ;  i++, k++)
      A[k] = arr2d[i][j] ; 
}

//------------------------------------------------------------------------------
// copy 2D array to matrix
void vtkALBAMatrixVectorMath::Copy2DArrayToMatrix4x4(const double arr2d[4][4],  double *A) const
  //------------------------------------------------------------------------------
{
  int k = 0 ;
  for (int j = 0 ;  j < 4 ;  j++)
    for (int i = 0 ;  i < 4 ;  i++, k++)
      A[k] = arr2d[i][j] ; 
}

//------------------------------------------------------------------------------
// copy matrix to 2D array
void vtkALBAMatrixVectorMath::CopyMatrixTo2DArray3x3(const double *A,  double arr2d[3][3]) const
  //------------------------------------------------------------------------------
{
  int k = 0 ;
  for (int j = 0 ;  j < 3 ;  j++)
    for (int i = 0 ;  i < 3 ;  i++, k++)
      arr2d[i][j] = A[k] ; 
}

//------------------------------------------------------------------------------
// copy matrix to 2D array
void vtkALBAMatrixVectorMath::CopyMatrixTo2DArray4x4(const double *A,  double arr2d[4][4]) const
  //------------------------------------------------------------------------------
{
  int k = 0 ;
  for (int j = 0 ;  j < 4 ;  j++)
    for (int i = 0 ;  i < 4 ;  i++, k++)
      arr2d[i][j] = A[k] ; 
}

