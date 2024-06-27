/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_CenterPoint.cpp
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

#include "albaInteractor2DMeasure_CenterPoint.h"
#include "albaInteractor2DMeasure.h"

#include "albaTagArray.h"
#include "albaVME.h"

#include "vtkALBATextActorMeter.h"
#include "vtkALBACircleSource.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkDataSet.h"
#include "vtkDataSetMapper.h"
#include "vtkDiskSource.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkPointSource.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkViewport.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DMeasure_CenterPoint)

//----------------------------------------------------------------------------
albaInteractor2DMeasure_CenterPoint::albaInteractor2DMeasure_CenterPoint() : albaInteractor2DMeasure()
{
	m_ShowText = false;
	m_TextSide = 1;

	m_MeasureTypeText = "CENTER_POINT";

	m_CurrPoint = NO_POINT;
	m_CenterPointSize = 5.0;

	Color color{ 0.6, 0.0, 0.6, 1.0 };

	SetColorDefault(color.R, color.G, color.B, 0.85);
	SetColorSelection(color.R, color.G, color.B, 1.0);
	SetColorDisable(color.R, color.G, color.B, 0.3);
	SetColorText(color.R, color.G, color.B, 0.5);
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure_CenterPoint::~albaInteractor2DMeasure_CenterPoint()
{
	// Lines and Points
	for (int i = 0; i < m_LineStackVector.size(); i++)
	{
		cppDEL(m_LineStackVector[i]);
		cppDEL(m_PointsStackVectorL[i]);
		cppDEL(m_PointsStackVectorR[i]);
		cppDEL(m_PointsStackVectorC[i]);		
		cppDEL(m_CircleStackVector[i]);
	}
}

/// RENDERING ////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::DrawNewMeasure(double * wp)
{
	AddMeasure(wp, wp);
	//Call FindAndHighLight to start the edit phase
	FindAndHighlight(wp);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::MoveMeasure(int index, double * point)
{
	if (index < 0)
		return;

	double linePoint1[3];
	double linePoint2[3];

	vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[index]->GetSourceAlgorithm();
	lineSource->GetPoint1(linePoint1);
	lineSource->GetPoint2(linePoint2);

	if (!m_MovingMeasure)
	{
		m_OldLineP1[X] = linePoint1[X] - m_StartMousePosition[X];
		m_OldLineP1[Y] = linePoint1[Y] - m_StartMousePosition[Y];
		m_OldLineP1[Z] = linePoint1[Z] - m_StartMousePosition[Z];

		m_OldLineP2[X] = linePoint2[X] - m_StartMousePosition[X];
		m_OldLineP2[Y] = linePoint2[Y] - m_StartMousePosition[Y];
		m_OldLineP2[Z] = linePoint2[Z] - m_StartMousePosition[Z];

		m_MovingMeasure = true;
	}

	// Initialization
	m_MeasureValue = 0.0;

	double tmp_pos1[3];
	double tmp_pos2[3];

	tmp_pos1[X] = point[X] + m_OldLineP1[X];
	tmp_pos1[Y] = point[Y] + m_OldLineP1[Y];
	tmp_pos1[Z] = point[Z] + m_OldLineP1[Z];

	tmp_pos2[X] = point[X] + m_OldLineP2[X];
	tmp_pos2[Y] = point[Y] + m_OldLineP2[Y];
	tmp_pos2[Z] = point[Z] + m_OldLineP2[Z];

	m_MeasureValue = DistanceBetweenPoints(tmp_pos1, tmp_pos2);

	UpdateLineActors(tmp_pos1, tmp_pos2);
	// Points
	UpdatePointsActor(tmp_pos1, tmp_pos2);
	// Circle
	UpdateCircleActor(tmp_pos1, tmp_pos2);
	// Text
	UpdateTextActor(tmp_pos1, tmp_pos2);

	//called on mouse up  
	if (!m_DraggingLeft)
	{
		m_ActorAdded = false;
	}

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::EditMeasure(int index, double *point)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	m_MovingMeasure = true;

	double point1[3];
	double point2[3];

	vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[index]->GetSourceAlgorithm();
	lineSource->GetPoint1(point1);
	lineSource->GetPoint2(point2);

	if (m_CurrPoint == POINT_1)
	{
		point1[X] = point[X];
		point1[Y] = point[Y];
		point1[Z] = point[Z];
	}
	else if (m_CurrPoint == POINT_2)
	{
		point2[X] = point[X];
		point2[Y] = point[Y];
		point2[Z] = point[Z];
	}

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	albaString text;
	text.Printf("Distance %.2f mm", DistanceBetweenPoints(point1, point2));
	//m_MeasureTextVector[index] = text;
	m_Measure2DVector[index].Text = text;

	// Line
	UpdateLineActors(point1, point2);
	// Points
	UpdatePointsActor(point1, point2);
	// Circle
	UpdateCircleActor(point1, point2);
	// Text
	UpdateTextActor(point1, point2);

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));

	//////////////////////////////////////////////////////////////////////////
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::FindAndHighlight(double * point)
{
	if (m_CurrMeasure < 0)
		SetAction(ACTION_ADD_MEASURE);

	if (m_EditMeasureEnable)
	{
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			albaActor2dStackHelper *lineStackVector = m_LineStackVector[i];
			if (m_Renderer != lineStackVector->GetRenderer())
				continue;

			double linePoint1[3], linePoint2[3];

			vtkLineSource* lineSource = (vtkLineSource*)lineStackVector->GetSourceAlgorithm();
			lineSource->GetPoint1(linePoint1);
			lineSource->GetPoint2(linePoint2);

			double centerPoint[3];
			GetMidPoint(centerPoint, linePoint1, linePoint2);

			double radius = vtkMath::Distance2BetweenPoints(linePoint2, centerPoint);

			if (DistancePointToLine(point, linePoint1, linePoint2) < POINT_UPDATE_DISTANCE)
			{
				SelectMeasure(i); 

				double p1Dist = DistanceBetweenPoints(point,linePoint1);
				double p2Dist = DistanceBetweenPoints(point,linePoint2);
				double p1p2Dist = DistanceBetweenPoints(linePoint1, linePoint2);
				double minDist = MIN(POINT_UPDATE_DISTANCE, (p1p2Dist/3.0));
				
				if ((p1Dist < p2Dist) && (p1Dist <= minDist))
				{
					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_1;
					m_PointsStackVectorL[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else if (p2Dist <= minDist)
				{
					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_2;
					m_PointsStackVectorR[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else if (vtkMath::Distance2BetweenPoints(centerPoint, point) < POINT_UPDATE_DISTANCE_2)
				{
					SetAction(ACTION_MOVE_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_1;
					m_PointsStackVectorC[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
// 				else
// 				{
// 					m_CurrMeasure = i;
// 					if (m_MoveMeasureEnable)
// 					{
// 						m_LineStackVector[i]->SetColor(m_Colors[COLOR_EDIT]);
// 						SetAction(ACTION_MOVE_MEASURE);
// 					}
// 				}
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
void albaInteractor2DMeasure_CenterPoint::UpdatePointsActor(double * point1, double * point2)
{
	// Left
	vtkPointSource* pointSourceL = (vtkPointSource*)m_PointsStackVectorL[m_CurrMeasure]->GetSourceAlgorithm();
	pointSourceL->SetCenter(point1);
	pointSourceL->Update();

	// Right
	vtkPointSource* pointSourceR = (vtkPointSource*)m_PointsStackVectorR[m_CurrMeasure]->GetSourceAlgorithm();
	pointSourceR->SetCenter(point2);
	pointSourceR->Update();

	// Center
	double pointC[3];
	GetMidPoint(pointC, point1, point2);

	vtkPointSource* pointSourceC = (vtkPointSource*)m_PointsStackVectorC[m_CurrMeasure]->GetSourceAlgorithm();
	pointSourceC->SetCenter(pointC);
	pointSourceC->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::UpdateLineActors(double * point1, double * point2)
{
	vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[m_CurrMeasure]->GetSourceAlgorithm();
	lineSource->SetPoint1(point1);
	lineSource->SetPoint2(point2);
	lineSource->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::UpdateCircleActor(double * point1, double * point2)
{
	double radius = DistanceBetweenPoints(point1, point2) / 2;

	double midPoint[3]; 
	GetMidPoint(midPoint, point1, point2);

	vtkALBACircleSource *circleSource = (vtkALBACircleSource *)m_CircleStackVector[m_CurrMeasure]->GetSourceAlgorithm();
	circleSource->SetPlane(m_CurrPlane);
	circleSource->SetRadius(radius);
	circleSource->SetCenter(midPoint);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::UpdateTextActor(double * point1, double * point2)
{
	double text_pos[3];
	GetMidPoint(text_pos, point1, point2);

	text_pos[X] += m_TextSide *TEXT_W_SHIFT;

	Superclass::UpdateTextActor(m_CurrMeasure, text_pos);
}

/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::AddMeasure(double *point1, double *point2)
{
	if (GetMeasureCount() > 0)
	{
		int index = GetMeasureCount() - 1;
		double oldPoint1[3], oldPoint2[3];
		GetMeasureLinePoints(index, oldPoint1, oldPoint2);

		bool hasSameRenderer = (m_Renderer == m_Measure2DVector[index].Renderer);

		if (DistanceBetweenPoints(oldPoint1, oldPoint2)<POINT_UPDATE_DISTANCE)
		{
			if (!hasSameRenderer) return;

			m_CurrMeasure = index;
			m_CurrPoint = POINT_2;
			EditMeasure(index, point2);
			return;
		}
	}

	Superclass::AddMeasure(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	int index = m_Measure2DVector.size() - 1;

	albaString text;
	text.Printf("Center (%.2f, %.2f) - Radius %.2f mm", point1[X], point1[Y], DistanceBetweenPoints(point1, point2));
	m_Measure2DVector[index].Text = text;

	// Update Edit Actors
	UpdateEditActors(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Add Line
	m_LineStackVector.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));

	// Add Points
	m_PointsStackVectorL.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorR.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorC.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));

	// Add Circle
	m_CircleStackVector.push_back(new albaActor2dStackHelper(vtkALBACircleSource::New(), m_Renderer));
	
	//////////Setting mapper/actors/proprieties//////////////
	int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;
	
	m_LineStackVector[index]->SetColor(m_Colors[col]);
	m_LineStackVector[index]->GetProperty()->SetLineStipplePattern(0xf0f0);
	m_LineStackVector[index]->GetProperty()->SetLineWidth(m_LineWidth);

	//---Points---
	// Left
	m_PointsStackVectorL[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorL[index]->SetColor(m_Colors[col]);

	// Right
	m_PointsStackVectorR[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorR[index]->SetColor(m_Colors[col]);

	// Center
	m_PointsStackVectorC[index]->GetProperty()->SetPointSize(m_CenterPointSize);
	m_PointsStackVectorC[index]->SetColor(m_Colors[col]);

	// Circle
	m_CircleStackVector[index]->GetProperty()->SetLineWidth(m_LineWidth);
	m_CircleStackVector[index]->SetColor(m_Colors[col]);

	vtkALBACircleSource *circleSource = (vtkALBACircleSource *)m_CircleStackVector[index]->GetSourceAlgorithm();
	circleSource->SetResolution(60);

	//////////////////////////////////////////////////////////////////////////

	m_CurrMeasure = index;

	UpdateLineActors(point1, point2);
	UpdatePointsActor(point1, point2);
	UpdateCircleActor(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasureText(m_CurrMeasure)));

	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::RemoveMeasure(int index)
{
	if (m_Renderer && index < GetMeasureCount())
	{
		Superclass::RemoveMeasure(index);

		//////////////////////////////////////////////////////////////////////////
		// Line
		cppDEL(m_LineStackVector[index]);
		m_LineStackVector.erase(m_LineStackVector.begin() + index);

		// POINTS
		// Left
		cppDEL(m_PointsStackVectorL[index]);
		m_PointsStackVectorL.erase(m_PointsStackVectorL.begin() + index);
		// Right
		cppDEL(m_PointsStackVectorR[index]);
		m_PointsStackVectorR.erase(m_PointsStackVectorR.begin() + index);
		// Center
		cppDEL(m_PointsStackVectorC[index]);
		m_PointsStackVectorC.erase(m_PointsStackVectorC.begin() + index);

		// Circle
		cppDEL(m_CircleStackVector[index]);
		m_CircleStackVector.erase(m_CircleStackVector.begin() + index);
	
		Render();
	}	
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::SelectMeasure(int index)
{
	if (GetMeasureCount() > 0)
	{
		// Deselect all
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

			m_LineStackVector[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorL[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorR[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorC[i]->SetColor(m_Colors[col]);
			m_CircleStackVector[i]->SetColor(m_Colors[col]);
			SetColor(m_TextActorVector[i], &m_Colors[col]);
		}

		if (index >= 0)
		{
			m_LineStackVector[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorL[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorR[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorC[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_CircleStackVector[index]->SetColor(m_Colors[COLOR_SELECTION]);
			SetColor(m_TextActorVector[index], &m_Colors[COLOR_SELECTION]);

			albaEventMacro(albaEvent(this, ID_MEASURE_SELECTED));
		}

		m_LastSelection = index;
		m_LastEditing = -1;
	}
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::Show(bool show)
{
	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_LineStackVector[i]->SetVisibility(show);
		m_PointsStackVectorL[i]->SetVisibility(show);
		m_PointsStackVectorR[i]->SetVisibility(show);
		m_PointsStackVectorC[i]->SetVisibility(show);
		m_CircleStackVector[i]->SetVisibility(show);
		m_TextActorVector[i]->SetVisibility(show && m_ShowText);
	}

	Render();
}

// SET/GET
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::SetPointSize(double size)
{
	m_PointSize = size;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVectorR[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorL[i]->GetProperty()->SetPointSize(m_PointSize);
		//m_PointsStackVectorC[i]->GetProperty()->SetPointSize(m_PointSize);
	}
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::SetCenterPointSize(double size)
{
	m_CenterPointSize = size;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVectorC[i]->GetProperty()->SetPointSize(m_CenterPointSize);
	}
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::SetLineWidth(double width)
{
	m_LineWidth = width;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_LineStackVector[i]->GetProperty()->SetLineWidth(m_LineWidth);
		m_CircleStackVector[i]->GetProperty()->SetLineWidth(m_LineWidth);
	}
	Render();
}

/// UTILS ///////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::GetMeasureLinePoints(int index, double *point1, double *point2)
{
	// Return line points values
	if (index >= 0 && index < GetMeasureCount())
	{
		vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[index]->GetSourceAlgorithm();
		lineSource->GetPoint1(point1);
		lineSource->GetPoint2(point2);
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_CenterPoint::GetCenter(int index, double *center)
{
	// Return center points values
	if (index >= 0 && index < GetMeasureCount())
	{
		vtkALBACircleSource* circleSource = (vtkALBACircleSource*)m_CircleStackVector[index]->GetSourceAlgorithm();
		circleSource->GetCenter(center);
	}
}

/// LOAD/SAVE ///////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_CenterPoint::Load(albaVME *input, wxString tag)
{
	if (input->GetTagArray()->IsTagPresent(tag + "MeasureCenterPoint1") && input->GetTagArray()->IsTagPresent(tag + "MeasureCenterPoint2"))
	{
		double point1[3], point2[3];
		albaTagItem *measureTypeTag = input->GetTagArray()->GetTag(tag + "MeasureType");
		albaTagItem *measureLabelTag = input->GetTagArray()->GetTag(tag + "MeasureCenterLabel");
		albaTagItem *measureCenterPoint1Tag = input->GetTagArray()->GetTag(tag + "MeasureCenterPoint1");
		albaTagItem *measureCenterPoint2Tag = input->GetTagArray()->GetTag(tag + "MeasureCenterPoint2");

		int nCenters = measureCenterPoint1Tag->GetNumberOfComponents() / 2;

		m_CurrentRenderer = m_Renderer;

		// Reload points
		for (int i = 0; i < nCenters; i++)
		{
			point1[X] = measureCenterPoint1Tag->GetValueAsDouble(i * 3 + 0);
			point1[Y] = measureCenterPoint1Tag->GetValueAsDouble(i * 3 + 1);
			point1[Z] = measureCenterPoint1Tag->GetValueAsDouble(i * 3 + 2);

			point2[X] = measureCenterPoint2Tag->GetValueAsDouble(i * 3 + 0);
			point2[Y] = measureCenterPoint2Tag->GetValueAsDouble(i * 3 + 1);
			point2[Z] = measureCenterPoint2Tag->GetValueAsDouble(i * 3 + 2);

			albaString measureType = measureTypeTag->GetValue(i);
			albaString measureLabel = measureLabelTag->GetValue(i);

			AddMeasure(point1, point2);
			SetMeasureLabel(i, measureLabel);
		}

		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_CenterPoint::Save(albaVME *input, wxString tag)
{
	bool result = false;
	int nCenters = GetMeasureCount();

	if (nCenters > 0)
	{
		albaTagItem measureTypeTag;
		measureTypeTag.SetName(tag + "MeasureType");
		measureTypeTag.SetNumberOfComponents(nCenters);

		albaTagItem measureLabelTag;
		measureLabelTag.SetName(tag + "MeasureCenterLabel");
		measureLabelTag.SetNumberOfComponents(nCenters);

		albaTagItem measureCenterPoint1Tag;
		measureCenterPoint1Tag.SetName(tag + "MeasureCenterPoint1");
		measureCenterPoint1Tag.SetNumberOfComponents(nCenters);

		albaTagItem measureCenterPoint2Tag;
		measureCenterPoint2Tag.SetName(tag + "MeasureCenterPoint2");
		measureCenterPoint2Tag.SetNumberOfComponents(nCenters);

		for (int i = 0; i < nCenters; i++)
		{
			double point1[3], point2[3];
			GetMeasureLinePoints(i, point1, point2);

			measureTypeTag.SetValue(GetTypeName(), i);
			measureLabelTag.SetValue(GetMeasureLabel(i), i);

			measureCenterPoint1Tag.SetValue(point1[X], i * 3 + 0);
			measureCenterPoint1Tag.SetValue(point1[Y], i * 3 + 1);
			measureCenterPoint1Tag.SetValue(point1[Z], i * 3 + 2);

			measureCenterPoint2Tag.SetValue(point2[X], i * 3 + 0);
			measureCenterPoint2Tag.SetValue(point2[Y], i * 3 + 1);
			measureCenterPoint2Tag.SetValue(point2[Z], i * 3 + 2);
		}

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureType"))
			input->GetTagArray()->DeleteTag(tag + "MeasureType");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureCenterLabel"))
			input->GetTagArray()->DeleteTag(tag + "MeasureLabel");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureCenterPoint1"))
			input->GetTagArray()->DeleteTag(tag + "MeasureCenterPoint1");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureCenterPoint2"))
			input->GetTagArray()->DeleteTag(tag + "MeasureCenterPoint2");

		input->GetTagArray()->SetTag(measureTypeTag);
		input->GetTagArray()->SetTag(measureLabelTag);
		input->GetTagArray()->SetTag(measureCenterPoint1Tag);
		input->GetTagArray()->SetTag(measureCenterPoint2Tag);

		result = true;
	}

	return result;
}