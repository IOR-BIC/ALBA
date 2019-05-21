/*=========================================================================

 Program: MAF2
 Module: mafInteractorSegmentationPicker
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "mafInteractorSegmentationPicker.h"

#include "mafView.h"
#include "mafDeviceButtonsPadTracker.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafAvatar3D.h"

#include "mafEventInteraction.h"

#include "mafRWIBase.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafTransform.h"

#include "vtkMAFSmartPointer.h"
#include "vtkCellPicker.h"
#include "vtkMAFRayCast3DPicker.h"
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
mafCxxTypeMacro(mafInteractorSegmentationPicker)

//------------------------------------------------------------------------------
mafInteractorSegmentationPicker::mafInteractorSegmentationPicker()
{
  m_IsPicking = false;
}
//------------------------------------------------------------------------------
mafInteractorSegmentationPicker::~mafInteractorSegmentationPicker()
{
}
//----------------------------------------------------------------------------
void mafInteractorSegmentationPicker::OnLeftButtonDown(mafEventInteraction *e)
{
  Superclass::OnLeftButtonDown(e);
	mafDeviceButtonsPadMouse *mouse = mafDeviceButtonsPadMouse::SafeDownCast((mafDevice *)e->GetSender());

	if (mouse && e->GetButton() == MAF_LEFT_BUTTON)
  { 
		double mouse_pos[2];
		e->Get2DPosition(mouse_pos);

    if (e->GetModifier(MAF_CTRL_KEY))
	    SendPickingInformation(mouse->GetView(), mouse_pos,MAF_CTRL_KEY, VME_PICKED);
		else if (e->GetModifier(MAF_ALT_KEY))
      SendPickingInformation(mouse->GetView(), mouse_pos,MAF_ALT_KEY, VME_PICKED);
		else 
			SendPickingInformation(mouse->GetView(), mouse_pos, 0, VME_PICKED);
	}
  m_IsPicking = true;
}
//----------------------------------------------------------------------------
void mafInteractorSegmentationPicker::OnLeftButtonUp() 
{
  Superclass::OnLeftButtonUp();
	mafEventMacro(mafEvent(this,MOUSE_UP));
  m_IsPicking = false;
}
//----------------------------------------------------------------------------
void mafInteractorSegmentationPicker::SendPickingInformation(mafView *v, double *mouse_pos,long modifier, int eventId)
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
			mafVME *pickedVME = v->GetPickedVme();
			if (pickedVME)
			{
				mafEvent pickEvent(this, eventId, modifier);
				pickEvent.SetPointer(m_PickPosition);
				mafEventMacro(pickEvent);
			}
		}
  }
  vtkDEL(cellPicker);
}

//------------------------------------------------------------------------------
void mafInteractorSegmentationPicker::OnEvent(mafEventBase *event)
{
	Superclass::OnEvent(event);
	mafEventInteraction *e = (mafEventInteraction *)event;

	if (m_IsPicking)
	{
		if (mafDeviceButtonsPadMouse *mouse = mafDeviceButtonsPadMouse::SafeDownCast((mafDevice *)event->GetSender()))
		{
			double mouse_pos[2];
			e->Get2DPosition(mouse_pos);
			SendPickingInformation(mouse->GetView(), mouse_pos,0, VME_PICKING);
		}
	}
}