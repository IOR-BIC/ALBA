/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractorGenericInterface.cpp,v $
  Language:  C++
  Date:      $Date: 2009-12-17 11:46:40 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#include "mafDevice.h"

#include "mafInteractorGenericInterface.h"

#include "mafInteractorConstraint.h"
#include "mafEventBase.h"
#include "mafVME.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mmuIdFactory.h"

#include "vtkRenderer.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafInteractorGenericInterface::MOVE_EVENT);

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafInteractorGenericInterface)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafInteractorGenericInterface::mafInteractorGenericInterface()
//------------------------------------------------------------------------------
{
  m_TranslationFlag       = true;
  m_RotationFlag          = true;
  m_ScalingFlag           = true;
  m_UniformScalingFlag    = true;
  m_SurfaceSnap           = false;
	m_SurfaceNormal         = false;
  m_ResultTransform       = NULL;
  m_TargetRefSys          = new mafRefSys;
  m_TranslationConstraint = new mafInteractorConstraint;
  m_RotationConstraint    = new mafInteractorConstraint;
  m_ScaleConstraint       = new mafInteractorConstraint;
  m_PivotRefSys           = new mafRefSys;
  m_RotationConstraint->GetRefSys()->SetTypeToView(); // the Renderer is set later!!!
  m_TranslationConstraint->GetRefSys()->SetTypeToView(); // the Renderer is set later!!!
  m_PivotRefSys->SetTypeToLocal(); // the VME is set later !!!
}

//------------------------------------------------------------------------------
mafInteractorGenericInterface::~mafInteractorGenericInterface()
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
void mafInteractorGenericInterface::SetResultTransform(mafTransform *result)
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
void mafInteractorGenericInterface::SetResultMatrix(mafMatrix *result)
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafTransform> trans; // create a transform on the fly
  trans->SetMatrixPointer(result); // make it reference the result matrix
  SetResultTransform(trans);
}

//------------------------------------------------------------------------------
mafMatrix *mafInteractorGenericInterface::GetResultMatrix()
//------------------------------------------------------------------------------
{ 
  return (m_ResultTransform?m_ResultTransform->GetMatrixPointer():NULL);
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::SetTargetRefSys(mafRefSys &ref_sys)
//------------------------------------------------------------------------------
{
  *m_TargetRefSys=ref_sys;
}
//------------------------------------------------------------------------------
void mafInteractorGenericInterface::SetTargetRefSys(mafRefSys *ref_sys)
//------------------------------------------------------------------------------
{
  m_TargetRefSys->DeepCopy(ref_sys);
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::SetPivotRefSys(mafRefSys *pivot_frame )
//------------------------------------------------------------------------------
{
  m_PivotRefSys->DeepCopy(pivot_frame);
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::SetPivotRefSys(mafRefSys &pivot_frame )
//------------------------------------------------------------------------------
{
  *m_PivotRefSys=pivot_frame;
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  m_TranslationConstraint->GetRefSys()->SetRenderer(ren);  
  m_RotationConstraint->GetRefSys()->SetRenderer(ren);  
  m_ScaleConstraint->GetRefSys()->SetRenderer(ren);  
  m_PivotRefSys->SetRenderer(ren);
  Superclass::SetRenderer(ren);
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::SetTranslationConstraint(mafInteractorConstraint *constrain)
//------------------------------------------------------------------------------
{
  *m_TranslationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::SetRotationConstraint(mafInteractorConstraint *constrain)
//------------------------------------------------------------------------------
{
  *m_RotationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::SetScaleConstraint(mafInteractorConstraint *constrain)
//------------------------------------------------------------------------------
{
  *m_ScaleConstraint=*constrain;
}
 
//------------------------------------------------------------------------------
void mafInteractorGenericInterface::SetVME(mafVME *vme)
//------------------------------------------------------------------------------
{
  Superclass::SetVME(vme);
  m_RotationConstraint->GetRefSys()->SetVME(vme);
  m_TranslationConstraint->GetRefSys()->SetVME(vme);
  m_ScaleConstraint->GetRefSys()->SetVME(vme);
  m_PivotRefSys->SetVME(vme);
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::EnableTranslation(bool enable)
//------------------------------------------------------------------------------
{
  EnableTranslationInternal(enable);
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::EnableRotation(bool enable)
//------------------------------------------------------------------------------
{
  EnableRotationInternal(enable);
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::EnableScaling(bool enable)
// ------------------------------------------------------------------------------
{
  EnableScalingInternal(enable);
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::EnableUniformScaling(bool enable)
//------------------------------------------------------------------------------
{
  EnableUniformScalingInternal(enable);
} 

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::EnableTranslationInternal(bool enable)
//------------------------------------------------------------------------------
{
  m_TranslationFlag = enable;
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::EnableRotationInternal(bool enable)
//------------------------------------------------------------------------------
{
  m_RotationFlag = enable;
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::EnableScalingInternal(bool enable)
//------------------------------------------------------------------------------
{
  m_ScalingFlag = enable;
}

//------------------------------------------------------------------------------
void mafInteractorGenericInterface::EnableUniformScalingInternal(bool enable)
//------------------------------------------------------------------------------
{
   m_UniformScalingFlag = enable;
}
