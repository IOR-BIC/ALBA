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
#include "vtkPolyDataSource.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkMath.h"
#include "vtkLine.h"

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

	m_LastSelection = -1;
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

		m_CurrentRwi = m_Mouse->GetRWI();
		m_Renderer = m_Mouse->GetRenderer();

		double *normal = m_Renderer->GetActiveCamera()->GetViewPlaneNormal();
		m_ViewPlaneNormal[X] = normal[X];
		m_ViewPlaneNormal[Y] = normal[Y];
		m_ViewPlaneNormal[Z] = normal[Z];

		albaLogMessage("ViewPlaneNormal (%.2f, %.2f, %.2f)", m_ViewPlaneNormal[X], m_ViewPlaneNormal[Y], m_ViewPlaneNormal[Z]);

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
	if (index >= 0 && index < m_Measure2DVector.size())
	{
		m_Renderer = m_CurrentRenderer = m_Measure2DVector[index].Renderer;

		m_LastSelection = index;
		m_LastEditing = -1;

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

		Color color = m_Colors[(m_LastSelection == index) ? COLOR_SELECTION : COLOR_TEXT];
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

	m_Renderer->GetActiveCamera()->SetViewPlaneNormal(0, 0, -1);

	albaLogMessage("StoW (%f, %f) -> (%f, %f, %f, %f)", screen[X], screen[Y], wp[X], wp[Y], wp[Z], wp[3]);
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

/// Geometry Utils

#define VTK_NO_INTERSECTION 0
#define VTK_YES_INTERSECTION  2
#define EPSILON 1e-5;

/// Points

//---------------------------------------------------------------------------
double GeometryUtils::Dot(double *point1, double *point2)
{
	return point1[X] * point2[X] + point1[Y] * point2[Y] + point1[Z] * point2[Z];
}
//---------------------------------------------------------------------------
double GeometryUtils::Mag(double *point1)
{
	return std::sqrt(point1[X] * point1[X] + point1[Y] * point1[Y] + point1[Z] * point1[Z]);
}
//---------------------------------------------------------------------------
float GeometryUtils::Norm2(double *point1)
{
	return point1[X] * point1[X] + point1[Y] * point1[Y] + point1[Z] * point1[Z];
}
//---------------------------------------------------------------------------
double GeometryUtils::Norm(double *point1)
{
	return sqrt(Norm2(point1));
}
//---------------------------------------------------------------------------
void GeometryUtils::Cross(double(&point)[3], double *point1, double *point2)
{
	point[X] = point1[Y] * point2[Z] - point1[Z] * point2[Y];
	point[Y] = point1[Z] * point2[X] - point1[X] * point2[Z];
	point[Z] = point1[X] * point2[Y] - point1[Y] * point2[X];
}
//---------------------------------------------------------------------------
double* GeometryUtils::Cross(double *point1, double *point2)
{
	double point[3];
	point[X] = point1[Y] * point2[Z] - point1[Z] * point2[Y];
	point[Y] = point1[Z] * point2[X] - point1[X] * point2[Z];
	point[Z] = point1[X] * point2[Y] - point1[Y] * point2[X];

	return point;
}

/// Points Utils

//---------------------------------------------------------------------------
bool GeometryUtils::Equal(double *point1, double *point2)
{
	return (point1[X] == point2[X]) && (point1[Y] == point2[Y]) && (point1[Z] == point2[Z]);
}

//----------------------------------------------------------------------------
double GeometryUtils::DistanceBetweenPoints(double *point1, double *point2)
{
	//return sqrt(vtkMath::Distance2BetweenPoints(point1, point2));
	return sqrt(pow(point1[X] - point2[X], 2) + pow(point1[Y] - point2[Y], 2) + pow(point1[Z] - point2[Z], 2));
}

//---------------------------------------------------------------------------
void GeometryUtils::GetMidPoint(double(&midPoint)[3], double *point1, double *point2)
{
	midPoint[X] = (point1[X] + point2[X]) / 2;
	midPoint[Y] = (point1[Y] + point2[Y]) / 2;
	midPoint[Z] = (point1[Z] + point2[Z]) / 2;
}

//----------------------------------------------------------------------------
double GeometryUtils::GetAngle(double point1[3], double point2[3]) // Degree
{
	return std::acos(Dot(point1, point2) / (Mag(point1)*Mag(point2))) * 180.0 / vtkMath::Pi();
}
//----------------------------------------------------------------------------
double GeometryUtils::GetAngle(double* point1, double* point2, double* origin)
{
	double ab[3] = { origin[X] - point1[X], origin[Y] - point1[Y], origin[Z] - point1[Z] };
	double bc[3] = { point2[X] - origin[X], point2[Y] - origin[Y], point2[Z] - origin[Z] };

	double abVec = sqrt(ab[X] * ab[X] + ab[Y] * ab[Y] + ab[Z] * ab[Z]);
	double bcVec = sqrt(bc[X] * bc[X] + bc[Y] * bc[Y] + bc[Z] * bc[Z]);

	double abNorm[3] = { ab[X] / abVec, ab[Y] / abVec, ab[Z] / abVec };
	double bcNorm[3] = { bc[X] / bcVec, bc[Y] / bcVec, bc[Z] / bcVec };

	double res = abNorm[X] * bcNorm[X] + abNorm[Y] * bcNorm[Y] + abNorm[Z] * bcNorm[Z];

	return 180.0 - (acos(res) * 180.0 / vtkMath::Pi());
}

//----------------------------------------------------------------------------
double GeometryUtils::GetDistancePointToLine(double * point, double * lineP1, double * lineP2)
{
	double ab[3] = { lineP2[X] - lineP1[X], lineP2[Y] - lineP1[Y], lineP2[Z] - lineP1[Z] };
	double ac[3] = { point[X] - lineP1[X], point[Y] - lineP1[Y], point[Z] - lineP1[Z] };

	double area = Mag(Cross(ab, ac));
	double cd = area / Mag(ab);

	return cd;
}

//----------------------------------------------------------------------------
bool GeometryUtils::GetLineLineIntersection(double(&point)[3], double *line1Point1, double *line1Point2, double *line2Point1, double *line2Point2)
{
	double da[3] = { line1Point2[X] - line1Point1[X], line1Point2[Y] - line1Point1[Y], line1Point2[Z] - line1Point1[Z] };
	double db[3] = { line2Point2[X] - line2Point1[X], line2Point2[Y] - line2Point1[Y], line2Point2[Z] - line2Point1[Z] };
	double dc[3] = { line2Point1[X] - line1Point1[X], line2Point1[Y] - line1Point1[Y], line2Point1[Z] - line1Point1[Z] };

	if (Dot(dc, Cross(da, db)) != 0.0) // Lines are not coplanar
		return false;

	double cross_dc_db[3];
	Cross(cross_dc_db, da, db);

	double s = Dot(Cross(dc, db), Cross(da, db)) / Norm2(Cross(da, db));
	if (s >= 0.0 && s <= 1.0)
	{
		point[X] = line1Point1[X] + da[X] * s;
		point[Y] = line1Point1[Y] + da[Y] * s;
		point[Z] = line1Point1[Z] + da[Z] * s;
		return true;
	}

	return false;
}



//----------------------------------------------------------------------------
void GeometryUtils::RotatePoint(double *point, double *origin, double angle)
{
	double s = sin(angle);
	double c = cos(angle);

	double d = Dot(origin, point);
	double cr[3];
	Cross(cr, origin, point);

		//glm::dvec3 rotated = (v * c) + (glm::cross(k, v) * s) + (k * glm::dot(k, v)) * (1 - c);

		double pnt1[3]{ point[X] * c, point[Y] * c, point[Z] * c };

	cr[X] *= s;
	cr[Y] *= s;
	cr[Z] *= s;

	double kdot[3]{ origin[X] * d, origin[Y] * d, origin[Z] * d };
	kdot[X] *= (1 - c);
	kdot[Y] *= (1 - c);
	kdot[Z] *= (1 - c);

	point[X] = pnt1[X] + cr[X] + kdot[X];
	point[Y] = pnt1[Y] + cr[Y] + kdot[Y];
	point[Z] = pnt1[Z] + cr[Z] + kdot[Z];

	return;
	// Translate point back to origin:
	point[X] -= origin[X];
	point[Y] -= origin[Y];
	point[Z] -= origin[Z];

	double RotatedPoint[3];

	//First rotate the Z:
	RotatedPoint[X] = point[X] * c - point[Y] * s;
	RotatedPoint[Y] = point[X] * s + point[Y] * c;
	RotatedPoint[Z] = point[Z];

	//Second rotate the Y:
	RotatedPoint[X] = RotatedPoint[X] * c + RotatedPoint[Z] * s;
	RotatedPoint[Y] = RotatedPoint[Y];
	RotatedPoint[Z] = RotatedPoint[Y] * s + RotatedPoint[Z] * c;

	//Third rotate the X:
	RotatedPoint[X] = RotatedPoint[X];
	RotatedPoint[Y] = RotatedPoint[Y] * c - RotatedPoint[Z] * s;
	RotatedPoint[Z] = RotatedPoint[Y] * s + RotatedPoint[Z] * c;

	// Translate point back:
	point[X] = RotatedPoint[X] + origin[X];
	point[Y] = RotatedPoint[Y] + origin[Y];
	point[Z] = RotatedPoint[Z] + origin[Z];
}


/// Lines

//----------------------------------------------------------------------------
int GeometryUtils::IntersectLineLine(double *l1p1, double *l1p2, double *l2p1, double *l2p2, double &perc)
{
	double x[3];
	double projXYZ[3];
	int i;
	double l2Perc;

	double tol = EPSILON;

	if (vtkLine::Intersection(l1p1, l1p2, l2p1, l2p2, perc, l2Perc) == VTK_YES_INTERSECTION)
	{
		// make sure we are within tolerance
		for (i = 0; i < 3; i++)
		{
			x[i] = l2p1[i] + l2Perc * (l2p2[i] - l2p1[i]);
			projXYZ[i] = l1p1[i] + perc*(l1p2[i] - l1p1[i]);
		}
		return vtkMath::Distance2BetweenPoints(x, projXYZ) <= tol*tol;
	}
	else return false;

}

//----------------------------------------------------------------------------
double GeometryUtils::GetPointToLineDistance(double *point, double *linePoint1, double *linePoint2)
{
	double a = linePoint1[Y] - linePoint2[Y]; // Note: this was incorrectly "y2 - y1" in the original answer
	double b = linePoint2[X] - linePoint1[X];
	double c = linePoint1[X] * linePoint2[Y] - linePoint2[X] * linePoint1[Y];

	return abs(a * point[X] + b * point[Y] + c) / sqrt(a * a + b * b);
}
//----------------------------------------------------------------------------
float GeometryUtils::DistancePointToLine(double * point, double * lineP1, double * lineP2)
{	
	return sqrt(vtkLine::DistanceToLine(point, lineP1, lineP2));

	double point_x = point[X];
	double point_y = point[Y];

	double line_x1 = lineP1[X];
	double line_y1 = lineP1[Y];
	double line_x2 = lineP2[X];
	double line_y2 = lineP2[Y];

	double diffX = line_x2 - line_x1;
	double diffY = line_y2 - line_y1;

	if ((diffX == 0) && (diffY == 0))
	{
		diffX = point_x - line_x1;
		diffY = point_y - line_y1;
		return sqrt(diffX * diffX + diffY * diffY);
	}

	float t = ((point_x - line_x1) * diffX + (point_y - line_y1) * diffY) / (diffX * diffX + diffY * diffY);

	if (t < 0)
	{
		// Point is nearest to the first point i.e x1 and y1
		diffX = point_x - line_x1;
		diffY = point_y - line_y1;
	}
	else if (t > 1)
	{
		// Point is nearest to the end point i.e x2 and y2
		diffX = point_x - line_x2;
		diffY = point_y - line_y2;
	}
	else
	{
		// If perpendicular line intersect the line segment.
		diffX = point_x - (line_x1 + t * diffX);
		diffY = point_y - (line_y1 + t * diffY);
	}

	// Returning shortest distance
	return sqrt(diffX * diffX + diffY * diffY);
}

//----------------------------------------------------------------------------
void GeometryUtils::GetParallelLine(double(&point1)[3], double(&point2)[3], double *linePoint1, double *linePoint2, double distance)
{
	double L = sqrt(pow((linePoint2[X] - linePoint1[X]), 2) + pow((linePoint2[Y] - linePoint1[Y]), 2) + pow((linePoint2[Z] - linePoint1[Z]), 2));

	point1[X] = linePoint1[X] + distance * (linePoint2[Y] - linePoint1[Y]) / L;
	point1[Y] = linePoint1[Y] + distance * (linePoint1[X] - linePoint2[X]) / L;
	point1[Z] = linePoint1[Z] + distance * (linePoint2[Z] - linePoint1[Z]) / L;

	point2[X] = linePoint2[X] + distance * (linePoint2[Y] - linePoint1[Y]) / L;
	point2[Y] = linePoint2[Y] + distance * (linePoint1[X] - linePoint2[X]) / L;
	point2[Z] = linePoint2[Z] + distance * (linePoint2[Z] - linePoint1[Z]) / L;
}
//----------------------------------------------------------------------------
bool GeometryUtils::FindPointOnLine(double(&point)[3], double *linePoint1, double *linePoint2, double distance)
{
	double L = sqrt(pow((linePoint2[X] - linePoint1[X]), 2) + pow((linePoint2[Y] - linePoint1[Y]), 2));
	double dist_ratio = distance / L;

	point[X] = (1 - dist_ratio)*linePoint1[X] + dist_ratio * linePoint2[X];
	point[Y] = (1 - dist_ratio)*linePoint1[Y] + dist_ratio * linePoint2[Y];
	//point[Z] = 0.0;

	return (dist_ratio > 0 && dist_ratio < 1); //the point is on the line.
}

//----------------------------------------------------------------------------
int GeometryUtils::PointUpDownLine(double *point, double *lp1, double *lp2)
{
	double d = (point[X] - lp1[X]) * (lp2[Y] - lp1[Y]) - (point[Y] - lp1[Y]) * (lp2[X] - lp1[X]);

	if (d > 0)
		return 1;
	else if (d < 0)
		return -1;
	else
		return 0;
}




void GeometryUtils::rotAroundZ(double *point, float degree)
{
	double n_point[3];

	n_point[X] = (point[X] * cos(degree * vtkMath::Pi() / 180.0)) - (point[Y] * sin(degree * vtkMath::Pi() / 180.0));
	n_point[Y] = (point[X] * sin(degree * vtkMath::Pi() / 180.0)) + (point[Y] * cos(degree * vtkMath::Pi() / 180.0));
	n_point[Z] = point[Z];

	point[X] = n_point[X];
	point[Y] = n_point[Y];
	point[Z] = n_point[Z];
}

void GeometryUtils::rotAroundY(double *point, float degree)
{
	double n_point[3];

	n_point[X] = (point[X] * cos(degree * vtkMath::Pi() / 180.0)) + (point[Z] * sin(degree * vtkMath::Pi() / 180.0));
	n_point[Y] = point[Y];
	n_point[Z] = ((point[X] * -1.0f) * sin(degree * vtkMath::Pi() / 180.0)) + (point[Z] * cos(degree * vtkMath::Pi() / 180.0));;

	point[X] = n_point[X];
	point[Y] = n_point[Y];
	point[Z] = n_point[Z];
}

void GeometryUtils::rotAroundA(double *point, double *axis, float zdegree)
{
	double v1[3] = { 1.0f, 0.0f, 0.0f };
	double v2[3] = { 0.0f, 1.0f, 0.0f };

	float xdegree = GetAngle(axis, v1);
	float ydegree = GetAngle(axis, v2);

	rotAroundZ(point, xdegree);
	rotAroundY(point, ydegree);
	rotAroundZ(point, zdegree);
	rotAroundY(point, -ydegree);
	rotAroundZ(point, -xdegree);
}

/*
void GeometryUtils::rotAObject(Object& obj, double *axis, float degree)
{
	axis = glm::normalize(axis);
	translate(axis, glm::vec3{ axis[X], axis[Y], axis[Z] });
	for (int i = 0; i < obj.vertices.size(); i++)
	{
		rotAroundA(obj.vertices[i], axis, degree);
	}
	rotAroundA(obj.mp, axis, degree);
	translate(axis, glm::vec3{ axis[X] * -1.0f, axis[Y] * -1.0f, axis[Z] * -1.0f });
}*/