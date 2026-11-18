/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformMouseFreeBehavior
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

#include "albaTransformMouseFreeBehavior.h"
#include "albaGenericMouseFreeTransformBehavior.h"
#include "albaInteractorConstraint.h"
#include "albaRefSys.h"
#include "albaViewCompound.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaTransformMouseFreeBehavior);

//------------------------------------------------------------------------------
albaGenericMouseBehavior *albaTransformMouseFreeBehavior::NewGenericMouse()
{
  return albaGenericMouseFreeTransformBehavior::New();
}

//------------------------------------------------------------------------------
void albaTransformMouseFreeBehavior::OnTransform()
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
bool albaTransformMouseFreeBehavior::IsSliceView(albaView *v)
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
