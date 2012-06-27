/*=========================================================================

 Program: MAF2
 Module: mafAvatar2D
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
