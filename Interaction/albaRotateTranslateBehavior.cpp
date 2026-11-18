/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRotateTranslateBehavior
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

#include "albaRotateTranslateBehavior.h"
#include "albaDecl.h"
#include "albaEventBase.h"
#include "albaTransformBehavior.h"
#include "albaInteractorConstraint.h"
#include "albaRefSys.h"
#include "albaVME.h"
#include "albaVMEOutput.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaRotateTranslateBehavior);

//------------------------------------------------------------------------------
albaRotateTranslateBehavior::albaRotateTranslateBehavior()
{
  m_Rotate = NULL;
  m_Translate = NULL;
  m_Roll = NULL;
  m_Active = NULL;
  m_RollEnabled = true;
  m_RefSysVME = NULL;
}

//------------------------------------------------------------------------------
albaRotateTranslateBehavior::~albaRotateTranslateBehavior()
{
  albaDEL(m_Rotate);
  albaDEL(m_Translate);
  albaDEL(m_Roll);
}

//------------------------------------------------------------------------------
albaTransformBehavior *albaRotateTranslateBehavior::NewTransformBehavior()
{
  return albaTransformBehavior::New();
}

//------------------------------------------------------------------------------
albaTransformBehavior *albaRotateTranslateBehavior::GetLeftButtonTransform()
{
  if (m_ShiftDown || (m_CtrlDown && !m_RollEnabled))
    return NULL;
  return m_CtrlDown ? m_Roll : m_Rotate;
}

//------------------------------------------------------------------------------
bool albaRotateTranslateBehavior::OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  if (SendModifierClick())
    return true;

  return Start(GetLeftButtonTransform(), view, ren, pos);
}

//------------------------------------------------------------------------------
bool albaRotateTranslateBehavior::OnMiddleButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  return Start((m_CtrlDown || m_ShiftDown) ? NULL : m_Translate, view, ren, pos);
}

//------------------------------------------------------------------------------
bool albaRotateTranslateBehavior::Start(albaTransformBehavior *transform, albaView *view, vtkRenderer *ren, double pos[2])
{
  Stop(view, ren, pos);
  if (transform == NULL)
    return false;

  // the transform behavior moves the VME as if it were picked with the left button
  transform->SetProp(m_Prop);
  transform->SetPickedPosition(m_PickedPosition);
  if (!transform->OnLeftButtonDown(view, ren, pos))
    return false;

  m_Active = transform;
  return true;
}

//------------------------------------------------------------------------------
bool albaRotateTranslateBehavior::Stop(albaView *view, vtkRenderer *ren, double pos[2])
{
  if (m_Active == NULL)
    return false;

  albaTransformBehavior *active = m_Active;
  m_Active = NULL;
  active->OnLeftButtonUp(view, ren, pos);
  return true;
}

//------------------------------------------------------------------------------
bool albaRotateTranslateBehavior::OnDrag(albaView *view, vtkRenderer *ren, double pos[2])
{
  return m_Active && m_Active->OnDrag(view, ren, pos);
}

//------------------------------------------------------------------------------
void albaRotateTranslateBehavior::CreateTransformBehaviors(albaVME *vme)
{
  // default ref sys is the vme ref sys
  m_RefSysVME = vme;
  albaMatrix *absMatrix = m_RefSysVME->GetOutput()->GetAbsMatrix();

  // rotate
  m_Rotate = NewTransformBehavior();
  m_Rotate->SetListener(this);
  m_Rotate->SetVME(vme);
  m_Rotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
  m_Rotate->GetRotationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  m_Rotate->GetPivotRefSys()->SetTypeToView();
  m_Rotate->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetMatrix());
  m_Rotate->EnableRotation(true);

  // translate
  m_Translate = NewTransformBehavior();
  m_Translate->SetStartButton(ALBA_MIDDLE_BUTTON);
  m_Translate->SetListener(this);
  m_Translate->SetVME(vme);
  m_Translate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  // set the pivot point
  m_Translate->GetTranslationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  m_Translate->GetPivotRefSys()->SetTypeToCustom(absMatrix);
  m_Translate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  m_Translate->EnableTranslation(true);

  // roll
  m_Roll = NewTransformBehavior();
  m_Roll->SetModifiers(ALBA_CTRL_KEY);
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
void albaRotateTranslateBehavior::SetRefSysVME(albaVME *refSysVME)
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
void albaRotateTranslateBehavior::SetRotationConstraint(int constraint)
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
void albaRotateTranslateBehavior::SetTranslationConstraint(int constraint)
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
void albaRotateTranslateBehavior::OnEvent(albaEventBase *event)
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
