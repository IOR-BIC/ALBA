/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiGizmoScale.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:38 $
  Version:   $Revision: 1.4 $
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


#include "mafGUIGizmoScale.h"
#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGUIButton.h"

#include "mafGizmoInterface.h"

#include "mafTransform.h"
#include "mafMatrix.h"

#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
mafGUIGizmoScale::mafGUIGizmoScale(mafObserver *listener)
//----------------------------------------------------------------------------
{
  m_Listener = listener;

  m_Scaling[0] = m_Scaling[1] = m_Scaling[2] = 1;
  CreateGui();
}

//----------------------------------------------------------------------------
mafGUIGizmoScale::~mafGUIGizmoScale() 
//----------------------------------------------------------------------------
{ 	
  
}

//----------------------------------------------------------------------------
void mafGUIGizmoScale::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  m_Gui->Divider(2);
  m_Gui->Label("scaling gizmo abs scaling", true);
  m_Gui->Double(ID_SCALE_X, "Scaling X", &m_Scaling[0], 0);
  m_Gui->Double(ID_SCALE_Y, "Scaling Y", &m_Scaling[1], 0);
  m_Gui->Double(ID_SCALE_Z, "Scaling Z", &m_Scaling[2], 0);
	m_Gui->Divider();
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafGUIGizmoScale::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafGUIGizmoScale::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_SCALE_X, enable);
  m_Gui->Enable(ID_SCALE_Y, enable);
  m_Gui->Enable(ID_SCALE_Z, enable);
}

//----------------------------------------------------------------------------
void mafGUIGizmoScale::SendAbsScaling(mafEventBase *sourceEvent)
//----------------------------------------------------------------------------
{
  // build abs matrix scaling
  mafMatrix m2send;
  mafTransform::Scale(m2send, m_Scaling[0], m_Scaling[1], m_Scaling[2], PRE_MULTIPLY);

  mafEvent event2Send;
  event2Send.SetSender(this);
  event2Send.SetId(sourceEvent->GetId());
  event2Send.SetMatrix(&m2send);
  
  mafEventMacro(event2Send);
}

//----------------------------------------------------------------------------
void mafGUIGizmoScale::SetAbsScaling(mafMatrix *pose)
//----------------------------------------------------------------------------
{
  mafTransform::GetScale(*pose, m_Scaling);

  assert(m_Gui);
  m_Gui->Update();
}
