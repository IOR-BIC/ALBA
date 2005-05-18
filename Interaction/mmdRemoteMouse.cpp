/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdRemoteMouse.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-18 17:29:05 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mafInteractor.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mafInteractionDecl.h"
#include "mafEventBase.h"
#include "mafView.h"
#include "mafSceneGraph.h"
#include "mafRWIBase.h"
#include "mmdRemoteMouse.h"

#include "mflSmartPointer.h"
#include "mflEventInteraction.h"

#include "vtkMatrix4x4.h"
#include "vtkAssemblyPath.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkCellPicker.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MFL_EVT_IMP(mmdRemoteMouse::MOUSE_2D_MOVE)
MFL_EVT_IMP(mmdRemoteMouse::MOUSE_CHAR_EVENT)

//------------------------------------------------------------------------------
mmdRemoteMouse::mmdRemoteMouse()
//------------------------------------------------------------------------------
{
  SetThreaded(0);
  LastPosition[0] = LastPosition[1] = 0;
  ButtonState[0]  = ButtonState[1] = ButtonState[2] = 0;
  SelectedView    = NULL;
  SelectedRWI     = NULL;
  Picker = vtkCellPicker::New();
}

//------------------------------------------------------------------------------
mmdRemoteMouse::~mmdRemoteMouse()
//------------------------------------------------------------------------------
{
  Picker->Delete();
}

/*//------------------------------------------------------------------------------
void mmdRemoteMouse::CreateSettings()
//------------------------------------------------------------------------------
{
  return; // NO Gui!
}*/

/*//------------------------------------------------------------------------------
int mmdRemoteMouse::InternalInitialize()
//------------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize())
    return -1;
  return 0;
}
*/

/*
//------------------------------------------------------------------------------
void mmdRemoteMouse::InternalShutdown()
//------------------------------------------------------------------------------
{
  Superclass::InternalShutdown();
}
*/
//------------------------------------------------------------------------------
void mmdRemoteMouse::ProcessEvent(mflEvent *event,unsigned long channel)
//------------------------------------------------------------------------------
{
  assert(event);

  mafID id=event->GetID();

  mflEventInteraction *e = (mflEventInteraction *)event;

  if (id == MOUSE_2D_MOVE)
  {
    double pos[2];
    e->Get2DPosition(pos);
    SetLastPosition(pos[0],pos[1],e->GetModifiers());
  }
  else if (id == ButtonDownEvent)
  {
    // store the Selected RWI is needed for compounded view
    e->Get2DPosition(LastPosition);
    SelectedRWI = (mafRWIBase *)event->GetSender();
    e->SetSender(this);
    InvokeEvent(e,DeviceInputChannel);
  }
  else if (id == ButtonUpEvent)
  {
    e->Get2DPosition(LastPosition);
    e->SetSender(this);
    InvokeEvent(e,DeviceInputChannel);
  }
  else if (id == ViewSelectedEvent)
  {
    SelectedView = ((mafEventBase *)event)->GetView();
  }
  else if (id == MOUSE_CHAR_EVENT)
  {
    unsigned char key = (unsigned char)((mafEventBase *)event)->GetArg();
    mflSMART(mflEventInteraction,ev)(MOUSE_CHAR_EVENT,this);
    ev->SetKey(key);
    InvokeEvent(ev,DeviceInputChannel);
  }
}

//------------------------------------------------------------------------------
void mmdRemoteMouse::SetLastPosition(double x,double y,unsigned long modifiers)
//------------------------------------------------------------------------------
{
  LastPosition[0] = x;
  LastPosition[1] = y;
  
  // create a new event with last position
  mflSMART(mflEventInteraction,e)(MOUSE_2D_MOVE,this,x,y);
  e->SetModifiers(modifiers);
  InvokeEvent(e,DeviceInputChannel);
}

//------------------------------------------------------------------------------
void mmdRemoteMouse::SendButtonEvent(mflEventInteraction *event)
//------------------------------------------------------------------------------
{
  event->Set2DPosition(GetLastPosition());
  Superclass::SendButtonEvent(event);
}
//------------------------------------------------------------------------------
vtkAssemblyPath *mmdRemoteMouse::Pick(int mouse_screen_pos[2])
//------------------------------------------------------------------------------
{
  vtkAssemblyPath *ap = NULL;

  if (SelectedView)
  {
    vtkRendererCollection *rc = SelectedRWI->GetRenderWindow()->GetRenderers();
    vtkRenderer *r = NULL;
    assert(rc);
    rc->InitTraversal();
    while(r = rc->GetNextItem())
    {
      if( Picker->Pick(mouse_screen_pos[0],mouse_screen_pos[1],0,r) )
      {
        ap = Picker->GetPath();
        break;
      }
    }
  }

  return ap;
}
//------------------------------------------------------------------------------
vtkRenderer *mmdRemoteMouse::GetRenderer()
//------------------------------------------------------------------------------
{
  vtkRenderer *r = NULL;
  r = SelectedRWI->FindPokedRenderer((int)LastPosition[0],(int)LastPosition[1]);

  return r;
}
//------------------------------------------------------------------------------
mafView *mmdRemoteMouse::GetView()
//------------------------------------------------------------------------------
{
  return SelectedView;
}
//------------------------------------------------------------------------------
vtkAbstractPropPicker *mmdRemoteMouse::GetPicker()
//------------------------------------------------------------------------------
{
  return (vtkAbstractPropPicker *)Picker;
}
//------------------------------------------------------------------------------
vtkRenderWindowInteractor *mmdRemoteMouse::GetInteractor()
//------------------------------------------------------------------------------
{
  if (SelectedView)
    return SelectedRWI->GetRenderWindow()->GetInteractor();

  return (vtkRenderWindowInteractor *)NULL;
}

