/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrix3x3.cpp,v $
  Language:  C++
  Date:      $Date: 2006-07-13 09:08:22 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafMatrix3x3.h"
#include "mafIndent.h"
#include <math.h>
#include <assert.h>

mafCxxTypeMacro(mafMatrix3x3)

//----------------------------------------------------------------------------
mafMatrix3x3::mafMatrix3x3()
//----------------------------------------------------------------------------
{
  Zero();
}

//----------------------------------------------------------------------------
mafMatrix3x3::~mafMatrix3x3()
//----------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mafMatrix3x3 &mafMatrix3x3::operator=(const mafMatrix3x3 &mat)
//------------------------------------------------------------------------------
{
  for (int i=0;i<9;i++)
  {
    for (int j=0;j<9;j++)
    {
      this->SetElement(i,j,mat.GetElement(i,j));
    }
  }
  Modified();
  return *this;
}

//------------------------------------------------------------------------------
void mafMatrix3x3::DeepCopy(mafMatrix3x3 *mat)
//------------------------------------------------------------------------------
{
  assert(mat);
  *this=*mat;
}

//------------------------------------------------------------------------------
mafMatrix3x3::mafMatrix3x3(mafMatrix3x3 &mat)
//------------------------------------------------------------------------------
{
  *this=mat;
}

//------------------------------------------------------------------------------
void mafMatrix3x3::Print (std::ostream& os, const int indent)// const
//------------------------------------------------------------------------------
{
  this->Superclass::Print(os, indent);
  mafIndent the_indent(indent);
  mafIndent next_indent = the_indent.GetNextIndent();

  int i, j;
  
  os << the_indent << "Elements:\n";
  for (i = 0; i < 3; i++) 
  {
    os << next_indent;
    for (j = 0; j < 3; j++) 
    {
      os << GetElements()[i][j] << " ";
    }
    os << std::endl;
  }
}

//----------------------------------------------------------------------------
// helper function, swap two 3-vectors
template<class T>
//----------------------------------------------------------------------------
inline void mafSwapVectors3(T v1[3], T v2[3])
{
  for (int i = 0; i < 3; i++)
  {
    T tmp = v1[i];
    v1[i] = v2[i];
    v2[i] = tmp;
  }
}

//----------------------------------------------------------------------------
void mafMatrix3x3::GetVersor(int axis, const mafMatrix3x3 &matrix, double versor[3])
//----------------------------------------------------------------------------
{
	if (0 <= axis && axis <= 2)
	{
		for (int i = 0; i < 3; i++)
		{
			versor[i] = matrix.GetElement(i, axis);
		}	
	}
}

//----------------------------------------------------------------------------
void mafMatrix3x3::Zero(double elements[9])
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 9; i++)
  {
    elements[i] = 0.0;
  }
}


//----------------------------------------------------------------------------
void mafMatrix3x3::Multiply(const double A[3][3], const double B[3][3],double C[3][3])
//----------------------------------------------------------------------------
{
  double D[3][3];

  for (int i = 0; i < 3; i++)
    {
    D[0][i] = A[0][0]*B[0][i] + A[0][1]*B[1][i] + A[0][2]*B[2][i];
    D[1][i] = A[1][0]*B[0][i] + A[1][1]*B[1][i] + A[1][2]*B[2][i];
    D[2][i] = A[2][0]*B[0][i] + A[2][1]*B[1][i] + A[2][2]*B[2][i];
    }

  for (int j = 0; j < 3; j++)
    {
    C[j][0] = D[j][0];
    C[j][1] = D[j][1];
    C[j][2] = D[j][2];
    }
}

//----------------------------------------------------------------------------
void mafMatrix3x3::Transpose(const double A[3][3], double AT[3][3])
//----------------------------------------------------------------------------
{
  double tmp;
  tmp = A[1][0];
  AT[1][0] = A[0][1];
  AT[0][1] = tmp;
  tmp = A[2][0];
  AT[2][0] = A[0][2];
  AT[0][2] = tmp;
  tmp = A[2][1];
  AT[2][1] = A[1][2];
  AT[1][2] = tmp;

  AT[0][0] = A[0][0];
  AT[1][1] = A[1][1];
  AT[2][2] = A[2][2];
}

