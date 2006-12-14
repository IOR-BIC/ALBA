/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiGizmoScale.cpp,v $
  Language:  C++
  Date:      $Date: 2006-12-14 10:00:21 $
  Version:   $Revision: 1.2 $
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


#include "mafGuiGizmoScale.h"
#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgButton.h"

#include "mafGizmoInterface.h"

#include "mafTransform.h"
#include "mafMatrix.h"

#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
mafGuiGizmoScale::mafGuiGizmoScale(mafObserver *listener)
//----------------------------------------------------------------------------
{
  m_Listener = listener;

  Scaling[0] = Scaling[1] = Scaling[2] = 1;
  CreateGui();
}

//----------------------------------------------------------------------------
mafGuiGizmoScale::~mafGuiGizmoScale() 
//----------------------------------------------------------------------------
{ 	
  
}

//----------------------------------------------------------------------------
void mafGuiGizmoScale::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);

  m_Gui->Divider(2);
  m_Gui->Label("scaling gizmo abs scaling", true);
  m_Gui->Double(ID_SCALE_X, "Scaling X", &Scaling[0], 0);
  m_Gui->Double(ID_SCALE_Y, "Scaling Y", &Scaling[1], 0);
  m_Gui->Double(ID_SCALE_Z, "Scaling Z", &Scaling[2], 0);
	m_Gui->Divider();
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafGuiGizmoScale::OnEvent(mafEventBase *maf_event)
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
void mafGuiGizmoScale::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_SCALE_X, enable);
  m_Gui->Enable(ID_SCALE_Y, enable);
  m_Gui->Enable(ID_SCALE_Z, enable);
}

//----------------------------------------------------------------------------
void mafGuiGizmoScale::SendAbsScaling(mafEventBase *sourceEvent)
//----------------------------------------------------------------------------
{
  // build abs matrix scaling
  mafMatrix m2send;
  mafTransform::Scale(m2send, Scaling[0], Scaling[1], Scaling[2], PRE_MULTIPLY);

  mafEvent event2Send;
  event2Send.SetSender(this);
  event2Send.SetId(sourceEvent->GetId());
  event2Send.SetMatrix(&m2send);
  
  mafEventMacro(event2Send);
}

//----------------------------------------------------------------------------
void mafGuiGizmoScale::SetAbsScaling(mafMatrix *pose)
//----------------------------------------------------------------------------
{
  mafTransform::GetScale(*pose, Scaling);

  assert(m_Gui);
  m_Gui->Update();
}
