/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiGenericInterface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:37 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#include "mafDevice.h"

#include "mmiGenericInterface.h"
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
MFL_EVT_IMP(mmiGenericInterface::MoveActionEvent);

//------------------------------------------------------------------------------
vtkStandardNewMacro(mmiGenericInterface)
vtkCxxSetObjectMacro(mmiGenericInterface,ResultTransform,mflTransform);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmiGenericInterface::mmiGenericInterface()
//------------------------------------------------------------------------------
{
  TranslationFlag       = true;
  RotationFlag          = true;
  ScalingFlag           = true;
  UniformScalingFlag    = true;
  SurfaceSnap           = false;
  ResultTransform       = NULL;
  TargetRefSys          = new mafRefSys;
  TranslationConstraint = new mmiConstraint;
  RotationConstraint    = new mmiConstraint;
  ScaleConstraint       = new mmiConstraint;
  PivotRefSys           = new mafRefSys;
  RotationConstraint->GetRefSys()->SetTypeToView(); // the Renderer is set later!!!
  TranslationConstraint->GetRefSys()->SetTypeToView(); // the Renderer is set later!!!
  PivotRefSys->SetTypeToLocal(); // the VME is set later !!!
}

//------------------------------------------------------------------------------
mmiGenericInterface::~mmiGenericInterface()
//------------------------------------------------------------------------------
{
  vtkDEL(ResultTransform);
  delete TargetRefSys; TargetRefSys = NULL;
  delete PivotRefSys; PivotRefSys = NULL;
  delete TranslationConstraint; TranslationConstraint = NULL;
  delete RotationConstraint; RotationConstraint = NULL;
  delete ScaleConstraint; ScaleConstraint = NULL;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetResultMatrix(vtkMatrix4x4 *result)
//------------------------------------------------------------------------------
{
  mflSmartPointer<mflTransform> trans; // create a transform on the fly
  trans->SetMatrix(result,false); // make it reference the result matrix
  SetResultTransform(trans);
}

//------------------------------------------------------------------------------
vtkMatrix4x4 *mmiGenericInterface::GetResultMatrix()
//------------------------------------------------------------------------------
{ 
  return (ResultTransform?ResultTransform->GetMatrix():NULL);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetTargetRefSys(mafRefSys &ref_sys)
//------------------------------------------------------------------------------
{
  *TargetRefSys=ref_sys;
}
//------------------------------------------------------------------------------
void mmiGenericInterface::SetTargetRefSys(mafRefSys *ref_sys)
//------------------------------------------------------------------------------
{
  TargetRefSys->DeepCopy(ref_sys);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetPivotRefSys(mafRefSys *pivot_frame )
//------------------------------------------------------------------------------
{
  PivotRefSys->DeepCopy(pivot_frame);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetPivotRefSys(mafRefSys &pivot_frame )
//------------------------------------------------------------------------------
{
  *PivotRefSys=pivot_frame;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  TranslationConstraint->GetRefSys()->SetRenderer(ren);  
  RotationConstraint->GetRefSys()->SetRenderer(ren);  
  ScaleConstraint->GetRefSys()->SetRenderer(ren);  
  PivotRefSys->SetRenderer(ren);
  Superclass::SetRenderer(ren);
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetTranslationConstraint(mmiConstraint *constrain)
//------------------------------------------------------------------------------
{
  *TranslationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetRotationConstraint(mmiConstraint *constrain)
//------------------------------------------------------------------------------
{
  *RotationConstraint=*constrain;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::SetScaleConstraint(mmiConstraint *constrain)
//------------------------------------------------------------------------------
{
  *ScaleConstraint=*constrain;
}
 
//------------------------------------------------------------------------------
void mmiGenericInterface::SetVME(mflVME *vme)
//------------------------------------------------------------------------------
{
  Superclass::SetVME(vme);
  RotationConstraint->GetRefSys()->SetVME(vme);
  TranslationConstraint->GetRefSys()->SetVME(vme);
  ScaleConstraint->GetRefSys()->SetVME(vme);
  PivotRefSys->SetVME(vme);
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
  TranslationFlag = enable;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::EnableRotationInternal(bool enable)
//------------------------------------------------------------------------------
{
  RotationFlag = enable;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::EnableScalingInternal(bool enable)
//------------------------------------------------------------------------------
{
  ScalingFlag = enable;
}

//------------------------------------------------------------------------------
void mmiGenericInterface::EnableUniformScalingInternal(bool enable)
//------------------------------------------------------------------------------
{
   UniformScalingFlag = enable;
}