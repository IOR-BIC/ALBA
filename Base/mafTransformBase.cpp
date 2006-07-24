/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTransformBase.cpp,v $
  Language:  C++
  Date:      $Date: 2006-07-24 08:52:50 $
  Version:   $Revision: 1.9 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafTransformBase.h"

#include "mafIndent.h"
#include "mafMatrix.h"
#include "mafEventSource.h"

//#include "vtkPoints.h" //SIL. 23-3-2005: 
#ifdef MAF_USE_VTK
  #include "vtkMAFToLinearTransform.h"
#endif

#include <ostream>

mafCxxAbstractTypeMacro(mafTransformBase);

//----------------------------------------------------------------------------
mafTransformBase::mafTransformBase()
//----------------------------------------------------------------------------
{
  mafNEW(m_Matrix); // dynamic allocation to allow reference counting
  m_EventSource = new mafEventSource;
#ifdef MAF_USE_VTK
  m_VTKTransform = NULL;
#endif
  m_TimeStamp = 0;
}

//----------------------------------------------------------------------------
mafTransformBase::~mafTransformBase()
//----------------------------------------------------------------------------
{
  cppDEL(m_EventSource);
  mafDEL(m_Matrix);
#ifdef MAF_USE_VTK
  vtkDEL(m_VTKTransform);
#endif
}

//----------------------------------------------------------------------------
mafTransformBase::mafTransformBase(const mafTransformBase& copy)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafTransformBase::Print(std::ostream& os, const int indent) const
//----------------------------------------------------------------------------
{
  Superclass::Print(os,indent);

  mafIndent the_indent(indent);
  os << the_indent << "Matrix:" << std::endl;
  m_Matrix->Print(os,the_indent.GetNextIndent());
  
}

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
  mafHomogeneousTransformPoint(m_Matrix->GetElements(),in,out);
}

//------------------------------------------------------------------------
void mafTransformBase::InternalTransformPoint(const double in[3], double out[3])
//------------------------------------------------------------------------
{
  mafHomogeneousTransformPoint(m_Matrix->GetElements(),in,out);
}

#ifdef MAF_USE_VTK
//----------------------------------------------------------------------------
vtkLinearTransform *mafTransformBase::GetVTKTransform()
//----------------------------------------------------------------------------
{
  if (!m_VTKTransform)
  {
    m_VTKTransform = vtkMAFToLinearTransform::New();
    m_VTKTransform->SetInputTransform(this);
  }
  return m_VTKTransform;
}
#endif

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