//----------------------------------------------------------------------------
void mafMatrix3x3::Invert(const double A[3][3], double AI[3][3])
//----------------------------------------------------------------------------
{
  int index[3];
  double tmp[3][3];

  for (int k = 0; k < 3; k++)
    {
    AI[k][0] = A[k][0];
    AI[k][1] = A[k][1];
    AI[k][2] = A[k][2];
    }
  // invert one column at a time
  mafMatrix3x3::LUFactor(AI,index);
  for (int i = 0; i < 3; i++)
    {
    double *x = tmp[i];
    x[0] = x[1] = x[2] = 0.0;
    x[i] = 1.0;
    mafMatrix3x3::LUSolve(AI,index,x);
    }
  for (int j = 0; j < 3; j++)
    {
    double *x = tmp[j];
    AI[0][j] = x[0];
    AI[1][j] = x[1];
    AI[2][j] = x[2];      
    }
}

//----------------------------------------------------------------------------
// Unrolled LU factorization of a 3x3 matrix with pivoting.
// This decomposition is non-standard in that the diagonal
// elements are inverted, to convert a division to a multiplication
// in the back substitution.
void mafMatrix3x3::LUFactor(double A[3][3], int index[3])
//----------------------------------------------------------------------------
{
  int i,maxI;
  double tmp,largest;
  double scale[3];

  // Loop over rows to get implicit scaling information

  for ( i = 0; i < 3; i++ ) 
    {
    largest =  fabs(A[i][0]);
    if ((tmp = fabs(A[i][1])) > largest)
      {
      largest = tmp;
      }
    if ((tmp = fabs(A[i][2])) > largest)
      {
      largest = tmp;
      }
    scale[i] = double(1.0)/largest;
    }
  
  // Loop over all columns using Crout's method

  // first column
  largest = scale[0]*fabs(A[0][0]);
  maxI = 0;
  if ((tmp = scale[1]*fabs(A[1][0])) >= largest) 
    {
    largest = tmp;
    maxI = 1;
    }
  if ((tmp = scale[2]*fabs(A[2][0])) >= largest) 
    {
    maxI = 2;
    }
  if (maxI != 0) 
    {
    mafSwapVectors3(A[maxI],A[0]);
    scale[maxI] = scale[0];
    }
  index[0] = maxI;

  A[0][0] = double(1.0)/A[0][0];
  A[1][0] *= A[0][0];
  A[2][0] *= A[0][0];
    
  // second column
  A[1][1] -= A[1][0]*A[0][1];
  A[2][1] -= A[2][0]*A[0][1];
  largest = scale[1]*fabs(A[1][1]);
  maxI = 1;
  if ((tmp = scale[2]*fabs(A[2][1])) >= largest) 
    {
    maxI = 2;
    mafSwapVectors3(A[2],A[1]);
    scale[2] = scale[1];
    }
  index[1] = maxI;
  A[1][1] = double(1.0)/A[1][1];
  A[2][1] *= A[1][1];

  // third column
  A[1][2] -= A[1][0]*A[0][2];
  A[2][2] -= A[2][0]*A[0][2] + A[2][1]*A[1][2];
  largest = scale[2]*fabs(A[2][2]);
  index[2] = 2;
  A[2][2] = double(1.0)/A[2][2];
}

//----------------------------------------------------------------------------
// Backsubsitution with an LU-decomposed matrix.  This is the standard
// LU decomposition, except that the diagonals elements have been inverted.
void mafMatrix3x3::LUSolve(const double A[3][3], const int index[3], double x[3])
//----------------------------------------------------------------------------
{
  double sum;

  // forward substitution
  
  sum = x[index[0]];
  x[index[0]] = x[0];
  x[0] = sum;

  sum = x[index[1]];
  x[index[1]] = x[1];
  x[1] = sum - A[1][0]*x[0];

  sum = x[index[2]];
  x[index[2]] = x[2];
  x[2] = sum - A[2][0]*x[0] - A[2][1]*x[1];

  // back substitution
  
  x[2] = x[2]*A[2][2];
  x[1] = (x[1] - A[1][2]*x[2])*A[1][1];
  x[0] = (x[0] - A[0][1]*x[1] - A[0][2]*x[2])*A[0][0];
}  

//----------------------------------------------------------------------------
void mafMatrix3x3::Identity(double A[3][3])
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 3; i++)
    {
    A[i][0] = A[i][1] = A[i][2] = double(0.0);
    A[i][i] = 1.0;
    }
}

#define MAF_ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
        a[k][l]=h+s*(g-h*tau)

#define MAF_MAX_ROTATIONS 20

