/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmiSelectPoint
 Authors: Matteo Giacomoni , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"

#include "mmiSelectPoint.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaInteractor.h"
#include "albaRWIBase.h"
#include "albaEventInteraction.h"
#include "albaEvent.h"

#include "vtkAbstractPicker.h"
#include "vtkPointPicker.h"
#include "vtkPoints.h"

#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(mmiSelectPoint)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmiSelectPoint::mmiSelectPoint()
//------------------------------------------------------------------------------
{
	m_IsPicking = false;
  m_UseCtrlModifier = true;
}
//------------------------------------------------------------------------------
mmiSelectPoint::~mmiSelectPoint()
//------------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmiSelectPoint::OnMouseMove() 
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
void mmiSelectPoint::OnLeftButtonDown(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
  if (e == NULL)
  {
    return;
  }
	else if (e->GetModifier(ALBA_CTRL_KEY) && m_UseCtrlModifier) 
	{
		// picking mode on
		m_IsPicking = true;

		// perform picking
		PickCell((albaDevice *)e->GetSender());
	}
  else if (!m_UseCtrlModifier)
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
void mmiSelectPoint::OnButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  if (e == NULL)
  {
    return;
  }

	// reset
	m_IsPicking = false;

	Superclass::OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mmiSelectPoint::PickCell( albaDevice *device )
//----------------------------------------------------------------------------
{
	int x = m_LastMousePose[0];
	int y = m_LastMousePose[1];

	albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast(device);
	if( mouse && m_Renderer)
	{
		double pos_picked[3];

		vtkPointPicker *pointPicker;
		vtkNEW(pointPicker);
		pointPicker->SetTolerance(0.01);

		if(pointPicker->Pick(x,y,0,m_Renderer))
		{
      pointPicker->GetPickPosition(pos_picked);
			vtkPoints *pickedPoint = vtkPoints::New();
			pickedPoint->SetNumberOfPoints(1);
			pickedPoint->SetPoint(0,pos_picked);
			albaEventMacro(albaEvent(this,VME_PICKED,(vtkObject *)pickedPoint,pointPicker->GetPointId()));
			pickedPoint->Delete();

			//  wxString msg = "picked something";
			//  int res = wxMessageBox(msg,"debug info", wxOK, NULL);

		}

		vtkDEL(pointPicker);
	}
}
//----------------------------------------------------------------------------
void mmiSelectPoint::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
	Superclass::OnEvent(event);
}

