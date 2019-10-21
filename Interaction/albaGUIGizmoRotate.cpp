/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIGizmoRotate
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


#include "albaGUIGizmoRotate.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGUIButton.h"

#include "albaGizmoInterface.h"
#include "albaGizmoTranslate.h"

#include "albaTransform.h"
#include "albaMatrix.h"

//----------------------------------------------------------------------------
albaGUIGizmoRotate::albaGUIGizmoRotate(albaObserver *listener, bool testMode)
//----------------------------------------------------------------------------
{
  m_TestMode = testMode;
  m_Listener = listener;

  m_Orientation[0] = m_Orientation[1] = m_Orientation[2] = 0;

  if (m_TestMode == false)
  {
    CreateGui();
  }
}

//----------------------------------------------------------------------------
albaGUIGizmoRotate::~albaGUIGizmoRotate() 
//----------------------------------------------------------------------------
{
  
}

//----------------------------------------------------------------------------
void albaGUIGizmoRotate::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
  
  m_Gui->Divider(2);
  m_Gui->Label("Rotation gizmo abs orientation", true);
  m_Gui->Double(ID_ROTATE_X, "Rotate X", &m_Orientation[0]);
  m_Gui->Double(ID_ROTATE_Y, "Rotate Y", &m_Orientation[1]);
  m_Gui->Double(ID_ROTATE_Z, "Rotate Z", &m_Orientation[2]);
	m_Gui->Divider();
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaGUIGizmoRotate::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_ROTATE_X:
      case ID_ROTATE_Y:
      case ID_ROTATE_Z:
      {
        SendAbsOrientation(e);
      }
      break;
      default:
        albaEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaGUIGizmoRotate::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_ROTATE_X, enable);
  m_Gui->Enable(ID_ROTATE_Y, enable);
  m_Gui->Enable(ID_ROTATE_Z, enable);
}

//----------------------------------------------------------------------------
void albaGUIGizmoRotate::SendAbsOrientation(albaEventBase *sourceEvent)
//----------------------------------------------------------------------------
{
  // build abs matrix position
  albaMatrix m2send;
  albaTransform::SetOrientation(m2send, m_Orientation[0], m_Orientation[1], m_Orientation[2]);

  albaEvent event2Send;
  event2Send.SetSender(this);
  event2Send.SetId(sourceEvent->GetId());
  event2Send.SetMatrix(&m2send);
  
  albaEventMacro(event2Send);
}

//----------------------------------------------------------------------------
void albaGUIGizmoRotate::SetAbsOrientation(albaMatrix *pose)
//----------------------------------------------------------------------------
{
  albaTransform::GetOrientation(*pose, m_Orientation);
  
  if (m_TestMode == false)
  {
    assert(m_Gui);
    m_Gui->Update();
  }
}
