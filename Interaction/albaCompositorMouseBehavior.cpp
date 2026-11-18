/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCompositorMouseBehavior
 Authors: Stefano Perticoni
 
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

#include "albaCompositorMouseBehavior.h"

#include "albaDecl.h"
#include "albaView.h"
#include "albaVME.h"

#include "vtkDoubleArray.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkObjectFactory.h"
#include "vtkCommand.h"

using namespace std;

albaCxxTypeMacro(albaCompositorMouseBehavior);

//----------------------------------------------------------------------------
albaCompositorMouseBehavior::albaCompositorMouseBehavior() 
//----------------------------------------------------------------------------
{
  m_StartButton = -1;

  m_ActiveTransform = NULL;  
}
//----------------------------------------------------------------------------
albaCompositorMouseBehavior::~albaCompositorMouseBehavior() 
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
albaTransformBehavior *albaCompositorMouseBehavior::CreateBehavior(MMI_ACTIVATOR activator)
//----------------------------------------------------------------------------
{
  // create the generic behavior and populate the map
  albaTransformBehavior *pISAG = NewTransformBehavior();
  
  m_ActivatorMap[activator] = pISAG;
  switch (activator)
  {    
    case MOUSE_LEFT:
      pISAG->SetStartButton(ALBA_LEFT_BUTTON);
      break;
    case MOUSE_LEFT_SHIFT:
      pISAG->SetStartButton(ALBA_LEFT_BUTTON);
      pISAG->SetModifiers(ALBA_SHIFT_KEY);
      break;
    case MOUSE_LEFT_CONTROL:
      pISAG->SetStartButton(ALBA_LEFT_BUTTON);
      pISAG->SetModifiers(ALBA_CTRL_KEY);
      break;
    case MOUSE_MIDDLE:
      pISAG->SetStartButton(ALBA_MIDDLE_BUTTON);
      break;
    case MOUSE_MIDDLE_SHIFT:
      pISAG->SetStartButton(ALBA_MIDDLE_BUTTON);
      pISAG->SetModifiers(ALBA_SHIFT_KEY);
      break;
    case MOUSE_MIDDLE_CONTROL:
      pISAG->SetStartButton(ALBA_MIDDLE_BUTTON);
      pISAG->SetModifiers(ALBA_CTRL_KEY);
      break;
    case MOUSE_RIGHT:
      pISAG->SetStartButton(ALBA_RIGHT_BUTTON);
      break;
    case MOUSE_RIGHT_SHIFT:
      pISAG->SetStartButton(ALBA_RIGHT_BUTTON);
      pISAG->SetModifiers(ALBA_SHIFT_KEY);
      break;
    case MOUSE_RIGHT_CONTROL:
      pISAG->SetStartButton(ALBA_RIGHT_BUTTON);
      pISAG->SetModifiers(ALBA_CTRL_KEY);
      break;
  }
  
  return pISAG;
}

//----------------------------------------------------------------------------
albaTransformBehavior *albaCompositorMouseBehavior::NewTransformBehavior()
{
  return albaTransformBehavior::New();
}

//----------------------------------------------------------------------------
albaTransformBehavior *albaCompositorMouseBehavior::GetBehavior(MMI_ACTIVATOR activator)
//----------------------------------------------------------------------------
{
  // return pointer to the behavior given the map key
  return m_ActivatorMap[activator];
}

//----------------------------------------------------------------------------
int albaCompositorMouseBehavior::MouseToActivator(int mouseButton, int shift, int ctrl)
//----------------------------------------------------------------------------
{
  if (mouseButton == MOUSE_LEFT_PRESSED)
  {
    if (shift == 1)
      return MOUSE_LEFT_SHIFT;
    else if (ctrl == 1)
      return MOUSE_LEFT_CONTROL;
    else
      return MOUSE_LEFT;
  }
  else if (mouseButton == MOUSE_MIDDLE_PRESSED)
  {
    if (shift == 1)
      return MOUSE_MIDDLE_SHIFT;
    else if (ctrl == 1)
      return MOUSE_MIDDLE_CONTROL;
    else
      return MOUSE_MIDDLE;
  }
  else if (mouseButton == MOUSE_RIGHT_PRESSED)
  {
    if (shift == 1)
      return MOUSE_RIGHT_SHIFT;
    else if (ctrl == 1)
      return MOUSE_RIGHT_CONTROL;
    else
      return MOUSE_RIGHT;
  }
  else
  {
    albaErrorMacro("Unknown mouse keyboard combination")
    return -1;
  }
}
   
//----------------------------------------------------------------------------
bool albaCompositorMouseBehavior::StartTransform(int buttonPressed, albaView *view, vtkRenderer *ren, double pos[2])
{
  StopTransform();

  int activator = MouseToActivator(buttonPressed, m_ShiftDown, m_CtrlDown);
  if (m_ActivatorMap.count(activator) == 0 || m_ActivatorMap[activator] == NULL)
    return false;

  // the transform behavior of the button starts as if it were picked with the left button
  albaTransformBehavior *transform = m_ActivatorMap[activator];
  transform->SetProp(m_Prop);
  transform->SetPickedPosition(m_PickedPosition);
  transform->SetModifierKeys(m_CtrlDown, m_AltDown, m_ShiftDown);
  if (!transform->OnLeftButtonDown(view, ren, pos))
    return false;

  m_ActiveTransform = transform;
  return true;
}

//----------------------------------------------------------------------------
bool albaCompositorMouseBehavior::StopTransform()
{
  if (m_ActiveTransform == NULL)
    return false;

  albaTransformBehavior *active = m_ActiveTransform;
  m_ActiveTransform = NULL;
  double pos[2] = {0, 0};
  active->OnLeftButtonUp(NULL, NULL, pos);
  return true;
}

//----------------------------------------------------------------------------
bool albaCompositorMouseBehavior::OnDrag(albaView *view, vtkRenderer *ren, double pos[2])
{
  return m_ActiveTransform && m_ActiveTransform->OnDrag(view, ren, pos);
}
