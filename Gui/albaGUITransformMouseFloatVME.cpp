/*=========================================================================
Program:   AssemblerPro
Module:    appGUITransformMouse.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"
#include "albaGUITransformMouseFloatVME.h"
#include "albaInteractorGenericMouseFloatVME.h"
#include "albaInteractorCompositorMouseFloatVME.h"

#include "albaViewCompound.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaGUITransformMouse.h"
#include "albaInteractorConstraint.h"
#include "albaRefSys.h"

//----------------------------------------------------------------------------
albaGUITransformMouseFloatVME::albaGUITransformMouseFloatVME(albaVME *input, albaObserver *listener): albaGUITransformMouse(input, listener)
{
	UpdateISA();
}
//----------------------------------------------------------------------------
albaGUITransformMouseFloatVME::~albaGUITransformMouseFloatVME() 
{ 
}

//----------------------------------------------------------------------------
void albaGUITransformMouseFloatVME::UpdateISA()
{
	//Delete the compositor created in the base class
	DetachInteractorFromVme();
	albaDEL(m_IsaCompositor);
		
	// Create the isa compositor:
	m_IsaCompositor = albaInteractorCompositorMouseFloatVME::New();

	albaMatrix *absMatrix;
	absMatrix = m_RefSysVME->GetOutput()->GetAbsMatrix();
	//----------------------------------------------------------------------------
	// create the rotate behavior
	//----------------------------------------------------------------------------

	m_IsaRotate = m_IsaCompositor->CreateBehavior(MOUSE_LEFT);

	m_IsaRotate->SetListener(this);
	m_IsaRotate->SetVME(m_InputVME);
	m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
	m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(absMatrix);

	m_IsaRotate->GetPivotRefSys()->SetTypeToView();
	m_IsaRotate->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetMatrix());

	m_IsaRotate->EnableRotation(true);

	//----------------------------------------------------------------------------
	// create the translate behavior
	//----------------------------------------------------------------------------

	m_IsaTranslate = m_IsaCompositor->CreateBehavior(MOUSE_MIDDLE);
	
	m_IsaTranslate->SetListener(this);
	m_IsaTranslate->SetVME(m_InputVME);
	m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
	// set the pivot point
	m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(absMatrix);
	m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(absMatrix);

	m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
	m_IsaTranslate->EnableTranslation(true);

	//----------------------------------------------------------------------------
	// create the roll behavior
	//----------------------------------------------------------------------------

	m_IsaRoll = m_IsaCompositor->CreateBehavior(MOUSE_LEFT_CONTROL);

	// isa gen is sending matrix to the operation
	m_IsaRoll->SetListener(this);
	m_IsaRoll->SetVME(m_InputVME);
	m_IsaRoll->GetRotationConstraint()->GetRefSys()->SetTypeToView();
	m_IsaRoll->GetRotationConstraint()->GetRefSys()->SetMatrix(absMatrix);

	m_IsaRoll->GetPivotRefSys()->SetTypeToView();
	m_IsaRoll->GetPivotRefSys()->SetMatrix(absMatrix);

	m_IsaRoll->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
	m_IsaRoll->EnableRotation(true);

	AttachInteractorToVme();
}

//----------------------------------------------------------------------------
bool albaGUITransformMouseFloatVME::IsSliceView(albaView *v)
{
  if (albaViewCompound::SafeDownCast(v))
	{
		albaView *childView = albaViewCompound::SafeDownCast(v)->GetSubView();
		return IsSliceView(childView);
	}
	else if( v->IsA("albaViewRX") || v->IsA("appViewSliceBlend"))// || v->IsA("appViewSlice") )
  {
		return true;
  }
  
  return false;
}

//----------------------------------------------------------------------------
void albaGUITransformMouseFloatVME::OnEvent(albaEventBase *alba_event)
{
  switch(alba_event->GetId())
  {
  case ID_ROTATION_AXES:
    {
      albaGUITransformMouse::OnEvent(alba_event);
    }
    break;
  case ID_TRASLATION_AXES:
    {
      albaGUITransformMouse::OnEvent(alba_event);
    }
    break;

  case ID_TRANSFORM:
    {    
      albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast(m_IsaRotate->GetDevice());
			
			if (mouse)
			{
				albaView *v = mouse->GetView();
				if (v)
				{
					if (IsSliceView(v))
					{
						m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
						m_IsaRotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
					}
					else
					{
							m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
							m_IsaRotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
					}
				}
			}

      // forward transform events to listener operation; the operation will move the vme
      alba_event->SetSender(this);
      albaEventMacro(*alba_event);
    }
    break;

  default:
    {
      albaEventMacro(*alba_event);
    }
    break;
  }
}

//----------------------------------------------------------------------------
albaInteractorGenericMouseFloatVME* albaGUITransformMouseFloatVME::CreateBehavior(MMI_ACTIVATOR activator)
{
	return (albaInteractorGenericMouseFloatVME*)m_IsaCompositor->CreateBehavior(activator);
}