//----------------------------------------------------------------------------
// Jacobi iteration for the solution of eigenvectors/eigenvalues of a nxn
// real symmetric matrix. Square nxn matrix a; size of matrix in n;
// output eigenvalues in w; and output eigenvectors in v. Resulting
// eigenvalues/vectors are sorted in decreasing order; eigenvectors are
// normalized.
int mafMatrix3x3::JacobiN(double **a, int n, double *w, double **v)
//----------------------------------------------------------------------------
{
  int i, j, k, iq, ip, numPos;
  double tresh, theta, tau, t, sm, s, h, g, c, tmp;
  double bspace[4], zspace[4];
  double *b = bspace;
  double *z = zspace;

  // only allocate memory if the matrix is large
  if (n > 4)
    {
    b = new double[n];
    z = new double[n]; 
    }

  // initialize
  for (ip=0; ip<n; ip++) 
    {
    for (iq=0; iq<n; iq++)
      {
      v[ip][iq] = 0.0;
      }
    v[ip][ip] = 1.0;
    }
  for (ip=0; ip<n; ip++) 
    {
    b[ip] = w[ip] = a[ip][ip];
    z[ip] = 0.0;
    }

  // begin rotation sequence
  for (i=0; i<MAF_MAX_ROTATIONS; i++) 
    {
    sm = 0.0;
    for (ip=0; ip<n-1; ip++) 
      {
      for (iq=ip+1; iq<n; iq++)
        {
        sm += fabs(a[ip][iq]);
        }
      }
    if (sm == 0.0)
      {
      break;
      }

    if (i < 3)                                // first 3 sweeps
      {
      tresh = 0.2*sm/(n*n);
      }
    else
      {
      tresh = 0.0;
      }

    for (ip=0; ip<n-1; ip++) 
      {
      for (iq=ip+1; iq<n; iq++) 
        {
        g = 100.0*fabs(a[ip][iq]);

        // after 4 sweeps
        if (i > 3 && (fabs(w[ip])+g) == fabs(w[ip])
        && (fabs(w[iq])+g) == fabs(w[iq]))
          {
          a[ip][iq] = 0.0;
          }
        else if (fabs(a[ip][iq]) > tresh) 
          {
          h = w[iq] - w[ip];
          if ( (fabs(h)+g) == fabs(h))
            {
            t = (a[ip][iq]) / h;
            }
          else 
            {
            theta = 0.5*h / (a[ip][iq]);
            t = 1.0 / (fabs(theta)+sqrt(1.0+theta*theta));
            if (theta < 0.0)
              {
              t = -t;
              }
            }
          c = 1.0 / sqrt(1+t*t);
          s = t*c;
          tau = s/(1.0+c);
          h = t*a[ip][iq];
          z[ip] -= h;
          z[iq] += h;
          w[ip] -= h;
          w[iq] += h;
          a[ip][iq]=0.0;

          // ip already shifted left by 1 unit
          for (j = 0;j <= ip-1;j++) 
            {
            MAF_ROTATE(a,j,ip,j,iq);
            }
          // ip and iq already shifted left by 1 unit
          for (j = ip+1;j <= iq-1;j++) 
            {
            MAF_ROTATE(a,ip,j,j,iq);
            }
          // iq already shifted left by 1 unit
          for (j=iq+1; j<n; j++) 
            {
            MAF_ROTATE(a,ip,j,iq,j);
            }
          for (j=0; j<n; j++) 
            {
            MAF_ROTATE(v,j,ip,j,iq);
            }
          }
        }
      }

    for (ip=0; ip<n; ip++) 
      {
      b[ip] += z[ip];
      w[ip] = b[ip];
      z[ip] = 0.0;
      }
    }

  //// this is NEVER called
  if ( i >= MAF_MAX_ROTATIONS )
  {
    mafWarningMacro("mafMatrix3x3::Jacobi: Error extracting eigenfunctions");
    return 0;
  }

  // sort eigenfunctions                 these changes do not affect accuracy 
  for (j=0; j<n-1; j++)                  // boundary incorrect
    {
    k = j;
    tmp = w[k];
    for (i=j+1; i<n; i++)                // boundary incorrect, shifted already
      {
      if (w[i] >= tmp)                   // why exchage if same?
        {
        k = i;
        tmp = w[k];
        }
      }
    if (k != j) 
      {
      w[k] = w[j];
      w[j] = tmp;
      for (i=0; i<n; i++) 
        {
        tmp = v[i][j];
        v[i][j] = v[i][k];
        v[i][k] = tmp;
        }
      }
    }
  // insure eigenvector consistency (i.e., Jacobi can compute vectors that
  // are negative of one another (.707,.707,0) and (-.707,-.707,0). This can
  // reek havoc in hyperstreamline/other stuff. We will select the most
  // positive eigenvector.
  int ceil_half_n = (n >> 1) + (n & 1);
  for (j=0; j<n; j++)
    {
    for (numPos=0, i=0; i<n; i++)
      {
      if ( v[i][j] >= 0.0 )
        {
        numPos++;
        }
      }
//    if ( numPos < ceil(double(n)/double(2.0)) )
    if ( numPos < ceil_half_n)
      {
      for(i=0; i<n; i++)
        {
        v[i][j] *= -1.0;
        }
      }
    }

  if (n > 4)
    {
    delete [] b;
    delete [] z;
    }
  return 1;
}


