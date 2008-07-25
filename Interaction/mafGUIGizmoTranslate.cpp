/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIGizmoTranslate.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 08:44:32 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGUIGizmoTranslate.h"
#include "mafDecl.h"
#include "mafGUIButton.h"
#include "mafGUI.h"

#include "mafGizmoInterface.h"
#include "mafGizmoTranslate.h"

#include "mafTransform.h"

#include "mafMatrix.h"

//----------------------------------------------------------------------------
mafGUIGizmoTranslate::mafGUIGizmoTranslate(mafObserver *listener)
//----------------------------------------------------------------------------
{
  m_Listener = listener;

  m_Position[0] = m_Position[1] = m_Position[2] = 0;
  CreateGui();
}
//----------------------------------------------------------------------------
mafGUIGizmoTranslate::~mafGUIGizmoTranslate() 
//----------------------------------------------------------------------------
{ 	
  
}

//----------------------------------------------------------------------------
void mafGUIGizmoTranslate::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  m_Gui->Divider(2);
  m_Gui->Label("translation gizmo abs position", true);
  m_Gui->Double(ID_TRANSLATE_X, "Translate X", &m_Position[0]);
  m_Gui->Double(ID_TRANSLATE_Y, "Translate Y", &m_Position[1]);
  m_Gui->Double(ID_TRANSLATE_Z, "Translate Z", &m_Position[2]);
	m_Gui->Divider();
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafGUIGizmoTranslate::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafGUIGizmoTranslate::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_TRANSLATE_X, enable);
  m_Gui->Enable(ID_TRANSLATE_Y, enable);
  m_Gui->Enable(ID_TRANSLATE_Z, enable);
}

//----------------------------------------------------------------------------
void mafGUIGizmoTranslate::SendAbsPosition(mafEventBase *sourceEvent)
//----------------------------------------------------------------------------
{
  // build abs matrix position
  mafMatrix m2send;
  mafTransform::SetPosition(m2send, m_Position);

  mafEvent event2Send;
  event2Send.SetSender(this);
  event2Send.SetId(sourceEvent->GetId());
  event2Send.SetMatrix(&m2send);
  
  mafEventMacro(event2Send);
}

//----------------------------------------------------------------------------
void mafGUIGizmoTranslate::SetAbsPosition(mafMatrix *pose)
//----------------------------------------------------------------------------
{
  mafTransform::GetPosition(*pose, m_Position);

  assert(m_Gui);
  m_Gui->Update();
}
