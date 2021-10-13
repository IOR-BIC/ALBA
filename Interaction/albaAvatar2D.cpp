/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAvatar2D
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaAvatar2D.h"  

// interactors & devices
#include "albaDeviceButtonsPadMouse.h"

// events
#include "albaEventInteraction.h"

// Visualization
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTimerLog.h"

// system includes
#include <assert.h>

albaCxxTypeMacro(albaAvatar2D);

//------------------------------------------------------------------------------
albaAvatar2D::albaAvatar2D()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
albaAvatar2D::~albaAvatar2D()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
albaDeviceButtonsPadMouse *albaAvatar2D::GetMouse()
//------------------------------------------------------------------------------
{
  return albaDeviceButtonsPadMouse::SafeDownCast(GetDevice());
}

//------------------------------------------------------------------------------
void albaAvatar2D::SetMouse(albaDeviceButtonsPadMouse *mouse)
//------------------------------------------------------------------------------
{
  // Note device is always set before initializing...
  SetDevice(mouse);
}

//------------------------------------------------------------------------------
void albaAvatar2D::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  // ChangeView event
	if (event->GetId() == VIEW_SELECT)
	{
		albaEvent *e = albaEvent::SafeDownCast(event);
		SetRendererAndView((vtkRenderer *)e->GetData(), e->GetView());
	}
}