//----------------------------------------------------------------------------
void mafMatrix3x3::mmuQuaternionToMatrix(const double quat[4], double A[3][3])
//----------------------------------------------------------------------------
{
  double ww = quat[0]*quat[0];
  double wx = quat[0]*quat[1];
  double wy = quat[0]*quat[2];
  double wz = quat[0]*quat[3];

  double xx = quat[1]*quat[1];
  double yy = quat[2]*quat[2];
  double zz = quat[3]*quat[3];

  double xy = quat[1]*quat[2];
  double xz = quat[1]*quat[3];
  double yz = quat[2]*quat[3];

  double rr = xx + yy + zz;
  // normalization factor, just in case quaternion was not normalized
  double f = double(1)/double(sqrt(ww + rr));
  double s = (ww - rr)*f;
  f *= 2;

  A[0][0] = xx*f + s;
  A[1][0] = (xy + wz)*f;
  A[2][0] = (xz - wy)*f;

  A[0][1] = (xy - wz)*f;
  A[1][1] = yy*f + s;
  A[2][1] = (yz + wx)*f;

  A[0][2] = (xz + wy)*f;
  A[1][2] = (yz - wx)*f;
  A[2][2] = zz*f + s;
}

//----------------------------------------------------------------------------
//  The solution is based on
//  Berthold K. P. Horn (1987),
//  "Closed-form solution of absolute orientation using unit quaternions,"
//  Journal of the Optical Society of America A, 4:629-642
void mafMatrix3x3::MatrixTommuQuaternion(const double A[3][3], double quat[4])
//----------------------------------------------------------------------------
{
  double N[4][4];

  // on-diagonal elements
  N[0][0] =  A[0][0]+A[1][1]+A[2][2];
  N[1][1] =  A[0][0]-A[1][1]-A[2][2];
  N[2][2] = -A[0][0]+A[1][1]-A[2][2];
  N[3][3] = -A[0][0]-A[1][1]+A[2][2];

  // off-diagonal elements
  N[0][1] = N[1][0] = A[2][1]-A[1][2];
  N[0][2] = N[2][0] = A[0][2]-A[2][0];
  N[0][3] = N[3][0] = A[1][0]-A[0][1];

  N[1][2] = N[2][1] = A[1][0]+A[0][1];
  N[1][3] = N[3][1] = A[0][2]+A[2][0];
  N[2][3] = N[3][2] = A[2][1]+A[1][2];

  double eigenvectors[4][4],eigenvalues[4];
  
  // convert into format that JacobiN can use,
  // then use Jacobi to find eigenvalues and eigenvectors
  double *NTemp[4],*eigenvectorsTemp[4];
  for (int i = 0; i < 4; i++)
    {
    NTemp[i] = N[i];
    eigenvectorsTemp[i] = eigenvectors[i];
    }
  mafMatrix3x3::JacobiN(NTemp,4,eigenvalues,eigenvectorsTemp);
  
  // the first eigenvector is the one we want
  quat[0] = eigenvectors[0][0];
  quat[1] = eigenvectors[1][0];
  quat[2] = eigenvectors[2][0];
  quat[3] = eigenvectors[3][0];
}
  
  
//----------------------------------------------------------------------------
//  The orthogonalization is done via quaternions in order to avoid
//  having to use a singular value decomposition algorithm.  
void mafMatrix3x3::Orthogonalize(const double A[3][3], double B[3][3])
//----------------------------------------------------------------------------
{
  int i;

  // copy the matrix
  for (i = 0; i < 3; i++)
    {
    B[0][i] = A[0][i];
    B[1][i] = A[1][i];
    B[2][i] = A[2][i];
    }

  // Pivot the matrix to improve accuracy
  double scale[3];
  int index[3];
  double tmp, largest;

  // Loop over rows to get implicit scaling information
  for (i = 0; i < 3; i++)
    {
    largest = fabs(B[i][0]);
    if ((tmp = fabs(B[i][1])) > largest)
      {
      largest = tmp;
      }
    if ((tmp = fabs(B[i][2])) > largest)
      {
      largest = tmp;
      }
    scale[i] = double(1.0)/largest;
    }

  // first column
  index[0] = 0;
  largest = scale[0]*fabs(B[0][0]);
  if ((tmp = scale[1]*fabs(B[1][0])) >= largest) 
    {
    largest = tmp;
    index[0] = 1;
    }
  if ((tmp = scale[2]*fabs(B[2][0])) >= largest) 
    {
    index[0] = 2;
    }
  if (index[0] != 0) 
    {
    mafSwapVectors3(B[index[0]],B[0]);
    scale[index[0]] = scale[0];
    }

  // second column
  index[1] = 1;
  largest = scale[1]*fabs(B[1][1]);
  if ((tmp = scale[2]*fabs(B[2][1])) >= largest) 
    {
    index[1] = 2;
    mafSwapVectors3(B[2],B[1]);
    }

  // third column
  index[2] = 2;

  // A quaternian can only describe a pure rotation, not
  // a rotation with a flip, therefore the flip must be
  // removed before the matrix is converted to a quaternion.
  double d = Determinant(B);
  if (d < 0)
    {
    for (i = 0; i < 3; i++)
      {
      B[0][i] = -B[0][i];
      B[1][i] = -B[1][i];
      B[2][i] = -B[2][i];
      }
    }

  // Do orthogonalization using a quaternion intermediate
  // (this, essentially, does the orthogonalization via
  // diagonalization of an appropriately constructed symmetric
  // 4x4 matrix rather than by doing SVD of the 3x3 matrix)
  double quat[4];
  MatrixTommuQuaternion(B,quat);
  mmuQuaternionToMatrix(quat,B);

  // Put the flip back into the orthogonalized matrix.
  if (d < 0)
    {
    for (i = 0; i < 3; i++)
      {
      B[0][i] = -B[0][i];
      B[1][i] = -B[1][i];
      B[2][i] = -B[2][i];
      }
    }

  // Undo the pivoting
  if (index[1] != 1)
    {
    mafSwapVectors3(B[index[1]],B[1]);
    }
  if (index[0] != 0)
    {
    mafSwapVectors3(B[index[0]],B[0]);
    }
}

