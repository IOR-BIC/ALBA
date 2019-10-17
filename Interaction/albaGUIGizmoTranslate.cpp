/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIGizmoTranslate
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


#include "albaGUIGizmoTranslate.h"
#include "albaDecl.h"
#include "albaGUIButton.h"
#include "albaGUI.h"

#include "albaGizmoInterface.h"
#include "albaGizmoTranslate.h"

#include "albaTransform.h"

#include "albaMatrix.h"

//----------------------------------------------------------------------------
albaGUIGizmoTranslate::albaGUIGizmoTranslate(albaObserver *listener, bool testMode)
//----------------------------------------------------------------------------
{
  m_Listener = listener;

  m_Position[0] = m_Position[1] = m_Position[2] = 0;

  m_TestMode = testMode;

  if (m_TestMode == false)
  {
    CreateGui();
  }
}
//----------------------------------------------------------------------------
albaGUIGizmoTranslate::~albaGUIGizmoTranslate() 
//----------------------------------------------------------------------------
{ 	
  
}

//----------------------------------------------------------------------------
void albaGUIGizmoTranslate::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);

  m_Gui->Divider(2);
  m_Gui->Label("Translation gizmo abs position", true);
  m_Gui->Double(ID_TRANSLATE_X, "Translate X", &m_Position[0]);
  m_Gui->Double(ID_TRANSLATE_Y, "Translate Y", &m_Position[1]);
  m_Gui->Double(ID_TRANSLATE_Z, "Translate Z", &m_Position[2]);
	m_Gui->Divider();
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaGUIGizmoTranslate::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_TRANSLATE_X:
      case ID_TRANSLATE_Y:
      case ID_TRANSLATE_Z:
      {
        SendAbsPosition(e);
      }
      break;

      default:
        albaEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaGUIGizmoTranslate::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_TRANSLATE_X, enable);
  m_Gui->Enable(ID_TRANSLATE_Y, enable);
  m_Gui->Enable(ID_TRANSLATE_Z, enable);
}

//----------------------------------------------------------------------------
void albaGUIGizmoTranslate::SendAbsPosition(albaEventBase *sourceEvent)
//----------------------------------------------------------------------------
{
  // build abs matrix position
  albaMatrix m2send;
  albaTransform::SetPosition(m2send, m_Position);

  albaEvent event2Send;
  event2Send.SetSender(this);
  event2Send.SetId(sourceEvent->GetId());
  event2Send.SetMatrix(&m2send);
  
  albaEventMacro(event2Send);
}

//----------------------------------------------------------------------------
void albaGUIGizmoTranslate::SetAbsPosition(albaMatrix *pose)
//----------------------------------------------------------------------------
{
  albaTransform::GetPosition(*pose, m_Position);
  
  if (m_TestMode == false)
  {
    assert(m_Gui);
    m_Gui->Update();
  }
}