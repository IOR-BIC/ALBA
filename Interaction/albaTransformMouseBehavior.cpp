/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformMouseBehavior
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

#include "albaTransformMouseBehavior.h"
#include "albaDecl.h"
#include "albaEventBase.h"
#include "albaTransformBehavior.h"
#include "albaInteractorConstraint.h"
#include "albaRefSys.h"
#include "albaVME.h"
#include "albaVMEOutput.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaTransformMouseBehavior);

//------------------------------------------------------------------------------
albaTransformMouseBehavior::albaTransformMouseBehavior()
{
  m_Rotate = NULL;
  m_Translate = NULL;
  m_Roll = NULL;
  m_RefSysVME = NULL;
}

//------------------------------------------------------------------------------
void albaTransformMouseBehavior::CreateTransformBehaviors(albaVME *vme)
{
  // default ref sys is the vme ref sys
  m_RefSysVME = vme;
  albaMatrix *absMatrix = m_RefSysVME->GetOutput()->GetAbsMatrix();

  // rotate
  m_Rotate = CreateBehavior(MOUSE_LEFT);
  m_Rotate->SetListener(this);
  m_Rotate->SetVME(vme);
  m_Rotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
  m_Rotate->GetRotationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  m_Rotate->GetPivotRefSys()->SetTypeToView();
  m_Rotate->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetMatrix());
  m_Rotate->EnableRotation(true);

  // translate
  m_Translate = CreateBehavior(MOUSE_MIDDLE);
  m_Translate->SetListener(this);
  m_Translate->SetVME(vme);
  m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  // set the pivot point
  m_Translate->GetTranslationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  m_Translate->GetPivotRefSys()->SetTypeToCustom(absMatrix);
  m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  m_Translate->EnableTranslation(true);

  // roll
  m_Roll = CreateBehavior(MOUSE_LEFT_CONTROL);
  m_Roll->SetListener(this);
  m_Roll->SetVME(vme);
  m_Roll->GetRotationConstraint()->GetRefSys()->SetTypeToView();
  m_Roll->GetRotationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  m_Roll->GetPivotRefSys()->SetTypeToView();
  m_Roll->GetPivotRefSys()->SetMatrix(absMatrix);
  m_Roll->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
  m_Roll->EnableRotation(true);
}

//------------------------------------------------------------------------------
void albaTransformMouseBehavior::SetRefSysVME(albaVME *refSysVME)
{
  if (refSysVME == NULL || m_Rotate == NULL)
    return;

  m_RefSysVME = refSysVME;
  m_Translate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_Translate->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_Rotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_Rotate->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_Roll->GetRotationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_Roll->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
}

//------------------------------------------------------------------------------
void albaTransformMouseBehavior::SetRotationConstraint(int constraint)
{
  // set rotation constraint;
  if (constraint == X_AXIS)
  {
    m_Rotate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Rotate->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Rotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);
  }
  else if (constraint == Y_AXIS)
  {
    m_Rotate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Rotate->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Rotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  }
  else if (constraint == Z_AXIS)
  {
    m_Rotate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Rotate->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Rotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
  }
  else if (constraint == VIEW_PLANE)
  {
    m_Rotate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Rotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
				//Modified by Matteo 30-05-2006
				//m_Rotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
    //End Matteo
				m_Rotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
  }
			else if (constraint == NORMAL_VIEW_PLANE)
			{
				m_Rotate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    //m_Rotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
				//Modified by Matteo 30-05-2006
				m_Rotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
    //End Matteo
				m_Rotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
			}
}

//------------------------------------------------------------------------------
void albaTransformMouseBehavior::SetTranslationConstraint(int constraint)
{
  // set translation constraint;
  if (constraint == X_AXIS)
  {
    m_Translate->SurfaceSnapOff();
    m_Translate->SurfaceNormalOff();
    m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);   
  }
  else if (constraint == Y_AXIS)
  {
    m_Translate->SurfaceSnapOff();
    m_Translate->SurfaceNormalOff();
    m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);        
  }
  else if (constraint == Z_AXIS)
  {                                             
    m_Translate->SurfaceSnapOff();
    m_Translate->SurfaceNormalOff();
    m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);        
  }      
  else if (constraint == VIEW_PLANE)
  {
    m_Translate->SurfaceSnapOff();
    m_Translate->SurfaceNormalOff();
    m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
				//Modified by Matteo 30-05-2006
				//m_Translate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
				//m_Translate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
				//m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
				//End Matteo
			}
			else if (constraint == NORMAL_VIEW_PLANE)
  {
    m_Translate->SurfaceSnapOff();
    m_Translate->SurfaceNormalOff();
    m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
				//Modified by Matteo 30-05-2006
				m_Translate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
				m_Translate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
				m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
				//End Matteo
			}

  if (constraint == XY_PLANE)
  {
    m_Translate->SurfaceSnapOff();
    m_Translate->SurfaceNormalOff();
    m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  }
  else if (constraint == XZ_PLANE)  
  {
    m_Translate->SurfaceSnapOff();
    m_Translate->SurfaceNormalOff();
    m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);       
  }
  else if (constraint == YZ_PLANE)
  {               
    m_Translate->SurfaceSnapOff();
    m_Translate->SurfaceNormalOff();
    m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE, albaInteractorConstraint::FREE);   
  }
  // isa gen is sending matrix to the operation
  else if (constraint == SURFACE_SNAP)
  {
    m_Translate->SurfaceSnapOn();
    m_Translate->SurfaceNormalOff();
    m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
				//Modified by Matteo 30-05-2006
				m_Translate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
				m_Translate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
				m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
				//End Matteo
  }
  else if (constraint == NORMAL_SURFACE)
  {
    m_Translate->SurfaceSnapOff();
    m_Translate->SurfaceNormalOn();
   
    m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
    //Modified by Matteo 30-05-2006
    m_Translate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetPivotRefSys()->SetTypeToCustom(m_RefSysVME->GetOutput()->GetAbsMatrix());
    m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
    //End Matteo
  }
}

//------------------------------------------------------------------------------
void albaTransformMouseBehavior::OnEvent(albaEventBase *event)
{
  // the events of the transform behaviors go to the listener
  if (event->GetChannel() != MCH_INPUT)
  {
    if (event->GetId() == ID_TRANSFORM)
      OnTransform();
    albaEventMacro(*event);
    return;
  }

  Superclass::OnEvent(event);
}
