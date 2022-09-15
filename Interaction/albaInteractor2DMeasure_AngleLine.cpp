/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_AngleLine.cpp
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

#include "albaInteractor2DMeasure_AngleLine.h"
#include "albaInteractor2DMeasure.h"

#include "albaTagArray.h"
#include "albaVME.h"

#include "vtkALBATextActorMeter.h"
#include "vtkALBACircleSource.h"
#include "vtkActor2D.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyDataSource.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkViewport.h"
#include "vtkPointSource.h"
#include "albaVect3d.h"
/*#include "appGeometry.h"*/

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DMeasure_AngleLine)

//----------------------------------------------------------------------------
albaInteractor2DMeasure_AngleLine::albaInteractor2DMeasure_AngleLine() : albaInteractor2DMeasure()
{
	m_ShowText = false;
	m_TextSide = 1;

	m_MeasureTypeText = "ANGLE_LINE";

	m_CurrPoint = NO_POINT;
	m_AddModeCompleted = true;

	m_LineExtensionLength = 1.0;
	m_LineStipplePattern = 0xFFFF;

	Color color{ 0.4, 0.4, 1, 1.0 };

	SetColorDefault(color.R, color.G, color.B, 0.85);
	SetColorSelection(color.R, color.G, color.B, 1.0);
	SetColorDisable(color.R, color.G, color.B, 0.3);
	SetColorText(color.R, color.G, color.B, 0.5);

	m_ColorAux.R = 0.0;
	m_ColorAux.G = 1.0;
	m_ColorAux.B = 0.0;
	m_ColorAux.Alpha = 0.5;
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure_AngleLine::~albaInteractor2DMeasure_AngleLine()
{
	// Lines
	for (int i = 0; i < m_PointsStackVectorA.size(); i++)
	{
		cppDEL(m_PointsStackVectorA[i]);
		cppDEL(m_PointsStackVectorB[i]);
		cppDEL(m_PointsStackVectorC[i]);
		cppDEL(m_PointsStackVectorD[i]);
		cppDEL(m_LineStackVectorAB[i]);
		cppDEL(m_LineStackVectorCD[i]);
		cppDEL(m_CircleStackVector[i]);
	}

	m_Angles.clear();
	m_SecondLineP1Added.clear();
	m_SecondLineP2Added.clear();
}

/// RENDERING ////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::DrawNewMeasure(double * wp)
{
	AddMeasure(wp, wp, wp, wp);

	// Call FindAndHighLight to start the edit phase
	FindAndHighlight(wp);
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::MoveMeasure(int index, double *point)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	if (!m_Measure2DVector[index].Active)
		return;

	double pointA[3], pointB[3], pointC[3], pointD[3];

	vtkPointSource* pointSourceA = (vtkPointSource*)m_PointsStackVectorA[index]->GetSource();
	vtkPointSource* pointSourceB = (vtkPointSource*)m_PointsStackVectorB[index]->GetSource();
	vtkPointSource* pointSourceC = (vtkPointSource*)m_PointsStackVectorC[index]->GetSource();
	vtkPointSource* pointSourceD = (vtkPointSource*)m_PointsStackVectorD[index]->GetSource();

	pointSourceC->GetCenter(pointC);
	pointSourceA->GetCenter(pointA);
	pointSourceB->GetCenter(pointB);
	pointSourceD->GetCenter(pointD);

	if (!m_MovingMeasure)
	{
		m_OldLineP1[X] = pointA[X] - m_StartMousePosition[X];
		m_OldLineP1[Y] = pointA[Y] - m_StartMousePosition[Y];
		m_OldLineP1[Z] = pointA[Z] - m_StartMousePosition[Z];

		m_OldLineP2[X] = pointB[X] - m_StartMousePosition[X];
		m_OldLineP2[Y] = pointB[Y] - m_StartMousePosition[Y];
		m_OldLineP2[Z] = pointB[Z] - m_StartMousePosition[Z];
		
		m_OldLineP3[X] = pointC[X] - m_StartMousePosition[X];
		m_OldLineP3[Y] = pointC[Y] - m_StartMousePosition[Y];
		m_OldLineP3[Z] = pointC[Z] - m_StartMousePosition[Z];

		m_OldLineP4[X] = pointD[X] - m_StartMousePosition[X];
		m_OldLineP4[Y] = pointD[Y] - m_StartMousePosition[Y];
		m_OldLineP4[Z] = pointD[Z] - m_StartMousePosition[Z];

		m_MovingMeasure = true;
	}

	// Initialization
	m_MeasureValue = 0.0;

	double tmp_posA[3], tmp_posB[3], tmp_posC[3], tmp_posD[3];

	tmp_posA[X] = m_MoveLineAB ? point[X] + m_OldLineP1[X] : pointA[X];
	tmp_posA[Y] = m_MoveLineAB ? point[Y] + m_OldLineP1[Y] : pointA[Y];
	tmp_posA[Z] = m_MoveLineAB ? point[Z] + m_OldLineP1[Z] : pointA[Z];

	tmp_posB[X] = m_MoveLineAB ? point[X] + m_OldLineP2[X] : pointB[X];
	tmp_posB[Y] = m_MoveLineAB ? point[Y] + m_OldLineP2[Y] : pointB[Y];
	tmp_posB[Z] = m_MoveLineAB ? point[Z] + m_OldLineP2[Z] : pointB[Z];

	tmp_posC[X] = m_MoveLineCD ? point[X] + m_OldLineP3[X] : pointC[X];
	tmp_posC[Y] = m_MoveLineCD ? point[Y] + m_OldLineP3[Y] : pointC[Y];
	tmp_posC[Z] = m_MoveLineCD ? point[Z] + m_OldLineP3[Z] : pointC[Z];

	tmp_posD[X] = m_MoveLineCD ? point[X] + m_OldLineP4[X] : pointD[X];
	tmp_posD[Y] = m_MoveLineCD ? point[Y] + m_OldLineP4[Y] : pointD[Y];
	tmp_posD[Z] = m_MoveLineCD ? point[Z] + m_OldLineP4[Z] : pointD[Z];

	m_MeasureValue = CalculateAngle(tmp_posA, tmp_posB, tmp_posC, tmp_posD);

	// Points
	UpdatePointsActor(tmp_posA, tmp_posB, tmp_posC, tmp_posD);
	// Lines
	UpdateLineActors(tmp_posA, tmp_posB, tmp_posC, tmp_posD);
	// Circle
	UpdateCircleActor(tmp_posA, m_MeasureValue, 10);

	//called on mouse up  
	if (!m_DraggingLeft)
	{
		m_ActorAdded = false;
	}

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::EditMeasure(int index, double *point)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	if (!m_Measure2DVector[index].Active)
		return;

	m_MovingMeasure = true;

	double pointA[3], pointB[3], pointC[3], pointD[3];
	
	vtkPointSource* pointSourceA = (vtkPointSource*)m_PointsStackVectorA[index]->GetSource();
	vtkPointSource* pointSourceB = (vtkPointSource*)m_PointsStackVectorB[index]->GetSource();
	vtkPointSource* pointSourceC = (vtkPointSource*)m_PointsStackVectorC[index]->GetSource();
	vtkPointSource* pointSourceD = (vtkPointSource*)m_PointsStackVectorD[index]->GetSource();

	pointSourceA->GetCenter(pointA);
	pointSourceB->GetCenter(pointB);
	pointSourceC->GetCenter(pointC);
	pointSourceD->GetCenter(pointD);

	if (m_CurrPoint == POINT_1)
	{
		pointA[X] = point[X];
		pointA[Y] = point[Y];
		pointA[Z] = point[Z];
	}
	else if (m_CurrPoint == POINT_2)
	{
		pointB[X] = point[X];
		pointB[Y] = point[Y];
		pointB[Z] = point[Z];
	}
	else if (m_CurrPoint == POINT_3)
	{
		pointC[X] = point[X];
		pointC[Y] = point[Y];
		pointC[Z] = point[Z];
	}
	else if (m_CurrPoint == POINT_4)
	{
		pointD[X] = point[X];
		pointD[Y] = point[Y];
		pointD[Z] = point[Z];
	}
	else if (m_CurrPoint == 5) // Edit POINT_3 and POINT_4
	{
		pointC[X] = point[X];
		pointC[Y] = point[Y];
		pointC[Z] = point[Z];

		pointD[X] = point[X];
		pointD[Y] = point[Y];
		pointD[Z] = point[Z];
	}

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	double angle = CalculateAngle(pointA, pointB, pointC, pointD);
	
	// Measure
	UpdateMeasure(index, angle);

	// Points
	UpdatePointsActor(pointA, pointB, pointC, pointD);
	// Line
	UpdateLineActors(pointA, pointB, pointC, pointD);
	// Circle
	UpdateCircleActor(pointA, angle, 10);
	// Text
	UpdateTextActor(pointA, pointB);

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::FindAndHighlight(double * point)
{
	if (m_CurrMeasure < 0)
		SetAction(ACTION_ADD_MEASURE);

	if (m_EditMeasureEnable)
	{
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			albaActor2dStackHelper *pointStackVectorA = m_PointsStackVectorA[i];
			albaActor2dStackHelper *pointStackVectorB = m_PointsStackVectorB[i];
			albaActor2dStackHelper *pointStackVectorC = m_PointsStackVectorC[i];
			albaActor2dStackHelper *pointStackVectorD = m_PointsStackVectorD[i];

			if (m_Renderer != pointStackVectorA->GetRenderer() && m_Renderer != pointStackVectorB->GetRenderer())
				continue;
			
			if (m_Renderer != pointStackVectorC->GetRenderer() && m_Renderer != pointStackVectorD->GetRenderer())
				continue;

			double pointA[3], pointB[3], pointC[3], pointD[3];

			vtkPointSource* pointSourceA = (vtkPointSource*)m_PointsStackVectorA[i]->GetSource();
			vtkPointSource* pointSourceB = (vtkPointSource*)m_PointsStackVectorB[i]->GetSource();
			vtkPointSource* pointSourceC = (vtkPointSource*)m_PointsStackVectorC[i]->GetSource();
			vtkPointSource* pointSourceD = (vtkPointSource*)m_PointsStackVectorD[i]->GetSource();
			
			pointSourceA->GetCenter(pointA);
			pointSourceB->GetCenter(pointB);
			pointSourceC->GetCenter(pointC);
			pointSourceD->GetCenter(pointD);

			if (DistancePointToLine(point, pointA, pointB) < POINT_UPDATE_DISTANCE)
			{
				SelectMeasure(i);

				if (m_Measure2DVector[i].Active)
				{
					if (vtkMath::Distance2BetweenPoints(pointA, point) < POINT_UPDATE_DISTANCE_2)
					{
						SetAction(ACTION_EDIT_MEASURE);
						m_CurrMeasure = i;
						m_CurrPoint = POINT_1;
						m_PointsStackVectorA[i]->SetColor(m_Colors[COLOR_EDIT]);
					}
					else if (vtkMath::Distance2BetweenPoints(pointB, point) < POINT_UPDATE_DISTANCE_2)
					{
						SetAction(ACTION_EDIT_MEASURE);
						m_CurrMeasure = i;
						m_CurrPoint = POINT_2;
						m_PointsStackVectorB[i]->SetColor(m_Colors[COLOR_EDIT]);
					}
					else
					{
						m_CurrMeasure = i;
						if (m_MoveMeasureEnable)
						{
							m_LineStackVectorAB[i]->SetColor(m_Colors[COLOR_EDIT]);
							SetAction(ACTION_MOVE_MEASURE);
						}

						m_MoveLineAB = true;
						m_MoveLineCD = false;
					}
				}

				Render();
				return;
			}
			else if (DistancePointToLine(point, pointC, pointD) < POINT_UPDATE_DISTANCE)
			{
				SelectMeasure(i);

				if (m_Measure2DVector[i].Active)
				{
					if (vtkMath::Distance2BetweenPoints(pointC, point) < POINT_UPDATE_DISTANCE_2)
					{
						SetAction(ACTION_EDIT_MEASURE);
						m_CurrMeasure = i;
						m_CurrPoint = POINT_3;
						m_PointsStackVectorC[i]->SetColor(m_Colors[COLOR_EDIT]);
					}
					else if (vtkMath::Distance2BetweenPoints(pointD, point) < POINT_UPDATE_DISTANCE_2)
					{
						SetAction(ACTION_EDIT_MEASURE);
						m_CurrMeasure = i;
						m_CurrPoint = POINT_4;
						m_PointsStackVectorD[i]->SetColor(m_Colors[COLOR_EDIT]);
					}
					else
					{
						m_CurrMeasure = i;
						if (m_MoveMeasureEnable)
						{
							m_LineStackVectorCD[i]->SetColor(m_Colors[COLOR_EDIT]);
							SetAction(ACTION_MOVE_MEASURE);
						}

						m_MoveLineAB = false;
						m_MoveLineCD = true;
					}
				}

				Render();
				return;
			}
		}

		if (m_CurrMeasure >= 0)
		{
			SelectMeasure(-1);
			m_CurrMeasure = -1;
			m_CurrPoint = NO_POINT;
			m_MoveLineAB = false;
			m_MoveLineCD = false;
			Render();
		}
	}
}

/// UPDATE ///////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::UpdateMeasure(int index, double measure)
{
	double angleDegree = measure * vtkMath::RadiansToDegrees();
	angleDegree = abs((angleDegree <= 180.0) ? angleDegree : 360.0 - angleDegree);

	albaString text;
	text.Printf("Angle %.2f°", angleDegree);
	m_Measure2DVector[index].Text = text;
	m_Measure2DVector[index].Value = angleDegree;
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::UpdatePointsActor(double * point1, double * point2, double * point3, double * point4)
{
	//Point A
	vtkPointSource* pointSourceA = (vtkPointSource*)m_PointsStackVectorA[m_CurrMeasure]->GetSource();
	pointSourceA->SetCenter(point1);
	pointSourceA->Update();

	//Point B
	vtkPointSource* pointSourceB = (vtkPointSource*)m_PointsStackVectorB[m_CurrMeasure]->GetSource();
	pointSourceB->SetCenter(point2);
	pointSourceB->Update();

	//Point C
	vtkPointSource* pointSourceC = (vtkPointSource*)m_PointsStackVectorC[m_CurrMeasure]->GetSource();
	pointSourceC->SetCenter(point3);
	pointSourceC->Update();

	//Point D
	vtkPointSource* pointSourceD = (vtkPointSource*)m_PointsStackVectorD[m_CurrMeasure]->GetSource();
	pointSourceD->SetCenter(point4);
	pointSourceD->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::UpdateLineActors(double * point1, double * point2, double * point3, double * point4)
{
	vtkLineSource* lineSourceAB = (vtkLineSource*)m_LineStackVectorAB[m_CurrMeasure]->GetSource();
	vtkLineSource* lineSourceCD = (vtkLineSource*)m_LineStackVectorCD[m_CurrMeasure]->GetSource();
	
	lineSourceAB->SetPoint1(point1);
	lineSourceAB->SetPoint2(point2);
	lineSourceAB->Update();

	lineSourceCD->SetPoint1(point3);
	lineSourceCD->SetPoint2(point4);
	lineSourceCD->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::UpdateCircleActor(double * point, double angle, double radius)
{
	vtkALBACircleSource *circleSource = (vtkALBACircleSource *)m_CircleStackVector[m_CurrMeasure]->GetSource();
	circleSource->SetPlane(m_CurrPlane);
	circleSource->SetRadius(radius);
	circleSource->SetCenter(point);
	circleSource->SetResolution(60);

	if (angle < 90.5 && angle > 89.5)
	{
		circleSource->SetResolution(2);
		angle = 180.00;
	}

	circleSource->SetAngleRange(0, angle*vtkMath::DegreesToRadians());
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::UpdateTextActor(double * point1, double * point2)
{
	double text_pos[3];
	GetMidPoint(text_pos, point1, point2);
	
	text_pos[X] += m_TextSide *TEXT_W_SHIFT;
	text_pos[Y] -= m_TextSide *TEXT_H_SHIFT;

	Superclass::UpdateTextActor(m_CurrMeasure, text_pos);
}


/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::AddMeasure(double *point1, double *point2, double *point3, double * point4)
{
	if (GetMeasureCount() > 0)
	{
		int index = GetMeasureCount() - 1;
		double oldPoint1[3], oldPoint2[3], oldPoint3[3], oldPoint4[3];
		GetMeasureLinePoints(index, oldPoint1, oldPoint2, oldPoint3, oldPoint4);

		bool hasSameRenderer = (m_Renderer == m_Measure2DVector[index].Renderer);

		if (DistanceBetweenPoints(oldPoint1, oldPoint2) < POINT_UPDATE_DISTANCE)
		{
			if (!hasSameRenderer) return;

			m_CurrMeasure = index;
			m_CurrPoint = POINT_2;
			EditMeasure(index, point2);

			if (m_AddModeCompleted == false)
			{
				//RemoveMeasure(GetMeasureCount() - 1);
				m_AddModeCompleted = true;
			}

			ActivateMeasure(-1, true);

			return;
		}
		else
		{
			if (DistanceBetweenPoints(oldPoint3, oldPoint4) > POINT_UPDATE_DISTANCE)
				m_SecondLineP2Added[index] = true;

			if (m_SecondLineP1Added[index] == false)
			{
				if (!hasSameRenderer) return;

				//Adding the first point of second line no need to add a new measure.
				m_SecondLineP1Added[index] = true;

				m_CurrMeasure = index;
				m_CurrPoint = 5; // Edit (POINT_3 and POINT_4)
				EditMeasure(index, point3);

				m_AddModeCompleted = true;
				ActivateMeasure(-1, true);

				return;
			}
			else if (m_SecondLineP2Added[index] == false)
			{
				if (!hasSameRenderer) return;

				//Adding the second point of the second line no need to add a new measure.
				m_SecondLineP2Added[index] = true;

				m_CurrMeasure = index;
				m_CurrPoint = POINT_4;
				EditMeasure(index, point4);
				return;
			}
		}
	}

	if (m_AddModeCompleted)
	{
		Superclass::AddMeasure(point1, point2);

		//////////////////////////////////////////////////////////////////////////
		// Update Measure
		int index = m_Measure2DVector.size() - 1;
		double angle = CalculateAngle(point1, point2, point3, point4);

		// Update Measure
		UpdateMeasure(index, angle);

		// Update Edit Actors
		UpdateEditActors(point1, point2);

		//////////////////////////////////////////////////////////////////////////
		// Add Points
		m_PointsStackVectorA.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
		m_PointsStackVectorB.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
		m_PointsStackVectorC.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
		m_PointsStackVectorD.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));

		// Add Lines
		m_LineStackVectorAB.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));
		m_LineStackVectorCD.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));

		// Add Circle
		m_CircleStackVector.push_back(new albaActor2dStackHelper(vtkALBACircleSource::New(), m_Renderer));

		m_Angles.push_back(0);
		m_SecondLineP1Added.push_back(false);
		m_SecondLineP2Added.push_back(false);

		//////////Setting proprieties//////////////
		int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

		//Point A
		m_PointsStackVectorA[index]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorA[index]->SetColor(m_Colors[col]);

		//Point B
		m_PointsStackVectorB[index]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorB[index]->SetColor(m_Colors[col]);

		//Point C
		m_PointsStackVectorC[index]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorC[index]->SetColor(m_Colors[col]);

		//Point D
		m_PointsStackVectorD[index]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorD[index]->SetColor(m_Colors[col]);

		//Line AB
		m_LineStackVectorAB[index]->SetColor(m_Colors[col]);
		m_LineStackVectorAB[index]->GetProperty()->SetLineWidth(m_LineWidth);
		m_LineStackVectorAB[index]->GetProperty()->SetLineStipplePattern(m_LineStipplePattern);

		//Line CD
		m_LineStackVectorCD[index]->SetColor(m_Colors[col]);
		m_LineStackVectorCD[index]->GetProperty()->SetLineWidth(m_LineWidth);
		m_LineStackVectorCD[index]->GetProperty()->SetLineStipplePattern(m_LineStipplePattern);

		// Circle
		m_CircleStackVector[index]->GetProperty()->SetLineWidth(m_LineWidth);
		m_CircleStackVector[index]->SetColor(m_ColorAux);
		m_CircleStackVector[index]->GetProperty()->SetLineStipplePattern(0xf0f0);

		vtkALBACircleSource *circleSource = (vtkALBACircleSource *)m_CircleStackVector[index]->GetSource();
		circleSource->SetResolution(60);

		m_CurrMeasure = index;
		m_AddModeCompleted = false;

		ActivateMeasure(-1, false);
		ActivateMeasure(index);

		UpdatePointsActor(point1, point2, point3, point4);
		UpdateLineActors(point1, point2, point3, point4);
		UpdateCircleActor(point1, angle, 10);
	}
	//////////////////////////////////////////////////////////////////////////
	albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasureText(GetMeasureCount() - 1)));

	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::RemoveMeasure(int index)
{
	if (m_Renderer && index < GetMeasureCount())
	{
		Superclass::RemoveMeasure(index);

		//////////////////////////////////////////////////////////////////////////

		//Point A
		cppDEL(m_PointsStackVectorA[index]);
		m_PointsStackVectorA.erase(m_PointsStackVectorA.begin() + index);
		//Point B
		cppDEL(m_PointsStackVectorB[index]);
		m_PointsStackVectorB.erase(m_PointsStackVectorB.begin() + index);
		//Point C
		cppDEL(m_PointsStackVectorC[index]);
		m_PointsStackVectorC.erase(m_PointsStackVectorC.begin() + index);
		//Point D
		cppDEL(m_PointsStackVectorD[index]);
		m_PointsStackVectorD.erase(m_PointsStackVectorD.begin() + index);

		//Line AB
		cppDEL(m_LineStackVectorAB[index]);
		m_LineStackVectorAB.erase(m_LineStackVectorAB.begin() + index);
		//Line CD
		cppDEL(m_LineStackVectorCD[index]);
		m_LineStackVectorCD.erase(m_LineStackVectorCD.begin() + index);

		// Circle
		cppDEL(m_CircleStackVector[index]);
		m_CircleStackVector.erase(m_CircleStackVector.begin() + index);

		m_Angles.erase(m_Angles.begin() + index);
		m_SecondLineP1Added.erase(m_SecondLineP1Added.begin() + index);
		m_SecondLineP2Added.erase(m_SecondLineP2Added.begin() + index);

		Render();
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::SelectMeasure(int index)
{
	if (GetMeasureCount() > 0)
	{
		// Deselect all
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

			m_PointsStackVectorA[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorB[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorC[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorD[i]->SetColor(m_Colors[col]);
			m_LineStackVectorAB[i]->SetColor(m_Colors[col]);
			m_LineStackVectorCD[i]->SetColor(m_Colors[col]);
			m_CircleStackVector[i]->SetColor(m_Colors[col]);
			SetColor(m_TextActorVector[i], &m_Colors[col]);
		}

		if (index >= 0)
		{
			m_PointsStackVectorA[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorB[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorC[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorD[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_LineStackVectorAB[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_LineStackVectorCD[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_CircleStackVector[index]->SetColor(m_ColorAux);
			SetColor(m_TextActorVector[index], &m_Colors[COLOR_SELECTION]);

			albaEventMacro(albaEvent(this, ID_MEASURE_SELECTED));
		}

		m_LastSelection = index;
		m_LastEditing = -1;
	}
}

/// SET/GET
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::SetPointSize(double size)
{
	m_PointSize = size;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVectorA[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorB[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorC[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorD[i]->GetProperty()->SetPointSize(m_PointSize);
	}
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::SetLineWidth(double width)
{
	m_LineWidth = width;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_LineStackVectorAB[i]->GetProperty()->SetLineWidth(m_LineWidth);
		m_LineStackVectorCD[i]->GetProperty()->SetLineWidth(m_LineWidth);
		m_CircleStackVector[i]->GetProperty()->SetLineWidth(m_LineWidth);
	}
	Render();
}

/// UTILS ///////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::GetMeasureLinePoints(int index, double *point1, double *point2, double *point3, double *point4)
{
	// Return line points values
	if (index >= 0 && index < GetMeasureCount())
	{
		vtkPointSource* pointSourceA = (vtkPointSource*)m_PointsStackVectorA[index]->GetSource();
		vtkPointSource* pointSourceB = (vtkPointSource*)m_PointsStackVectorB[index]->GetSource();
		vtkPointSource* pointSourceC = (vtkPointSource*)m_PointsStackVectorC[index]->GetSource();
		vtkPointSource* pointSourceD = (vtkPointSource*)m_PointsStackVectorD[index]->GetSource();

		pointSourceA->GetCenter(point1);
		pointSourceB->GetCenter(point2);
		pointSourceC->GetCenter(point3);
		pointSourceD->GetCenter(point4);
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_AngleLine::GetCenter(int index, double *center)
{
	// Return center points values
	if (index >= 0 && index < GetMeasureCount())
	{
		vtkALBACircleSource* circleSource = (vtkALBACircleSource*)m_CircleStackVector[index]->GetSource();
		circleSource->GetCenter(center);
	}
}

/// LOAD/SAVE ///////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_AngleLine::Load(albaVME *input, wxString tag)
{
	if (input->GetTagArray()->IsTagPresent(tag + "MeasureAngleLinePoint1") && input->GetTagArray()->IsTagPresent(tag + "MeasureAngleLinePoint1"))
	{
		double point1[3], point2[3], point3[3], point4[3];
		albaTagItem *measureTypeTag = input->GetTagArray()->GetTag(tag + "MeasureType");
		albaTagItem *measureLabelTag = input->GetTagArray()->GetTag(tag + "MeasureAngleLineLabel");
		albaTagItem *MeasureAngleLinePoint1Tag = input->GetTagArray()->GetTag(tag + "MeasureAngleLinePoint1");
		albaTagItem *MeasureAngleLinePoint2Tag = input->GetTagArray()->GetTag(tag + "MeasureAngleLinePoint2");
		albaTagItem *MeasureAngleLinePoint3Tag = input->GetTagArray()->GetTag(tag + "MeasureAngleLinePoint3");
		albaTagItem *MeasureAngleLinePoint4Tag = input->GetTagArray()->GetTag(tag + "MeasureAngleLinePoint3");
		albaTagItem *MeasureAngleLineTag = input->GetTagArray()->GetTag(tag + "MeasureAngleLine");
		
		int nAngles = MeasureAngleLinePoint1Tag->GetNumberOfComponents() / 2;

		// Reload points
		for (int i = 0; i < nAngles; i++)
		{
			point1[X] = MeasureAngleLinePoint1Tag->GetValueAsDouble(i * 3 + 0);
			point1[Y] = MeasureAngleLinePoint1Tag->GetValueAsDouble(i * 3 + 1);
			point1[Z] = MeasureAngleLinePoint1Tag->GetValueAsDouble(i * 3 + 2);

			point2[X] = MeasureAngleLinePoint2Tag->GetValueAsDouble(i * 3 + 0);
			point2[Y] = MeasureAngleLinePoint2Tag->GetValueAsDouble(i * 3 + 1);
			point2[Z] = MeasureAngleLinePoint2Tag->GetValueAsDouble(i * 3 + 2);

			point3[X] = MeasureAngleLinePoint3Tag->GetValueAsDouble(i * 3 + 0);
			point3[Y] = MeasureAngleLinePoint3Tag->GetValueAsDouble(i * 3 + 1);
			point3[Z] = MeasureAngleLinePoint3Tag->GetValueAsDouble(i * 3 + 2);

			point4[X] = MeasureAngleLinePoint4Tag->GetValueAsDouble(i * 3 + 0);
			point4[Y] = MeasureAngleLinePoint4Tag->GetValueAsDouble(i * 3 + 1);
			point4[Z] = MeasureAngleLinePoint4Tag->GetValueAsDouble(i * 3 + 2);

			albaString measureType = measureTypeTag->GetValue(i);
			albaString measureLabel = measureLabelTag->GetValue(i);

			AddMeasure(point1, point2, point3, point4);
			m_CurrMeasure = m_Angles.size() - 1;
			m_SecondLineP1Added[m_CurrMeasure] = true;
			m_SecondLineP2Added[m_CurrMeasure] = true;
			m_Angles[m_CurrMeasure] = MeasureAngleLineTag->GetValueAsDouble(i);

			UpdateLineActors(point1, point2, point3, point4);
			m_CurrMeasure = -1;
			SetMeasureLabel(i, measureLabel);
		}

		Render();

		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_AngleLine::Save(albaVME *input, wxString tag)
{
	bool result = false;
	int nAngles = GetMeasureCount();

	if (nAngles > 0)
	{
		albaTagItem measureTypeTag;
		measureTypeTag.SetName(tag + "MeasureType");
		measureTypeTag.SetNumberOfComponents(nAngles);

		albaTagItem measureLabelTag;
		measureLabelTag.SetName(tag + "MeasureAngleLineLabel");
		measureLabelTag.SetNumberOfComponents(nAngles);

		albaTagItem MeasureAngleLinePoint1Tag;
		MeasureAngleLinePoint1Tag.SetName(tag + "MeasureAngleLinePoint1");
		MeasureAngleLinePoint1Tag.SetNumberOfComponents(nAngles);

		albaTagItem MeasureAngleLinePoint2Tag;
		MeasureAngleLinePoint2Tag.SetName(tag + "MeasureAngleLinePoint2");
		MeasureAngleLinePoint2Tag.SetNumberOfComponents(nAngles);

		albaTagItem MeasureAngleLinePoint3Tag;
		MeasureAngleLinePoint3Tag.SetName(tag + "MeasureAngleLinePoint3");
		MeasureAngleLinePoint3Tag.SetNumberOfComponents(nAngles);

		albaTagItem MeasureAngleLinePoint4Tag;
		MeasureAngleLinePoint4Tag.SetName(tag + "MeasureAngleLinePoint4");
		MeasureAngleLinePoint4Tag.SetNumberOfComponents(nAngles);

		albaTagItem MeasureAngleLineTag;
		MeasureAngleLineTag.SetName(tag + "MeasureAngleLine");
		MeasureAngleLineTag.SetNumberOfComponents(nAngles);


		for (int i = 0; i < nAngles; i++)
		{
			double point1[3], point2[3], point3[3], point4[3];
			GetMeasureLinePoints(i, point1, point2, point3, point4);

			measureTypeTag.SetValue(GetTypeName(), i);
			measureLabelTag.SetValue(GetMeasureLabel(i), i);

			MeasureAngleLinePoint1Tag.SetValue(point1[X], i * 3 + 0);
			MeasureAngleLinePoint1Tag.SetValue(point1[Y], i * 3 + 1);
			MeasureAngleLinePoint1Tag.SetValue(point1[Z], i * 3 + 2);

			MeasureAngleLinePoint2Tag.SetValue(point2[X], i * 3 + 0);
			MeasureAngleLinePoint2Tag.SetValue(point2[Y], i * 3 + 1);
			MeasureAngleLinePoint2Tag.SetValue(point2[Z], i * 3 + 2);

			MeasureAngleLinePoint3Tag.SetValue(point3[X], i * 3 + 0);
			MeasureAngleLinePoint3Tag.SetValue(point3[Y], i * 3 + 1);
			MeasureAngleLinePoint3Tag.SetValue(point3[Z], i * 3 + 2);

			MeasureAngleLinePoint4Tag.SetValue(point4[X], i * 3 + 0);
			MeasureAngleLinePoint4Tag.SetValue(point4[Y], i * 3 + 1);
			MeasureAngleLinePoint4Tag.SetValue(point4[Z], i * 3 + 2);

			MeasureAngleLineTag.SetValue(m_Angles[i], i);
		}

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureType"))
			input->GetTagArray()->DeleteTag(tag + "MeasureType");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureAngleLineLabel"))
			input->GetTagArray()->DeleteTag(tag + "MeasureAngleLineLabel");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureAngleLinePoint1"))
			input->GetTagArray()->DeleteTag(tag + "MeasureAngleLinePoint1");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureAngleLinePoint2"))
			input->GetTagArray()->DeleteTag(tag + "MeasureAngleLinePoint2");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureAngleLinePoint3"))
			input->GetTagArray()->DeleteTag(tag + "MeasureAngleLinePoint3");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureAngleLinePoint4"))
			input->GetTagArray()->DeleteTag(tag + "MeasureAngleLinePoint4");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureAngleLine"))
			input->GetTagArray()->DeleteTag(tag + "MeasureAngleLine");

		input->GetTagArray()->SetTag(measureTypeTag);
		input->GetTagArray()->SetTag(measureLabelTag);
		input->GetTagArray()->SetTag(MeasureAngleLinePoint1Tag);
		input->GetTagArray()->SetTag(MeasureAngleLinePoint2Tag);
		input->GetTagArray()->SetTag(MeasureAngleLinePoint3Tag);
		input->GetTagArray()->SetTag(MeasureAngleLinePoint4Tag);
		input->GetTagArray()->SetTag(MeasureAngleLineTag);

		result = true;
	}

	return result;
}

//----------------------------------------------------------------------------
double albaInteractor2DMeasure_AngleLine::GetMeasureAngle(int idx)
{
	if (idx >= 0 && idx < m_Angles.size())
		return fabs(m_Angles[idx]);
	else
		return 0;
}
//----------------------------------------------------------------------------
double albaInteractor2DMeasure_AngleLine::CalculateAngle(double * point1, double * point2, double * point3, double * point4)
{
	// Translate Second Line
	double diff[3] = { point3[X] - point2[X], point3[Y] - point2[Y], point3[Z] - point2[Z] };
	double newP4[3] = { point4[X] - diff[X], point4[Y] - diff[Y], point4[Z] - diff[Z] };

	double diff2[3] = { point4[X] - point2[X], point4[Y] - point2[Y], point4[Z] - point2[Z] };
	double newP3[3] = { point3[X] - diff2[X], point3[Y] - diff2[Y], point3[Z] - diff2[Z] };

	// Calculate Smaller angle
	double angle1 = GetAngle(point1, newP4, point2);
	double angle2 = GetAngle(point1, newP3, point2);
	double angle3 = GetAngle(newP4, point1, point2);
	double angle4 = GetAngle(newP3, point1, point2);

	return MIN(MIN(angle1, angle2), MIN(angle3, angle4));
	
	//return GetAngle(point1, newP4, point2);
}