/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorSelectCell
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"

#include "albaInteractorSelectCell.h"
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
albaCxxTypeMacro(albaInteractorSelectCell)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaInteractorSelectCell::albaInteractorSelectCell()
//------------------------------------------------------------------------------
{
  m_IsPicking = false;
}

//------------------------------------------------------------------------------
albaInteractorSelectCell::~albaInteractorSelectCell()
//------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaInteractorSelectCell::OnMouseMove() 
//----------------------------------------------------------------------------
{
  // if something has been picked do not move the camera
  if (m_IsPicking == true)
  {
    return;
  }
  else
  {
    Superclass::OnMouseMove();
  }
}
//----------------------------------------------------------------------------
void albaInteractorSelectCell::OnLeftButtonDown(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
  if (e->GetModifier(ALBA_CTRL_KEY)) 
  {
    // picking mode on
    m_IsPicking = true;

    // perform picking
    PickCell((albaDevice *)e->GetSender());
  }
  else
  {
    m_IsPicking = false;
    Superclass::OnLeftButtonDown(e);
  }
}
//----------------------------------------------------------------------------
void albaInteractorSelectCell::OnButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  // reset
  m_IsPicking = false;

  Superclass::OnButtonUp(e);
}
//----------------------------------------------------------------------------
void albaInteractorSelectCell::PickCell( albaDevice *device )
{
  int x = m_LastMousePose[0];
  int y = m_LastMousePose[1];

  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast(device);
  if( mouse && m_Renderer)
  {
    double pos_picked[3];

    vtkCellPicker *cellPicker;
    vtkNEW(cellPicker);
    cellPicker->SetTolerance(0.001);
    
    if(cellPicker->Pick(x,y,0,m_Renderer))
    {
      //BES: 29.1.2009 - get the picked coordinates
      cellPicker->GetPickedPositions()->GetPoint(0, pos_picked);

      vtkPoints *pickedPoint = vtkPoints::New();
      pickedPoint->SetNumberOfPoints(1);
      pickedPoint->SetPoint(0,pos_picked);
      albaEventMacro(albaEvent(this,VME_PICKED,(vtkObject *)pickedPoint,cellPicker->GetCellId()));
      pickedPoint->Delete();
      
    }
  
    vtkDEL(cellPicker);
  }
}
void albaInteractorSelectCell::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  // if we are in pick modality...
  if (m_IsPicking)
  {
    // is this an interaction event?
    albaEventInteraction* eventInteraction = NULL;
    eventInteraction = albaEventInteraction::SafeDownCast(event);

    // if yes handle the picking
    if (eventInteraction != NULL)
    {
      // is it coming from the mouse?
      if (albaDeviceButtonsPadMouse *mouse=albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)eventInteraction->GetSender()))
      { 
        PickCell(mouse);
      }
      else
      {
        albaLogMessage("only handling events from the mouse!more code is required in order to handle this device!");
        assert(false);
      } 
    }
  } 

  Superclass::OnEvent(event);
}

