/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoCrossRotate
 Authors: Stefano Perticoni
 
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


#include "albaGizmoCrossRotate.h"
#include "albaDecl.h"
#include "albaGUIGizmoRotate.h"
#include "albaSmartPointer.h"

#include "albaInteractorGenericMouse.h"

#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEOutput.h"

#include "vtkTransform.h"
#include "albaGizmoCrossRotateAxis.h"
#include "albaGizmoCrossRotateFan.h"

//----------------------------------------------------------------------------
albaGizmoCrossRotate::albaGizmoCrossRotate(albaVME* input, albaObserver *listener, bool buildGUI , int axis)
//----------------------------------------------------------------------------
{
	axis = Z;
	assert(input);
	m_BuildGUI = buildGUI;
	m_InputVME    = input;
	m_Listener  = listener;

	m_GizmoCrossRotateFan = NULL;
	m_GizmoCrossRotateAxisNS = NULL;
	m_GizmoCrossRotateAxisEW = NULL;
	m_GuiGizmoRotate = NULL;
	m_CircleFanRadius = -1;

	// create the fan and send events to this
	m_GizmoCrossRotateFan = new albaGizmoCrossRotateFan(input, this);
	m_GizmoCrossRotateFan->SetAxis(axis);

	// Create albaGizmoRotateCircle and send events to the corresponding fan
	m_GizmoCrossRotateAxisNS = new albaGizmoCrossRotateAxis(input, m_GizmoCrossRotateFan);
	m_GizmoCrossRotateAxisNS->SetGizmoDirection(albaGizmoCrossRotateAxis::NS);
	m_GizmoCrossRotateAxisNS->SetRotationAxis(axis);
	m_GizmoCrossRotateAxisNS->SetMediator(this);

	m_GizmoCrossRotateAxisEW = new albaGizmoCrossRotateAxis(input, m_GizmoCrossRotateFan);
	m_GizmoCrossRotateAxisEW->SetGizmoDirection(albaGizmoCrossRotateAxis::EW);
	m_GizmoCrossRotateAxisEW->SetRotationAxis(axis);
	m_GizmoCrossRotateAxisEW->SetMediator(this);

	if (m_BuildGUI)
	{
		// create the gizmo gui
		// gui is sending events to this
		m_GuiGizmoRotate = new albaGUIGizmoRotate(this);

		// initialize gizmo gui
		m_GuiGizmoRotate->SetAbsOrientation(m_InputVME->GetOutput()->GetAbsMatrix());
	}
}
//----------------------------------------------------------------------------
albaGizmoCrossRotate::~albaGizmoCrossRotate() 
//----------------------------------------------------------------------------
{
	//Destroy:
	//1 albaGizmoRotateCircle 
	//1 albaGizmoRotateFan

	cppDEL(m_GizmoCrossRotateAxisNS);
	cppDEL(m_GizmoCrossRotateAxisEW);
	cppDEL(m_GizmoCrossRotateFan);
	cppDEL(m_GuiGizmoRotate);
}

//----------------------------------------------------------------------------
void albaGizmoCrossRotate::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	// get the sender
	void *sender = alba_event->GetSender();

	if (sender == m_GizmoCrossRotateFan)
	{
		OnEventGizmoComponents(alba_event); // process events from fans
	}
	else if (sender == m_GuiGizmoRotate)
	{
		OnEventGizmoGui(alba_event); // process events from gui
	}
	else
	{
		// forward to the listener
		albaEventMacro(*alba_event);
	}
}

//----------------------------------------------------------------------------
void albaGizmoCrossRotate::OnEventGizmoComponents(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
			//receiving pose matrices from the fan
		case ID_TRANSFORM:
			{
				void *sender = e->GetSender();
				long arg = e->GetArg();

				// if a gizmo circle has been picked
				if (arg == albaInteractorGenericMouse::MOUSE_DOWN)
				{
					if (sender == m_GizmoCrossRotateFan)
					{
						this->Highlight(true);
					}
				}  
				else if (arg == albaInteractorGenericMouse::MOUSE_MOVE)
				{
					// gizmo mode == local; gizmo is rotating during mouse move events
					if (m_Modality == G_LOCAL)
					{
						// get the old abs pose
						vtkTransform *currTr = vtkTransform::New();
						currTr->PostMultiply();
						currTr->SetMatrix(GetAbsPose()->GetVTKMatrix());
						currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
						currTr->Update();

						albaMatrix newAbsMatr;
						newAbsMatr.DeepCopy(currTr->GetMatrix());
						newAbsMatr.SetTimeStamp(GetAbsPose()->GetTimeStamp());

						// set the new pose to the gizmo
						SetAbsPose(&newAbsMatr, false);
						currTr->Delete();
					}
				}
				else if (arg == albaInteractorGenericMouse::MOUSE_UP)
				{
					this->Highlight(false);
					// gizmo mode == local
					if (m_Modality == G_LOCAL)
					{
						SetAbsPose(GetAbsPose());
					}
				}

				// forward event to the listener ie the operation
				// instanciating the gizmo; the sender is changed to "this" so that the operation can check for
				// the gizmo sending events
				e->SetSender(this);
				albaEventMacro(*e);
			}
			break;
		default:
			{
				albaEventMacro(*e);
			}
		}
	}
}

