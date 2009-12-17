/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractorGeneric.cpp,v $
  Language:  C++
  Date:      $Date: 2009-12-17 11:46:39 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#include "mafDevice.h"

#include "mafInteractorGeneric.h"
#include "vtkObjectFactory.h"

#include "mafInteractorConstraint.h"
#include "mflEvent.h"
#include "mflVME.h"
#include "vtkMatrix4x4.h"
#include "mflTransform.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MFL_EVT_IMP(mafInteractorGeneric::MoveActionEvent);

//------------------------------------------------------------------------------
vtkStandardNewMacro(mafInteractorGeneric)
vtkCxxSetObjectMacro(mafInteractorGeneric,ResultTransform,mflTransform);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafInteractorGeneric::mafInteractorGeneric()
//------------------------------------------------------------------------------
{
  TranslationFlag       = true;
  RotationFlag          = true;
  ScalingFlag           = true;
  UniformScalingFlag    = true;
  ResultTransform       = NULL;
  TargetRefSys          = new mafRefSys;
  TranslationConstraint = new mafInteractorConstraint;
  RotationConstraint    = new mafInteractorConstraint;
  ScaleConstraint       = new mafInteractorConstraint;
  PivotRefSys           = new mafRefSys;
  RotationConstraint->GetRefSys()->SetTypeToCamera(); // the Renderer is set later!!!
  TranslationConstraint->GetRefSys()->SetTypeToCamera(); // the Renderer is set later!!!
  PivotRefSys->SetTypeToLocal(); // the VME is set later !!!
}

//------------------------------------------------------------------------------
mafInteractorGeneric::~mafInteractorGeneric()
//------------------------------------------------------------------------------
{
  vtkDEL(ResultTransform);
  delete TargetRefSys; TargetRefSys = NULL;
  delete TranslationConstraint; TranslationConstraint = NULL;
  delete RotationConstraint; RotationConstraint = NULL;
  delete ScaleConstraint; ScaleConstraint = NULL;
  delete PivotRefSys; PivotRefSys = NULL;
}

//------------------------------------------------------------------------------
void mafInteractorGeneric::SetResultMatrix(vtkMatrix4x4 *result)
//------------------------------------------------------------------------------
{
  mflSmartPointer<mflTransform> trans; // create a transform on the fly
  trans->SetMatrix(result,true); // make it reference the result matrix
  SetResultTransform(trans);
}

//------------------------------------------------------------------------------
vtkMatrix4x4 *mafInteractorGeneric::GetResultMatrix()
//------------------------------------------------------------------------------
{ 
  return (ResultTransform?ResultTransform->GetMatrix():NULL);
}

//------------------------------------------------------------------------------
void mafInteractorGeneric::SetTargetRefSys(mafRefSys &ref_sys)
//------------------------------------------------------------------------------
{
  *TargetRefSys=ref_sys;
}
//------------------------------------------------------------------------------
void mafInteractorGeneric::SetTargetRefSys(mafRefSys *ref_sys)
//------------------------------------------------------------------------------
{
  TargetRefSys->DeepCopy(ref_sys);
}
//------------------------------------------------------------------------------
void mafInteractorGeneric::SetPivotRefSys(mafRefSys *pivot_frame )
//------------------------------------------------------------------------------
{
  PivotRefSys->DeepCopy(pivot_frame);
}
//------------------------------------------------------------------------------
void mafInteractorGeneric::SetPivotRefSys(mafRefSys &pivot_frame )
//------------------------------------------------------------------------------
{
  *PivotRefSys=pivot_frame;
}

//------------------------------------------------------------------------------
void mafInteractorGeneric::SetTranslationConstraint(mafInteractorConstraint *constrain)
//------------------------------------------------------------------------------
{
  *TranslationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mafInteractorGeneric::SetRotationConstraint(mafInteractorConstraint *constrain)
//------------------------------------------------------------------------------
{
  *RotationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mafInteractorGeneric::SetScaleConstraint(mafInteractorConstraint *constrain)
//------------------------------------------------------------------------------
{
  *ScaleConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mafInteractorGeneric::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  RotationConstraint->GetRefSys()->SetRenderer(ren);
  TranslationConstraint->GetRefSys()->SetRenderer(ren);
  ScaleConstraint->GetRefSys()->SetRenderer(ren);
  PivotRefSys->SetRenderer(ren);
  
}

//------------------------------------------------------------------------------
void mafInteractorGeneric::SetVME(mflVME *vme)
//------------------------------------------------------------------------------
{
  Superclass::SetVME(vme);
  RotationConstraint->GetRefSys()->SetVME(vme);
  TranslationConstraint->GetRefSys()->SetVME(vme);
  ScaleConstraint->GetRefSys()->SetVME(vme);
  PivotRefSys->SetVME(vme);
}
