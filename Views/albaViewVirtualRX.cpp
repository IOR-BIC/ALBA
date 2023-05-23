/*=========================================================================
Program: ALBA (Agile Library for Biomedical Applications)
Module: albaViewVirtualRX
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

#include "albaViewVirtualRX.h"
#include "albaViewVTK.h"
#include "albaViewRX.h"
#include "albaViewSlice.h"
#include "albaPipeSlice.h"
#include "albaPipeSurfaceSlice.h"
#include "albaVMEIterator.h"
#include "albaGUILutPreset.h"
#include "albaGUI.h"
#include "albaGUILutSwatch.h"
#include "albaGUILutSlider.h"
#include "albaGizmoSlice.h"
#include "mmaVolumeMaterial.h"
#include "albaVMEVolume.h"
#include "albaVMESurface.h"

#include "vtkDataSet.h"
#include "vtkLookupTable.h"
#include "vtkPoints.h"
#include "albaPipeVolumeOrthoSlice.h"
#include "albaPipeVolumeSliceBlend.h"
#include "albaPipeVolumeProjected.h"
#include "vtkALBAProjectVolume.h"
#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum VTRX_SUBVIEW_ID
{
	RX_FRONT_VIEW = 0,
	RX_SIDE_VIEW,
	VIEWS_NUMBER,
};

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewVirtualRX);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewVirtualRX::albaViewVirtualRX(wxString label)
	: albaViewCompound(label, 1, 3)
{
	m_LutWidget = NULL;
	m_CurrentVolume = NULL;
	m_VolumeSize[0] = m_VolumeSize[1] = m_VolumeSize[2] = 0.0;

	m_LayoutConfiguration = LAYOUT_CUSTOM;

	m_ViewsRX[RX_FRONT_VIEW] = m_ViewsRX[RX_SIDE_VIEW] = NULL;
	m_LutSliders[RX_FRONT_VIEW] = m_LutSliders[RX_SIDE_VIEW] = NULL;
	m_VtkLUT[RX_FRONT_VIEW] = m_VtkLUT[RX_SIDE_VIEW] = NULL;
	
	m_RightOrLeft = 1;
	
	for (int j = GIZMO_XA; j < GIZMOS_NUMBER; j++)
	{
		m_PrjGizmo[j] = NULL;
		m_GizmoPoints[j] = 0.0;
	}

	m_ProjectionRangeGuiSliderX = NULL;
	m_ProjectionRangeGuiSliderY = NULL;
	m_ProjectionRangeGuiSliderZ = NULL;

	m_ProjectionMode = 0;
	m_ProjectionPlane = 0;
}
//----------------------------------------------------------------------------
albaViewVirtualRX::~albaViewVirtualRX()
{
	m_ViewsRX[RX_FRONT_VIEW] = m_ViewsRX[RX_SIDE_VIEW] = NULL;

	for (int i = RX_FRONT_VIEW; i < VIEWS_NUMBER; i++)
	{
		cppDEL(m_LutSliders[i]);
		vtkDEL(m_VtkLUT[i]);
	}
}

//----------------------------------------------------------------------------
char ** albaViewVirtualRX::GetIcon()
{
#include "pic/VIEW_ARBITRARY.xpm"
	return VIEW_ARBITRARY_xpm;
}

//----------------------------------------------------------------------------
albaView *albaViewVirtualRX::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
	m_LightCopyEnabled = lightCopyEnabled;
	albaViewVirtualRX *v = new albaViewVirtualRX(m_Label);
	v->m_Listener = Listener;
	v->m_Id = m_Id;
	for (int i = 0; i<m_PluggedChildViewList.size(); i++)
	{
		v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
	}

	v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
	v->Create();

	return v;
}

//----------------------------------------------------------------------------
void albaViewVirtualRX::VmeShow(albaVME *vme, bool show)
{
	for (int i = 0; i < VIEWS_NUMBER; i++)
		((albaViewRX *)m_ChildViewList[i])->VmeShow(vme, show);

	if (vme->GetOutput()->IsA("albaVMEOutputVolume"))
	{
		if (show)
		{
			// set the range for every slider widget
			for (int childID = RX_FRONT_VIEW; childID < VIEWS_NUMBER; childID++)
			{
				double minMax[2];
				((albaViewRX *)(m_ChildViewList[childID]))->GetLutRange(minMax);

				m_LutSliders[childID]->SetRange(minMax[0], minMax[1]);
				m_LutSliders[childID]->SetSubRange(minMax[0], minMax[1]);

				// create a lookup table for each RX view
				vtkNEW(m_VtkLUT[childID]);
				m_VtkLUT[childID]->SetRange(minMax);
				m_VtkLUT[childID]->Build();
				lutPreset(4, m_VtkLUT[childID]);
				m_VtkLUT[childID]->SetRange(minMax);
				m_VtkLUT[childID]->Build();
				lutPreset(4, m_VtkLUT[childID]);

				((albaViewRX *)m_ChildViewList[childID])->SetLutRange(minMax[0], minMax[1]);
			}

			m_CurrentVolume = vme;

			CreateGizmo();
		}
		else
		{
			m_CurrentVolume = NULL;
			m_VolumeSize[0] = m_VolumeSize[1] = m_VolumeSize[2] = 0.0;

			DestroyGizmo();
		}
	}

	EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void albaViewVirtualRX::VmeRemove(albaVME *vme)
{
	if (m_CurrentVolume && vme == m_CurrentVolume)
	{
		m_CurrentVolume = NULL;
		DestroyGizmo();
	}

	Superclass::VmeRemove(vme);
}

//----------------------------------------------------------------------------
void albaViewVirtualRX::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (alba_event->GetId())
		{
			// events from the slider
		case ID_RANGE_MODIFIED:
		{
			// is the volume visible?
			if (((albaViewSlice *)m_ChildViewList[RX_FRONT_VIEW])->VolumeIsVisible())
			{
				double low, hi;

				// from which lut slider the event is coming?
				if (alba_event->GetSender() == m_LutSliders[RX_FRONT_VIEW])
				{
					m_LutSliders[RX_FRONT_VIEW]->GetSubRange(&low, &hi);
					((albaViewRX *)m_ChildViewList[RX_FRONT_VIEW])->SetLutRange(low, hi);
				}
				else if (alba_event->GetSender() == m_LutSliders[RX_SIDE_VIEW])
				{
					m_LutSliders[RX_SIDE_VIEW]->GetSubRange(&low, &hi);
					((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->SetLutRange(low, hi);
				}


				double min, max;
				if (e->GetSender() == m_ProjectionRangeGuiSliderX)
				{
					m_ProjectionRangeGuiSliderX->GetSubRange(&min, &max);

					UpdateProjection(min, max, RX_SIDE_VIEW, m_ProjectionMode);
					UpdateProjectionGizmos();
				}
				else if (e->GetSender() == m_ProjectionRangeGuiSliderY)
				{
					m_ProjectionRangeGuiSliderY->GetSubRange(&min, &max);

					UpdateProjection(min, max, RX_FRONT_VIEW, m_ProjectionMode);
					UpdateProjectionGizmos();
				}
				else if (e->GetSender() == m_ProjectionRangeGuiSliderZ)
				{
					m_ProjectionRangeGuiSliderZ->GetSubRange(&min, &max);

					UpdateProjection(min, max, 2, m_ProjectionMode);
					UpdateProjectionGizmos();
				}

				GetLogicManager()->CameraUpdate();

				//CameraUpdate();
			}
		}
		break;
		case ID_RIGHT_OR_LEFT:
		{
			if (m_RightOrLeft == 0)
				((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->CameraSet(CAMERA_RX_RIGHT);
			else
				((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->CameraSet(CAMERA_RX_LEFT);
		}
		break;

		case ID_PROJECTION_MODE:
		{
			UpdateProjection(m_GizmoPoints[0], m_GizmoPoints[1], RX_SIDE_VIEW, m_ProjectionMode);
			UpdateProjection(m_GizmoPoints[2], m_GizmoPoints[3], RX_FRONT_VIEW, m_ProjectionMode);
			//UpdateProjection(m_GizmoPoints[4], m_GizmoPoints[5], 2, m_ProjectionMode);
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_PROJECTION_RESET:
		{
			ResetProjection();
		}
		break;

		case MOUSE_UP:
		case MOUSE_MOVE: // Manage MOUSE events
		{			
			OnEventMouseMove(e);
		}
		break;
		default:
			albaViewCompound::OnEvent(alba_event);
		}
	}
	else
	{
		albaViewCompound::OnEvent(alba_event);
	}
}
//----------------------------------------------------------------------------
void albaViewVirtualRX::OnEventMouseMove(albaEvent *e)
{
	// Function
	auto BoundsValidate = [](double *pos, double *bounds)
	{
		for (int i = 0; i < 3; i++)
		{
			if (pos[i] < bounds[i * 2]) pos[i] = bounds[i * 2];
			if (pos[i] > bounds[i * 2 + 1]) pos[i] = bounds[i * 2 + 1];
		}
	};

	vtkPoints *p = (vtkPoints *)e->GetVtkObj();

	if (p == NULL)
		return;

	long gizmoId = e->GetArg();

	// Get point picked
	double gizmoPoint[3];
	p->GetPoint(0, gizmoPoint);
	
	BoundsValidate(gizmoPoint, m_VolumeBounds);
	
	if (gizmoId == 0 || gizmoId == 1) // RX_FRONT_VIEW X
	{
		if (gizmoId == 1 && gizmoPoint[0] < m_GizmoPoints[0]) return; // gizmoId = 0;
		if (gizmoId == 0 && gizmoPoint[0] > m_GizmoPoints[1]) return; // gizmoId = 1;

		m_GizmoPoints[gizmoId] = gizmoPoint[0];

		// Sort Points
		m_GizmoPoints[0] = MIN(m_GizmoPoints[0], m_GizmoPoints[1]);
		m_GizmoPoints[1] = MAX(m_GizmoPoints[0], m_GizmoPoints[1]);

		UpdateProjection(m_GizmoPoints[0], m_GizmoPoints[1], RX_SIDE_VIEW, m_ProjectionMode);
	}
	else if (gizmoId == 2 || gizmoId == 3) // RX_SIDE_VIEW Y
	{
		if (gizmoId == 3 && gizmoPoint[1] < m_GizmoPoints[2]) gizmoId = 2;
		if (gizmoId == 2 && gizmoPoint[1] > m_GizmoPoints[3]) gizmoId = 3;

		m_GizmoPoints[gizmoId] = gizmoPoint[1];

		// Sort Points
		m_GizmoPoints[2] = MIN(m_GizmoPoints[2], m_GizmoPoints[3]);
		m_GizmoPoints[3] = MAX(m_GizmoPoints[2], m_GizmoPoints[3]);

		UpdateProjection(m_GizmoPoints[2], m_GizmoPoints[3], RX_FRONT_VIEW, m_ProjectionMode);
	}
	else if (gizmoId == 4 || gizmoId == 5) // RX_TOP_VIEW Z
	{
		if (gizmoId == 5 && gizmoPoint[2] < m_GizmoPoints[4]) gizmoId = 4;
		if (gizmoId == 4 && gizmoPoint[2] > m_GizmoPoints[5]) gizmoId = 5;

		m_GizmoPoints[gizmoId] = gizmoPoint[2];

		// Sort Points
		m_GizmoPoints[4] = MIN(m_GizmoPoints[4], m_GizmoPoints[5]);
		m_GizmoPoints[5] = MAX(m_GizmoPoints[4], m_GizmoPoints[5]);

		UpdateProjection(m_GizmoPoints[4], m_GizmoPoints[5], 2, m_ProjectionMode);
	}

	UpdateProjectionGui();
	GetLogicManager()->CameraUpdate();
}

//-------------------------------------------------------------------------
albaGUI* albaViewVirtualRX::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

	wxString povChoices[2]{ "Right","Left" };
	m_Gui->Radio(ID_RIGHT_OR_LEFT, "POV", &m_RightOrLeft, 2, povChoices, 2, "Point Of View");

	m_Gui->Divider(1);
	
	m_Gui->Label("Projection");

	m_ProjectionRangeGuiSliderX = NULL;
	m_ProjectionRangeGuiSliderX = new albaGUILutSlider(m_Gui, ID_PROJECTION_RANGE, wxPoint(0, 0), wxSize(-1, 24));
	m_ProjectionRangeGuiSliderX->SetListener(this);
	m_ProjectionRangeGuiSliderX->SetText(1, "Front");
	m_Gui->Add(m_ProjectionRangeGuiSliderX);
	
	m_ProjectionRangeGuiSliderY = NULL;
	m_ProjectionRangeGuiSliderY = new albaGUILutSlider(m_Gui, ID_PROJECTION_RANGE, wxPoint(0, 0), wxSize(-1, 24));
	m_ProjectionRangeGuiSliderY->SetListener(this);
	m_ProjectionRangeGuiSliderY->SetText(1, "Side");
	m_Gui->Add(m_ProjectionRangeGuiSliderY);

	m_ProjectionRangeGuiSliderZ = NULL;
	m_ProjectionRangeGuiSliderZ = new albaGUILutSlider(m_Gui, ID_PROJECTION_RANGE, wxPoint(0, 0), wxSize(-1, 24));
	m_ProjectionRangeGuiSliderZ->SetListener(this);
	m_ProjectionRangeGuiSliderZ->SetText(1, "Top");
	//m_Gui->Add(m_ProjectionRangeGuiSliderZ);

	wxString prjChoices[2]{ "Mean", "Max" };
	m_Gui->Radio(ID_PROJECTION_MODE, "Mode", &m_ProjectionMode, 2, prjChoices, 2, "Projection Modality",1);

	m_Gui->Label("");
	m_Gui->Button(ID_PROJECTION_RESET, "Reset");
	m_Gui->Divider(1);

	//
	EnableWidgets(m_CurrentVolume != NULL);

	for (int i = RX_FRONT_VIEW; i <= RX_SIDE_VIEW; i++)
		((albaViewRX*)m_ChildViewList[i])->GetGui();

	m_Gui->Divider();

	return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewVirtualRX::CreateGuiView()
{
	m_GuiView = new albaGUI(this);
	wxBoxSizer *lutsSizer = new wxBoxSizer(wxHORIZONTAL);

	// Create three windowing widgets
	for (int i = RX_FRONT_VIEW; i < VIEWS_NUMBER; i++)
	{
		m_LutSliders[i] = new albaGUILutSlider(m_GuiView, -1, wxPoint(0, 0), wxSize(10, 24));
		m_LutSliders[i]->SetListener(this);
		m_LutSliders[i]->SetMinSize(wxSize(500, 24));
		lutsSizer->Add(m_LutSliders[i], wxALIGN_CENTER | wxRIGHT);
	}

	m_GuiView->Add(lutsSizer);
	m_GuiView->Reparent(m_Win);
	m_GuiView->FitGui();
	m_GuiView->Update();
}

//----------------------------------------------------------------------------
void albaViewVirtualRX::PackageView()
{
	int cam_pos[2] = { CAMERA_RX_FRONT, CAMERA_RX_LEFT };
	for (int v = RX_FRONT_VIEW; v < VIEWS_NUMBER; v++)
	{
		// Create to the child view
		m_ViewsRX[v] = new albaViewRX("RX child view", cam_pos[v]);
		m_ViewsRX[v]->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeProjected", MUTEX);
		m_ViewsRX[v]->PlugVisualPipe("albaVMELabeledVolume", "albaPipeVolumeProjected", MUTEX);

		SetCanSpin(false);

		PlugChildView(m_ViewsRX[v]);
	}
}
//----------------------------------------------------------------------------
void albaViewVirtualRX::EnableWidgets(bool enable)
{
	if (m_Gui)
	{
		m_Gui->Enable(ID_LUT_WIDGET, enable);

		m_Gui->Enable(ID_RANGE_MODIFIED, enable);
		m_Gui->Enable(ID_PROJECTION_RANGE, enable);
		m_Gui->Enable(ID_PROJECTION_MODE, enable);
		m_Gui->Enable(ID_PROJECTION_RESET, enable);
	}
}

//----------------------------------------------------------------------------
void albaViewVirtualRX::LayoutSubView(int width, int height)
{
	// This implement the Fixed SubViews Layout
	
	int border = 2;
	int x_pos, c, i = 0;
	int nViews = 2;
	int step_width = (width - border) / nViews;

	for (c = 0; c < m_NumOfChildView; c++)
	{
		x_pos = c*(step_width + border);
		m_ChildViewList[i]->GetWindow()->SetSize(x_pos, 0, step_width, height);
		i++;
	}
	wxSize sizeToSend = m_ChildViewList[i - 1]->GetWindow()->GetSize();
	wxSizeEvent event(sizeToSend);
	((albaViewCompound *)m_ChildViewList[i - 1])->OnSize(event);
}

//----------------------------------------------------------------------------
void albaViewVirtualRX::CreateGizmo()
{
	if (m_PrjGizmo[0] || m_PrjGizmo[1] || m_PrjGizmo[2] ||
		m_PrjGizmo[3] || m_PrjGizmo[4] || m_PrjGizmo[5]) DestroyGizmo();
	
	if (m_CurrentVolume)
	{
		vtkDataSet * vtkData = m_CurrentVolume->GetOutput()->GetVTKData();
		vtkData->GetBounds(m_VolumeBounds);
		
		m_VolumeSize[0] = m_VolumeBounds[1] - m_VolumeBounds[0];
		m_VolumeSize[1] = m_VolumeBounds[3] - m_VolumeBounds[2];
		m_VolumeSize[2] = m_VolumeBounds[5] - m_VolumeBounds[4];
	}
		
	// Creates the Gizmos
	for (int gizmoId = GIZMO_XA; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		m_PrjGizmo[gizmoId] = new albaGizmoSlice(m_CurrentVolume, this);
	}
	
	ResetProjection();
	
	ShowGizmo();

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewVirtualRX::DestroyGizmo()
{
	for (int gizmoId = GIZMO_XA; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		if (!m_PrjGizmo[gizmoId]) return;
	}

	// ZN view
	m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_PrjGizmo[0]->GetOutput(), false);
	m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_PrjGizmo[1]->GetOutput(), false);
// 	m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_PrjGizmo[4]->GetOutput(), false);
// 	m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_PrjGizmo[5]->GetOutput(), false);

	// XN view
	m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_PrjGizmo[2]->GetOutput(), false);
	m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_PrjGizmo[3]->GetOutput(), false);
// 	m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_PrjGizmo[4]->GetOutput(), false);
// 	m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_PrjGizmo[5]->GetOutput(), false);

	// YN view
// 	m_ChildViewList[2]->VmeShow(m_PrjGizmo[0]->GetOutput(), false);
// 	m_ChildViewList[2]->VmeShow(m_PrjGizmo[1]->GetOutput(), false);
// 	m_ChildViewList[2]->VmeShow(m_PrjGizmo[4]->GetOutput(), false);
// 	m_ChildViewList[2]->VmeShow(m_PrjGizmo[5]->GetOutput(), false);
	
	for (int gizmoId = GIZMO_XA; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		cppDEL(m_PrjGizmo[gizmoId]);
	}
}
//----------------------------------------------------------------------------
void albaViewVirtualRX::ShowGizmo()
{
	for (int gizmoId = GIZMO_XA; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		if (m_PrjGizmo[gizmoId] == NULL) return;
	}

	double colors[] = { 0.8,0,0, 0.4,0,0, 0,0.8,0, 0,0.4,0, 0,0,0.8, 0,0,0.4 };

	for (int gizmoId = GIZMO_XA; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		m_PrjGizmo[gizmoId]->SetGizmoMovingModalityToBound();
		m_PrjGizmo[gizmoId]->SetColor(&colors[gizmoId * 3]);
	}

	// ZN view
	m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_PrjGizmo[0]->GetOutput(), true);
	m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_PrjGizmo[1]->GetOutput(), true);
// 	m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_PrjGizmo[4]->GetOutput(), true);
// 	m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_PrjGizmo[5]->GetOutput(), true);

	// ZN view
	m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_PrjGizmo[2]->GetOutput(), true);
	m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_PrjGizmo[3]->GetOutput(), true);
// 	m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_PrjGizmo[4]->GetOutput(), true);
// 	m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_PrjGizmo[5]->GetOutput(), true);

	// YN view
// 	m_ChildViewList[2]->VmeShow(m_PrjGizmo[0]->GetOutput(), true);
// 	m_ChildViewList[2]->VmeShow(m_PrjGizmo[1]->GetOutput(), true);
// 	m_ChildViewList[2]->VmeShow(m_PrjGizmo[4]->GetOutput(), true);
// 	m_ChildViewList[2]->VmeShow(m_PrjGizmo[5]->GetOutput(), true);
}

//----------------------------------------------------------------------------
void albaViewVirtualRX::UpdateProjection(double min, double max, int plane, int modality)
{
	m_ProjectionPlane = plane;
	m_ProjectionMode = modality;

	albaPipeVolumeProjected *pipe = albaPipeVolumeProjected::SafeDownCast((m_ChildViewList[m_ProjectionPlane])->GetNodePipe(m_CurrentVolume));
	if (pipe)
	{
		pipe->SetProjectionModality(m_ProjectionMode);

		int range[2] = { ((min - m_VolumeBounds[plane*2]) / m_VolumeSize[plane]) * 512, ((max - m_VolumeBounds[plane*2]) / m_VolumeSize[plane]) * 512 };

		pipe->SetProjectionRange(range);
		pipe->EnableRangeProjection(true);
	}
}
//----------------------------------------------------------------------------
void albaViewVirtualRX::ResetProjection()
{
	double direction[] = { albaGizmoSlice::GIZMO_SLICE_X, albaGizmoSlice::GIZMO_SLICE_Y, albaGizmoSlice::GIZMO_SLICE_Z };

	for (int gizmoId = GIZMO_XA; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		m_PrjGizmo[gizmoId]->UpdateGizmoSliceInLocalPositionOnAxis(gizmoId, direction[gizmoId / 2], m_VolumeBounds[gizmoId]);
		m_GizmoPoints[gizmoId] = m_VolumeBounds[gizmoId];
	}

	UpdateProjection(m_GizmoPoints[0], m_GizmoPoints[1], RX_SIDE_VIEW, m_ProjectionMode);
	UpdateProjection(m_GizmoPoints[2], m_GizmoPoints[3], RX_FRONT_VIEW, m_ProjectionMode);
	//UpdateProjection(m_GizmoPoints[4], m_GizmoPoints[5], 2, m_ProjectionMode);

	GetLogicManager()->CameraUpdate();

	UpdateProjectionGui();
}

//----------------------------------------------------------------------------
void albaViewVirtualRX::UpdateProjectionGui()
{
	if (m_Gui)
	{
		m_ProjectionRangeGuiSliderX->SetRange(m_VolumeBounds[0], m_VolumeBounds[1]);
		m_ProjectionRangeGuiSliderX->SetSubRange(m_GizmoPoints[0], m_GizmoPoints[1]);

		m_ProjectionRangeGuiSliderY->SetRange(m_VolumeBounds[2], m_VolumeBounds[3]);
		m_ProjectionRangeGuiSliderY->SetSubRange(m_GizmoPoints[2], m_GizmoPoints[3]);

		m_ProjectionRangeGuiSliderZ->SetRange(m_VolumeBounds[4], m_VolumeBounds[5]);
		m_ProjectionRangeGuiSliderZ->SetSubRange(m_GizmoPoints[4], m_GizmoPoints[5]);

		m_Gui->Update();
	}
}
//----------------------------------------------------------------------------
void albaViewVirtualRX::UpdateProjectionGizmos()
{
	double direction[] = { albaGizmoSlice::GIZMO_SLICE_X, albaGizmoSlice::GIZMO_SLICE_Y, albaGizmoSlice::GIZMO_SLICE_Z };

	for (int gizmoId = GIZMO_XA; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		m_PrjGizmo[gizmoId]->UpdateGizmoSliceInLocalPositionOnAxis(gizmoId, direction[gizmoId / 2], m_GizmoPoints[gizmoId]);
	}

	GetLogicManager()->CameraUpdate();
}
