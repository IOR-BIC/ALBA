/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaBehaviorRotateTranslate
 Authors: Gianluigi Crimi, from albaGUITransformMouse by Stefano Perticoni

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaBehaviorRotateTranslate.h"
#include "albaDecl.h"
#include "albaTransformConstraint.h"
#include "albaRefSys.h"
#include "albaVME.h"
#include "albaVMEOutput.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaBehaviorRotateTranslate);

//------------------------------------------------------------------------------
albaBehaviorRotateTranslate::albaBehaviorRotateTranslate()
{
  m_RollEnabled = true;
  m_RefSysVME = NULL;
  m_RollConstraint = new albaTransformConstraint;
  m_RollConstraint->GetRefSys()->SetTypeToView();
  m_RollConstraint->SetConstraintModality(albaTransformConstraint::LOCK, albaTransformConstraint::LOCK, albaTransformConstraint::FREE);
}

//------------------------------------------------------------------------------
albaBehaviorRotateTranslate::~albaBehaviorRotateTranslate()
{
  delete m_RollConstraint;
}

//------------------------------------------------------------------------------
void albaBehaviorRotateTranslate::SetVME(albaVME *vme)
{
  Superclass::SetVME(vme);
  m_RollConstraint->GetRefSys()->SetVME(vme);
  if (vme == NULL)
    return;

  // default: rotation and translation in the view plane, pivot and ref sys of the constraints from the VME
  m_RefSysVME = vme;
  albaMatrix *absMatrix = m_RefSysVME->GetOutput()->GetAbsMatrix();
  GetRotationConstraint()->GetRefSys()->SetTypeToView();
  GetRotationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  GetRotationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
  GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  GetTranslationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
  m_RollConstraint->GetRefSys()->SetMatrix(absMatrix);
  GetPivotRefSys()->SetTypeToCustom(absMatrix);
}

//------------------------------------------------------------------------------
void albaBehaviorRotateTranslate::SetRendererAndView(vtkRenderer *ren, albaView *view)
{
  m_RollConstraint->GetRefSys()->SetRenderer(ren);
  Superclass::SetRendererAndView(ren, view);
}

//------------------------------------------------------------------------------
bool albaBehaviorRotateTranslate::PrepareTransform(int button)
{
  if (button == ALBA_LEFT_BUTTON)
  {
    if (m_ShiftDown || (m_CtrlDown && !m_RollEnabled))
      return false;
    if (m_CtrlDown)
      m_CurrentRotationConstraint = m_RollConstraint;
    EnableRotation(true);
    return true;
  }

  if (button == ALBA_MIDDLE_BUTTON)
  {
    if (m_ShiftDown || m_CtrlDown)
      return false;
    EnableTranslation(true);
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
void albaBehaviorRotateTranslate::SetRefSysVME(albaVME *refSysVME)
{
  if (refSysVME == NULL)
    return;

  m_RefSysVME = refSysVME;
  albaMatrix *absMatrix = m_RefSysVME->GetOutput()->GetAbsMatrix();
  GetTranslationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  GetRotationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  m_RollConstraint->GetRefSys()->SetMatrix(absMatrix);
  GetPivotRefSys()->SetMatrix(absMatrix);
}

//------------------------------------------------------------------------------
void albaBehaviorRotateTranslate::SetRotationConstraint(int constraint)
{
  if (m_RefSysVME == NULL)
    return;

  // set rotation constraint;
  if (constraint == X_AXIS)
  {
    GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetRotationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::LOCK, albaTransformConstraint::LOCK);
  }
  else if (constraint == Y_AXIS)
  {
    GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetRotationConstraint()->SetConstraintModality(albaTransformConstraint::LOCK, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
  }
  else if (constraint == Z_AXIS)
  {
    GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetRotationConstraint()->SetConstraintModality(albaTransformConstraint::LOCK, albaTransformConstraint::LOCK, albaTransformConstraint::FREE);
  }
  else if (constraint == VIEW_PLANE)
  {
    GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetRotationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
				//Modified by Matteo 30-05-2006
				//GetRotationConstraint()->SetConstraintModality(albaTransformConstraint::LOCK, albaTransformConstraint::LOCK, albaTransformConstraint::FREE);
    //End Matteo
				GetRotationConstraint()->GetRefSys()->SetTypeToView();
  }
			else if (constraint == NORMAL_VIEW_PLANE)
			{
				GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    //GetRotationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
				//Modified by Matteo 30-05-2006
				GetRotationConstraint()->SetConstraintModality(albaTransformConstraint::LOCK, albaTransformConstraint::LOCK, albaTransformConstraint::FREE);
    //End Matteo
				GetRotationConstraint()->GetRefSys()->SetTypeToView();
			}
}

//------------------------------------------------------------------------------
void albaBehaviorRotateTranslate::SetTranslationConstraint(int constraint)
{
  if (m_RefSysVME == NULL)
    return;

  // set translation constraint;
  if (constraint == X_AXIS)
  {
    SurfaceSnapOff();
    SurfaceNormalOff();
    GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::LOCK, albaTransformConstraint::LOCK);   
  }
  else if (constraint == Y_AXIS)
  {
    SurfaceSnapOff();
    SurfaceNormalOff();
    GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::LOCK, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);        
  }
  else if (constraint == Z_AXIS)
  {                                             
    SurfaceSnapOff();
    SurfaceNormalOff();
    GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::LOCK, albaTransformConstraint::LOCK, albaTransformConstraint::FREE);        
  }      
  else if (constraint == VIEW_PLANE)
  {
    SurfaceSnapOff();
    SurfaceNormalOff();
    GetTranslationConstraint()->GetRefSys()->SetTypeToView();
				//Modified by Matteo 30-05-2006
				//GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
				//GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
				//End Matteo
			}
			else if (constraint == NORMAL_VIEW_PLANE)
  {
    SurfaceSnapOff();
    SurfaceNormalOff();
    GetTranslationConstraint()->GetRefSys()->SetTypeToView();
				//Modified by Matteo 30-05-2006
				GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
				GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
				//End Matteo
			}

  if (constraint == XY_PLANE)
  {
    SurfaceSnapOff();
    SurfaceNormalOff();
    GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
  }
  else if (constraint == XZ_PLANE)  
  {
    SurfaceSnapOff();
    SurfaceNormalOff();
    GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::LOCK, albaTransformConstraint::FREE);       
  }
  else if (constraint == YZ_PLANE)
  {               
    SurfaceSnapOff();
    SurfaceNormalOff();
    GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::LOCK, albaTransformConstraint::FREE, albaTransformConstraint::FREE);   
  }
  // the behavior is sending matrix to the operation
  else if (constraint == SURFACE_SNAP)
  {
    SurfaceSnapOn();
    SurfaceNormalOff();
    GetTranslationConstraint()->GetRefSys()->SetTypeToView();
				//Modified by Matteo 30-05-2006
				GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
				GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
				//End Matteo
  }
  else if (constraint == NORMAL_SURFACE)
  {
    SurfaceSnapOff();
    SurfaceNormalOn();
   
    GetTranslationConstraint()->GetRefSys()->SetTypeToView();
    //Modified by Matteo 30-05-2006
    GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
    GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
    //End Matteo
  }
}
