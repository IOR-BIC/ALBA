/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar2D.cpp,v $
  Language:  C++
  Date:      $Date: 2009-05-25 14:49:03 $
  Version:   $Revision: 1.3.22.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafAvatar2D.h"  

// interactors & devices
#include "mafDeviceButtonsPadMouse.h"

// events
#include "mafEventInteraction.h"

// Visualization
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTimerLog.h"

// system includes
#include <assert.h>

mafCxxTypeMacro(mafAvatar2D);

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
mafDeviceButtonsPadMouse *mafAvatar2D::GetMouse()
//------------------------------------------------------------------------------
{
  return mafDeviceButtonsPadMouse::SafeDownCast(GetDevice());
}

//------------------------------------------------------------------------------
void mafAvatar2D::SetMouse(mafDeviceButtonsPadMouse *mouse)
//------------------------------------------------------------------------------
{
  // Note device is always set before initializing...
  SetDevice(mouse);
}

//------------------------------------------------------------------------------
void mafAvatar2D::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  mafID id=event->GetId();

  // ChangeView event
  if (id==VIEW_SELECT)
    SetRenderer((vtkRenderer *)event->GetData());
}
