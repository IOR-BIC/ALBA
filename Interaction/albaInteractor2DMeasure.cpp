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
	m_View = NULL;

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

	for (int i = 0; i < m_TextActorVector.size(); i++)
	{
		// Texts
		m_Renderer->RemoveActor2D(m_TextActorVector[i]);
		vtkDEL(m_TextActorVector[i]);
	}

	m_Renderer->GetRenderWindow()->Render();

	m_TextActorVector.clear();
// 	m_MeasureTextVector.clear();
// 	m_MeasureLabelVector.clear();

	m_Measure2DVector.clear();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure::InitRenderer(albaEventInteraction *e)
{
	albaEventMacro(albaEvent(this, CAMERA_UPDATE));

	if (m_Renderer == NULL)
	{
		if (m_Mouse == NULL)
		{
			albaDevice *device = albaDevice::SafeDownCast((albaDevice*)e->GetSender());
			albaDeviceButtonsPadMouse  *mouse = albaDeviceButtonsPadMouse::SafeDownCast(device);
			m_Mouse = mouse;
		}

		m_Renderer = m_Mouse->GetRenderer();
	}

	if (m_Renderer)
	{
		m_ParallelView = m_Renderer->GetActiveCamera()->GetParallelProjection() != 0;

		if (m_ParallelScale_OnStart == -1) // Save current Parallel Scale
			m_ParallelScale_OnStart = m_Renderer->GetActiveCamera()->GetParallelScale();
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::Render()
{
	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

/// MOUSE EVENTS /////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::OnLeftButtonDown(albaEventInteraction *e)
{
	albaEventMacro(albaEvent(this, CAMERA_UPDATE));

	if (!m_ParallelView)
	{
		InitRenderer(e);
	}

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

		double pos_2d[2];
		e->Get2DPosition(pos_2d);
		double pointCoord[3];
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

	if (!m_ParallelView)
	{
		InitRenderer(e);
	}

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
		else
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

	point1[Z] = 0.0;

	if (point2 != NULL) point2[Z] = 0.0;;

	//////////////////////////////////////////////////////////////////////////
	// Add Measure
	albaString text;
	text.Printf("Point (%.2f, %.2f)", point1[X], point1[Y]);

	Measure2D newMeasure;
	newMeasure.Active = true;
	newMeasure.MeasureType = m_MeasureTypeText;
	newMeasure.Text = text;
	newMeasure.Label = "";

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
	if (m_Renderer && index < GetMeasureCount())
	{
		//////////////////////////////////////////////////////////////////////////
		// Remove Measure
		m_Measure2DVector.erase(m_Measure2DVector.begin() + index);

		//////////////////////////////////////////////////////////////////////////
		// Remove Text
		m_Renderer->RemoveActor2D(m_TextActorVector[index]);
		vtkDEL(m_TextActorVector[index]);
		m_TextActorVector.erase(m_TextActorVector.begin() + index);
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

// 	for (int i = 0; i < m_TextActorVector.size(); i++)
// 	{
// 		m_TextActorVector[i]->SetVisibility(show);
// 	}

	Update();
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

	// Searching for a frontal renderer on render collection
	if (rc)
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

		m_Measure2DVector[index].Text = text; //

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
		if (pos[0] > m_Bounds[0] && pos[0] < m_Bounds[1]) // MarginLeft & MarginRight
			if (pos[1] > m_Bounds[2] && pos[1] < m_Bounds[3]) // MarginUp & MarginDown
				m_IsInBound = true;
	}

	return m_IsInBound;
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::ScreenToWorld(double screen[2], double world[3])
{
	double wp[4];

	m_Renderer->SetDisplayPoint(screen[0], screen[1], 0);
	m_Renderer->DisplayToWorld();
	m_Renderer->GetWorldPoint(wp);

	world[0] = wp[0];
	world[1] = wp[1];
	world[2] = 0;
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure::WorldToScreen(double world[3], double screen[2])
{
	double scr[3];

	m_Renderer->SetWorldPoint(world);
	m_Renderer->WorldToDisplay();
	m_Renderer->GetDisplayPoint(scr);

	screen[0] = scr[0];
	screen[1] = scr[1];
}

//----------------------------------------------------------------------------
vtkPointSource * albaInteractor2DMeasure::GetNewPointSource()
{
	vtkPointSource *ps = vtkPointSource::New();
	ps->SetNumberOfPoints(1);
	ps->SetRadius(0);

	return ps;
}


#define VTK_NO_INTERSECTION 0
#define VTK_YES_INTERSECTION  2
#define EPSILON 1e-5;

/// Points

//---------------------------------------------------------------------------
bool appGeometry::Equal(double *point1, double *point2)
{
	return point1[0] == point2[0] && point1[1] == point2[1] && point1[2] == point2[2];
}

//---------------------------------------------------------------------------
double * appGeometry::GetMidPoint(double *point1, double *point2)
{
	double midPoint[3];
	midPoint[0] = (point1[0] + point2[0]) / 2;
	midPoint[1] = (point1[1] + point2[1]) / 2;
	midPoint[2] = 0.0;

	return midPoint;
}

//----------------------------------------------------------------------------
void appGeometry::RotatePoint(double *point, double *origin, double angle)
{
	double s = sin(angle);
	double c = cos(angle);

	// Translate point back to origin:
	point[X] -= origin[X];
	point[Y] -= origin[Y];

	// Rotate point
	double xnew = point[X] * c - point[Y] * s;
	double ynew = point[X] * s + point[Y] * c;

	// Translate point back:
	point[X] = xnew + origin[X];
	point[Y] = ynew + origin[Y];
	point[Z] = 0;
}

//----------------------------------------------------------------------------
double appGeometry::CalculateAngle(double point1[3], double point2[3], double origin[3])
{
	double angleToP1 = atan2((point1[X] - origin[X]), (point1[Y] - origin[Y]));
	double angleToP2 = atan2((point2[X] - origin[X]), (point2[Y] - origin[Y]));

	double angle = angleToP2 - angleToP1;

	if (angle < 0) angle += (2 * vtkMath::Pi());

	return angle;
}

//----------------------------------------------------------------------------
double appGeometry::DistanceBetweenPoints(double *point1, double *point2)
{
	return sqrt(pow(point1[X] - point2[X], 2) + pow(point1[Y] - point2[Y], 2));
}

/// Lines
//----------------------------------------------------------------------------
bool appGeometry::FindIntersectionLines(double(&point)[3], double *line1Point1, double *line1Point2, double *line2Point1, double *line2Point2)
{
	// Line1 represented as a1x + b1y = c1
	double a1 = line1Point2[1] - line1Point1[1];
	double b1 = line1Point1[0] - line1Point2[0];
	double c1 = a1*(line1Point1[0]) + b1*(line1Point1[1]);

	// Line2 represented as a2x + b2y = c2
	double a2 = line2Point2[1] - line2Point1[1];
	double b2 = line2Point1[0] - line2Point2[0];
	double c2 = a2*(line2Point1[0]) + b2*(line2Point1[0]);

	double determinant = a1*b2 - a2*b1;

	if (determinant == 0)
	{
		// The lines are parallel. This is simplified
		point[0] = DBL_MAX;
		point[1] = DBL_MAX;
		point[2] = 0.0;
	}
	else
	{
		point[0] = (b2*c1 - b1*c2) / determinant;
		point[1] = (a1*c2 - a2*c1) / determinant;
		point[2] = 0.0;

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------
int appGeometry::IntersectLineLine(double *l1p1, double *l1p2, double *l2p1, double *l2p2, double &perc)
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
double appGeometry::GetPointToLineDistance(double *point, double *linePoint1, double *linePoint2)
{
	double a = linePoint1[1] - linePoint2[1]; // Note: this was incorrectly "y2 - y1" in the original answer
	double b = linePoint2[0] - linePoint1[0];
	double c = linePoint1[0] * linePoint2[1] - linePoint2[0] * linePoint1[1];

	return abs(a * point[0] + b * point[1] + c) / sqrt(a * a + b * b);
}
//----------------------------------------------------------------------------
float appGeometry::DistancePointToLine(double * point, double * lineP1, double * lineP2)
{
	double point_x = point[0];
	double point_y = point[1];

	double line_x1 = lineP1[0];
	double line_y1 = lineP1[1];
	double line_x2 = lineP2[0];
	double line_y2 = lineP2[1];

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
void appGeometry::GetParallelLine(double(&point1)[3], double(&point2)[3], double *linePoint1, double *linePoint2, double distance)
{
	double L = sqrt(pow((linePoint2[0] - linePoint1[0]), 2) + pow((linePoint2[1] - linePoint1[1]), 2));

	point1[0] = linePoint1[0] + distance * (linePoint2[1] - linePoint1[1]) / L;
	point1[1] = linePoint1[1] + distance * (linePoint1[0] - linePoint2[0]) / L;
	point1[2] = 0.0;

	point2[0] = linePoint2[0] + distance * (linePoint2[1] - linePoint1[1]) / L;
	point2[1] = linePoint2[1] + distance * (linePoint1[0] - linePoint2[0]) / L;
	point2[2] = 0.0;
}
//----------------------------------------------------------------------------
bool appGeometry::FindPointOnLine(double(&point)[3], double *linePoint1, double *linePoint2, double distance)
{
	double L = sqrt(pow((linePoint2[0] - linePoint1[0]), 2) + pow((linePoint2[1] - linePoint1[1]), 2));
	double dist_ratio = distance / L;

	point[0] = (1 - dist_ratio)*linePoint1[0] + dist_ratio * linePoint2[0];
	point[1] = (1 - dist_ratio)*linePoint1[1] + dist_ratio * linePoint2[1];
	point[2] = 0.0;

	return (dist_ratio > 0 && dist_ratio < 1); //the point is on the line.
}

//----------------------------------------------------------------------------
int appGeometry::PointUpDownLine(double *point, double *lp1, double *lp2)
{
	double d = (point[0] - lp1[0]) * (lp2[1] - lp1[1]) - (point[1] - lp1[1]) * (lp2[0] - lp1[0]);

	if (d > 0)
		return 1;
	else if (d < 0)
		return -1;
	else
		return 0;
}
