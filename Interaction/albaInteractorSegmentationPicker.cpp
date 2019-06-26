/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorSegmentationPicker
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include "albaInteractorSegmentationPicker.h"

#include "albaView.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaAvatar3D.h"

#include "albaEventInteraction.h"

#include "albaRWIBase.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaTransform.h"

#include "vtkALBASmartPointer.h"
#include "vtkCellPicker.h"
#include "vtkALBARayCast3DPicker.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorSegmentationPicker)

//------------------------------------------------------------------------------
albaInteractorSegmentationPicker::albaInteractorSegmentationPicker()
{
  m_IsPicking = false;
}
//------------------------------------------------------------------------------
albaInteractorSegmentationPicker::~albaInteractorSegmentationPicker()
{
}
//----------------------------------------------------------------------------
void albaInteractorSegmentationPicker::OnLeftButtonDown(albaEventInteraction *e)
{
  Superclass::OnLeftButtonDown(e);
	albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)e->GetSender());

	if (mouse && e->GetButton() == ALBA_LEFT_BUTTON)
  { 
		double mouse_pos[2];
		e->Get2DPosition(mouse_pos);

    if (e->GetModifier(ALBA_CTRL_KEY))
	    SendPickingInformation(mouse->GetView(), mouse_pos,ALBA_CTRL_KEY, VME_PICKED);
		else if (e->GetModifier(ALBA_ALT_KEY))
      SendPickingInformation(mouse->GetView(), mouse_pos,ALBA_ALT_KEY, VME_PICKED);
		else 
			SendPickingInformation(mouse->GetView(), mouse_pos, 0, VME_PICKED);
	}
  m_IsPicking = true;
}
//----------------------------------------------------------------------------
void albaInteractorSegmentationPicker::OnLeftButtonUp() 
{
  Superclass::OnLeftButtonUp();
	albaEventMacro(albaEvent(this,MOUSE_UP));
  m_IsPicking = false;
}
//----------------------------------------------------------------------------
void albaInteractorSegmentationPicker::SendPickingInformation(albaView *v, double *mouse_pos,long modifier, int eventId)
{
  bool picked_something = false;

  vtkCellPicker *cellPicker;
  vtkNEW(cellPicker);
  cellPicker->SetTolerance(0.001);
  if (v)
  {
    vtkRendererCollection *rc = v->GetRWI()->GetRenderWindow()->GetRenderers();
    vtkRenderer *r = NULL;
    rc->InitTraversal();
    while(r = rc->GetNextItem())
    {
      if(cellPicker->Pick(mouse_pos[0],mouse_pos[1],0,r))
      {
        picked_something = true;
				break;
      }
    }
		if (picked_something)
		{
			cellPicker->GetPickPosition(m_PickPosition);
			albaVME *pickedVME = v->GetPickedVme();
			if (pickedVME)
			{
				albaEvent pickEvent(this, eventId, modifier);
				pickEvent.SetPointer(m_PickPosition);
				albaEventMacro(pickEvent);
			}
		}
  }
  vtkDEL(cellPicker);
}

//------------------------------------------------------------------------------
void albaInteractorSegmentationPicker::OnEvent(albaEventBase *event)
{
	Superclass::OnEvent(event);
	albaEventInteraction *e = (albaEventInteraction *)event;

	if (m_IsPicking)
	{
		if (albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)event->GetSender()))
		{
			double mouse_pos[2];
			e->Get2DPosition(mouse_pos);
			SendPickingInformation(mouse->GetView(), mouse_pos,0, VME_PICKING);
		}
	}
}