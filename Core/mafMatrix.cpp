/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrix.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-04 09:29:15 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafMatrix.h"
#include "vtkObjectFactory.h"

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
mafMatrix::mafMatrix(const mafMatrix &mat)
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  m_VTKMatrix->DeepCopy(mat.m_VTKMatrix);
#endif  

  m_TimeStamp=mat.m_TimeStamp;
  Modified();
}

//------------------------------------------------------------------------------
bool mafMatrix::operator==(const mafMatrix& mat) const
//------------------------------------------------------------------------------
{
  if (m_TimeStamp!=mat.m_TimeStamp)
    return false;

  for (int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      if (abs(GetElements()[i][j]-mat.GetElements()[i][j])>(1e-17))
        return false;
    }
  }
  return true;
}

#ifdef MAF_USE_VTK
//------------------------------------------------------------------------------
mafMatrix::mafMatrix(vtkMatrix4x4* mat, mafTimeStamp t)
//----------------------------------------------------------------------------
{
  if (mat&&t>=0)
  {
    m_VTKMatrix->DeepCopy(mat);
    m_TimeStamp=t;
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
      if (abs(GetElements()[i][j]-mat->Element[i][j])>(1e-17))
        return false;
    }
  }
  return true;
}
#endif

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
