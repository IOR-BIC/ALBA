/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiGizmoRotate.cpp,v $
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


#include "mafGuiGizmoRotate.h"
#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgButton.h"

#include "mafGizmoInterface.h"
#include "mafGizmoTranslate.h"

#include "mafTransform.h"
#include "mafMatrix.h"

//----------------------------------------------------------------------------
mafGuiGizmoRotate::mafGuiGizmoRotate(mafObserver *listener)
//----------------------------------------------------------------------------
{
  m_Listener = listener;

  m_Orientation[0] = m_Orientation[1] = m_Orientation[2] = 0;
  CreateGui();
}
//----------------------------------------------------------------------------
mafGuiGizmoRotate::~mafGuiGizmoRotate() 
//----------------------------------------------------------------------------
{
  
}

//----------------------------------------------------------------------------
void mafGuiGizmoRotate::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  
  m_Gui->Divider(2);
  m_Gui->Label("rotation gizmo abs orientation", true);
  m_Gui->Double(ID_ROTATE_X, "Rotate X", &m_Orientation[0]);
  m_Gui->Double(ID_ROTATE_Y, "Rotate Y", &m_Orientation[1]);
  m_Gui->Double(ID_ROTATE_Z, "Rotate Z", &m_Orientation[2]);
	m_Gui->Divider();
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafGuiGizmoRotate::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafGuiGizmoRotate::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_ROTATE_X, enable);
  m_Gui->Enable(ID_ROTATE_Y, enable);
  m_Gui->Enable(ID_ROTATE_Z, enable);
}

//----------------------------------------------------------------------------
void mafGuiGizmoRotate::SendAbsOrientation(mafEventBase *sourceEvent)
//----------------------------------------------------------------------------
{
  // build abs matrix position
  mafMatrix m2send;
  mafTransform::SetOrientation(m2send, m_Orientation[0], m_Orientation[1], m_Orientation[2]);

  mafEvent event2Send;
  event2Send.SetSender(this);
  event2Send.SetId(sourceEvent->GetId());
  event2Send.SetMatrix(&m2send);
  
  mafEventMacro(event2Send);
}

//----------------------------------------------------------------------------
void mafGuiGizmoRotate::SetAbsOrientation(mafMatrix *pose)
//----------------------------------------------------------------------------
{
  mafTransform::GetOrientation(*pose, m_Orientation);

  assert(m_Gui);
  m_Gui->Update();
}
