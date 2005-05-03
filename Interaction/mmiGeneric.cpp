/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiGeneric.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:37 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#include "mafDevice.h"

#include "mmiGeneric.h"
#include "vtkObjectFactory.h"

#include "mmiConstraint.h"
#include "mflEvent.h"
#include "mflVME.h"
#include "vtkMatrix4x4.h"
#include "mflTransform.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MFL_EVT_IMP(mmiGeneric::MoveActionEvent);

//------------------------------------------------------------------------------
vtkStandardNewMacro(mmiGeneric)
vtkCxxSetObjectMacro(mmiGeneric,ResultTransform,mflTransform);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmiGeneric::mmiGeneric()
//------------------------------------------------------------------------------
{
  TranslationFlag       = true;
  RotationFlag          = true;
  ScalingFlag           = true;
  UniformScalingFlag    = true;
  ResultTransform       = NULL;
  TargetRefSys          = new mafRefSys;
  TranslationConstraint = new mmiConstraint;
  RotationConstraint    = new mmiConstraint;
  ScaleConstraint       = new mmiConstraint;
  PivotRefSys           = new mafRefSys;
  RotationConstraint->GetRefSys()->SetTypeToCamera(); // the Renderer is set later!!!
  TranslationConstraint->GetRefSys()->SetTypeToCamera(); // the Renderer is set later!!!
  PivotRefSys->SetTypeToLocal(); // the VME is set later !!!
}

//------------------------------------------------------------------------------
mmiGeneric::~mmiGeneric()
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
void mmiGeneric::SetResultMatrix(vtkMatrix4x4 *result)
//------------------------------------------------------------------------------
{
  mflSmartPointer<mflTransform> trans; // create a transform on the fly
  trans->SetMatrix(result,true); // make it reference the result matrix
  SetResultTransform(trans);
}

//------------------------------------------------------------------------------
vtkMatrix4x4 *mmiGeneric::GetResultMatrix()
//------------------------------------------------------------------------------
{ 
  return (ResultTransform?ResultTransform->GetMatrix():NULL);
}

//------------------------------------------------------------------------------
void mmiGeneric::SetTargetRefSys(mafRefSys &ref_sys)
//------------------------------------------------------------------------------
{
  *TargetRefSys=ref_sys;
}
//------------------------------------------------------------------------------
void mmiGeneric::SetTargetRefSys(mafRefSys *ref_sys)
//------------------------------------------------------------------------------
{
  TargetRefSys->DeepCopy(ref_sys);
}
//------------------------------------------------------------------------------
void mmiGeneric::SetPivotRefSys(mafRefSys *pivot_frame )
//------------------------------------------------------------------------------
{
  PivotRefSys->DeepCopy(pivot_frame);
}
//------------------------------------------------------------------------------
void mmiGeneric::SetPivotRefSys(mafRefSys &pivot_frame )
//------------------------------------------------------------------------------
{
  *PivotRefSys=pivot_frame;
}

//------------------------------------------------------------------------------
void mmiGeneric::SetTranslationConstraint(mmiConstraint *constrain)
//------------------------------------------------------------------------------
{
  *TranslationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mmiGeneric::SetRotationConstraint(mmiConstraint *constrain)
//------------------------------------------------------------------------------
{
  *RotationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mmiGeneric::SetScaleConstraint(mmiConstraint *constrain)
//------------------------------------------------------------------------------
{
  *ScaleConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mmiGeneric::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  RotationConstraint->GetRefSys()->SetRenderer(ren);
  TranslationConstraint->GetRefSys()->SetRenderer(ren);
  ScaleConstraint->GetRefSys()->SetRenderer(ren);
  PivotRefSys->SetRenderer(ren);
  
}

//------------------------------------------------------------------------------
void mmiGeneric::SetVME(mflVME *vme)
//------------------------------------------------------------------------------
{
  Superclass::SetVME(vme);
  RotationConstraint->GetRefSys()->SetVME(vme);
  TranslationConstraint->GetRefSys()->SetVME(vme);
  ScaleConstraint->GetRefSys()->SetVME(vme);
  PivotRefSys->SetVME(vme);
}