/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTransformBase.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-29 09:33:05 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafTransformBase.h"

//#include "vtkMath.h"
#include "mafMatrix.h"
#include "vtkPoints.h"
#include "vtkMAFToLinearTransform.h"

mafCxxAbstractTypeMacro(mafTransformBase);

//----------------------------------------------------------------------------
mafTransformBase::mafTransformBase()
//----------------------------------------------------------------------------
{
  m_VTKTransform = NULL;
  //m_MyInverse = NULL;
}

//----------------------------------------------------------------------------
mafTransformBase::~mafTransformBase()
//----------------------------------------------------------------------------
{
  vtkDEL(m_VTKTransform);
  //mafDEL(m_MyInverse);
}

//----------------------------------------------------------------------------
mafTransformBase::mafTransformBase(const mafTransformBase& copy)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
/*void mafTransformBase::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os, indent);
}*/

//------------------------------------------------------------------------
template <class T1, class T2, class T3>
inline double mafHomogeneousTransformPoint(T1 M[4][4], T2 in[3], T3 out[3])
//------------------------------------------------------------------------
{
  double x = M[0][0]*in[0] + M[0][1]*in[1] + M[0][2]*in[2] + M[0][3];
  double y = M[1][0]*in[0] + M[1][1]*in[1] + M[1][2]*in[2] + M[1][3];
  double z = M[2][0]*in[0] + M[2][1]*in[1] + M[2][2]*in[2] + M[2][3];
  double w = M[3][0]*in[0] + M[3][1]*in[1] + M[3][2]*in[2] + M[3][3];

  double f = 1.0/w;
  out[0] = x*f; 
  out[1] = y*f; 
  out[2] = z*f;

  return f;
}
//------------------------------------------------------------------------
void mafTransformBase::InternalTransformPoint(const float in[3], float out[3])
//------------------------------------------------------------------------
{
  mafHomogeneousTransformPoint(m_Matrix.GetElements(),in,out);
}

//------------------------------------------------------------------------
void mafTransformBase::InternalTransformPoint(const double in[3], double out[3])
//------------------------------------------------------------------------
{
  mafHomogeneousTransformPoint(m_Matrix.GetElements(),in,out);
}

//----------------------------------------------------------------------------
mafTransformBase *mafTransformBase::GetInverse()
//----------------------------------------------------------------------------
{
// to be implemented
  m_InverseMutex.Lock();
  /*if (this->m_MyInverse == NULL)
  {
    mafTransformInverse *my_inverse = new mafTransformInverse;
    my_inverse->SetInverse(this);
    m_MyInverse=my_inverse;
  }*/
 m_InverseMutex.Unlock();
 return m_MyInverse;
}

//----------------------------------------------------------------------------
vtkLinearTransform *mafTransformBase::GetVTKTransform()
//----------------------------------------------------------------------------
{
  if (!m_VTKTransform)
  {
    m_VTKTransform = vtkMAFToLinearTransform::New();
  }
  return m_VTKTransform;
}

//----------------------------------------------------------------------------
void mafTransformBase::Update()
//----------------------------------------------------------------------------
{
  // locking is require to ensure that the class is thread-safe
  m_UpdateMutex.Lock();

  if (GetMTime() >= m_UpdateTime.GetMTime())
  {  
    InternalUpdate();
    m_UpdateTime.Modified();
  }
  m_UpdateMutex.Unlock();
}

//----------------------------------------------------------------------------
/*int mafTransformBase::CircuitCheck(mafTransformBase *transform)
//----------------------------------------------------------------------------
{
  return (transform == this || (this->DependsOnInverse && 
                                this->m_MyInverse->CircuitCheck(transform)));
}*/

