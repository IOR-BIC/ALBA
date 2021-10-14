/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorGenericInterface
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// To be included first because of wxWindows
#include "albaDevice.h"

#include "albaInteractorGenericInterface.h"

#include "albaRefSys.h"
#include "albaInteractorConstraint.h"
#include "albaEventBase.h"
#include "albaVME.h"
#include "albaMatrix.h"
#include "albaTransform.h"
#include "mmuIdFactory.h"

#include "vtkRenderer.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
ALBA_ID_IMP(albaInteractorGenericInterface::MOVE_EVENT);

//------------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaInteractorGenericInterface)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaInteractorGenericInterface::albaInteractorGenericInterface()
//------------------------------------------------------------------------------
{
  m_TranslationFlag       = true;
  m_RotationFlag          = true;
  m_ScalingFlag           = true;
  m_UniformScalingFlag    = true;
  m_SurfaceSnap           = false;
	m_SurfaceNormal         = false;
  m_ResultTransform       = NULL;
  m_TargetRefSys          = new albaRefSys;
  m_TranslationConstraint = new albaInteractorConstraint;
  m_RotationConstraint    = new albaInteractorConstraint;
  m_ScaleConstraint       = new albaInteractorConstraint;
  m_PivotRefSys           = new albaRefSys;
  m_RotationConstraint->GetRefSys()->SetTypeToView(); // the Renderer is set later!!!
  m_TranslationConstraint->GetRefSys()->SetTypeToView(); // the Renderer is set later!!!
  m_PivotRefSys->SetTypeToLocal(); // the VME is set later !!!
}

//------------------------------------------------------------------------------
albaInteractorGenericInterface::~albaInteractorGenericInterface()
//------------------------------------------------------------------------------
{
  albaDEL(m_ResultTransform);
  delete m_TargetRefSys; m_TargetRefSys = NULL;
  delete m_PivotRefSys; m_PivotRefSys = NULL;
  delete m_TranslationConstraint; m_TranslationConstraint = NULL;
  delete m_RotationConstraint; m_RotationConstraint = NULL;
  delete m_ScaleConstraint; m_ScaleConstraint = NULL;
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::SetResultTransform(albaTransform *result)
//------------------------------------------------------------------------------
{
  if (result!=m_ResultTransform)
  {
    albaDEL(m_ResultTransform);
    m_ResultTransform=result;
    m_ResultTransform->Register(this);
  }
}
//------------------------------------------------------------------------------
void albaInteractorGenericInterface::SetResultMatrix(albaMatrix *result)
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaTransform> trans; // create a transform on the fly
  trans->SetMatrixPointer(result); // make it reference the result matrix
  SetResultTransform(trans);
}

//------------------------------------------------------------------------------
albaMatrix *albaInteractorGenericInterface::GetResultMatrix()
//------------------------------------------------------------------------------
{ 
  return (m_ResultTransform?m_ResultTransform->GetMatrixPointer():NULL);
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::SetTargetRefSys(albaRefSys &ref_sys)
//------------------------------------------------------------------------------
{
  *m_TargetRefSys=ref_sys;
}
//------------------------------------------------------------------------------
void albaInteractorGenericInterface::SetTargetRefSys(albaRefSys *ref_sys)
//------------------------------------------------------------------------------
{
  m_TargetRefSys->DeepCopy(ref_sys);
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::SetPivotRefSys(albaRefSys *pivot_frame )
//------------------------------------------------------------------------------
{
  m_PivotRefSys->DeepCopy(pivot_frame);
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::SetPivotRefSys(albaRefSys &pivot_frame )
//------------------------------------------------------------------------------
{
  *m_PivotRefSys=pivot_frame;
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::SetRendererAndView(vtkRenderer *ren, albaView *view)
//------------------------------------------------------------------------------
{
  m_TranslationConstraint->GetRefSys()->SetRenderer(ren);  
  m_RotationConstraint->GetRefSys()->SetRenderer(ren);  
  m_ScaleConstraint->GetRefSys()->SetRenderer(ren);  
  m_PivotRefSys->SetRenderer(ren);
  Superclass::SetRendererAndView(ren,view);
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::SetTranslationConstraint(albaInteractorConstraint *constrain)
//------------------------------------------------------------------------------
{
  *m_TranslationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::SetRotationConstraint(albaInteractorConstraint *constrain)
//------------------------------------------------------------------------------
{
  *m_RotationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::SetScaleConstraint(albaInteractorConstraint *constrain)
//------------------------------------------------------------------------------
{
  *m_ScaleConstraint=*constrain;
}
 
//------------------------------------------------------------------------------
void albaInteractorGenericInterface::SetVME(albaVME *vme)
//------------------------------------------------------------------------------
{
  Superclass::SetVME(vme);
  m_RotationConstraint->GetRefSys()->SetVME(vme);
  m_TranslationConstraint->GetRefSys()->SetVME(vme);
  m_ScaleConstraint->GetRefSys()->SetVME(vme);
  m_PivotRefSys->SetVME(vme);
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::EnableTranslation(bool enable)
//------------------------------------------------------------------------------
{
  EnableTranslationInternal(enable);
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::EnableRotation(bool enable)
//------------------------------------------------------------------------------
{
  EnableRotationInternal(enable);
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::EnableScaling(bool enable)
// ------------------------------------------------------------------------------
{
  EnableScalingInternal(enable);
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::EnableUniformScaling(bool enable)
//------------------------------------------------------------------------------
{
  EnableUniformScalingInternal(enable);
} 

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::EnableTranslationInternal(bool enable)
//------------------------------------------------------------------------------
{
  m_TranslationFlag = enable;
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::EnableRotationInternal(bool enable)
//------------------------------------------------------------------------------
{
  m_RotationFlag = enable;
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::EnableScalingInternal(bool enable)
//------------------------------------------------------------------------------
{
  m_ScalingFlag = enable;
}

//------------------------------------------------------------------------------
void albaInteractorGenericInterface::EnableUniformScalingInternal(bool enable)
//------------------------------------------------------------------------------
{
   m_UniformScalingFlag = enable;
}
