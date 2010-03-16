/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractorExtractIsosurface.cpp,v $
  Language:  C++
  Date:      $Date: 2010-03-16 09:43:54 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Paolo Quadrani & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"

#include "mafInteractorExtractIsosurface.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafAvatar3D.h"
#include "mafInteractor.h"
#include "mafRWIBase.h"
#include "mafEventInteraction.h"
#include "mafEvent.h"

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
mafCxxTypeMacro(mafInteractorExtractIsosurface)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafInteractorExtractIsosurface::mafInteractorExtractIsosurface()
//------------------------------------------------------------------------------
{
  m_PickValue = false;
}

//------------------------------------------------------------------------------
mafInteractorExtractIsosurface::~mafInteractorExtractIsosurface()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
int mafInteractorExtractIsosurface::StartInteraction(mafDeviceButtonsPadMouse *mouse)
//------------------------------------------------------------------------------
{
  return Superclass::StartInteraction(mouse);
}

//----------------------------------------------------------------------------
void mafInteractorExtractIsosurface::OnMouseMove() 
//----------------------------------------------------------------------------
{ 
  if (m_Renderer == NULL)
  {
    std::ostringstream stringStream;
    stringStream << "No vtkRenderer defined for mafInteractorExtractIsosurface" << std::endl;
    stringStream << "Skipping mafInteractorExtractIsosurface::OnMouseMove()" << std::endl;
    mafLogMessage(stringStream.str().c_str());
    return;
  }

  m_PickValue = false;
  Superclass::OnMouseMove();
}
//----------------------------------------------------------------------------
void mafInteractorExtractIsosurface::OnLeftButtonDown(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
  m_PickValue = true;
  Superclass::OnLeftButtonDown(e);
}
//----------------------------------------------------------------------------
void mafInteractorExtractIsosurface::OnButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  if (m_Renderer == NULL)
  {
    std::ostringstream stringStream;
    stringStream << "No vtkRenderer defined for mafInteractorExtractIsosurface" << std::endl;
    stringStream << "Skipping mafInteractorExtractIsosurface::OnButtonUp()" << std::endl;
    mafLogMessage(stringStream.str().c_str());
    return;
  }

  m_ButtonPressed = e->GetButton();

  switch(m_ButtonPressed) 
  {
    case MAF_LEFT_BUTTON:
      if(m_PickValue)
        this->PickIsoValue((mafDevice *)e->GetSender());
      OnLeftButtonUp();
    break;
    case MAF_MIDDLE_BUTTON:
      OnMiddleButtonUp();
    break;
    case MAF_RIGHT_BUTTON:
      OnRightButtonUp();
    break;
  }
	m_Renderer->GetRenderWindow()->SetDesiredUpdateRate(0.001);
  m_Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void mafInteractorExtractIsosurface::PickIsoValue(mafDevice *device)
//----------------------------------------------------------------------------
{
  int x = m_LastMousePose[0];
  int y = m_LastMousePose[1];

  mafDeviceButtonsPadMouse *mouse = mafDeviceButtonsPadMouse::SafeDownCast(device);
  if( mouse && m_Renderer)
  {
    double pos_picked[3];
    if (mouse->GetRWI()->GetPicker()->Pick(x,y,0,m_Renderer))
    {
      mouse->GetRWI()->GetPicker()->GetPickPosition(pos_picked);
      vtkPoints *p = vtkPoints::New();
      p->SetNumberOfPoints(1);
      p->SetPoint(0,pos_picked);
      mafEventMacro(mafEvent(this,VME_PICKED,(vtkObject *)p));
      p->Delete();
    }
  }
  else if (m_Renderer == NULL && mouse != NULL)
  {
    std::ostringstream stringStream;
    stringStream << "No vtkRenderer defined for mafInteractorExtractIsosurface" << std::endl;
    stringStream << "Skipping mafInteractorExtractIsosurface::PickIsoValue()" << std::endl;
    mafLogMessage(stringStream.str().c_str());
    return;
  } 
  else if (m_Renderer != NULL && mouse == NULL)
  {
    std::ostringstream stringStream;
    stringStream << "No device available for picking" << std::endl;
    stringStream << "Skipping mafInteractorExtractIsosurface::PickIsoValue()" << std::endl;
    mafLogMessage(stringStream.str().c_str());
    return;
  }
  else
  {
    std::ostringstream stringStream;
    stringStream << "No vtkRenderer defined for mafInteractorExtractIsosurface" << std::endl;
    stringStream << "and no picking device available." << std::endl;
    stringStream << "Skipping mafInteractorExtractIsosurface::PickIsoValue()" << std::endl;
    mafLogMessage(stringStream.str().c_str());
    return;
  }
}
