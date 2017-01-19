/*=========================================================================

 Program: MAF2
 Module: mafDeviceButtonsPadMouse
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafDeviceButtonsPadMouse.h"
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
// MAF_ID_IMP(mafDeviceButtonsPadMouse::MOUSE_2D_MOVE)
// MAF_ID_IMP(mafDeviceButtonsPadMouse::MOUSE_CHAR_EVENT)
// MAF_ID_IMP(mafDeviceButtonsPadMouse::MOUSE_DCLICK)

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDeviceButtonsPadMouse)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDeviceButtonsPadMouse::mafDeviceButtonsPadMouse()
//------------------------------------------------------------------------------
{
  SetThreaded(0);
  m_LastPosition[0] = m_LastPosition[1] = 0;
  m_ButtonState[0]  = m_ButtonState[1] = m_ButtonState[2] = 0;
  m_SelectedView    = NULL;
  m_SelectedRWI     = NULL;
  
  m_UpdateRwiInOnMoveFlag = false;
  m_ButtonPressed         = false;
}

//------------------------------------------------------------------------------
mafDeviceButtonsPadMouse::~mafDeviceButtonsPadMouse()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
void mafDeviceButtonsPadMouse::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  mafID id = event->GetId();

  mafEventInteraction *e = mafEventInteraction::SafeDownCast(event);

  if (id == GetMouse2DMoveId())
  {
    double pos[2];
    e->Get2DPosition(pos);
    if (m_UpdateRwiInOnMoveFlag)
    {
      m_SelectedRWI = (mafRWIBase *)event->GetSender();
    }
    SetLastPosition(pos[0],pos[1],e->GetModifiers());
  }
	else if (id == GetWheelId())
	{
		double pos[2];
		e->Get2DPosition(m_LastPosition);
		m_SelectedRWI = (mafRWIBase *)event->GetSender();
		e->SetSender(this);
		InvokeEvent(e, MCH_INPUT);
	}
  else if (id == GetButtonDownId() || id == GetMouseDClickId())
  {
    // store the Selected RWI is needed for compounded view
    m_ButtonPressed = true;
    e->Get2DPosition(m_LastPosition);
    m_SelectedRWI = (mafRWIBase *)event->GetSender();
    e->SetSender(this);
    InvokeEvent(e,MCH_INPUT);
  }
  else if (id == GetButtonUpId())
  {
    m_ButtonPressed = false;
    e->Get2DPosition(m_LastPosition);
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
  else if (id == GetMouseCharEventId())
  {
    mafEvent *ev = mafEvent::SafeDownCast(event);
    if (ev)
    {
      unsigned char key = (unsigned char)ev->GetArg();
      mafEventInteraction ei(this,GetMouseCharEventId());
      ei.SetKey(key);
      InvokeEvent(&ei,MCH_INPUT);
    }
  }
}

//------------------------------------------------------------------------------
void mafDeviceButtonsPadMouse::SetLastPosition(double x,double y,unsigned long modifiers)
//------------------------------------------------------------------------------
{
  m_LastPosition[0] = x;
  m_LastPosition[1] = y;
  
  // create a new event with last position
  mafEventInteraction e(this,GetMouse2DMoveId(),x,y);
  e.SetModifiers(modifiers);
  InvokeEvent(e,MCH_INPUT);
}

//------------------------------------------------------------------------------
void mafDeviceButtonsPadMouse::SendButtonEvent(mafEventInteraction *event)
//------------------------------------------------------------------------------
{
  event->Set2DPosition(GetLastPosition());
  Superclass::SendButtonEvent(event);
}
//------------------------------------------------------------------------------
vtkRenderer *mafDeviceButtonsPadMouse::GetRenderer()
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
mafView *mafDeviceButtonsPadMouse::GetView()
//------------------------------------------------------------------------------
{
  return m_SelectedView;
}
//------------------------------------------------------------------------------
vtkRenderWindowInteractor *mafDeviceButtonsPadMouse::GetInteractor()
//------------------------------------------------------------------------------
{
  if (m_SelectedRWI)
    return m_SelectedRWI->GetRenderWindow()->GetInteractor();

  return (vtkRenderWindowInteractor *)NULL;
}
//------------------------------------------------------------------------------
mafRWIBase *mafDeviceButtonsPadMouse::GetRWI()
//------------------------------------------------------------------------------
{
  return m_SelectedRWI;
}
//------------------------------------------------------------------------------
void mafDeviceButtonsPadMouse::DisplayToNormalizedDisplay(double display[2])
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
void mafDeviceButtonsPadMouse::NormalizedDisplayToDisplay(double normalized[2])
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
mafID mafDeviceButtonsPadMouse::GetMouse2DMoveId()
//------------------------------------------------------------------------------
{
  static const mafID mouse2DMoveId = mmuIdFactory::GetNextId("MOUSE_2D_MOVE");
  return mouse2DMoveId;
}
//------------------------------------------------------------------------------
mafID mafDeviceButtonsPadMouse::GetMouseCharEventId()
//------------------------------------------------------------------------------
{
  static const mafID mouseCharEventId = mmuIdFactory::GetNextId("MOUSE_CHAR_EVENT");
  return mouseCharEventId;
}
//------------------------------------------------------------------------------
mafID mafDeviceButtonsPadMouse::GetMouseDClickId()
//------------------------------------------------------------------------------
{
  static const mafID mouseDClickId = mmuIdFactory::GetNextId("MOUSE_DCLICK");
  return mouseDClickId;
}
