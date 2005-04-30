/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdMouse.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:56 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafEventBase.h"
#include "mafView.h"
#include "mafSceneGraph.h"
#include "mafRWIBase.h"
#include "mmdMouse.h"

#include "mafEventInteraction.h"
#include "mmuIdFactory.h"

#include "vtkAssemblyPath.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkCellPicker.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mmdMouse::MOUSE_2D_MOVE)
MAF_ID_IMP(mmdMouse::MOUSE_CHAR_EVENT)

//------------------------------------------------------------------------------
mmdMouse::mmdMouse()
//------------------------------------------------------------------------------
{
  SetThreaded(0);
  m_LastPosition[0] = m_LastPosition[1] = 0;
  m_ButtonState[0]  = m_ButtonState[1] = m_ButtonState[2] = 0;
  m_SelectedView    = NULL;
  m_SelectedRWI     = NULL;
  m_Picker = vtkCellPicker::New();
  m_Picker->SetTolerance(0.001);  //modified by Stefano 19-1-2005
}

//------------------------------------------------------------------------------
mmdMouse::~mmdMouse()
//------------------------------------------------------------------------------
{
  m_Picker->Delete();
}

/*//------------------------------------------------------------------------------
void mmdMouse::CreateSettings()
//------------------------------------------------------------------------------
{
  return; // NO Gui!
}*/

/*//------------------------------------------------------------------------------
int mmdMouse::InternalInitialize()
//------------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize())
    return -1;
  return 0;
}
*/

/*
//------------------------------------------------------------------------------
void mmdMouse::InternalShutdown()
//------------------------------------------------------------------------------
{
  Superclass::InternalShutdown();
}
*/
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
    SetLastPosition(pos[0],pos[1],e->GetModifiers());
  }
  else if (id == BUTTON_DOWN)
  {
    // store the Selected RWI is needed for compounded view
    e->Get2DPosition(m_LastPosition);
    m_SelectedRWI = (mafRWIBase *)event->GetSender();
    e->SetSender(this);
    ForwardEvent(e,MCH_INPUT);
  }
  else if (id == BUTTON_UP)
  {
    e->Get2DPosition(m_LastPosition);
    e->SetSender(this);
    ForwardEvent(e,MCH_INPUT);
  }
  else if (id == VIEW_SELECT)
  {
    // clean picked list...
    m_Picker->InitializePickList(); 
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
    ForwardEvent(&ev,MCH_INPUT);
  }
}

//------------------------------------------------------------------------------
void mmdMouse::SetLastPosition(double x,double y,unsigned long modifiers)
//------------------------------------------------------------------------------
{
  m_LastPosition[0] = x;
  m_LastPosition[1] = y;
  
  // create a new event with last position
  mafEventInteraction e(this,MOUSE_2D_MOVE,x,y);
  e.SetModifiers(modifiers);
  ForwardEvent(e,MCH_INPUT);
}

//------------------------------------------------------------------------------
void mmdMouse::SendButtonEvent(mafEventInteraction *event)
//------------------------------------------------------------------------------
{
  event->Set2DPosition(GetLastPosition());
  Superclass::SendButtonEvent(event);
}
//------------------------------------------------------------------------------
vtkAssemblyPath *mmdMouse::Pick(int mouse_screen_pos[2])
//------------------------------------------------------------------------------
{
  vtkAssemblyPath *ap = NULL;

  if (m_SelectedView)
  {
    vtkRendererCollection *rc = m_SelectedRWI->GetRenderWindow()->GetRenderers();
    vtkRenderer *r = NULL;
    assert(rc);
    rc->InitTraversal();
    while(r = rc->GetNextItem())
    {
      if( m_Picker->Pick(mouse_screen_pos[0],mouse_screen_pos[1],0,r) )
      {
        ap = m_Picker->GetPath();
        break;
      }
    }
  }

  return ap;
}
//------------------------------------------------------------------------------
vtkRenderer *mmdMouse::GetRenderer()
//------------------------------------------------------------------------------
{
  vtkRenderer *r = NULL;
  r = m_SelectedRWI->FindPokedRenderer((int)m_LastPosition[0],(int)m_LastPosition[1]);

  return r;
}
//------------------------------------------------------------------------------
mafView *mmdMouse::GetView()
//------------------------------------------------------------------------------
{
  return m_SelectedView;
}
//------------------------------------------------------------------------------
vtkAbstractPropPicker *mmdMouse::GetPicker()
//------------------------------------------------------------------------------
{
  return (vtkAbstractPropPicker *)m_Picker;
}
//------------------------------------------------------------------------------
vtkRenderWindowInteractor *mmdMouse::GetInteractor()
//------------------------------------------------------------------------------
{
  if (m_SelectedView)
    return m_SelectedRWI->GetRenderWindow()->GetInteractor();

  return (vtkRenderWindowInteractor *)NULL;
}
