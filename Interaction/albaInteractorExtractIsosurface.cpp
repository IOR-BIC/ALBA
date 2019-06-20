/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorExtractIsosurface
 Authors: Paolo Quadrani & Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"

#include "albaInteractorExtractIsosurface.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaAvatar3D.h"
#include "albaInteractor.h"
#include "albaRWIBase.h"
#include "albaEventInteraction.h"
#include "albaEvent.h"

#include "vtkAbstractPicker.h"
#include "vtkCellPicker.h"
#include "vtkPoints.h"
#include "vtkMath.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkTransform.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorExtractIsosurface)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaInteractorExtractIsosurface::albaInteractorExtractIsosurface()
//------------------------------------------------------------------------------
{
  m_PickValue = false;
}

//------------------------------------------------------------------------------
albaInteractorExtractIsosurface::~albaInteractorExtractIsosurface()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
int albaInteractorExtractIsosurface::StartInteraction(albaDeviceButtonsPadMouse *mouse)
//------------------------------------------------------------------------------
{
  return Superclass::StartInteraction(mouse);
}

//----------------------------------------------------------------------------
void albaInteractorExtractIsosurface::OnMouseMove() 
//----------------------------------------------------------------------------
{ 
  if (m_Renderer == NULL)
  {
    std::ostringstream stringStream;
    stringStream << "No vtkRenderer defined for albaInteractorExtractIsosurface" << std::endl;
    stringStream << "Skipping albaInteractorExtractIsosurface::OnMouseMove()" << std::endl;
    albaLogMessage(stringStream.str().c_str());
    return;
  }

  m_PickValue = false;
  Superclass::OnMouseMove();
}
//----------------------------------------------------------------------------
void albaInteractorExtractIsosurface::OnLeftButtonDown(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
  m_PickValue = true;
  Superclass::OnLeftButtonDown(e);
}
//----------------------------------------------------------------------------
void albaInteractorExtractIsosurface::OnButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  if (m_Renderer == NULL)
  {
    std::ostringstream stringStream;
    stringStream << "No vtkRenderer defined for albaInteractorExtractIsosurface" << std::endl;
    stringStream << "Skipping albaInteractorExtractIsosurface::OnButtonUp()" << std::endl;
    albaLogMessage(stringStream.str().c_str());
    return;
  }

  m_ButtonPressed = e->GetButton();

  switch(m_ButtonPressed) 
  {
    case ALBA_LEFT_BUTTON:
      if(m_PickValue)
        this->PickIsoValue((albaDevice *)e->GetSender());
      OnLeftButtonUp();
    break;
    case ALBA_MIDDLE_BUTTON:
      OnMiddleButtonUp();
    break;
    case ALBA_RIGHT_BUTTON:
      OnRightButtonUp();
    break;
  }
	m_Renderer->GetRenderWindow()->SetDesiredUpdateRate(0.001);
  m_Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void albaInteractorExtractIsosurface::PickIsoValue(albaDevice *device)
//----------------------------------------------------------------------------
{
  int x = m_LastMousePose[0];
  int y = m_LastMousePose[1];

  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast(device);
  if( mouse && m_Renderer)
  {
    double pos_picked[3];
    if (mouse->GetRWI()->GetPicker()->Pick(x,y,0,m_Renderer))
    {
      mouse->GetRWI()->GetPicker()->GetPickPosition(pos_picked);
      vtkPoints *p = vtkPoints::New();
      p->SetNumberOfPoints(1);
      p->SetPoint(0,pos_picked);
      albaEventMacro(albaEvent(this,VME_PICKED,(vtkObject *)p));
      p->Delete();
    }
  }
  else if (m_Renderer == NULL && mouse != NULL)
  {
    std::ostringstream stringStream;
    stringStream << "No vtkRenderer defined for albaInteractorExtractIsosurface" << std::endl;
    stringStream << "Skipping albaInteractorExtractIsosurface::PickIsoValue()" << std::endl;
    albaLogMessage(stringStream.str().c_str());
    return;
  } 
  else if (m_Renderer != NULL && mouse == NULL)
  {
    std::ostringstream stringStream;
    stringStream << "No device available for picking" << std::endl;
    stringStream << "Skipping albaInteractorExtractIsosurface::PickIsoValue()" << std::endl;
    albaLogMessage(stringStream.str().c_str());
    return;
  }
  else
  {
    std::ostringstream stringStream;
    stringStream << "No vtkRenderer defined for albaInteractorExtractIsosurface" << std::endl;
    stringStream << "and no picking device available." << std::endl;
    stringStream << "Skipping albaInteractorExtractIsosurface::PickIsoValue()" << std::endl;
    albaLogMessage(stringStream.str().c_str());
    return;
  }
}
