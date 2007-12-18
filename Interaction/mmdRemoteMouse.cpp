/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdRemoteMouse.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-18 14:08:16 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone
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

#include "mmdRemoteMouse.h"
#include "mafEventBase.h"
#include "mafView.h"
#include "mafSceneGraph.h"
#include "mafRWIBase.h"

#include "mafEventInteraction.h"
#include "mmuIdFactory.h"

#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmdRemoteMouse)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmdRemoteMouse::mmdRemoteMouse()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
mmdRemoteMouse::~mmdRemoteMouse()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
void mmdRemoteMouse::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  mafID id = event->GetId();
  mafEventInteraction *e = mafEventInteraction::SafeDownCast(event);

  m_FromRemote = true;
  double pos[2];
  if (e)
  {
    e->Get2DPosition(pos);
  }

  if (id == mmdMouse::MOUSE_2D_MOVE)
  {
    NormalizedDisplayToDisplay(pos);
    SetLastPosition(pos[0],pos[1],e->GetModifiers());
  }
  else if (id == BUTTON_DOWN || id == MOUSE_DCLICK)
  {
    // store the Selected RWI is needed for compounded view
    m_ButtonPressed = true;
    if (m_SelectedView)
    {
      m_SelectedRWI = m_SelectedView->GetRWI();
    }
    NormalizedDisplayToDisplay(pos);
    e->SetSender(this);
    m_LastPosition[0] = pos[0];
    m_LastPosition[1] = pos[1];
    e->Set2DPosition(m_LastPosition);
    InvokeEvent(e,MCH_INPUT);
  }
  else if (id == BUTTON_UP)
  {
    NormalizedDisplayToDisplay(pos);
    m_LastPosition[0] = pos[0];
    m_LastPosition[1] = pos[1];
    e->Set2DPosition(m_LastPosition);
    e->SetSender(this);
    InvokeEvent(e,MCH_INPUT);
  }
  else if (id == VIEW_SELECT)
  {
    mafEvent *ev = mafEvent::SafeDownCast(event);
    if (ev)
    {
      m_SelectedView = ev->GetView();
    }
  }
  else if (id == VIEW_DELETE)
  {
    m_SelectedView = NULL;
    m_SelectedRWI  = NULL;
  }
  else if (id == mmdMouse::MOUSE_CHAR_EVENT)
  {
    mafEvent *ev = mafEvent::SafeDownCast(event);
    if (ev)
    {
      unsigned char key = (unsigned char)ev->GetArg();
      mafEventInteraction ei(this, mmdMouse::MOUSE_CHAR_EVENT);
      ei.SetKey(key);
      InvokeEvent(ei,MCH_INPUT);
    }
  }

  m_FromRemote = false;
}
