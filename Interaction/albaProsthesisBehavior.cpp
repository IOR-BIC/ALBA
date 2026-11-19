/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaProsthesisBehavior
 Authors: Gianluigi Crimi, from albaGUITransformMouseFloatVME by Nicola Vanella

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

#include "albaProsthesisBehavior.h"
#include "albaProsthesisTransformBehavior.h"
#include "albaInteractorConstraint.h"
#include "albaRefSys.h"
#include "albaViewCompound.h"
#include "albaDecl.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaProsthesisBehavior);

//------------------------------------------------------------------------------
albaProsthesisBehavior::albaProsthesisBehavior()
{
  m_ShiftTransform = NULL;
}

//------------------------------------------------------------------------------
albaProsthesisBehavior::~albaProsthesisBehavior()
{
  albaDEL(m_ShiftTransform);
}

//------------------------------------------------------------------------------
void albaProsthesisBehavior::CreateTransformBehaviors(albaVME *vme)
{
  Superclass::CreateTransformBehaviors(vme);

  m_ShiftTransform = NewTransformBehavior();
  m_ShiftTransform->SetListener(this);
  m_ShiftTransform->SetVME(vme);
}

//------------------------------------------------------------------------------
albaTransformBehavior *albaProsthesisBehavior::GetLeftButtonTransform()
{
  return m_ShiftDown ? m_ShiftTransform : Superclass::GetLeftButtonTransform();
}

//------------------------------------------------------------------------------
albaTransformBehavior *albaProsthesisBehavior::NewTransformBehavior()
{
  return albaProsthesisTransformBehavior::New();
}

//------------------------------------------------------------------------------
void albaProsthesisBehavior::OnTransform()
{
  albaView *v = m_Rotate->GetView();
  if (v == NULL)
    return;

  m_Rotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
  if (IsSliceView(v))
    m_Rotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
  else
    m_Rotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
}

//------------------------------------------------------------------------------
bool albaProsthesisBehavior::IsSliceView(albaView *v)
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
