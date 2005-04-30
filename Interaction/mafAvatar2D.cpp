/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar2D.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:54 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mafAvatar2D.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// base includes
#include "mafAvatar2D.h"

// interactors & devices
#include "mmdMouse.h"

// factory
#include "mafInteractionFactory.h"

// events
#include "mafInteractionDecl.h"
#include "mflEventInteraction.h"

// Visualization
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTimerLog.h"

// system includes
#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAvatar2D::mafAvatar2D()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
mafAvatar2D::~mafAvatar2D()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mmdMouse *mafAvatar2D::GetMouse()
//------------------------------------------------------------------------------
{
  return mmdMouse::SafeDownCast(GetDevice());
}

//------------------------------------------------------------------------------
void mafAvatar2D::SetMouse(mmdMouse *mouse)
//------------------------------------------------------------------------------
{
  // Note device is always set before initializing...
  SetDevice(mouse);
}

//------------------------------------------------------------------------------
void mafAvatar2D::ProcessEvent(mflEvent *event,mafID ch)
//------------------------------------------------------------------------------
{
  assert(event);

  mafID id=event->GetID();

  // ChangeView event
  if (ch==CameraUpdateChannel)
  {
    if (id==ViewSelectedEvent)
      SetRenderer((vtkRenderer *)event->GetData());
  }

}
