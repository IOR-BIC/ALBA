/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformBase
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaTransformBase.h"

#include "albaIndent.h"
#include "albaMatrix.h"

//#include "vtkPoints.h" //SIL. 23-3-2005: 
#ifdef ALBA_USE_VTK
  #include "vtkALBAToLinearTransform.h"
#endif

#include <ostream>

albaCxxAbstractTypeMacro(albaTransformBase);

//----------------------------------------------------------------------------
albaTransformBase::albaTransformBase()
//----------------------------------------------------------------------------
{
  albaNEW(m_Matrix); // dynamic allocation to allow reference counting
#ifdef ALBA_USE_VTK
  m_VTKTransform = NULL;
#endif
  m_TimeStamp = 0;
}

//----------------------------------------------------------------------------
albaTransformBase::~albaTransformBase()
//----------------------------------------------------------------------------
{
  albaDEL(m_Matrix);
#ifdef ALBA_USE_VTK
  vtkDEL(m_VTKTransform);
#endif
}

//----------------------------------------------------------------------------
albaTransformBase::albaTransformBase(const albaTransformBase& copy)
//----------------------------------------------------------------------------
{
	albaNEW(m_Matrix); // dynamic allocation to allow reference counting
	m_TimeStamp = 0;

  m_Matrix->DeepCopy(copy.m_Matrix);
#ifdef ALBA_USE_VTK
  if(copy.m_VTKTransform)
	  GetVTKTransform()->DeepCopy(copy.m_VTKTransform);
  else
    m_VTKTransform = NULL;
#endif
	m_TimeStamp = copy.m_TimeStamp;
}

//----------------------------------------------------------------------------
void albaTransformBase::Print(std::ostream& os, const int indent) const
//----------------------------------------------------------------------------
{
  Superclass::Print(os,indent);

  albaIndent the_indent(indent);
  os << the_indent << "Matrix:" << std::endl;
  m_Matrix->Print(os,the_indent.GetNextIndent());
  
}

//------------------------------------------------------------------------
template <class T1, class T2, class T3>
inline double albaHomogeneousTransformPoint(T1 M[4][4], T2 in[3], T3 out[3])
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
void albaTransformBase::InternalTransformPoint(const float in[3], float out[3])
//------------------------------------------------------------------------
{
  albaHomogeneousTransformPoint(m_Matrix->GetElements(),in,out);
}

//------------------------------------------------------------------------
void albaTransformBase::InternalTransformPoint(const double in[3], double out[3])
//------------------------------------------------------------------------
{
  albaHomogeneousTransformPoint(m_Matrix->GetElements(),in,out);
}

#ifdef ALBA_USE_VTK
//----------------------------------------------------------------------------
vtkLinearTransform *albaTransformBase::GetVTKTransform()
//----------------------------------------------------------------------------
{
  if (!m_VTKTransform)
  {
    m_VTKTransform = vtkALBAToLinearTransform::New();
    m_VTKTransform->SetInputTransform(this);
  }
  return m_VTKTransform;
}
#endif

//----------------------------------------------------------------------------
void albaTransformBase::Update()
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
