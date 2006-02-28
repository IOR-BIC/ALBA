/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdRemoteMouse.cpp,v $
  Language:  C++
  Date:      $Date: 2006-02-28 14:47:40 $
  Version:   $Revision: 1.3 $
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
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mmdRemoteMouse::MOUSE_2D_MOVE)
MAF_ID_IMP(mmdRemoteMouse::MOUSE_CHAR_EVENT)

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmdRemoteMouse)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmdRemoteMouse::mmdRemoteMouse()
//------------------------------------------------------------------------------
{
  SetThreaded(0);
  m_LastPosition[0] = m_LastPosition[1] = 0;
  m_ButtonState[0]  = m_ButtonState[1] = m_ButtonState[2] = 0;
  m_SelectedView    = NULL;
  m_SelectedRWI     = NULL;
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
  mafEventInteraction *e = (mafEventInteraction *)event;
  if (id == MOUSE_2D_MOVE)
  {
    double pos[2];
    e->Get2DPosition(pos);
    SetLastPosition(pos[0],pos[1],e->GetModifiers());
  }
  else if (id == BUTTON_DOWN)
  {
    // store the Selected RWI is needed for compounded view
    e->Get2DPosition(m_LastPosition);
    m_SelectedRWI = (mafRWIBase *)event->GetSender();
    e->SetSender(this);
    InvokeEvent(e,MCH_INPUT);
  }
  else if (id == BUTTON_UP)
  {
    e->Get2DPosition(m_LastPosition);
    e->SetSender(this);
    InvokeEvent(e,MCH_INPUT);
  }
  else if (id == VIEW_SELECT)
  {
    mafEvent *e=mafEvent::SafeDownCast(event);
    assert(e);
    m_SelectedView = e->GetView();    
  }
  else if (id == MOUSE_CHAR_EVENT)
  {
    mafEvent *e=mafEvent::SafeDownCast(event);
    assert(e);
    unsigned char key = (unsigned char)e->GetArg();
    mafEventInteraction ev(this,MOUSE_CHAR_EVENT);
    ev.SetKey(key);
    InvokeEvent(ev,MCH_INPUT);
  }
}

//------------------------------------------------------------------------------
void mmdRemoteMouse::SetLastPosition(double x,double y,unsigned long modifiers)
//------------------------------------------------------------------------------
{
  m_LastPosition[0] = x;
  m_LastPosition[1] = y;
  
  // create a new event with last position
  mafEventInteraction e(this,MOUSE_2D_MOVE,x,y);
  e.SetModifiers(modifiers);
  InvokeEvent(e,MCH_INPUT);
}

//------------------------------------------------------------------------------
void mmdRemoteMouse::SendButtonEvent(mafEventInteraction *event)
//------------------------------------------------------------------------------
{
  event->Set2DPosition(GetLastPosition());
  Superclass::SendButtonEvent(event);
}
//------------------------------------------------------------------------------
vtkRenderer *mmdRemoteMouse::GetRenderer()
//------------------------------------------------------------------------------
{
  vtkRenderer *r = NULL;
  if (m_SelectedView)
  {
    r = m_SelectedRWI->FindPokedRenderer((int)m_LastPosition[0],(int)m_LastPosition[1]);
  }

  return r;
}
//------------------------------------------------------------------------------
mafView *mmdRemoteMouse::GetView()
//------------------------------------------------------------------------------
{
  return m_SelectedView;
}
//------------------------------------------------------------------------------
vtkRenderWindowInteractor *mmdRemoteMouse::GetInteractor()
//------------------------------------------------------------------------------
{
  if (m_SelectedRWI)
    return m_SelectedRWI->GetRenderWindow()->GetInteractor();

  return (vtkRenderWindowInteractor *)NULL;
}
//------------------------------------------------------------------------------
mafRWIBase *mmdRemoteMouse::GetRWI()
//------------------------------------------------------------------------------
{
  return m_SelectedRWI;
}
