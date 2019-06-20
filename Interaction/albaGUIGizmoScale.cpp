/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIGizmoScale
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


#include "albaGUIGizmoScale.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGUIButton.h"

#include "albaGizmoInterface.h"

#include "albaTransform.h"
#include "albaMatrix.h"

#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
albaGUIGizmoScale::albaGUIGizmoScale(albaObserver *listener, bool testMode)
//----------------------------------------------------------------------------
{
  m_Listener = listener;
  m_TestMode = testMode;

  m_Scaling[0] = m_Scaling[1] = m_Scaling[2] = 1;

  if (m_TestMode == false)
  {
    CreateGui();
  }  
}

//----------------------------------------------------------------------------
albaGUIGizmoScale::~albaGUIGizmoScale() 
//----------------------------------------------------------------------------
{ 	
  
}

//----------------------------------------------------------------------------
void albaGUIGizmoScale::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);

  m_Gui->Divider(2);
  m_Gui->Label("scaling gizmo abs scaling", true);
  m_Gui->Double(ID_SCALE_X, "Scaling X", &m_Scaling[0], 0);
  m_Gui->Double(ID_SCALE_Y, "Scaling Y", &m_Scaling[1], 0);
  m_Gui->Double(ID_SCALE_Z, "Scaling Z", &m_Scaling[2], 0);
	m_Gui->Divider();
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaGUIGizmoScale::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_SCALE_X:
      case ID_SCALE_Y:
      case ID_SCALE_Z:
      {
        SendAbsScaling(e);
      }
      break;

      default:
        albaEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaGUIGizmoScale::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_SCALE_X, enable);
  m_Gui->Enable(ID_SCALE_Y, enable);
  m_Gui->Enable(ID_SCALE_Z, enable);
}

//----------------------------------------------------------------------------
void albaGUIGizmoScale::SendAbsScaling(albaEventBase *sourceEvent)
//----------------------------------------------------------------------------
{
  // build abs matrix scaling
  albaMatrix m2send;
  albaTransform::Scale(m2send, m_Scaling[0], m_Scaling[1], m_Scaling[2], PRE_MULTIPLY);

  albaEvent event2Send;
  event2Send.SetSender(this);
  event2Send.SetId(sourceEvent->GetId());
  event2Send.SetMatrix(&m2send);
  
  albaEventMacro(event2Send);
}

//----------------------------------------------------------------------------
void albaGUIGizmoScale::SetAbsScaling(albaMatrix *pose)
//----------------------------------------------------------------------------
{
  albaTransform::GetScale(*pose, m_Scaling);
  
  if (m_TestMode == false)
  {
    assert(m_Gui);
    m_Gui->Update();
  }
}
