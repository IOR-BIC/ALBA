/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_MidPoint.cpp
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

#include "albaInteractor2DMeasure_MidPoint.h"
#include "albaInteractor2DMeasure.h"

#include "albaTagArray.h"
#include "albaVME.h"

#include "vtkALBATextActorMeter.h"
#include "vtkActor2D.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkPointSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyDataSource.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkViewport.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DMeasure_MidPoint)

//----------------------------------------------------------------------------
albaInteractor2DMeasure_MidPoint::albaInteractor2DMeasure_MidPoint() : albaInteractor2DMeasure()
{
	m_ShowText = false;
	m_TextSide = 1;

	m_MeasureTypeText = "MIDPOINT";

	m_CurrPoint = NO_POINT;

	m_MidPointSize = 5.0;

	Color color{ 0.0, 0.4, 1, 1.0 };

	SetColorDefault(color.R, color.G, color.B, 0.85);
	SetColorSelection(color.R, color.G, color.B, 1.0);
	SetColorDisable(color.R, color.G, color.B, 0.3);
	SetColorText(color.R, color.G, color.B, 0.5);
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure_MidPoint::~albaInteractor2DMeasure_MidPoint()
{
	// Lines and Points
	for (int i = 0; i < m_LineStackVector.size(); i++)
	{
		cppDEL(m_LineStackVector[i]);
		cppDEL(m_PointsStackVectorL[i]);
		cppDEL(m_PointsStackVectorR[i]);
		cppDEL(m_MidPointsStackVector[i]);
	}
}

/// RENDERING ////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::DrawNewMeasure(double * wp)
{
	AddMeasure(wp, wp);
	//Call FindAndHighLight to start the edit phase
	FindAndHighlight(wp);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::MoveMeasure(int index, double * point)
{
	if (index < 0)
		return;

	double linePoint1[3];
	double linePoint2[3];

	vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[index]->GetSource();
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

	m_MeasureValue = albaGeometryUtils::DistanceBetweenPoints(tmp_pos1, tmp_pos2);

	UpdateLineActors(tmp_pos1, tmp_pos2);
	// Points
	UpdatePointsActor(tmp_pos1, tmp_pos2);
	// Mid Points
	UpdateMidPointActor(tmp_pos1, tmp_pos2);

	//called on mouse up  
	if (!m_DraggingLeft)
	{
		m_ActorAdded = false;
	}

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::EditMeasure(int index, double *point)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	m_MovingMeasure = true;

	double point1[3];
	double point2[3];

	vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[index]->GetSource();
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
	text.Printf("Distance %.2f mm", albaGeometryUtils::DistanceBetweenPoints(point1, point2));
	//m_MeasureTextVector[index] = text;
	m_Measure2DVector[index].Text = text;

	// Line
	UpdateLineActors(point1, point2);
	// Points
	UpdatePointsActor(point1, point2);
	// Mid Points
	UpdateMidPointActor(point1, point2);
	// Text
	UpdateTextActor(point1, point2);

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::FindAndHighlight(double * point)
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

			vtkLineSource* lineSource = (vtkLineSource*)lineStackVector->GetSource();
			lineSource->GetPoint1(linePoint1);
			lineSource->GetPoint2(linePoint2);

			if (albaGeometryUtils::DistancePointToLine(point, linePoint1, linePoint2) < POINT_UPDATE_DISTANCE)
			{
				SelectMeasure(i);

				if (vtkMath::Distance2BetweenPoints(linePoint2, point) < POINT_UPDATE_DISTANCE_2)
				{
					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_2;
					m_PointsStackVectorR[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else if (vtkMath::Distance2BetweenPoints(linePoint1, point) < POINT_UPDATE_DISTANCE_2)
				{
					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_1;
					m_PointsStackVectorL[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else
				{
					m_CurrMeasure = i;
					if (m_MoveMeasureEnable)
					{
						lineStackVector->SetColor(m_Colors[COLOR_EDIT]);
						SetAction(ACTION_MOVE_MEASURE);
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
			Render();
		}
	}
}

/// UPDATE ///////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::UpdatePointsActor(double * point1, double * point2)
{
	// Left
	vtkPointSource* pointSourceL = (vtkPointSource*)m_PointsStackVectorL[m_CurrMeasure]->GetSource();
	pointSourceL->SetCenter(point1);
	pointSourceL->Update();

	// Right
	vtkPointSource* pointSourceR = (vtkPointSource*)m_PointsStackVectorR[m_CurrMeasure]->GetSource();
	pointSourceR->SetCenter(point2);
	pointSourceR->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::UpdateLineActors(double * point1, double * point2)
{
	vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[m_CurrMeasure]->GetSource();
	lineSource->SetPoint1(point1);
	lineSource->SetPoint2(point2);
	lineSource->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::UpdateMidPointActor(double * point1, double * point2)
{
	double midPoint[3];
	albaGeometryUtils::GetMidPoint(midPoint, point1, point2);

	vtkPointSource* pointSource = (vtkPointSource*)m_MidPointsStackVector[m_CurrMeasure]->GetSource();
	pointSource->SetCenter(midPoint);
	pointSource->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::UpdateTextActor(double * point1, double * point2)
{
	double text_pos[3];
	albaGeometryUtils::GetMidPoint(text_pos, point1, point2);

	text_pos[X] += m_TextSide *TEXT_W_SHIFT;
	text_pos[Y] -= m_TextSide *TEXT_H_SHIFT;
	
	Superclass::UpdateTextActor(m_CurrMeasure, text_pos);
}

/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::AddMeasure(double *point1, double *point2)
{
	if (GetMeasureCount() > 0)
	{
		int index = GetMeasureCount() - 1;
		double oldPoint1[3], oldPoint2[3];
		GetMeasureLinePoints(index, oldPoint1, oldPoint2);

		bool hasSameRenderer = (m_Renderer == m_Measure2DVector[index].Renderer);

		if (albaGeometryUtils::DistanceBetweenPoints(oldPoint1, oldPoint2)<POINT_UPDATE_DISTANCE)
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
	text.Printf("Distance %.2f mm", albaGeometryUtils::DistanceBetweenPoints(point1, point2));
	m_Measure2DVector[index].Text = text;

	// Update Edit Actors
	UpdateEditActors(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Add Line
	m_LineStackVector.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));

	// Add Points
	m_PointsStackVectorL.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorR.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_MidPointsStackVector.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));

	//////////Setting mapper/actors/proprieties//////////////
	int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

	m_LineStackVector[index]->SetColor(m_Colors[col]);
	m_LineStackVector[index]->GetProperty()->SetLineWidth(m_LineWidth);
	m_LineStackVector[index]->GetProperty()->SetLineStipplePattern(0xf0f0);

	//---Points---
	// Left
	m_PointsStackVectorL[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorL[index]->SetColor(m_Colors[col]);

	// Right
	m_PointsStackVectorR[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorR[index]->SetColor(m_Colors[col]);

	// MidPoint
	m_MidPointsStackVector[index]->GetProperty()->SetPointSize(m_MidPointSize);
	m_MidPointsStackVector[index]->SetColor(m_Colors[col]);

	m_CurrMeasure = index;

	UpdateLineActors(point1, point2);
	UpdatePointsActor(point1, point2);
	UpdateMidPointActor(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasureText(m_CurrMeasure)));

	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::RemoveMeasure(int index)
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
		// MidPoint
		cppDEL(m_MidPointsStackVector[index]);
		m_MidPointsStackVector.erase(m_MidPointsStackVector.begin() + index);

		Render();
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::SelectMeasure(int index)
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
			m_MidPointsStackVector[i]->SetColor(m_Colors[col]);
			SetColor(m_TextActorVector[i], &m_Colors[col]);
		}

		if (index >= 0)
		{
			m_LineStackVector[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorL[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorR[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_MidPointsStackVector[index]->SetColor(m_Colors[COLOR_SELECTION]);
			SetColor(m_TextActorVector[index], &m_Colors[COLOR_SELECTION]);

			albaEventMacro(albaEvent(this, ID_MEASURE_SELECTED));
		}

		m_LastSelection = index;
		m_LastEditing = -1;
	}
}

// SET/GET
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::SetPointSize(double size)
{
	m_PointSize = size;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVectorR[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorL[i]->GetProperty()->SetPointSize(m_PointSize);
		//m_MidPointsStackVector[i]->GetProperty()->SetPointSize(m_PointSize);
	}
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::SetMidPointSize(double size)
{
	m_MidPointSize = size;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_MidPointsStackVector[i]->GetProperty()->SetPointSize(m_MidPointSize);
	}
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::SetLineWidth(double width)
{
	m_LineWidth = width;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_LineStackVector[i]->GetProperty()->SetLineWidth(m_LineWidth);
	}
	Render();
}

/// UTILS ///////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::GetMeasureLinePoints(int index, double *point1, double *point2)
{
	// Return line points values
	if (index >= 0 && index < GetMeasureCount())
	{
		vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[index]->GetSource();
		lineSource->GetPoint1(point1);
		lineSource->GetPoint2(point2);
	}
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_MidPoint::GetMeasureMidPoint(int index, double *point)
{
	// Return line points values
	if (index >= 0 && index < GetMeasureCount())
	{
		vtkPointSource* pointSource = (vtkPointSource*)m_MidPointsStackVector[index]->GetSource();
		pointSource->GetCenter(point);
	}
}

/// LOAD/SAVE ///////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_MidPoint::Load(albaVME *input, wxString tag)
{
	if (input->GetTagArray()->IsTagPresent(tag + "MeasureMidPoint1") && input->GetTagArray()->IsTagPresent(tag + "MeasureMidPoint2"))
	{
		double point1[3], point2[3];
		albaTagItem *measureTypeTag = input->GetTagArray()->GetTag(tag + "MeasureType");
		albaTagItem *measureLabelTag = input->GetTagArray()->GetTag(tag + "MeasureMidPointLabel");
		albaTagItem *measureMidPoint1Tag = input->GetTagArray()->GetTag(tag + "MeasureMidPoint1");
		albaTagItem *measureMidPoint2Tag = input->GetTagArray()->GetTag(tag + "MeasureMidPoint2");

		int nLines = measureMidPoint1Tag->GetNumberOfComponents() / 2;

		// Reload points
		for (int i = 0; i < nLines; i++)
		{
			point1[X] = measureMidPoint1Tag->GetValueAsDouble(i * 2 + 0);
			point1[Y] = measureMidPoint1Tag->GetValueAsDouble(i * 2 + 1);
			point1[Z] = measureMidPoint1Tag->GetValueAsDouble(i * 2 + 2);

			point2[X] = measureMidPoint2Tag->GetValueAsDouble(i * 2 + 0);
			point2[Y] = measureMidPoint2Tag->GetValueAsDouble(i * 2 + 1);
			point2[Z] = measureMidPoint2Tag->GetValueAsDouble(i * 2 + 2);

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
bool albaInteractor2DMeasure_MidPoint::Save(albaVME *input, wxString tag)
{
	bool result = false;
	int nLines = GetMeasureCount();

	if (nLines > 0)
	{
		albaTagItem measureTypeTag;
		measureTypeTag.SetName(tag + "MeasureType");
		measureTypeTag.SetNumberOfComponents(nLines);

		albaTagItem measureLabelTag;
		measureLabelTag.SetName(tag + "MeasureMidPointLabel");
		measureLabelTag.SetNumberOfComponents(nLines);

		albaTagItem measureMidPoint1Tag;
		measureMidPoint1Tag.SetName(tag + "MeasureMidPoint1");
		measureMidPoint1Tag.SetNumberOfComponents(nLines);

		albaTagItem measureMidPoint2Tag;
		measureMidPoint2Tag.SetName(tag + "MeasureMidPoint2");
		measureMidPoint2Tag.SetNumberOfComponents(nLines);

		for (int i = 0; i < nLines; i++)
		{
			double point1[3], point2[3];
			GetMeasureLinePoints(i, point1, point2);

			measureTypeTag.SetValue(GetTypeName(), i);
			measureLabelTag.SetValue(GetMeasureLabel(i), i);

			measureMidPoint1Tag.SetValue(point1[X], i * 2 + 0);
			measureMidPoint1Tag.SetValue(point1[Y], i * 2 + 1);
			measureMidPoint1Tag.SetValue(point1[Z], i * 2 + 2);

			measureMidPoint2Tag.SetValue(point2[X], i * 2 + 0);
			measureMidPoint2Tag.SetValue(point2[Y], i * 2 + 1);
			measureMidPoint2Tag.SetValue(point2[Z], i * 2 + 2);
		}

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureType"))
			input->GetTagArray()->DeleteTag(tag + "MeasureType");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureMidPointLabel"))
			input->GetTagArray()->DeleteTag(tag + "MeasureMidPointLabel");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureMidPoint1"))
			input->GetTagArray()->DeleteTag(tag + "MeasureMidPoint1");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureMidPoint2"))
			input->GetTagArray()->DeleteTag(tag + "MeasureMidPoint2");

		input->GetTagArray()->SetTag(measureTypeTag);
		input->GetTagArray()->SetTag(measureLabelTag);
		input->GetTagArray()->SetTag(measureMidPoint1Tag);
		input->GetTagArray()->SetTag(measureMidPoint2Tag);

		result = true;
	}

	return result;
}