//----------------------------------------------------------------------------
void albaGizmoCrossRotate::OnEventGizmoGui(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	// process events from the gui
	switch (alba_event->GetId())
	{
	case (albaGUIGizmoRotate::ID_ROTATE_X):
		{
			// receiving abs orientation from gui
			SendTransformMatrixFromGui(alba_event);
		}
		break;
	case (albaGUIGizmoRotate::ID_ROTATE_Y):
		{
			SendTransformMatrixFromGui(alba_event);     
		}
		break;
	case (albaGUIGizmoRotate::ID_ROTATE_Z):
		{
			SendTransformMatrixFromGui(alba_event);     
		}
		break;
	default:
		{
			albaEventMacro(*alba_event);
		}
		break;
	}
}

//----------------------------------------------------------------------------
void albaGizmoCrossRotate::Highlight (bool highlight) 
//----------------------------------------------------------------------------
{
	m_GizmoCrossRotateAxisNS->Highlight(highlight);
	m_GizmoCrossRotateAxisEW->Highlight(highlight);
	m_GizmoCrossRotateFan->Show(highlight);

}

//----------------------------------------------------------------------------  
void albaGizmoCrossRotate::Show(bool show)
//----------------------------------------------------------------------------
{
	// set visibility ivar
	m_Visibility = show;

	m_GizmoCrossRotateAxisNS->Show(show);
	m_GizmoCrossRotateAxisEW->Show(show);
	m_GizmoCrossRotateFan->Show(show);

	// if auxiliary ref sys is different from vme its orientation cannot be changed
	// so gui must not be keyable. Otherwise set gui keyability to show.
	if (m_BuildGUI)
	{
		if (m_RefSysVME == m_InputVME)
		{
			m_GuiGizmoRotate->EnableWidgets(show);
		}
		else
		{
			m_GuiGizmoRotate->EnableWidgets(false);
		}
	}
}

//----------------------------------------------------------------------------  
void albaGizmoCrossRotate::SetAbsPose(albaMatrix *absPose, bool applyPoseToFans)
//----------------------------------------------------------------------------
{
	// remove scaling part from gizmo abs pose; gizmo not scale
	double pos[3] = {0,0,0};
	double orient[3] = {0,0,0};

	albaTransform::GetPosition(*absPose, pos);
	albaTransform::GetOrientation(*absPose, orient);

	albaSmartPointer<albaMatrix> tmpMatr;
	tmpMatr->SetTimeStamp(absPose->GetTimeStamp());
	albaTransform::SetPosition(*tmpMatr.GetPointer(), pos);
	albaTransform::SetOrientation(*tmpMatr.GetPointer(), orient);

	for (int i = 0; i < 3; i++)
	{
		m_GizmoCrossRotateAxisNS->SetAbsPose(tmpMatr);
		m_GizmoCrossRotateAxisEW->SetAbsPose(tmpMatr);
		if (applyPoseToFans == true)
		{
			m_GizmoCrossRotateFan->SetAbsPose(tmpMatr);
		}
	}
	if (m_BuildGUI) m_GuiGizmoRotate->SetAbsOrientation(tmpMatr);
}

//----------------------------------------------------------------------------
albaMatrix *albaGizmoCrossRotate::GetAbsPose()
//----------------------------------------------------------------------------
{
	return m_GizmoCrossRotateAxisNS->GetAbsPose();
}

//----------------------------------------------------------------------------  
void albaGizmoCrossRotate::SetInput(albaVME *input)
//----------------------------------------------------------------------------
{
	m_InputVME = input;

	m_GizmoCrossRotateAxisNS->SetInput(input);
	m_GizmoCrossRotateAxisEW->SetInput(input);
	m_GizmoCrossRotateFan->SetInput(input);

}

