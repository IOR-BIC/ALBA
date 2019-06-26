/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPadMouse
 Authors: Marco Petrone
 
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


#include "albaDeviceButtonsPadMouse.h"
#include "albaEventBase.h"
#include "albaView.h"
#include "albaSceneGraph.h"
#include "albaRWIBase.h"

#include "albaEventInteraction.h"
#include "mmuIdFactory.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
// ALBA_ID_IMP(albaDeviceButtonsPadMouse::MOUSE_2D_MOVE)
// ALBA_ID_IMP(albaDeviceButtonsPadMouse::MOUSE_CHAR_EVENT)
// ALBA_ID_IMP(albaDeviceButtonsPadMouse::MOUSE_DCLICK)

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaDeviceButtonsPadMouse)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaDeviceButtonsPadMouse::albaDeviceButtonsPadMouse()
//------------------------------------------------------------------------------
{
  SetThreaded(0);
  m_LastPosition[0] = m_LastPosition[1] = 0;
  m_ButtonState[0]  = m_ButtonState[1] = m_ButtonState[2] = 0;
  m_SelectedView    = NULL;
  m_SelectedRWI     = NULL;
  m_ButtonPressed   = false;
}

//------------------------------------------------------------------------------
albaDeviceButtonsPadMouse::~albaDeviceButtonsPadMouse()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
void albaDeviceButtonsPadMouse::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  albaID id = event->GetId();

  albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);

  if (id == GetMouse2DMoveId())
  {
    double pos[2];
    e->Get2DPosition(pos);
    m_SelectedRWI = (albaRWIBase *)event->GetSender();
    SetLastPosition(pos[0],pos[1],e->GetModifiers());
  }
	else if (id == GetWheelId())
	{
		double pos[2];
		e->Get2DPosition(m_LastPosition);
		m_SelectedRWI = (albaRWIBase *)event->GetSender();
		e->SetSender(this);
		InvokeEvent(e, (albaID)MCH_INPUT);
	}
  else if (id == GetButtonDownId() || id == GetMouseDClickId())
  {
    // store the Selected RWI is needed for compounded view
    m_ButtonPressed = true;
    e->Get2DPosition(m_LastPosition);
    m_SelectedRWI = (albaRWIBase *)event->GetSender();
    e->SetSender(this);
    InvokeEvent(e,(albaID)MCH_INPUT);
  }
  else if (id == GetButtonUpId())
  {
    m_ButtonPressed = false;
    e->Get2DPosition(m_LastPosition);
    e->SetSender(this);
    InvokeEvent(e,(albaID)MCH_INPUT);
  }
  else if (id == VIEW_SELECT)
  {
    albaEvent *ev = albaEvent::SafeDownCast(event);
    if (ev)
    {
      m_SelectedView = ev->GetView();    
    }
  }
  else if (id == GetMouseCharEventId())
  {
    albaEvent *ev = albaEvent::SafeDownCast(event);
    if (ev)
    {
      unsigned char key = (unsigned char)ev->GetArg();
      albaEventInteraction ei(this,GetMouseCharEventId());
      ei.SetKey(key);
      InvokeEvent(&ei,(albaID)MCH_INPUT);
    }
  }
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPadMouse::SetLastPosition(double x,double y,unsigned long modifiers)
//------------------------------------------------------------------------------
{
  m_LastPosition[0] = x;
  m_LastPosition[1] = y;
  
  // create a new event with last position
  albaEventInteraction e(this,GetMouse2DMoveId(),x,y);
  e.SetModifiers(modifiers);
  InvokeEvent(e,MCH_INPUT);
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPadMouse::SendButtonEvent(albaEventInteraction *event)
//------------------------------------------------------------------------------
{
  event->Set2DPosition(GetLastPosition());
  Superclass::SendButtonEvent(event);
}
//------------------------------------------------------------------------------
vtkRenderer *albaDeviceButtonsPadMouse::GetRenderer()
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
albaView *albaDeviceButtonsPadMouse::GetView()
//------------------------------------------------------------------------------
{
  return m_SelectedView;
}
//------------------------------------------------------------------------------
vtkRenderWindowInteractor *albaDeviceButtonsPadMouse::GetInteractor()
//------------------------------------------------------------------------------
{
  if (m_SelectedRWI)
    return m_SelectedRWI->GetRenderWindow()->GetInteractor();

  return (vtkRenderWindowInteractor *)NULL;
}
//------------------------------------------------------------------------------
albaRWIBase *albaDeviceButtonsPadMouse::GetRWI()
//------------------------------------------------------------------------------
{
  return m_SelectedRWI;
}
//------------------------------------------------------------------------------
void albaDeviceButtonsPadMouse::DisplayToNormalizedDisplay(double display[2])
//------------------------------------------------------------------------------
{
  vtkRenderer *r = GetRenderer();
  if (r == NULL) {return;}

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
void albaDeviceButtonsPadMouse::NormalizedDisplayToDisplay(double normalized[2])
//------------------------------------------------------------------------------
{
  vtkRenderer *r = GetRenderer();
  if (r == NULL) {return;}

  int *size;

  /* get physical window dimensions */
  size = r->GetVTKWindow()->GetSize();

  normalized[0] = normalized[0]*size[1];
  normalized[1] = normalized[1]*size[1];

  normalized[0] += (size[0]*.5);
  normalized[1] += (size[1]*.5);
  //  r->NormalizedDisplayToDisplay(normalized[0],normalized[1]);
}
//------------------------------------------------------------------------------
albaID albaDeviceButtonsPadMouse::GetMouse2DMoveId()
//------------------------------------------------------------------------------
{
  static const albaID mouse2DMoveId = mmuIdFactory::GetNextId("MOUSE_2D_MOVE");
  return mouse2DMoveId;
}
//------------------------------------------------------------------------------
albaID albaDeviceButtonsPadMouse::GetMouseCharEventId()
//------------------------------------------------------------------------------
{
  static const albaID mouseCharEventId = mmuIdFactory::GetNextId("MOUSE_CHAR_EVENT");
  return mouseCharEventId;
}
//------------------------------------------------------------------------------
albaID albaDeviceButtonsPadMouse::GetMouseDClickId()
//------------------------------------------------------------------------------
{
  static const albaID mouseDClickId = mmuIdFactory::GetNextId("MOUSE_DCLICK");
  return mouseDClickId;
}
