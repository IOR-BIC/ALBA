/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTransformBase.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-25 19:16:43 $
  Version:   $Revision: 1.2 $
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

