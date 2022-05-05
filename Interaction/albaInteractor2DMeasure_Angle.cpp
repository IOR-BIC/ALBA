/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Angle.cpp
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

#include "albaInteractor2DMeasure_Angle.h"
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

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DMeasure_Angle)

//----------------------------------------------------------------------------
albaInteractor2DMeasure_Angle::albaInteractor2DMeasure_Angle() : albaInteractor2DMeasure()
{
	m_ShowText = false;
	m_TextSide = 1;

	m_MeasureTypeText = "ANGLE";

	m_CurrPoint = NO_POINT;

	m_LineExtensionLength = 1.0;
	m_LineStipplePattern = 0xFFFF;

	Color color{ 0.4, 0.4, 1, 1.0 };

	SetColorDefault(color.R, color.G, color.B, 0.85);
	SetColorSelection(color.R, color.G, color.B, 1.0);
	SetColorDisable(color.R, color.G, color.B, 0.3);
	SetColorText(color.R, color.G, color.B, 0.5);

// 	m_ColorAux.R = 0.0;
// 	m_ColorAux.G = 1.0;
// 	m_ColorAux.B = 0.0;
// 	m_ColorAux.Alpha = 0.5;
	m_ColorAux = { 0.0, 1.0, 0.0, 0.5 };
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure_Angle::~albaInteractor2DMeasure_Angle()
{
	// Lines
	for (int i = 0; i < m_PointsStackVectorOri.size(); i++)
	{
		cppDEL(m_PointsStackVectorOri[i]);
		cppDEL(m_PointsStackVectorA[i]);
		cppDEL(m_PointsStackVectorB[i]);
		cppDEL(m_LineStackVectorOA[i]);
		cppDEL(m_LineStackVectorOB[i]);
		cppDEL(m_CircleStackVector[i]);
	}

	m_SecondLineAdded.clear();
}

/// RENDERING ////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::DrawNewMeasure(double * wp)
{
	AddMeasure(wp, wp, wp);

	// Call FindAndHighLight to start the edit phase
	FindAndHighlight(wp);
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::MoveMeasure(int index, double *point)
{
	if (index < 0)
		return;

	double linePointO[3], linePointA[3], linePointB[3];

	vtkPointSource* pointSourceO = (vtkPointSource*)m_PointsStackVectorOri[index]->GetSource();
	vtkPointSource* pointSourceA = (vtkPointSource*)m_PointsStackVectorA[index]->GetSource();
	vtkPointSource* pointSourceB = (vtkPointSource*)m_PointsStackVectorB[index]->GetSource();

	pointSourceO->GetCenter(linePointO);
	pointSourceA->GetCenter(linePointA);
	pointSourceB->GetCenter(linePointB);

	if (!m_MovingMeasure)
	{
		m_OldLineP1[X] = linePointO[X] - m_StartMousePosition[X];
		m_OldLineP1[Y] = linePointO[Y] - m_StartMousePosition[Y];
		m_OldLineP1[Z] = linePointO[Z] - m_StartMousePosition[Z];

		m_OldLineP2[X] = linePointA[X] - m_StartMousePosition[X];
		m_OldLineP2[Y] = linePointA[Y] - m_StartMousePosition[Y];
		m_OldLineP2[Z] = linePointA[Z] - m_StartMousePosition[Z];

		m_OldLineP3[X] = linePointB[X] - m_StartMousePosition[X];
		m_OldLineP3[Y] = linePointB[Y] - m_StartMousePosition[Y];
		m_OldLineP3[Z] = linePointB[Z] - m_StartMousePosition[Z];

		m_MovingMeasure = true;
	}

	// Initialization
	m_MeasureValue = 0.0;

	double tmp_posO[3] = { point[X] + m_OldLineP1[X] , point[Y] + m_OldLineP1[Y] , point[Z] + m_OldLineP1[Z] };
	double tmp_posA[3] = { point[X] + m_OldLineP2[X] , point[Y] + m_OldLineP2[Y] , point[Z] + m_OldLineP2[Z] };
	double tmp_posB[3] = { point[X] + m_OldLineP3[X] , point[Y] + m_OldLineP3[Y] , point[Z] + m_OldLineP3[Z] };

	m_MeasureValue = GetAngle(tmp_posA, tmp_posB, tmp_posO);

	// Points
	UpdatePointsActor(tmp_posO, tmp_posA, tmp_posB);
	// Lines
	UpdateLineActors(tmp_posO, tmp_posA, tmp_posB);
	// Circle
	UpdateCircleActor(tmp_posA, tmp_posB, tmp_posO);

	//called on mouse up  
	if (!m_DraggingLeft)
	{
		m_ActorAdded = false;
	}

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::EditMeasure(int index, double *point)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	m_MovingMeasure = true;

	double pointO[3], pointA[3], pointB[3];

	vtkPointSource* pointSourceO = (vtkPointSource*)m_PointsStackVectorOri[index]->GetSource();
	vtkPointSource* pointSourceA = (vtkPointSource*)m_PointsStackVectorA[index]->GetSource();
	vtkPointSource* pointSourceB = (vtkPointSource*)m_PointsStackVectorB[index]->GetSource();

	pointSourceO->GetCenter(pointO);
	pointSourceA->GetCenter(pointA);
	pointSourceB->GetCenter(pointB);
	
	if (m_CurrPoint == POINT_1)
	{
		pointO[X] = point[X];
		pointO[Y] = point[Y];
		pointO[Z] = point[Z];
	}
	else if (m_CurrPoint == POINT_2)
	{
		pointA[X] = point[X];
		pointA[Y] = point[Y];
		pointA[Z] = point[Z];
	}
	else if (m_CurrPoint == POINT_3)
	{
		pointB[X] = point[X];
		pointB[Y] = point[Y];
		pointB[Z] = point[Z];
	}

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////
	
	double angle = GetAngle(pointA, pointB, pointO);

	// Update Measure
	UpdateMeasure(index, angle);

	// Points
	UpdatePointsActor(pointO, pointA, pointB);
	// Line
	UpdateLineActors(pointO, pointA, pointB);
	// Circle
	UpdateCircleActor(pointA, pointB, pointO);
	// Text
	UpdateTextActor(pointO, pointA);

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::FindAndHighlight(double * point)
{
	if (m_CurrMeasure < 0)
		SetAction(ACTION_ADD_MEASURE);

	if (m_EditMeasureEnable)
	{
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			albaActor2dStackHelper *pointStackVector = m_PointsStackVectorOri[i];
			if (m_Renderer != pointStackVector->GetRenderer())
				continue;

			double pointO[3], pointA[3], pointB[3];

			vtkPointSource* pointSourceO = (vtkPointSource*)m_PointsStackVectorOri[i]->GetSource();
			vtkPointSource* pointSourceA = (vtkPointSource*)m_PointsStackVectorA[i]->GetSource();
			vtkPointSource* pointSourceB = (vtkPointSource*)m_PointsStackVectorB[i]->GetSource();

			pointSourceO->GetCenter(pointO);
			pointSourceA->GetCenter(pointA);
			pointSourceB->GetCenter(pointB);

			if (DistancePointToLine(point, pointO, pointA) < POINT_UPDATE_DISTANCE)
			{
				SelectMeasure(i);

				if (vtkMath::Distance2BetweenPoints(pointA, point) < POINT_UPDATE_DISTANCE_2)
				{
					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_2;
					m_PointsStackVectorA[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else if (vtkMath::Distance2BetweenPoints(pointO, point) < POINT_UPDATE_DISTANCE_2)
				{
					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_1;
					m_PointsStackVectorOri[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else
				{
					m_CurrMeasure = i;
					if (m_MoveMeasureEnable)
					{
						m_LineStackVectorOA[i]->SetColor(m_Colors[COLOR_EDIT]);
						SetAction(ACTION_MOVE_MEASURE);
					}

					m_MoveLineB = false;
				}

				Render();
				return;
			}
			else if (DistancePointToLine(point, pointO, pointB) < POINT_UPDATE_DISTANCE)
			{
				SelectMeasure(i);

				if (vtkMath::Distance2BetweenPoints(pointB, point) < POINT_UPDATE_DISTANCE_2)
				{
					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_3;
					m_PointsStackVectorB[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else
				{
					m_CurrMeasure = i;
					if (m_MoveMeasureEnable)
					{
						m_LineStackVectorOB[i]->SetColor(m_Colors[COLOR_EDIT]);
						SetAction(ACTION_MOVE_MEASURE);
					}

					m_MoveLineB = true;
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
			Render();
		}
	}
}

/// UPDATE ///////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::UpdateMeasure(int index, double measure)
{
	double angleDegree = measure * vtkMath::RadiansToDegrees();
	angleDegree = abs((angleDegree <= 180.0) ? angleDegree : 360.0 - angleDegree);

	albaString text;
	text.Printf("Angle %.2f°", angleDegree);
	m_Measure2DVector[index].Text = text;
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::UpdatePointsActor(double * point1, double * point2, double * point3)
{
	//Point O (origin)
	vtkPointSource* pointSourceO = (vtkPointSource*)m_PointsStackVectorOri[m_CurrMeasure]->GetSource();
	pointSourceO->SetCenter(point1);
	pointSourceO->Update();

	//Point A
	vtkPointSource* pointSourceA = (vtkPointSource*)m_PointsStackVectorA[m_CurrMeasure]->GetSource();
	pointSourceA->SetCenter(point2);
	pointSourceA->Update();

	//Point B
	vtkPointSource* pointSourceB = (vtkPointSource*)m_PointsStackVectorB[m_CurrMeasure]->GetSource();
	pointSourceB->SetCenter(point3);
	pointSourceB->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::UpdateLineActors(double * point1, double * point2, double * point3)
{
	vtkLineSource* lineSourceOA = (vtkLineSource*)m_LineStackVectorOA[m_CurrMeasure]->GetSource();
	vtkLineSource* lineSourceOB = (vtkLineSource*)m_LineStackVectorOB[m_CurrMeasure]->GetSource();
	
	lineSourceOA->SetPoint1(point1);
	lineSourceOA->SetPoint2(point2);
	lineSourceOA->Update();

	lineSourceOB->SetPoint1(point1);
	lineSourceOB->SetPoint2(point3);
	lineSourceOB->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::UpdateCircleActor(double * point1, double * point2, double * ori)
{
	double radius = MIN(DistanceBetweenPoints(point1, ori), DistanceBetweenPoints(point2, ori)) * 0.8;

	int A = 0, B = 1, C = 2;

	if (m_CurrPlane == 0) { A = 0; B = 1; C = 2; }; //XY
	if (m_CurrPlane == 1) { A = 1; B = 2; C = 0; }; //YZ
	if (m_CurrPlane == 2) { A = 0; B = 2; C = 1; }; //XZ

	double axis[3];
	axis[A] = ori[A];
	axis[B] = ori[B] + 1.0;
	axis[C] = ori[C];

	double angle = GetAngle(point1, point2, ori);
	double angle1 = GetAngle(axis, point1, ori);
	double angle2 = GetAngle(point1, point2, ori) + angle1;

	vtkALBACircleSource *circleSource = (vtkALBACircleSource *)m_CircleStackVector[m_CurrMeasure]->GetSource();

	circleSource->SetPlane(m_CurrPlane);
	circleSource->SetRadius(radius);
	circleSource->SetCenter(ori);

	if (angle <= vtkMath::Pi())
		circleSource->SetAngleRange(angle1, angle2);
	else
		circleSource->SetAngleRange(angle2, angle1 + vtkMath::Pi() * 2);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::UpdateTextActor(double * point1, double * point2)
{
	double text_pos[3];
	GetMidPoint(text_pos, point1, point2);

	text_pos[X] += m_TextSide *TEXT_W_SHIFT;
	text_pos[Y] -= m_TextSide *TEXT_H_SHIFT;

	Superclass::UpdateTextActor(m_CurrMeasure, text_pos);
}

/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::AddMeasure(double *point1, double *point2, double *point3)
{
	if (GetMeasureCount() > 0)
	{
		int index = GetMeasureCount() - 1;
		double oldPoint1[3], oldPoint2[3], oldPoint3[3];
		GetMeasureLinePoints(index, oldPoint1, oldPoint2, oldPoint3);

		bool hasSameRenderer = (m_Renderer == m_Measure2DVector[index].Renderer);

		if (DistanceBetweenPoints(oldPoint1, oldPoint2) < POINT_UPDATE_DISTANCE)
		{
			if (!hasSameRenderer) return;

			m_CurrMeasure = index;
			m_CurrPoint = POINT_2;
			EditMeasure(index, point2);
			return;
		}
		else if (m_SecondLineAdded[index] == false)
		{
			if (!hasSameRenderer) return;

			//Adding the second line no need to add a new measure.
			m_SecondLineAdded[index] = true;
			
			m_CurrMeasure = index;
			m_CurrPoint = POINT_3;
			EditMeasure(index, point3);

			return;
		}
	}

	Superclass::AddMeasure(point1, point2);

	//////////////////////////////////////////////////////////////////////////

	int index = m_Measure2DVector.size() - 1;
	double angle = GetAngle(point2, point3, point1);

	// Update Measure
	UpdateMeasure(index, angle);

	// Update Edit Actors
	UpdateEditActors(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Add Points
	m_PointsStackVectorOri.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorA.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorB.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));

	// Add Lines
	m_LineStackVectorOA.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));
	m_LineStackVectorOB.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));

	// Add Circle
	m_CircleStackVector.push_back(new albaActor2dStackHelper(vtkALBACircleSource::New(), m_Renderer));

	m_SecondLineAdded.push_back(false);
	m_Angles.push_back(0);

	//////////Setting proprieties//////////////
	int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

	//Point O (Origin)
	m_PointsStackVectorOri[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorOri[index]->SetColor(m_Colors[col]);

	//Point A
	m_PointsStackVectorA[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorA[index]->SetColor(m_Colors[col]);

	//Point B
	m_PointsStackVectorB[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorB[index]->SetColor(m_Colors[col]);

	//Line OA
	m_LineStackVectorOA[index]->SetColor(m_Colors[col]);
	m_LineStackVectorOA[index]->GetProperty()->SetLineWidth(m_LineWidth);
	m_LineStackVectorOA[index]->GetProperty()->SetLineStipplePattern(m_LineStipplePattern);

	//Line OB
	m_LineStackVectorOB[index]->SetColor(m_Colors[col]);
	m_LineStackVectorOB[index]->GetProperty()->SetLineWidth(m_LineWidth);
	m_LineStackVectorOB[index]->GetProperty()->SetLineStipplePattern(m_LineStipplePattern);

	// Circle
	m_CircleStackVector[index]->GetProperty()->SetLineWidth(m_LineWidth);
	m_CircleStackVector[index]->SetColor(m_ColorAux);
	m_CircleStackVector[index]->GetProperty()->SetLineStipplePattern(0xf0f0);

	vtkALBACircleSource *circleSource = (vtkALBACircleSource *)m_CircleStackVector[index]->GetSource();
	circleSource->SetResolution(60);

	m_CurrMeasure = index;

	UpdatePointsActor(point1, point2, point3);
	UpdateLineActors(point1, point2, point3);
	UpdateCircleActor(point2, point3, point1);

	//////////////////////////////////////////////////////////////////////////
	albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasureText(GetMeasureCount() - 1)));

	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::RemoveMeasure(int index)
{
	if (m_Renderer && index < GetMeasureCount())
	{
		Superclass::RemoveMeasure(index);

		//////////////////////////////////////////////////////////////////////////

		//Point O (Origin)
		cppDEL(m_PointsStackVectorOri[index]);
		m_PointsStackVectorOri.erase(m_PointsStackVectorOri.begin() + index);
		//Point A
		cppDEL(m_PointsStackVectorA[index]);
		m_PointsStackVectorA.erase(m_PointsStackVectorA.begin() + index);
		//Point B
		cppDEL(m_PointsStackVectorB[index]);
		m_PointsStackVectorB.erase(m_PointsStackVectorB.begin() + index);
		
		//Line OA
		cppDEL(m_LineStackVectorOA[index]);
		m_LineStackVectorOA.erase(m_LineStackVectorOA.begin() + index);
		//Line OB
		cppDEL(m_LineStackVectorOB[index]);
		m_LineStackVectorOB.erase(m_LineStackVectorOB.begin() + index);

		// Circle
		cppDEL(m_CircleStackVector[index]);
		m_CircleStackVector.erase(m_CircleStackVector.begin() + index);

		m_Angles.erase(m_Angles.begin() + index);

		m_SecondLineAdded.erase(m_SecondLineAdded.begin() + index);

		Render();
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::SelectMeasure(int index)
{
	if (GetMeasureCount() > 0)
	{
		// Deselect all
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;
		
			m_PointsStackVectorOri[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorA[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorB[i]->SetColor(m_Colors[col]);
			m_LineStackVectorOA[i]->SetColor(m_Colors[col]);
			m_LineStackVectorOB[i]->SetColor(m_Colors[col]);
			m_CircleStackVector[i]->SetColor(m_Colors[col]);
			SetColor(m_TextActorVector[i], &m_Colors[col]);
		}

		if (index >= 0)
		{
			m_PointsStackVectorOri[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorA[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorB[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_LineStackVectorOA[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_LineStackVectorOB[index]->SetColor(m_Colors[COLOR_SELECTION]);
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
void albaInteractor2DMeasure_Angle::SetPointSize(double size)
{
	m_PointSize = size;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVectorOri[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorA[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorB[i]->GetProperty()->SetPointSize(m_PointSize);
	}
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::SetLineWidth(double width)
{
	m_LineWidth = width;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_LineStackVectorOA[i]->GetProperty()->SetLineWidth(m_LineWidth);
		m_LineStackVectorOB[i]->GetProperty()->SetLineWidth(m_LineWidth);
		m_CircleStackVector[i]->GetProperty()->SetLineWidth(m_LineWidth);
	}
	Render();
}

/// UTILS ///////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::GetMeasureLinePoints(int index, double *point1, double *point2, double *point3)
{
	// Return line points values
	if (index >= 0 && index < GetMeasureCount())
	{
		vtkPointSource* pointSourceO = (vtkPointSource*)m_PointsStackVectorOri[index]->GetSource();
		vtkPointSource* pointSourceA = (vtkPointSource*)m_PointsStackVectorA[index]->GetSource();
		vtkPointSource* pointSourceB = (vtkPointSource*)m_PointsStackVectorB[index]->GetSource();

		pointSourceO->GetCenter(point1);
		pointSourceA->GetCenter(point2);
		pointSourceB->GetCenter(point3);
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Angle::GetCenter(int index, double *center)
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
bool albaInteractor2DMeasure_Angle::Load(albaVME *input, wxString tag)
{
	if (input->GetTagArray()->IsTagPresent(tag + "MeasureAnglePoint1") && input->GetTagArray()->IsTagPresent(tag + "MeasureAnglePoint1"))
	{
		double point1[3], point2[3], point3[3];
		albaTagItem *measureTypeTag = input->GetTagArray()->GetTag(tag + "MeasureType");
		albaTagItem *measureLabelTag = input->GetTagArray()->GetTag(tag + "MeasureAngleLabel");
		albaTagItem *measureAnglePoint1Tag = input->GetTagArray()->GetTag(tag + "MeasureAnglePoint1");
		albaTagItem *measureAnglePoint2Tag = input->GetTagArray()->GetTag(tag + "MeasureAnglePoint2");
		albaTagItem *measureAnglePoint3Tag = input->GetTagArray()->GetTag(tag + "MeasureAnglePoint3");
		albaTagItem *measureAngleTag = input->GetTagArray()->GetTag(tag + "MeasureAngle");
		
		int nAngles = measureAnglePoint1Tag->GetNumberOfComponents() / 2;

		// Reload points
		for (int i = 0; i < nAngles; i++)
		{
			point1[X] = measureAnglePoint1Tag->GetValueAsDouble(i * 2 + 0);
			point1[Y] = measureAnglePoint1Tag->GetValueAsDouble(i * 2 + 1);
			point1[Z] = measureAnglePoint1Tag->GetValueAsDouble(i * 2 + 2);

			point2[X] = measureAnglePoint2Tag->GetValueAsDouble(i * 2 + 0);
			point2[Y] = measureAnglePoint2Tag->GetValueAsDouble(i * 2 + 1);
			point2[Z] = measureAnglePoint2Tag->GetValueAsDouble(i * 2 + 2);

			point3[X] = measureAnglePoint3Tag->GetValueAsDouble(i * 2 + 0);
			point3[Y] = measureAnglePoint3Tag->GetValueAsDouble(i * 2 + 1);
			point3[Z] = measureAnglePoint3Tag->GetValueAsDouble(i * 2 + 2);

			albaString measureType = measureTypeTag->GetValue(i);
			albaString measureLabel = measureLabelTag->GetValue(i);

			AddMeasure(point1, point2, point3);
			m_CurrMeasure = m_Angles.size() - 1;
			m_SecondLineAdded[m_CurrMeasure] = true;
			m_Angles[m_CurrMeasure] = measureAngleTag->GetValueAsDouble(i);
			UpdateLineActors(point1, point2, point3);
			m_CurrMeasure = -1;
			SetMeasureLabel(i, measureLabel);
		}

		Render();

		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_Angle::Save(albaVME *input, wxString tag)
{
	bool result = false;
	int nAngles = GetMeasureCount();

	if (nAngles > 0)
	{
		albaTagItem measureTypeTag;
		measureTypeTag.SetName(tag + "MeasureType");
		measureTypeTag.SetNumberOfComponents(nAngles);

		albaTagItem measureLabelTag;
		measureLabelTag.SetName(tag + "MeasureAngleLabel");
		measureLabelTag.SetNumberOfComponents(nAngles);

		albaTagItem measureAnglePoint1Tag;
		measureAnglePoint1Tag.SetName(tag + "MeasureAnglePoint1");
		measureAnglePoint1Tag.SetNumberOfComponents(nAngles);

		albaTagItem measureAnglePoint2Tag;
		measureAnglePoint2Tag.SetName(tag + "MeasureAnglePoint2");
		measureAnglePoint2Tag.SetNumberOfComponents(nAngles);

		albaTagItem measureAnglePoint3Tag;
		measureAnglePoint3Tag.SetName(tag + "MeasureAnglePoint3");
		measureAnglePoint3Tag.SetNumberOfComponents(nAngles);

		albaTagItem measureAngleTag;
		measureAngleTag.SetName(tag + "MeasureAngle");
		measureAngleTag.SetNumberOfComponents(nAngles);


		for (int i = 0; i < nAngles; i++)
		{
			double point1[3], point2[3], point3[3];
			GetMeasureLinePoints(i, point1, point2, point3);

			measureTypeTag.SetValue(GetTypeName(), i);
			measureLabelTag.SetValue(GetMeasureLabel(i), i);

			measureAnglePoint1Tag.SetValue(point1[X], i * 2 + 0);
			measureAnglePoint1Tag.SetValue(point1[Y], i * 2 + 1);
			measureAnglePoint1Tag.SetValue(point1[Z], i * 2 + 2);

			measureAnglePoint2Tag.SetValue(point2[X], i * 2 + 0);
			measureAnglePoint2Tag.SetValue(point2[Y], i * 2 + 1);
			measureAnglePoint2Tag.SetValue(point2[Z], i * 2 + 1);

			measureAnglePoint3Tag.SetValue(point3[X], i * 2 + 0);
			measureAnglePoint3Tag.SetValue(point3[Y], i * 2 + 1);
			measureAnglePoint3Tag.SetValue(point3[Z], i * 2 + 1);

			measureAngleTag.SetValue(m_Angles[i], i);
		}

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureType"))
			input->GetTagArray()->DeleteTag(tag + "MeasureType");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureAngleLabel"))
			input->GetTagArray()->DeleteTag(tag + "MeasureAngleLabel");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureAnglePoint1"))
			input->GetTagArray()->DeleteTag(tag + "MeasureAnglePoint1");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureAnglePoint2"))
			input->GetTagArray()->DeleteTag(tag + "MeasureAnglePoint2");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureAnglePoint3"))
			input->GetTagArray()->DeleteTag(tag + "MeasureAnglePoint3");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureAngle"))
			input->GetTagArray()->DeleteTag(tag + "MeasureAngle");

		input->GetTagArray()->SetTag(measureTypeTag);
		input->GetTagArray()->SetTag(measureLabelTag);
		input->GetTagArray()->SetTag(measureAnglePoint1Tag);
		input->GetTagArray()->SetTag(measureAnglePoint2Tag);
		input->GetTagArray()->SetTag(measureAnglePoint3Tag);
		input->GetTagArray()->SetTag(measureAngleTag);

		result = true;
	}

	return result;
}

//----------------------------------------------------------------------------
double albaInteractor2DMeasure_Angle::GetMeasureAngle(int idx)
{
	if (idx >= 0 && idx < m_Angles.size())
		return fabs(m_Angles[idx]);
	else
		return 0;
}