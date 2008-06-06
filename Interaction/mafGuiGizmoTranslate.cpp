/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiGizmoTranslate.cpp,v $
  Language:  C++
  Date:      $Date: 2008-06-06 10:59:10 $
  Version:   $Revision: 1.3 $
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


#include "mafGuiGizmoTranslate.h"
#include "mafDecl.h"
#include "mmgButton.h"
#include "mmgGui.h"

#include "mafGizmoInterface.h"
#include "mafGizmoTranslate.h"

#include "mafTransform.h"

#include "mafMatrix.h"

//----------------------------------------------------------------------------
mafGuiGizmoTranslate::mafGuiGizmoTranslate(mafObserver *listener)
//----------------------------------------------------------------------------
{
  m_Listener = listener;

  m_Position[0] = m_Position[1] = m_Position[2] = 0;
  CreateGui();
}
//----------------------------------------------------------------------------
mafGuiGizmoTranslate::~mafGuiGizmoTranslate() 
//----------------------------------------------------------------------------
{ 	
  
}

//----------------------------------------------------------------------------
void mafGuiGizmoTranslate::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);

  m_Gui->Divider(2);
  m_Gui->Label("translation gizmo abs position", true);
  m_Gui->Double(ID_TRANSLATE_X, "Translate X", &m_Position[0]);
  m_Gui->Double(ID_TRANSLATE_Y, "Translate Y", &m_Position[1]);
  m_Gui->Double(ID_TRANSLATE_Z, "Translate Z", &m_Position[2]);
	m_Gui->Divider();
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafGuiGizmoTranslate::OnEvent(mafEventBase *maf_event)
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
void mafGuiGizmoTranslate::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_TRANSLATE_X, enable);
  m_Gui->Enable(ID_TRANSLATE_Y, enable);
  m_Gui->Enable(ID_TRANSLATE_Z, enable);
}

//----------------------------------------------------------------------------
void mafGuiGizmoTranslate::SendAbsPosition(mafEventBase *sourceEvent)
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
void mafGuiGizmoTranslate::SetAbsPosition(mafMatrix *pose)
//----------------------------------------------------------------------------
{
  mafTransform::GetPosition(*pose, m_Position);

  assert(m_Gui);
  m_Gui->Update();
}
