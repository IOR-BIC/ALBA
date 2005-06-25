/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar2D.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-25 11:53:44 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafAvatar2D.h"  

// interactors & devices
#include "mmdMouse.h"

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
void mafAvatar2D::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  mafID id=event->GetId();

  // ChangeView event
  if (id==VIEW_SELECT)
    SetRenderer((vtkRenderer *)event->GetData());
}
