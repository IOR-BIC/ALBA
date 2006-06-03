/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdMouse.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-03 11:01:39 $
  Version:   $Revision: 1.12 $
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


#include "mmdMouse.h"
#include "mafEventBase.h"
#include "mafView.h"
#include "mafSceneGraph.h"
#include "mafRWIBase.h"

#include "mafEventInteraction.h"
#include "mmuIdFactory.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mmdMouse::MOUSE_2D_MOVE)
MAF_ID_IMP(mmdMouse::MOUSE_CHAR_EVENT)

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmdMouse)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmdMouse::mmdMouse()
//------------------------------------------------------------------------------
{
  SetThreaded(0);
  m_LastPosition[0] = m_LastPosition[1] = 0;
  m_ButtonState[0]  = m_ButtonState[1] = m_ButtonState[2] = 0;
  m_SelectedView    = NULL;
  m_SelectedRWI     = NULL;
  
  m_UpdateRwiInOnMoveFlag = false;
  m_CollaborateStatus     = false;
  m_FromRemote            = false;
  m_ButtonPressed         = false;
}

//------------------------------------------------------------------------------
mmdMouse::~mmdMouse()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
void mmdMouse::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  mafID id=event->GetId();

  mafEventInteraction *e = (mafEventInteraction *)event;

  if (id == MOUSE_2D_MOVE)
  {
    double pos[2];
    e->Get2DPosition(pos);
    if (m_UpdateRwiInOnMoveFlag)
    {
      m_SelectedRWI = (mafRWIBase *)event->GetSender();
    }
    SetLastPosition(pos[0],pos[1],e->GetModifiers());
  }
  else if (id == BUTTON_DOWN)
  {
    // store the Selected RWI is needed for compounded view
    m_ButtonPressed = true;
    e->Get2DPosition(m_LastPosition);
    m_SelectedRWI = (mafRWIBase *)event->GetSender();
    e->SetSender(this);
    InvokeEvent(e,MCH_INPUT);
    if (m_CollaborateStatus)
    {
      double disp[2];
      e->Get2DPosition(disp);
      DisplayToNormalizedDisplay(disp);
      mafEventInteraction remoteEv;
      remoteEv.SetSender(this);
      remoteEv.SetId(BUTTON_DOWN);
      remoteEv.SetButton(e->GetButton());
      remoteEv.Set2DPosition(disp);
      remoteEv.SetModifiers(e->GetModifiers());
      InvokeEvent(remoteEv,REMOTE_COMMAND_CHANNEL);
    }
  }
  else if (id == BUTTON_UP)
  {
    m_ButtonPressed = false;
    e->Get2DPosition(m_LastPosition);
    e->SetSender(this);
    InvokeEvent(e,MCH_INPUT);
    if (m_CollaborateStatus)
    {
      double disp[2];
      e->Get2DPosition(disp);
      DisplayToNormalizedDisplay(disp);
      mafEventInteraction remoteEv;
      remoteEv.SetSender(this);
      remoteEv.SetId(BUTTON_UP);
      remoteEv.SetButton(e->GetButton());
      remoteEv.Set2DPosition(disp);
      remoteEv.SetModifiers(e->GetModifiers());
      InvokeEvent(remoteEv,REMOTE_COMMAND_CHANNEL);
    }
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
    InvokeEvent(&ev,MCH_INPUT);
  }
}

//------------------------------------------------------------------------------
void mmdMouse::SetLastPosition(double x,double y,unsigned long modifiers)
//------------------------------------------------------------------------------
{
  m_LastPosition[0] = x;
  m_LastPosition[1] = y;
  
  if (m_CollaborateStatus && m_SelectedRWI && !m_FromRemote && m_ButtonPressed)
  {
    double disp[2];
    disp[0] = (double)x;
    disp[1] = (double)y;
    DisplayToNormalizedDisplay(disp);
    mafEventInteraction remoteEv;
    remoteEv.SetSender(this);
    remoteEv.SetId(MOUSE_2D_MOVE);
    remoteEv.SetModifiers(modifiers);
    remoteEv.Set2DPosition(disp);
    InvokeEvent(remoteEv,REMOTE_COMMAND_CHANNEL);
  }
  m_FromRemote = false;

  // create a new event with last position
  mafEventInteraction e(this,MOUSE_2D_MOVE,x,y);
  e.SetModifiers(modifiers);
  InvokeEvent(e,MCH_INPUT);
}

//------------------------------------------------------------------------------
void mmdMouse::SendButtonEvent(mafEventInteraction *event)
//------------------------------------------------------------------------------
{
  event->Set2DPosition(GetLastPosition());
  Superclass::SendButtonEvent(event);
}
//------------------------------------------------------------------------------
vtkRenderer *mmdMouse::GetRenderer()
//------------------------------------------------------------------------------
{
  vtkRenderer *r = NULL;
  if (m_SelectedRWI)
  {
    r = m_SelectedRWI->FindPokedRenderer((int)m_LastPosition[0],(int)m_LastPosition[1]);
  }

  return r;
}
//------------------------------------------------------------------------------
mafView *mmdMouse::GetView()
//------------------------------------------------------------------------------
{
  return m_SelectedView;
}
//------------------------------------------------------------------------------
vtkRenderWindowInteractor *mmdMouse::GetInteractor()
//------------------------------------------------------------------------------
{
  if (m_SelectedRWI)
    return m_SelectedRWI->GetRenderWindow()->GetInteractor();

  return (vtkRenderWindowInteractor *)NULL;
}
//------------------------------------------------------------------------------
mafRWIBase *mmdMouse::GetRWI()
//------------------------------------------------------------------------------
{
  return m_SelectedRWI;
}
//------------------------------------------------------------------------------
void mmdMouse::DisplayToNormalizedDisplay(double display[2])
//------------------------------------------------------------------------------
{
  vtkRenderer *r = GetRenderer();
  int *size;

  /* get physical window dimensions */
  size = r->GetVTKWindow()->GetSize();

  display[0] -= (size[0]*.5);
  display[1] -= (size[1]*.5);

  display[0] = display[0]/size[1];
  display[1] = display[1]/size[1];
  //r->DisplayToNormalizedDisplay(display[0],display[1]);
}
//------------------------------------------------------------------------------
void mmdMouse::NormalizedDisplayToDisplay(double normalized[2])
//------------------------------------------------------------------------------
{
  vtkRenderer *r = GetRenderer();
  int *size;

  /* get physical window dimensions */
  size = r->GetVTKWindow()->GetSize();

  normalized[0] = normalized[0]*size[1];
  normalized[1] = normalized[1]*size[1];

  normalized[0] += (size[0]*.5);
  normalized[1] += (size[1]*.5);
  //  r->NormalizedDisplayToDisplay(normalized[0],normalized[1]);
}
