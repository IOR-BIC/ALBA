/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiGenericInterface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-21 07:55:51 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#include "mafDevice.h"

#include "mmiGenericInterface.h"

#include "mmiConstraint.h"
#include "mafEventBase.h"
#include "mafVME.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mmuIdFactory.h"

#include "vtkRenderer.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
//MAF_ID_IMP(mmiGenericInterface::MOVE_EVENT);

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mmiGenericInterface)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmiGenericInterface::mmiGenericInterface()
//------------------------------------------------------------------------------
{
  m_TranslationFlag       = true;
  m_RotationFlag          = true;
  m_ScalingFlag           = true;
  m_UniformScalingFlag    = true;
  m_SurfaceSnap           = false;
  m_ResultTransform       = NULL;
  m_TargetRefSys          = new mafRefSys;
  m_TranslationConstraint = new mmiConstraint;
  m_RotationConstraint    = new mmiConstraint;
  m_ScaleConstraint       = new mmiConstraint;
  m_PivotRefSys           = new mafRefSys;
  m_RotationConstraint->GetRefSys()->SetTypeToView(); // the Renderer is set later!!!
  m_TranslationConstraint->GetRefSys()->SetTypeToView(); // the Renderer is set later!!!
  m_PivotRefSys->SetTypeToLocal(); // the VME is set later !!!
}

//------------------------------------------------------------------------------
mmiGenericInterface::~mmiGenericInterface()
//------------------------------------------------------------------------------
{
  mafDEL(m_ResultTransform);
  delete m_TargetRefSys; m_TargetRefSys = NULL;
  delete m_PivotRefSys; m_PivotRefSys = NULL;
  delete m_TranslationConstraint; m_TranslationConstraint = NULL;
  delete m_RotationConstraint; m_RotationConstraint = NULL;
  delete m_ScaleConstraint; m_ScaleConstraint = NULL;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetResultTransform(mafTransform *result)
//------------------------------------------------------------------------------
{
  if (result!=m_ResultTransform)
  {
    mafDEL(m_ResultTransform);
    m_ResultTransform=result;
    m_ResultTransform->Register(this);
  }
}
//------------------------------------------------------------------------------
void mmiGenericInterface::SetResultMatrix(mafMatrix *result)
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafTransform> trans; // create a transform on the fly
  trans->SetMatrixPointer(result); // make it reference the result matrix
  SetResultTransform(trans);
}

//------------------------------------------------------------------------------
mafMatrix *mmiGenericInterface::GetResultMatrix()
//------------------------------------------------------------------------------
{ 
  return (m_ResultTransform?m_ResultTransform->GetMatrixPointer():NULL);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetTargetRefSys(mafRefSys &ref_sys)
//------------------------------------------------------------------------------
{
  *m_TargetRefSys=ref_sys;
}
//------------------------------------------------------------------------------
void mmiGenericInterface::SetTargetRefSys(mafRefSys *ref_sys)
//------------------------------------------------------------------------------
{
  m_TargetRefSys->DeepCopy(ref_sys);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetPivotRefSys(mafRefSys *pivot_frame )
//------------------------------------------------------------------------------
{
  m_PivotRefSys->DeepCopy(pivot_frame);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetPivotRefSys(mafRefSys &pivot_frame )
//------------------------------------------------------------------------------
{
  *m_PivotRefSys=pivot_frame;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  m_TranslationConstraint->GetRefSys()->SetRenderer(ren);  
  m_RotationConstraint->GetRefSys()->SetRenderer(ren);  
  m_ScaleConstraint->GetRefSys()->SetRenderer(ren);  
  m_PivotRefSys->SetRenderer(ren);
  Superclass::SetRenderer(ren);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetTranslationConstraint(mmiConstraint *constrain)
//------------------------------------------------------------------------------
{
  *m_TranslationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetRotationConstraint(mmiConstraint *constrain)
//------------------------------------------------------------------------------
{
  *m_RotationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetScaleConstraint(mmiConstraint *constrain)
//------------------------------------------------------------------------------
{
  *m_ScaleConstraint=*constrain;
}
 
//------------------------------------------------------------------------------
void mmiGenericInterface::SetVME(mafVME *vme)
//------------------------------------------------------------------------------
{
  Superclass::SetVME(vme);
  m_RotationConstraint->GetRefSys()->SetVME(vme);
  m_TranslationConstraint->GetRefSys()->SetVME(vme);
  m_ScaleConstraint->GetRefSys()->SetVME(vme);
  m_PivotRefSys->SetVME(vme);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::EnableTranslation(bool enable)
//------------------------------------------------------------------------------
{
  EnableTranslationInternal(enable);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::EnableRotation(bool enable)
//------------------------------------------------------------------------------
{
  EnableRotationInternal(enable);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::EnableScaling(bool enable)
// ------------------------------------------------------------------------------
{
  EnableScalingInternal(enable);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::EnableUniformScaling(bool enable)
//------------------------------------------------------------------------------
{
  EnableUniformScalingInternal(enable);
} 

//------------------------------------------------------------------------------
void mmiGenericInterface::EnableTranslationInternal(bool enable)
//------------------------------------------------------------------------------
{
  m_TranslationFlag = enable;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::EnableRotationInternal(bool enable)
//------------------------------------------------------------------------------
{
  m_RotationFlag = enable;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::EnableScalingInternal(bool enable)
//------------------------------------------------------------------------------
{
  m_ScalingFlag = enable;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::EnableUniformScalingInternal(bool enable)
//------------------------------------------------------------------------------
{
   m_UniformScalingFlag = enable;
}
