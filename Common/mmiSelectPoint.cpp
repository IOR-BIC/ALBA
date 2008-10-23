/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmiSelectPoint.cpp,v $
Language:  C++
Date:      $Date: 2008-10-23 09:10:54 $
Version:   $Revision: 1.2.2.1 $
Authors:   Matteo Giacomoni	
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"

#include "mmiSelectPoint.h"
#include "mmdMouse.h"
#include "mafAvatar3D.h"
#include "mafInteractor.h"
#include "mafRWIBase.h"
#include "mafEventInteraction.h"
#include "mafEvent.h"

#include "vtkAbstractPicker.h"
#include "vtkPointPicker.h"
#include "vtkPoints.h"
#include "vtkMath.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkTransform.h"

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
	if (e->GetModifier(MAF_CTRL_KEY) && m_UseCtrlModifier) 
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

	mmdMouse *mouse = mmdMouse::SafeDownCast(device);
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
	// if we are in pick modality...
	/*if (m_IsPicking)
	{
		// is this an interaction event?
		mafEventInteraction* eventInteraction = NULL;
		eventInteraction = mafEventInteraction::SafeDownCast(event);

		// if yes handle the picking
		if (eventInteraction != NULL)
		{
			// is it coming from the mouse?
			if (mmdMouse *mouse=mmdMouse::SafeDownCast((mafDevice *)eventInteraction->GetSender()))
			{ 
				PickCell(mouse);
			}
			else
			{
				mafLogMessage("only handling events from the mouse!more code is required in order to handle this device!");
				assert(false);
			} 
		}
	} */

	Superclass::OnEvent(event);
}

