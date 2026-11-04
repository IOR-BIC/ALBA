/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPERPicker
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaInteractorPERPicker.h"
#include "albaPickHelper.h"
#include "vtkALBASmartPointer.h"

#include "albaView.h"
#include "albaVME.h"
#include "albaObject.h"
#include "albaMatrix.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaAvatar.h"
#include "albaEventInteraction.h"
#include "albaRWIBase.h"

#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkIntArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkCellPicker.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorPERPicker)
//------------------------------------------------------------------------------
albaInteractorPERPicker::albaInteractorPERPicker() :
	albaInteractorPER()
//------------------------------------------------------------------------------
{
	m_MovedAfterMouseDown = false;
}

//------------------------------------------------------------------------------
albaInteractorPERPicker::~albaInteractorPERPicker()
//------------------------------------------------------------------------------
{}

//----------------------------------------------------------------------------
void albaInteractorPERPicker::OnLeftButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
	// a click without dragging picks a VME (mouse only)
	albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast((albaDevice*)e->GetSender());
	if (m_DraggingLeft && !m_MovedAfterMouseDown && mouse)
	{
		double mouse_pos[2];
		e->Get2DPosition(mouse_pos);
		SendPickingInformation(mouse->GetView(), mouse_pos);
	}

	m_MovedAfterMouseDown = false;

	Superclass::OnButtonUp(e);

}

//----------------------------------------------------------------------------
void albaInteractorPERPicker::OnMove(albaEventInteraction *e)
{
	if (m_DraggingLeft)
		m_MovedAfterMouseDown = true;
	Superclass::OnMove(e);
}

//----------------------------------------------------------------------------
void albaInteractorPERPicker::SendPickingInformation(albaView *v, double *mouse_pos, int msg_id, albaMatrix *tracker_pos, bool mouse_flag)
//----------------------------------------------------------------------------
{
	// gizmos are not picked
	double pos_picked[3], scalar_value;
	albaVME *pickedVME;
	vtkIdType pid;
	if (albaPickHelper::PickPoint(v, mouse_flag ? mouse_pos : NULL, mouse_flag ? NULL : tracker_pos, pos_picked, pickedVME, pid, scalar_value) && !pickedVME->IsA("albaVMEGizmo"))
		SendPickedPointEvent(msg_id, pos_picked, pid, scalar_value, pickedVME);
}