//----------------------------------------------------------------------------
// Extract the eigenvalues and eigenvectors from a 3x3 matrix.
// The eigenvectors (the columns of V) will be normalized. 
// The eigenvectors are aligned optimally with the x, y, and z
// axes respectively.
void mafMatrix3x3::Diagonalize(const double A[3][3], double w[3], double V[3][3])
{
  int i,j,k,maxI;
  double tmp, maxVal;

  // do the matrix[3][3] to **matrix conversion for Jacobi
  double C[3][3];
  double *ATemp[3],*VTemp[3];
  for (i = 0; i < 3; i++)
    {
    C[i][0] = A[i][0];
    C[i][1] = A[i][1];
    C[i][2] = A[i][2];
    ATemp[i] = C[i];
    VTemp[i] = V[i];
    }

  // diagonalize using Jacobi
  mafMatrix3x3::JacobiN(ATemp,3,w,VTemp);
  
  // if all the eigenvalues are the same, return identity matrix
  if (w[0] == w[1] && w[0] == w[2])
    {
    mafMatrix3x3::Identity(V);
    return;
    }

  // transpose temporarily, it makes it easier to sort the eigenvectors
  mafMatrix3x3::Transpose(V,V);

  // if two eigenvalues are the same, re-orthogonalize to optimally line
  // up the eigenvectors with the x, y, and z axes
  for (i = 0; i < 3; i++)
    {
    if (w[(i+1)%3] == w[(i+2)%3]) // two eigenvalues are the same
      {
      // find maximum element of the independant eigenvector
      maxVal = fabs(V[i][0]);
      maxI = 0;
      for (j = 1; j < 3; j++)
        {
        if (maxVal < (tmp = fabs(V[i][j])))
          {
          maxVal = tmp;
          maxI = j;
          }
        }
      // swap the eigenvector into its proper position
      if (maxI != i)
        {
        tmp = w[maxI];
        w[maxI] = w[i];
        w[i] = tmp;
        mafSwapVectors3(V[i],V[maxI]);
        }
      // maximum element of eigenvector should be positive
      if (V[maxI][maxI] < 0)
        {
        V[maxI][0] = -V[maxI][0];
        V[maxI][1] = -V[maxI][1];
        V[maxI][2] = -V[maxI][2];
        }
      
      // re-orthogonalize the other two eigenvectors
      j = (maxI+1)%3;
      k = (maxI+2)%3;

      V[j][0] = 0.0; 
      V[j][1] = 0.0; 
      V[j][2] = 0.0;
      V[j][j] = 1.0;
      mafMatrix3x3::Cross(V[maxI],V[j],V[k]);
      mafMatrix3x3::Normalize(V[k]);
      mafMatrix3x3::Cross(V[k],V[maxI],V[j]);

      // transpose vectors back to columns
      mafMatrix3x3::Transpose(V,V);
      return;
      }
    }

  // the three eigenvalues are different, just sort the eigenvectors
  // to align them with the x, y, and z axes

  // find the vector with the largest x element, make that vector
  // the first vector
  maxVal = fabs(V[0][0]);
  maxI = 0;
  for (i = 1; i < 3; i++)
    {
    if (maxVal < (tmp = fabs(V[i][0])))
      {
      maxVal = tmp;
      maxI = i;
      }
    }
  // swap eigenvalue and eigenvector
  if (maxI != 0)
    {
    tmp = w[maxI];
    w[maxI] = w[0];
    w[0] = tmp;
    mafSwapVectors3(V[maxI],V[0]);
    }
  // do the same for the y element
  if (fabs(V[1][1]) < fabs(V[2][1]))
    {
    tmp = w[2];
    w[2] = w[1];
    w[1] = tmp;
    mafSwapVectors3(V[2],V[1]);
    }

  // ensure that the sign of the eigenvectors is correct
  for (i = 0; i < 2; i++)
    {
    if (V[i][i] < 0)
      {
      V[i][0] = -V[i][0];
      V[i][1] = -V[i][1];
      V[i][2] = -V[i][2];
      }
    }
  // set sign of final eigenvector to ensure that determinant is positive
  if (mafMatrix3x3::Determinant(V) < 0)
    {
    V[2][0] = -V[2][0];
    V[2][1] = -V[2][1];
    V[2][2] = -V[2][2];
    }

  // transpose the eigenvectors back again
  mafMatrix3x3::Transpose(V,V);
}