//----------------------------------------------------------------------------  
albaInteractorGenericInterface *albaGizmoCrossRotate::GetInteractor(int axis)
//----------------------------------------------------------------------------  
{
	return m_GizmoCrossRotateAxisNS->GetInteractor();
}

//----------------------------------------------------------------------------
void albaGizmoCrossRotate::SendTransformMatrixFromGui(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		// send matrix to be postmultiplied to listener
		//                                                                  -1    
		// [NewAbsPose] = [M]*[OldAbsPose] => [M] = [NewAbsPose][OldAbsPose]

		// build objects
		albaSmartPointer<albaMatrix> M;
		albaMatrix invOldAbsPose;
		albaSmartPointer<albaMatrix> newAbsPose;

		// incoming matrix is a rotation matrix
		newAbsPose->DeepCopy(GetAbsPose());
		// copy rotation from incoming matrix
		albaTransform::CopyRotation(*e->GetMatrix(), *newAbsPose.GetPointer());

		invOldAbsPose.DeepCopy(this->GetAbsPose());
		invOldAbsPose.Invert();

		albaMatrix::Multiply4x4(*newAbsPose.GetPointer(),invOldAbsPose,*M.GetPointer());

		// update gizmo abs pose
		this->SetAbsPose(newAbsPose, true);

		// send transfrom to postmultiply to the listener. Events is sent as a transform event
		SendTransformMatrix(M, ID_TRANSFORM, albaInteractorGenericMouse::MOUSE_MOVE);
	}
}

//----------------------------------------------------------------------------
void albaGizmoCrossRotate::SetRefSys(albaVME *refSys)
//----------------------------------------------------------------------------
{
	assert(m_InputVME);  

	m_RefSysVME = refSys;
	SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());

	if (m_RefSysVME == m_InputVME)
	{
		SetModalityToLocal();

		// if the gizmo is visible set the widgets visibility to true
		// if the refsys is local
		if (m_Visibility == true && m_BuildGUI == true)
		{
			m_GuiGizmoRotate->EnableWidgets(true);
		}
	}
	else
	{
		SetModalityToGlobal();    

		// if the gizmo is visible set the widgets visibility to false
		// if the refsys is global since this refsys cannot be changed
		if (m_Visibility == true && m_BuildGUI == true)
		{
			m_GuiGizmoRotate->EnableWidgets(false);
		}
	}
}
//----------------------------------------------------------------------------
void albaGizmoCrossRotate::SetCircleFanRadius(double radius)
//----------------------------------------------------------------------------
{	
	//  if(m_GRCircle) m_GRCircle->SetRadius(radius);
	if(m_GizmoCrossRotateFan) m_GizmoCrossRotateFan->SetRadius(radius);

	m_CircleFanRadius = radius;
}

albaVME* albaGizmoCrossRotate::GetRefSys()
{
	return m_RefSysVME;
}

double albaGizmoCrossRotate::GetCircleFanRadius()
{
	return m_CircleFanRadius;
}

void albaGizmoCrossRotate::SetAutoscale( bool autoscale )
{
	albaGizmoInterface::SetAutoscale(autoscale);

	m_GizmoCrossRotateFan->SetAutoscale(autoscale);
	m_GizmoCrossRotateAxisNS->SetAutoscale(autoscale);
    m_GizmoCrossRotateAxisEW->SetAutoscale(autoscale);
}

void albaGizmoCrossRotate::SetAlwaysVisible( bool alwaysVisible )
{
	albaGizmoInterface::SetAlwaysVisible(alwaysVisible);

	m_GizmoCrossRotateFan->SetAlwaysVisible(alwaysVisible);
	m_GizmoCrossRotateAxisNS->SetAlwaysVisible(alwaysVisible);
	m_GizmoCrossRotateAxisEW->SetAlwaysVisible(alwaysVisible);
}

void albaGizmoCrossRotate::SetRenderWindowHeightPercentage(double percentage)
{
	albaGizmoInterface::SetRenderWindowHeightPercentage(percentage);

	m_GizmoCrossRotateFan->SetRenderWindowHeightPercentage(percentage);
	m_GizmoCrossRotateAxisNS->SetRenderWindowHeightPercentage(percentage);
	m_GizmoCrossRotateAxisEW->SetRenderWindowHeightPercentage(percentage);
}
