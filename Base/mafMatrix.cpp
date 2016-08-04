/*=========================================================================

 Program: MAF2
 Module: mafMatrix
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "mafMatrix.h"
#include "mafMatrix3x3.h"
#include "mafIndent.h"
#include <assert.h>
//#include <winbase.h>

#ifdef MAF_USE_VTK
  #include "vtkMatrix4x4.h"
#endif


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafMatrix);
//----------------------------------------------------------------------------
//#include "mafMemDbg.h"

//----------------------------------------------------------------------------
mafMatrix::mafMatrix()
//----------------------------------------------------------------------------
{
  m_TimeStamp=0;

#ifdef MAF_USE_VTK 
  // in case we are building under VTK we store the elements in a VTK Matrix
  vtkNEW(m_VTKMatrix);
#endif
}

//----------------------------------------------------------------------------
mafMatrix::~mafMatrix()
//----------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  vtkDEL(m_VTKMatrix);
#endif
}

//------------------------------------------------------------------------------
mafMatrix &mafMatrix::operator=(const mafMatrix &mat)
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  m_VTKMatrix->DeepCopy(mat.m_VTKMatrix);
#else
  for (int i=0;i<4;i++)
    for (int j=0;j<4;j++)
      m_Elements[i][j]=mat.m_Elements[i][j];
#endif  

  m_TimeStamp=mat.m_TimeStamp;
  Modified();
  return *this;
}
//------------------------------------------------------------------------------
mafMatrix::mafMatrix(const mafMatrix &mat)
//------------------------------------------------------------------------------
{
  m_TimeStamp=0;

#ifdef MAF_USE_VTK 
  // in case we are building under VTK we store the elements in a VTK Matrix
  vtkNEW(m_VTKMatrix);
#endif

  *this=mat;
}

//------------------------------------------------------------------------------
bool mafMatrix::operator==(const mafMatrix& mat) const
//------------------------------------------------------------------------------
{
  if (!mafEquals(m_TimeStamp,mat.m_TimeStamp))
    return false;

  for (int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      mafMatrixElements myelements = GetElements();
      mafMatrixElements otherelements = mat.GetElements();
      if (!mafEquals(myelements[i][j],otherelements[i][j])) // only 15 digits are considered since the I/O mechanism saves only 16 digits to avoid dirty bits
        return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
bool mafMatrix::Equals(const mafMatrix *mat) const
//------------------------------------------------------------------------------
{
  assert(mat);
  return (*this==*mat);
}

#ifdef MAF_USE_VTK
//------------------------------------------------------------------------------
mafMatrix::mafMatrix(vtkMatrix4x4 *mat)
//------------------------------------------------------------------------------
{
  m_VTKMatrix=NULL;
  m_TimeStamp=0;
  SetVTKMatrix(mat);

}

//------------------------------------------------------------------------------
//mafMatrix::mafMatrix(vtkMatrix4x4* mat, mafTimeStamp t)
void mafMatrix::SetVTKMatrix(vtkMatrix4x4 *mat,mafTimeStamp t)
//----------------------------------------------------------------------------
{
  assert(mat);
  assert(t>=0);
  if (mat&&t>=0)
  {
    vtkMatrix4x4 *old_mat=m_VTKMatrix;
    m_VTKMatrix=mat;
    m_VTKMatrix->Register(NULL);
    if (old_mat)
      vtkDEL(old_mat);
    m_TimeStamp=t;
    Modified();
  }
  else
  {
    mafErrorMacro("Trying to set NULL internl VTK matrix or negative timestamp");
  }
}
//------------------------------------------------------------------------------
//mafMatrix::mafMatrix(vtkMatrix4x4* mat, mafTimeStamp t)
void mafMatrix::DeepCopy(vtkMatrix4x4 *mat)
//----------------------------------------------------------------------------
{
  assert(mat);
  if (mat)
  {
    m_VTKMatrix->DeepCopy(mat);
    Modified();
  }
  else
  {
    mafErrorMacro("Trying to copy NULL pointer matrix");
  }
}

//------------------------------------------------------------------------------
bool mafMatrix::operator==(vtkMatrix4x4 *mat) const
//------------------------------------------------------------------------------
{
  for (int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      mafMatrixElements myelements=GetElements(); 
      if (fabs(myelements[i][j]-mat->Element[i][j])>1.0e-17)
        return false;
    }
  }
  return true;
}
#endif

//------------------------------------------------------------------------------
unsigned long mafMatrix::GetMTime() const
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  return m_VTKMatrix->GetMTime();
#else
  return m_MTime.GetMTime();
#endif
}

//------------------------------------------------------------------------------
void mafMatrix::Modified()
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  m_VTKMatrix->Modified();
#else
  m_MTime.Modified();
#endif
}


//------------------------------------------------------------------------------
mafMatrixElements mafMatrix::GetElements() const
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  return (mafMatrixElements)m_VTKMatrix->Element;
#else
  return (mafMatrixElements)m_Elements;
#endif
}

//------------------------------------------------------------------------------
void mafMatrix::Print (std::ostream& os, const int indent) const
//------------------------------------------------------------------------------
{
  this->Superclass::Print(os, indent);
  mafIndent the_indent(indent);
  mafIndent next_indent = the_indent.GetNextIndent();

  int i, j;
  
  os << the_indent << "TimeStamp: " << m_TimeStamp << std::endl;
  os << the_indent << "Elements:\n";
  for (i = 0; i < 4; i++) 
  {
    os << next_indent;
    for (j = 0; j < 4; j++) 
    {
      os << GetElements()[i][j] << " ";
    }
    os << std::endl;
  }
}

//----------------------------------------------------------------------------
void mafMatrix::GetVersor(int axis, const mafMatrix &matrix, double versor[3])
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
void mafMatrix::CopyRotation(const mafMatrix &source, mafMatrix &target)
//----------------------------------------------------------------------------
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
		  target.SetElement(i,j, source.GetElement(i,j));
		}
	}
  target.Modified();
}

//----------------------------------------------------------------------------
void mafMatrix::Zero(double elements[16])
//----------------------------------------------------------------------------
{
  mafMatrixElements elem  = (mafMatrixElements)elements;
  int i,j;
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      elem[i][j] = 0.0;
    }
  }
}

//----------------------------------------------------------------------------
void mafMatrix::Identity(double elements[16])
//----------------------------------------------------------------------------
{
  elements[0] = elements[5] = elements[10] = elements[15] = 1.0;
  elements[1] = elements[2] = elements[3] = elements[4] = 
  elements[6] = elements[7] = elements[8] = elements[9] = 
  elements[11] = elements[12] = elements[13] = elements[14] = 0.0;

}

//----------------------------------------------------------------------------
// Multiplies matrices a and b and stores the result in c.
void mafMatrix::Multiply4x4(const double a[16], const double b[16], double c[16])
//----------------------------------------------------------------------------
{
  mafMatrixElements aMat = (mafMatrixElements) a;
  mafMatrixElements bMat = (mafMatrixElements) b;
  mafMatrixElements cMat = (mafMatrixElements) c;
  int i, k;
  double Accum[4][4];

  for (i = 0; i < 4; i++) 
  {
    for (k = 0; k < 4; k++) 
    {
      Accum[i][k] = aMat[i][0] * bMat[0][k] +
                    aMat[i][1] * bMat[1][k] +
                    aMat[i][2] * bMat[2][k] +
                    aMat[i][3] * bMat[3][k];
    }
  }

  // Copy to final dest
  for (i = 0; i < 4; i++)
  {
    cMat[i][0] = Accum[i][0];
    cMat[i][1] = Accum[i][1];
    cMat[i][2] = Accum[i][2];
    cMat[i][3] = Accum[i][3];
  }
}

//----------------------------------------------------------------------------
// Transpose the matrix and put it into out.   
void mafMatrix::Transpose(const double inElements[16], 
                              double outElements[16])
//----------------------------------------------------------------------------
{
  mafMatrixElements inElem = (mafMatrixElements)inElements;
  mafMatrixElements outElem = (mafMatrixElements)outElements;
  int i, j;
  double temp;

  for (i=0; i<4; i++)
  {
    for(j=i; j<4; j++)
    {
      temp = inElem[i][j];
      outElem[i][j] = inElem[j][i];
      outElem[j][i] = temp;
    }
  }
}

//----------------------------------------------------------------------------
// Matrix Inversion (adapted from Richard Carling in "Graphics Gems," 
// Academic Press, 1990).
void mafMatrix::Invert(const double inElements[16], double outElements[16])
//----------------------------------------------------------------------------
{
  mafMatrixElements outElem = (mafMatrixElements)outElements;

  // inverse( original_matrix, inverse_matrix )
  // calculate the inverse of a 4x4 matrix
  //
  //     -1     
  //     A  = ___1__ adjoint A
  //         det A
  //
  
  int i, j;
  double det;

  // calculate the 4x4 determinant
  // if the determinant is zero, 
  // then the inverse matrix is not unique.

  det = mafMatrix::Determinant(inElements);
  if ( det == 0.0 ) 
    {
    //vtkErrorMacro(<< "Singular matrix, no inverse!" );
    return;
    }

  // calculate the adjoint matrix
  mafMatrix::Adjoint(inElements, outElements );

  // scale the adjoint matrix to get the inverse
  for (i=0; i<4; i++)
    {
    for(j=0; j<4; j++)
      {
      outElem[i][j] = outElem[i][j] / det;
      }
    }
}

//----------------------------------------------------------------------------
double mafMatrix::Determinant(const double Elements[16])
//----------------------------------------------------------------------------
{
  mafMatrixElements elem = (mafMatrixElements)Elements;

  double a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

  // assign to individual variable names to aid selecting
  //  correct elements

  a1 = elem[0][0]; b1 = elem[0][1]; 
  c1 = elem[0][2]; d1 = elem[0][3];

  a2 = elem[1][0]; b2 = elem[1][1]; 
  c2 = elem[1][2]; d2 = elem[1][3];

  a3 = elem[2][0]; b3 = elem[2][1]; 
  c3 = elem[2][2]; d3 = elem[2][3];

  a4 = elem[3][0]; b4 = elem[3][1]; 
  c4 = elem[3][2]; d4 = elem[3][3];

  return a1 * mafMatrix3x3::Determinant( b2, b3, b4, c2, c3, c4, d2, d3, d4)
       - b1 * mafMatrix3x3::Determinant( a2, a3, a4, c2, c3, c4, d2, d3, d4)
       + c1 * mafMatrix3x3::Determinant( a2, a3, a4, b2, b3, b4, d2, d3, d4)
       - d1 * mafMatrix3x3::Determinant( a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

//----------------------------------------------------------------------------
void mafMatrix::Adjoint(const double inElements[16], double outElements[16])
//----------------------------------------------------------------------------
{
  mafMatrixElements inElem = (mafMatrixElements) inElements;
  mafMatrixElements outElem = (mafMatrixElements) outElements;

  // 
  //   adjoint( original_matrix, inverse_matrix )
  // 
  //     calculate the adjoint of a 4x4 matrix
  //
  //      Let  a   denote the minor determinant of matrix A obtained by
  //           ij
  //
  //      deleting the ith row and jth column from A.
  //
  //                    i+j
  //     Let  b   = (-1)    a
  //          ij            ji
  //
  //    The matrix B = (b  ) is the adjoint of A
  //                     ij
  //
  double a1, a2, a3, a4, b1, b2, b3, b4;
  double c1, c2, c3, c4, d1, d2, d3, d4;

  // assign to individual variable names to aid
  // selecting correct values

  a1 = inElem[0][0]; b1 = inElem[0][1]; 
  c1 = inElem[0][2]; d1 = inElem[0][3];

  a2 = inElem[1][0]; b2 = inElem[1][1]; 
  c2 = inElem[1][2]; d2 = inElem[1][3];

  a3 = inElem[2][0]; b3 = inElem[2][1];
  c3 = inElem[2][2]; d3 = inElem[2][3];

  a4 = inElem[3][0]; b4 = inElem[3][1]; 
  c4 = inElem[3][2]; d4 = inElem[3][3];


  // row column labeling reversed since we transpose rows & columns

  outElem[0][0]  =   
    mafMatrix3x3::Determinant( b2, b3, b4, c2, c3, c4, d2, d3, d4);
  outElem[1][0]  = 
    - mafMatrix3x3::Determinant( a2, a3, a4, c2, c3, c4, d2, d3, d4);
  outElem[2][0]  =   
    mafMatrix3x3::Determinant( a2, a3, a4, b2, b3, b4, d2, d3, d4);
  outElem[3][0]  = 
    - mafMatrix3x3::Determinant( a2, a3, a4, b2, b3, b4, c2, c3, c4);

  outElem[0][1]  = 
    - mafMatrix3x3::Determinant( b1, b3, b4, c1, c3, c4, d1, d3, d4);
  outElem[1][1]  =   
    mafMatrix3x3::Determinant( a1, a3, a4, c1, c3, c4, d1, d3, d4);
  outElem[2][1]  = 
    - mafMatrix3x3::Determinant( a1, a3, a4, b1, b3, b4, d1, d3, d4);
  outElem[3][1]  =   
    mafMatrix3x3::Determinant( a1, a3, a4, b1, b3, b4, c1, c3, c4);
        
  outElem[0][2]  =   
    mafMatrix3x3::Determinant( b1, b2, b4, c1, c2, c4, d1, d2, d4);
  outElem[1][2]  = 
    - mafMatrix3x3::Determinant( a1, a2, a4, c1, c2, c4, d1, d2, d4);
  outElem[2][2]  =   
    mafMatrix3x3::Determinant( a1, a2, a4, b1, b2, b4, d1, d2, d4);
  outElem[3][2]  = 
    - mafMatrix3x3::Determinant( a1, a2, a4, b1, b2, b4, c1, c2, c4);
        
  outElem[0][3]  = 
    - mafMatrix3x3::Determinant( b1, b2, b3, c1, c2, c3, d1, d2, d3);
  outElem[1][3]  =   
    mafMatrix3x3::Determinant( a1, a2, a3, c1, c2, c3, d1, d2, d3);
  outElem[2][3]  = 
    - mafMatrix3x3::Determinant( a1, a2, a3, b1, b2, b3, d1, d2, d3);
  outElem[3][3]  =   
    mafMatrix3x3::Determinant( a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

//----------------------------------------------------------------------------
void mafMatrix::MultiplyPoint(const double elem[16], 
                                 const double in[4], double out[4])
//----------------------------------------------------------------------------
{
  double v1 = in[0];
  double v2 = in[1];
  double v3 = in[2];
  double v4 = in[3];

  out[0] = v1*elem[0]  + v2*elem[1]  + v3*elem[2]  + v4*elem[3];
  out[1] = v1*elem[4]  + v2*elem[5]  + v3*elem[6]  + v4*elem[7];
  out[2] = v1*elem[8]  + v2*elem[9]  + v3*elem[10] + v4*elem[11];
  out[3] = v1*elem[12] + v2*elem[13] + v3*elem[14] + v4*elem[15];
}

//----------------------------------------------------------------------------
void mafMatrix::PointMultiply(const double Elements[16], 
                                 const double in[4], double result[4])
//----------------------------------------------------------------------------
{
  double newElements[16];
  mafMatrix::Transpose(Elements,newElements);
  mafMatrix::MultiplyPoint(newElements,in,result);
}

//----------------------------------------------------------------------------
void mafMatrix::SetFromDirectionCosines(const double orientation[6])
{
	//transform direction cosines to be used to set vtkMatrix
	/*
	[ orientation[0] orientation[3] dst_nrm_dircos_x  0 ]
	[ orientation[1] orientation[4] dst_nrm_dircos_y  0 ]
	[ orientation[2] orientation[5] dst_nrm_dircos_z  0 ]
	[ 0                 0                 0           1 ]*/

	double dst_nrm_dircos_x = orientation[1] * orientation[5] - orientation[2] * orientation[4];
	double dst_nrm_dircos_y = orientation[2] * orientation[3] - orientation[0] * orientation[5];
	double dst_nrm_dircos_z = orientation[0] * orientation[4] - orientation[1] * orientation[3];

	m_VTKMatrix->Identity();

	m_VTKMatrix->SetElement(0, 0, orientation[0]);
	m_VTKMatrix->SetElement(1, 0, orientation[1]);
	m_VTKMatrix->SetElement(2, 0, orientation[2]);
	m_VTKMatrix->SetElement(3, 0, 0);
	m_VTKMatrix->SetElement(0, 1, orientation[3]);
	m_VTKMatrix->SetElement(1, 1, orientation[4]);
	m_VTKMatrix->SetElement(2, 1, orientation[5]);
	m_VTKMatrix->SetElement(3, 1, 0);
	m_VTKMatrix->SetElement(0, 2, dst_nrm_dircos_x);
	m_VTKMatrix->SetElement(1, 2, dst_nrm_dircos_y);
	m_VTKMatrix->SetElement(2, 2, dst_nrm_dircos_z);
	m_VTKMatrix->SetElement(3, 2, 0);
	m_VTKMatrix->SetElement(3, 3, 1);	
}