//----------------------------------------------------------------------------
// Perform singular value decomposition on the matrix A:
//    A = U * W * VT
// where U and VT are orthogonal W is diagonal (the diagonal elements
// are returned in vector w).
// The matrices U and VT will both have positive determinants.
// The scale factors w are ordered according to how well the
// corresponding eigenvectors (in VT) match the x, y and z axes
// respectively.
//
// The singular value decomposition is used to decompose a linear
// transformation into a rotation, followed by a scale, followed
// by a second rotation.  The scale factors w will be negative if
// the determinant of matrix A is negative.
//
// by David Gobbi (dgobbi@irus.rri.on.ca)
void mafMatrix3x3::SingularValueDecomposition(const double A[3][3], 
                                          double U[3][3], double w[3],
                                          double VT[3][3])
//----------------------------------------------------------------------------
{
  int i;
  double B[3][3];

  // copy so that A can be used for U or VT without risk
  for (i = 0; i < 3; i++)
  {
    B[0][i] = A[0][i];
    B[1][i] = A[1][i];
    B[2][i] = A[2][i];
  }

  // temporarily flip if determinant is negative
  double d = mafMatrix3x3::Determinant(B);
  if (d < 0)
  {
    for (i = 0; i < 3; i++)
    {
      B[0][i] = -B[0][i];
      B[1][i] = -B[1][i];
      B[2][i] = -B[2][i];
    }
  }

  // orthogonalize, diagonalize, etc.
  mafMatrix3x3::Orthogonalize(B, U);
  mafMatrix3x3::Transpose(B, B);
  mafMatrix3x3::Multiply(B, U, VT);
  mafMatrix3x3::Diagonalize(VT, w, VT);
  mafMatrix3x3::Multiply(U, VT, U);
  mafMatrix3x3::Transpose(VT, VT);

  // re-create the flip
  if (d < 0)
  {
    w[0] = -w[0];
    w[1] = -w[1];
    w[2] = -w[2];
  }
}

