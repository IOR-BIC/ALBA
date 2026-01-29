/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaInteractor2DMeasure.h"

#include "albaDecl.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaEventInteraction.h"
#include "albaRWI.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaView.h"

#include "vtkALBATextActorMeter.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkObjectFactory.h"
#include "vtkPointSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkMath.h"
#include "vtkLine.h"
#include "albaVect3d.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DMeasure)

//----------------------------------------------------------------------------
albaInteractor2DMeasure::albaInteractor2DMeasure()
{
	m_Coordinate = vtkCoordinate::New();
	m_Coordinate->SetCoordinateSystemToWorld();

	m_Mouse = NULL;
	m_Renderer = NULL;
	m_CurrentRenderer = NULL;
	m_AllRenderersVector.clear();
	m_View = NULL;
	m_CurrentRwi = NULL;
	m_CurrPlane = 0; // XY;

	m_ParallelView = false;				// Set on InitRenderer
	m_ParallelScale_OnStart = -1; // Set on InitRenderer

	m_Bounds = NULL;
	m_IsInBound = false;
	m_ButtonDownInside = false;

	m_TextSide = 1;

	m_PointSize = 5.0;
	m_LineWidth = 2.0;

	m_Measure2DVector.clear();
	m_MaxMeasures = -1; // -1 = Infinite
	m_CurrMeasure = -1;

	m_DraggingLeft = false;
	m_EndMeasure = false;

	m_AddMeasureEnable = true;
	m_EditMeasureEnable = true; // Enable FindAndHighlight
	m_MoveMeasureEnable = true;
	m_MovingMeasure = false;

	m_IsEnabled = true;

	m_ShowText = false;

	m_Action = ACTION_NONE;

	m_AddMeasurePhase_Counter = 0;

	m_MeasureValue = 0;
	
	SetColorDefault(1.0, 0.0, 1.0);
	SetColorSelection(0.0, 1.0, 0.0);
	SetColorEdit(1.0, 1.0, 0.0, 1.0); // yellow
	SetColorDisable(1.0, 0.0, 0.0, 0.3);
	SetColorText(1.0, 0.0, 0.0, 0.3);
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure::~albaInteractor2DMeasure()
{
	SetAction(ACTION_NONE);

	vtkDEL(m_Coordinate);

	RemoveAllMeasures();
	Render();

	m_TextActorVector.clear();
	m_Measure2DVector.clear();
	m_AllRenderersVector.clear();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure::InitRenderer(albaEventInteraction *e)
{
	albaEventMacro(albaEvent(this, CAMERA_UPDATE));

		if (m_Mouse == NULL)
		{
			albaDevice *device = albaDevice::SafeDownCast((albaDevice*)e->GetSender());
			albaDeviceButtonsPadMouse  *mouse = albaDeviceButtonsPadMouse::SafeDownCast(device);
			m_Mouse = mouse;
		}

		albaRWIBase * mouseRWI = m_Mouse->GetRWI();

		if (m_CurrentRwi == mouseRWI)
			return;

		m_CurrentRwi = mouseRWI;
		m_Renderer = m_Mouse->GetRenderer();

		double *normal = m_Renderer->GetActiveCamera()->GetViewPlaneNormal();
		m_ViewPlaneNormal[X] = normal[X];
		m_ViewPlaneNormal[Y] = normal[Y];
		m_ViewPlaneNormal[Z] = normal[Z];

		if (m_ViewPlaneNormal[X] != 0) m_CurrPlane = 1;// YZ;
		if (m_ViewPlaneNormal[Y] != 0) m_CurrPlane = 2;// XZ;
		if (m_ViewPlaneNormal[Z] != 0) m_CurrPlane = 0;// XY;

		//albaLogMessage("ViewPlaneNormal (%.2f, %.2f, %.2f)", m_ViewPlaneNormal[X], m_ViewPlaneNormal[Y], m_ViewPlaneNormal[Z]);

		if (m_Renderer->GetLayer() != 1)//Frontal Render
		{
			vtkRendererCollection *rc = m_Mouse->GetRenderer()->GetRenderWindow()->GetRenderers();

			// Searching for a frontal renderer on render collection
			if (rc)
			{
				rc->InitTraversal();
				vtkRenderer *ren;
				while (ren = rc->GetNextItem())
					if (ren->GetLayer() == 1) //Frontal Render
					{
						m_Renderer = ren;
						break;
					}
			}
		}

	if (m_Renderer)
	{
		m_ParallelView = m_Renderer->GetActiveCamera()->GetParallelProjection() != 0;

		if (m_ParallelScale_OnStart == -1) // Save current Parallel Scale
			m_ParallelScale_OnStart = m_Renderer->GetActiveCamera()->GetParallelScale();

		// Add Renderer to Vector
		bool newRenderer = true;
		for (int i = 0; i < m_AllRenderersVector.size(); i++)
		{
			if (m_AllRenderersVector[i] == m_Renderer)
			{
				newRenderer = false;
				break;
			}
		}
		if (newRenderer) m_AllRenderersVector.push_back(m_Renderer);
	}
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure::Render()
{
	for (int i = 0; i < m_AllRenderersVector.size(); i++)
	{
		if (m_AllRenderersVector[i])
			m_AllRenderersVector[i]->GetRenderWindow()->Render();
	}

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}


//----------------------------------------------------------------------------
void albaInteractor2DMeasure::SetUpdateDistance(double dist)
{
	m_PointUpdateDist = dist;
	m_PointUpdateDist2 = (dist * dist);
}

/// MOUSE EVENTS /////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::OnLeftButtonDown(albaEventInteraction *e)
{
	albaEventMacro(albaEvent(this, CAMERA_UPDATE));

	InitRenderer(e);

	m_CurrentRenderer = m_Renderer;

	m_ShiftPressed = e->GetModifier(ALBA_SHIFT_KEY) ? 1 : 0;
	m_AltPressed = e->GetModifier(ALBA_ALT_KEY) ? 1 : 0;
	m_ButtonDownInside = m_IsInBound;

	if (m_ShiftPressed)
	{
		Superclass::OnLeftButtonDown(e);
	}
	else if (m_ParallelView && m_IsEnabled)// && m_IsInBound)
	{
		OnButtonDown(e);
		
		albaVME *picked_vme = GetPickedVME(m_Mouse);
		if (picked_vme && picked_vme->IsA("albaVMEGizmo"))
		{
			return;
		}

		m_DraggingLeft = true;

		double pos_2d[2], pointCoord[3];
		e->Get2DPosition(pos_2d);
		ScreenToWorld(pos_2d, pointCoord);

		albaEventMacro(albaEvent(this, ID_MEASURE_STARTED));

		switch (m_Action)
		{
		case ACTION_ADD_MEASURE:
		{
			DrawNewMeasure(pointCoord);
		}
		break;
		case ACTION_EDIT_MEASURE:
		{
			EditMeasure(m_CurrMeasure, pointCoord);
		}
		break;
		case ACTION_MOVE_MEASURE:
		{
			m_StartMousePosition[X] = pointCoord[X];
			m_StartMousePosition[Y] = pointCoord[Y];
			m_StartMousePosition[Z] = pointCoord[Z];

			MoveMeasure(m_CurrMeasure, m_StartMousePosition);
		}
		break;
		}
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::OnLeftButtonUp(albaEventInteraction *e)
{
	if (!m_IsEnabled) return;

	if (m_CurrentRenderer != m_Renderer) return;

	m_DraggingLeft = false;
	OnButtonUp(e);

	albaVME *picked_vme = GetPickedVME(m_Mouse);
	if (picked_vme && picked_vme->IsA("albaVMEGizmo"))
	{
		return;
	}

	if (m_ShiftPressed)
	{
		Superclass::OnLeftButtonUp(e);
	}
	else if(m_ParallelView)// && m_IsInBound && m_ButtonDownInside)
	{
		double pos_2d[2];
		double pointCoord[4];
		e->Get2DPosition(pos_2d);
		ScreenToWorld(pos_2d, pointCoord);

		switch (m_Action)
		{
		case ACTION_ADD_MEASURE:
		{
			DrawNewMeasure(pointCoord);
		}
		break;
		case ACTION_EDIT_MEASURE:
		{
			EditMeasure(m_CurrMeasure, pointCoord);
		}
		break;
		case ACTION_MOVE_MEASURE:
		{
			MoveMeasure(m_CurrMeasure, pointCoord);
		}
		break;
		}

		m_EndMeasure = true;
		m_MovingMeasure = false;

		FindAndHighlight(pointCoord);

		albaEventMacro(albaEvent(this, ID_MEASURE_FINISHED));
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::OnRightButtonUp(albaEventInteraction *e)
{
	if (m_ShiftPressed)
	{
		Superclass::OnRightButtonUp(e);
	}
	else if(m_CurrMeasure >= 0)
	{
		albaEventMacro(albaEvent(this, ID_MEASURE_RCLICK));
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::OnMove(albaEventInteraction *e)
{
	if (!m_IsEnabled) return;

	InitRenderer(e);

	m_ShiftPressed = e->GetModifier(ALBA_SHIFT_KEY) ? 1 : 0;
	m_AltPressed = e->GetModifier(ALBA_ALT_KEY) ? 1 : 0;

	if (m_ShiftPressed)
	{
		Superclass::OnMove(e);
	}
	else if(m_ParallelView)
	{
		double pos_2d[2];
		e->Get2DPosition(pos_2d);
		double pointCoord[3];
		ScreenToWorld(pos_2d, pointCoord);

		if (m_IsInBound != IsInBound(pointCoord))
		{
			if (m_Renderer)
				m_Renderer->GetRenderWindow()->Render();
		}

		if (!m_DraggingLeft)
		{
			FindAndHighlight(pointCoord);
		}
		else if (m_CurrentRenderer == m_Renderer)
		{
			switch (m_Action)
			{
			case ACTION_ADD_MEASURE:
			{
				DrawNewMeasure(pointCoord);
			}
			break;
			case ACTION_EDIT_MEASURE:
			{
				EditMeasure(m_CurrMeasure, pointCoord);
			}
			break;
			case ACTION_MOVE_MEASURE:
			{
				MoveMeasure(m_CurrMeasure, pointCoord);
			}
			break;
			}
		}
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::OnEvent(albaEventBase *event)
{
	if (!m_IsEnabled) return;

	if (event->GetChannel() == MCH_INPUT)
	{
		albaID id = event->GetId();

		if (id == albaDeviceButtonsPadTracker::GetTracker3DMoveId() || id == albaDeviceButtonsPadMouse::GetMouse2DMoveId())
		{
			albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);

			OnMove(e);
		}
	}

	Superclass::OnEvent(event);
}

/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::AddMeasure(double *point1, double *point2 /*= NULL*/)
{
	if (point1 == NULL) return;

	while (m_MaxMeasures > 0 && (GetMeasureCount() >= m_MaxMeasures))
		RemoveMeasure(GetMeasureCount() - 1);

	//////////////////////////////////////////////////////////////////////////
	// Add Measure
	albaString text;
	text.Printf("Point (%.2f, %.2f, %.2f)", point1[X], point1[Y], point1[Z]);

	Measure2D newMeasure;
	newMeasure.Active = true;
	newMeasure.MeasureType = m_MeasureTypeText;
	newMeasure.Text = text;
	newMeasure.Label = "";
	newMeasure.Renderer = m_CurrentRenderer;
	newMeasure.Rwi = m_CurrentRwi;

	m_Measure2DVector.push_back(newMeasure);

	//////////////////////////////////////////////////////////////////////////
	// Add Text
	m_TextActorVector.push_back(vtkALBATextActorMeter::New());
	m_Renderer->AddActor2D(m_TextActorVector[m_TextActorVector.size() - 1]);

	UpdateTextActor(m_TextActorVector.size() - 1, point1);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::RemoveMeasure(int index)
{
	if (index >= 0 && index < GetMeasureCount() && m_Measure2DVector[index].Renderer)
	{
		//////////////////////////////////////////////////////////////////////////
		// Remove Text
		m_Measure2DVector[index].Renderer->RemoveActor2D(m_TextActorVector[index]);
		vtkDEL(m_TextActorVector[index]);
		m_TextActorVector.erase(m_TextActorVector.begin() + index);

		//////////////////////////////////////////////////////////////////////////
		// Remove Measure
		m_Measure2DVector.erase(m_Measure2DVector.begin() + index);
	}
}
//---------------------------------------------------------------------------
void albaInteractor2DMeasure::RemoveAllMeasures()
{
	int nMeasures = GetMeasureCount();
	for (int i = nMeasures; i >= 0; i--)
		RemoveMeasure(i);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::SelectMeasure(int index)
{
	if (index != m_CurrMeasure && index < m_Measure2DVector.size())
	{
		m_CurrMeasure = index;
		m_LastEditing = -1;

		if(index >= 0)
		m_Renderer = m_CurrentRenderer = m_Measure2DVector[index].Renderer;

		Update();
		Render();
		albaEventMacro(albaEvent(this, ID_MEASURE_SELECTED));
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::ActivateMeasure(int index, bool activate)
{
	if (index >= 0 && index < m_Measure2DVector.size())
		m_Measure2DVector[index].Active = activate;

	if (index < 0) // All Measures
		for (int i = 0; i < m_Measure2DVector.size(); i++)
			m_Measure2DVector[i].Active = activate;

	Update();
}

/// GET-SET /////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
double albaInteractor2DMeasure::GetMeasureValue(int index)
{
	if (index >= 0 && index < m_Measure2DVector.size())
		return m_Measure2DVector[index].Value;

	return 0.0;
}
//---------------------------------------------------------------------------
albaString albaInteractor2DMeasure::GetMeasureText(int index)
{
	if (index >= 0 && index < m_Measure2DVector.size())
		return m_Measure2DVector[index].Text;

	return "No Measure";
}
//---------------------------------------------------------------------------
albaString albaInteractor2DMeasure::GetMeasureLabel(int index)
{
	if (index >= 0 && index < m_Measure2DVector.size())
		return m_Measure2DVector[index].Label;

	return "No Label";
}
//---------------------------------------------------------------------------
void albaInteractor2DMeasure::SetMeasureLabel(int index, albaString text)
{
	if (index >= 0 && index < m_Measure2DVector.size())
		m_Measure2DVector[index].Label = text;

	Update(index);
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure::iSetColor(int color, double r, double g, double b, double a)
{
	m_Colors[color].R = r;
	m_Colors[color].G = g;
	m_Colors[color].B = b;
	m_Colors[color].Alpha = a;
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure::ShowText(bool show)
{
	m_ShowText = show;
	for (int i = 0; i < m_TextActorVector.size(); i++)
		ShowText(i, show);

	Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::ShowText(int measure, bool show)
{
	Color color = m_Colors[COLOR_TEXT];

	m_TextActorVector[measure]->SetColor(color.R, color.G, color.B);
	m_TextActorVector[measure]->SetOpacity(color.Alpha);
	m_TextActorVector[measure]->SetVisibility(show);
	m_Measure2DVector[measure].Renderer->Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure::SetAction(MEASURE_ACTIONS action)
{
	if (m_Action == action)
		return;

	m_Action = action;

	if (m_View)
	{
		// Set Mouse Cursor
		wxCursor cursor = wxCursor(wxCURSOR_ARROW);

		switch (m_Action)
		{
		case ACTION_ADD_MEASURE:
			cursor = wxCursor(wxCURSOR_PENCIL);
			break;
		case ACTION_EDIT_MEASURE:
			cursor = wxCursor(wxCURSOR_HAND);
			break;
		case ACTION_MOVE_MEASURE:
			cursor = wxCursor(wxCURSOR_SIZING);
			break;
		}

		m_View->GetWindow()->SetCursor(cursor);
		Update();
	}
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure::SetColor(vtkActor2D *actor, Color *color)
{
	actor->GetProperty()->SetColor(color->R, color->G, color->B);
	actor->GetProperty()->SetOpacity(color->Alpha);
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure::SetRendererByView(albaView * view)
{
	m_View = view;

	vtkRenderer *newRenderer = NULL;

	vtkRendererCollection *rc;
	rc = view->GetRWI()->GetRenderWindow()->GetRenderers();
		
	if (rc) // Searching for a frontal renderer on render collection
	{
		rc->InitTraversal();
		vtkRenderer *ren;
		while (ren = rc->GetNextItem())
			if (ren->GetLayer() == 1)//Frontal Render
			{
				newRenderer = ren;
				break;
			}
	}

	//if the renderer is changed we move all actor to new renderer 
	if (m_Renderer != newRenderer)
	{
		// Removing from old renderer
		if (m_Renderer)
		{
			RemoveAllMeasures();
		}
		// Adding to new renderer
		if (newRenderer)
		{
			// 			double p1[3];
			// 			double p2[3];
			// 
			// 			for (int i = 0; i < m_LineSourceVector.size(); i++)
			// 			{
			// 				m_LineSourceVector[i]->GetPoint1(p1);
			// 				m_LineSourceVector[i]->GetPoint2(p2);
			// 				AddMeasure(p1, p2);
			// 			}
		}
	}

	m_Renderer = newRenderer;
	m_CurrentRenderer = newRenderer;
	m_Renderer->GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure::UpdateTextActor(int index, double *text_pos)
{
	if (index > -1)
	{
		albaString text = GetMeasureLabel(index);

		if (text.IsEmpty())
			text = GetMeasureText(index);

		m_TextActorVector[index]->SetText(text);
		m_TextActorVector[index]->SetTextPosition(text_pos);

		Color color = m_Colors[(m_CurrMeasure == index) ? COLOR_SELECTION : COLOR_TEXT];
		if (!m_Measure2DVector[index].Active) color = m_Colors[COLOR_DISABLE];

		m_TextActorVector[index]->SetColor(color.R, color.G, color.B);
		m_TextActorVector[index]->SetOpacity(color.Alpha);
		m_TextActorVector[index]->SetVisibility(m_ShowText);
	}
}

// UTILS /////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
bool albaInteractor2DMeasure::IsInBound(double *pos)
{
	double bounds[6];
	m_Renderer->ComputeVisiblePropBounds(bounds);
	m_Bounds = bounds;

	m_IsInBound = false;

	if (m_Bounds)
	{
		if (pos[X] > m_Bounds[0] && pos[X] < m_Bounds[1]) // MarginLeft & MarginRight
			if (pos[Y] > m_Bounds[2] && pos[Y] < m_Bounds[3]) // MarginUp & MarginDown
				if (pos[Z] > m_Bounds[4] && pos[Z] < m_Bounds[5]) // MarginUp & MarginDown
				m_IsInBound = true;
	}

	return m_IsInBound;
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::ScreenToWorld(double screen[2], double world[3])
{
	double wp[4];

	m_Renderer->SetDisplayPoint(screen[X], screen[Y], 0);
	m_Renderer->DisplayToWorld();
	m_Renderer->GetWorldPoint(wp);

	world[X] = wp[X];
	world[Y] = wp[Y];
	world[Z] = wp[Z];

	//This is an hack to stabilize the Interactor, VTK uses the camera perspective matrix to calculate the
	//world coordinate, this return may lead in changes of the view plane coordinate output after show or hide 
	//stuffs.
	//This hack fix the plane coordinate to zero, witch is correct for a 2d measure

	if (m_CurrPlane == 2) world[1] = 0;
	else if (m_CurrPlane == 1) world[0] = 0;
	else world[2] = 0;
}

//----------------------------------------------------------------------------
double albaInteractor2DMeasure::PixelSizeInWorld()
{
	double p1[2] = { 0,0 };
	double p2[2] = { 0,1 };
	albaVect3d w1, w2;

	ScreenToWorld(p1, w1.GetVect());
	ScreenToWorld(p2, w2.GetVect());

	return w1.Distance(w2);
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure::WorldToScreen(double world[3], double screen[2])
{
	double scr[3];

	m_Renderer->SetWorldPoint(world);
	m_Renderer->WorldToDisplay();
	m_Renderer->GetDisplayPoint(scr);

	screen[X] = scr[X];
	screen[Y] = scr[Y];

	//albaLogMessage("WtoS (%f, %f, %f) -> (%f, %f) %f", world[X], world[Y], world[Z], screen[X], screen[Y], scr[Z]);
}

//----------------------------------------------------------------------------
vtkPointSource * albaInteractor2DMeasure::GetNewPointSource()
{
	vtkPointSource *ps = vtkPointSource::New();
	ps->SetNumberOfPoints(1);
	ps->SetRadius(0);

	return ps;
}


//----------------------------------------------------------------------------
double albaInteractor2DMeasure::DistanceBetweenPoints(double *point1, double *point2)
{
	return albaGeometryUtils::DistanceBetweenPoints(point1, point2);
}
//----------------------------------------------------------------------------
double albaInteractor2DMeasure::DistancePointToLine(double * point, double * lineP1, double * lineP2)
{
	return albaGeometryUtils::DistancePointToLine(point, lineP1, lineP2, m_CurrPlane);
}
//---------------------------------------------------------------------------
void albaInteractor2DMeasure::GetMidPoint(double(&midPoint)[3], double *point1, double *point2)
{
	albaGeometryUtils::GetMidPoint(midPoint, point1, point2);
}
//----------------------------------------------------------------------------
bool albaInteractor2DMeasure::FindPointOnLine(double(&point)[3], double *linePoint1, double *linePoint2, double distance)
{
	return albaGeometryUtils::FindPointOnLine(point, linePoint1, linePoint2, distance, m_CurrPlane);
}
//----------------------------------------------------------------------------
double albaInteractor2DMeasure::GetAngle(double* point1, double* point2, double* origin)
{
	return albaGeometryUtils::GetAngle(point1, point2, origin, m_CurrPlane);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::RotatePoint(double *point, double *origin, double angle)
{
	albaGeometryUtils::RotatePoint(point, origin, angle, m_CurrPlane);
}
//----------------------------------------------------------------------------
int albaInteractor2DMeasure::PointUpDownLine(double *point, double *lp1, double *lp2)
{
	return albaGeometryUtils::PointUpDownLine(point, lp1, lp2, m_CurrPlane);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::GetParallelLine(double(&point1)[3], double(&point2)[3], double *linePoint1, double *linePoint2, double distance)
{
	albaGeometryUtils::GetParallelLine(point1, point2, linePoint1, linePoint2, distance, m_CurrPlane);
}
