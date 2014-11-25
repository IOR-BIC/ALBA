/*=========================================================================

 Program: MAF2Medical
 Module: mmiSelectPoint
 Authors: Matteo Giacomoni , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"

#include "mmiSelectPoint.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafInteractor.h"
#include "mafRWIBase.h"
#include "mafEventInteraction.h"
#include "mafEvent.h"

#include "vtkAbstractPicker.h"
#include "vtkPointPicker.h"
#include "vtkPoints.h"

#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmiSelectPoint)
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
void mmiSelectPoint::OnLeftButtonDown(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
  if (e == NULL)
  {
    return;
  }
	else if (e->GetModifier(MAF_CTRL_KEY) && m_UseCtrlModifier) 
	{
		// picking mode on
		m_IsPicking = true;

		// perform picking
		PickCell((mafDevice *)e->GetSender());
	}
  else if (!m_UseCtrlModifier)
  {
    // picking mode on
    m_IsPicking = true;

    // perform picking
    PickCell((mafDevice *)e->GetSender());
  }
	else
	{
		m_IsPicking = false;
		Superclass::OnLeftButtonDown(e);
	}
}
//----------------------------------------------------------------------------
void mmiSelectPoint::OnButtonUp(mafEventInteraction *e)
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
void mmiSelectPoint::PickCell( mafDevice *device )
//----------------------------------------------------------------------------
{
	int x = m_LastMousePose[0];
	int y = m_LastMousePose[1];

	mafDeviceButtonsPadMouse *mouse = mafDeviceButtonsPadMouse::SafeDownCast(device);
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
			mafEventMacro(mafEvent(this,VME_PICKED,(vtkObject *)pickedPoint,pointPicker->GetPointId()));
			pickedPoint->Delete();

			//  wxString msg = "picked something";
			//  int res = wxMessageBox(msg,"debug info", wxOK, NULL);

		}

		vtkDEL(pointPicker);
	}
}
//----------------------------------------------------------------------------
void mmiSelectPoint::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
	Superclass::OnEvent(event);
}

