/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Distance.cpp
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

#include "albaInteractor2DMeasure_Distance.h"
#include "albaInteractor2DMeasure.h"

#include "albaTagArray.h"
#include "albaVME.h"

#include "vtkALBATextActorMeter.h"
#include "vtkActor2D.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkViewport.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DMeasure_Distance)

//----------------------------------------------------------------------------
albaInteractor2DMeasure_Distance::albaInteractor2DMeasure_Distance() : albaInteractor2DMeasure()
{
	m_ShowText = false;
	m_TextSide = 1;

	m_MeasureTypeText = "DISTANCE";

	m_CurrPoint = NO_POINT;
	m_MinDistance = 0.0;
	m_LineTickWidth = 2.0;
	m_TickLenght = -1.0;

	Color color{ 0.4, 0.4, 1, 1.0 };

	SetColorDefault(color.R, color.G, color.B, 0.85);
	SetColorSelection(color.R, color.G, color.B, 1.0);
	SetColorDisable(color.R, color.G, color.B, 0.3);
	SetColorText(color.R, color.G, color.B, 0.5);
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure_Distance::~albaInteractor2DMeasure_Distance()
{
	// Lines
	for (int i = 0; i < m_LineStackVector.size(); i++)
	{
		cppDEL(m_LineStackVector[i]);
		cppDEL(m_TickStackVectorL[i]);
		cppDEL(m_TickStackVectorR[i]);
	}
}

/// RENDERING ////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::DrawNewMeasure(double * wp)
{
	AddMeasure(wp, wp);
	//Call FindAndHighLight to start the edit phase
	FindAndHighlight(wp);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::MoveMeasure(int index, double * point)
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
		m_OldLineP1[0] = linePoint1[0] - m_StartMousePosition[0];
		m_OldLineP1[1] = linePoint1[1] - m_StartMousePosition[1];
		m_OldLineP2[0] = linePoint2[0] - m_StartMousePosition[0];
		m_OldLineP2[1] = linePoint2[1] - m_StartMousePosition[1];

		m_MovingMeasure = true;
	}

	// Initialization
	m_MeasureValue = 0.0;

	double tmp_pos1[3];
	double tmp_pos2[3];

	tmp_pos1[0] = point[0] + m_OldLineP1[0];
	tmp_pos1[1] = point[1] + m_OldLineP1[1];
	tmp_pos1[2] = 0.0;

	tmp_pos2[0] = point[0] + m_OldLineP2[0];
	tmp_pos2[1] = point[1] + m_OldLineP2[1];
	tmp_pos2[2] = 0.0;

	m_MeasureValue = GeometryUtils::DistanceBetweenPoints(tmp_pos1, tmp_pos2);

	UpdateLineActors(tmp_pos1, tmp_pos2);
	// Points
	UpdateLineTickActor(tmp_pos1, tmp_pos2);

	//called on mouse up  
	if (!m_DraggingLeft)
	{
		m_ActorAdded = false;
	}

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::EditMeasure(int index, double *point)
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
	}
	else if (m_CurrPoint == POINT_2)
	{
		point2[X] = point[X];
		point2[Y] = point[Y];
	}

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////
	// Update Measure

	double dist = GeometryUtils::DistanceBetweenPoints(point1, point2);

	if (dist >= m_MinDistance)
	{
		albaString text;
		text.Printf("Distance %.2f mm", dist);
		m_Measure2DVector[index].Text = text;

		// Line
		UpdateLineActors(point1, point2);
		// Points
		UpdateLineTickActor(point1, point2);
		// Text
		UpdateTextActor(point1, point2);
	}

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::FindAndHighlight(double * point)
{
	if (m_CurrMeasure < 0 && m_AddMeasureEnable)
		SetAction(ACTION_ADD_MEASURE);
	else
		SetAction(ACTION_NONE);

	if (m_EditMeasureEnable)
	{
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			double linePoint1[3];
			double linePoint2[3];

			vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[i]->GetSource();
			lineSource->GetPoint1(linePoint1);
			lineSource->GetPoint2(linePoint2);

			if (GeometryUtils::DistancePointToLine(point, linePoint1, linePoint2) < POINT_UPDATE_DISTANCE)
			{
				SelectMeasure(i);

				if (vtkMath::Distance2BetweenPoints(linePoint2, point) < POINT_UPDATE_DISTANCE_2)
				{
					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_2;
					m_TickStackVectorR[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else if (vtkMath::Distance2BetweenPoints(linePoint1, point) < POINT_UPDATE_DISTANCE_2)
				{
					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_1;
					m_TickStackVectorL[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else
				{
					m_CurrMeasure = i;
					if (m_MoveMeasureEnable)
					{
						m_LineStackVector[i]->SetColor(m_Colors[COLOR_EDIT]);
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
void albaInteractor2DMeasure_Distance::UpdateLineActors(double * point1, double * point2)
{
	vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[m_CurrMeasure]->GetSource();

	lineSource->SetPoint1(point1);
	lineSource->SetPoint2(point2);
	lineSource->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::UpdateLineTickActor(double * point1, double * point2)
{
	double tickLenght = GeometryUtils::DistanceBetweenPoints(point1, point2) * 0.05;
	tickLenght = tickLenght == 0.0 ? 0.5 : tickLenght;

	if (m_TickLenght > 0.0) tickLenght = m_TickLenght;

	// tickL
	double tick1Point1[3]{ point1[X] - tickLenght, point1[Y], 0.0 };
	double tick1Point2[3]{ point1[X] + tickLenght, point1[Y], 0.0 };

	double angle = (M_PI / 2) - GeometryUtils::CalculateAngle(tick1Point1, point2, point1);

	GeometryUtils::RotatePoint(tick1Point1, point1, angle);
	GeometryUtils::RotatePoint(tick1Point2, point1, angle);

	vtkLineSource* tickSourceL = (vtkLineSource*)m_TickStackVectorL[m_CurrMeasure]->GetSource();
	tickSourceL->SetPoint1(tick1Point1);
	tickSourceL->SetPoint2(tick1Point2);
	tickSourceL->Update();

	// tickR
	double tick2Point1[3]{ point2[X] - tickLenght, point2[Y], 0.0 };
	double tick2Point2[3]{ point2[X] + tickLenght, point2[Y], 0.0 };

	GeometryUtils::RotatePoint(tick2Point1, point2, angle);
	GeometryUtils::RotatePoint(tick2Point2, point2, angle);

	vtkLineSource* tickSourceR = (vtkLineSource*)m_TickStackVectorR[m_CurrMeasure]->GetSource();
	tickSourceR->SetPoint1(tick2Point1);
	tickSourceR->SetPoint2(tick2Point2);
	tickSourceR->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::UpdateTextActor(double * point1, double * point2)
{
	double text_pos[3];
	text_pos[X] = (point1[X] + point2[X]) / 2;
	text_pos[Y] = (point1[Y] + point2[Y]) / 2;
	text_pos[Z] = (point1[Z] + point2[Z]) / 2;

	text_pos[X] += m_TextSide *TEXT_W_SHIFT;
	text_pos[Y] -= m_TextSide *TEXT_H_SHIFT;

	Superclass::UpdateTextActor(m_CurrMeasure, text_pos);
}

/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::AddMeasure(double *point1, double *point2)
{
	if (GetMeasureCount() > 0)
	{
		int index = GetMeasureCount() - 1;
		double oldPoint1[3], oldPoint2[3];
		GetMeasureLinePoints(index, oldPoint1, oldPoint2);

		if (GeometryUtils::DistanceBetweenPoints(oldPoint1, oldPoint2)<POINT_UPDATE_DISTANCE)
		{
			m_CurrMeasure = index;
			m_CurrPoint = POINT_2;
			EditMeasure(index, point2);
			return;
		}
	}

	if (GeometryUtils::DistanceBetweenPoints(point1, point2) < m_MinDistance)
	{
		point2[X] += m_MinDistance;
	}

	Superclass::AddMeasure(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	int index = m_Measure2DVector.size() - 1;

	albaString text;
	text.Printf("Distance %.2f mm", GeometryUtils::DistanceBetweenPoints(point1, point2));
	m_Measure2DVector[index].Text = text;

	// Update Edit Actors
	UpdateEditActors(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Add Line and Ticks

	m_LineStackVector.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));
	m_TickStackVectorR.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));
	m_TickStackVectorL.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));

	//////////Setting mapper/actors/proprieties//////////////
	int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

	m_LineStackVector[index]->SetColor(m_Colors[col]);
	m_LineStackVector[index]->GetProperty()->SetLineWidth(m_LineWidth);
	m_TickStackVectorR[index]->SetColor(m_Colors[col]);
	m_TickStackVectorR[index]->GetProperty()->SetPointSize(10.0);
	m_TickStackVectorR[index]->GetProperty()->SetLineWidth(m_LineTickWidth);
	m_TickStackVectorL[index]->SetColor(m_Colors[col]);
	m_TickStackVectorL[index]->GetProperty()->SetPointSize(10.0);
	m_TickStackVectorL[index]->GetProperty()->SetLineWidth(m_LineTickWidth);

	m_CurrMeasure = index;

	UpdateLineActors(point1, point2);
	UpdateLineTickActor(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasureText(GetMeasureCount() - 1)));

	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::RemoveMeasure(int index)
{
	if (m_Renderer && index < GetMeasureCount())
	{
		Superclass::RemoveMeasure(index);

		//////////////////////////////////////////////////////////////////////////
		// Line
		cppDEL(m_LineStackVector[index]);
		m_LineStackVector.erase(m_LineStackVector.begin() + index);
		//Left
		cppDEL(m_TickStackVectorL[index]);
		m_TickStackVectorL.erase(m_TickStackVectorL.begin() + index);
		//Right
		cppDEL(m_TickStackVectorR[index]);
		m_TickStackVectorR.erase(m_TickStackVectorR.begin() + index);

		Render();
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::SelectMeasure(int index)
{
	if (GetMeasureCount() > 0)
	{
		// Deselect all
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			int col = m_IsEnabled ?  COLOR_DEFAULT : COLOR_DISABLE;

			m_LineStackVector[i]->SetColor(m_Colors[col]);
			m_TickStackVectorL[i]->SetColor(m_Colors[col]);
			m_TickStackVectorR[i]->SetColor(m_Colors[col]);
			SetColor(m_TextActorVector[i], &m_Colors[col]);
		}

		if (index >= 0)
		{
			m_LineStackVector[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_TickStackVectorL[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_TickStackVectorR[index]->SetColor(m_Colors[COLOR_SELECTION]);
			SetColor(m_TextActorVector[index], &m_Colors[COLOR_SELECTION]);

			albaEventMacro(albaEvent(this, ID_MEASURE_SELECTED));
		}

		m_LastSelection = index;
		m_LastEditing = -1;
	}
}

/// SET/GET
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::SetMinDistance(double minDistance)
{
	m_MinDistance = minDistance;
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::SetLineWidth(double width)
{
	m_LineWidth = width;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_LineStackVector[i]->GetProperty()->SetLineWidth(m_LineWidth);
	}
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::SetLineTickWidth(double width)
{
	m_LineTickWidth = width;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_TickStackVectorL[i]->GetProperty()->SetLineWidth(m_LineTickWidth);
		m_TickStackVectorR[i]->GetProperty()->SetLineWidth(m_LineTickWidth);
	}
	Render();
}

//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::SetLineTickLenght(double lenght)
{
	m_TickLenght = lenght;
}

/// UTILS ///////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::GetMeasureLinePoints(int index, double *point1, double *point2)
{
	// Return line points values
	if (index >= 0 && index < GetMeasureCount())
	{
		vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[index]->GetSource();
		lineSource->GetPoint1(point1);
		lineSource->GetPoint2(point2);
	}
}

/// LOAD/SAVE ///////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_Distance::Load(albaVME *input, wxString tag)
{
	if (input->GetTagArray()->IsTagPresent(tag + "MeasureDistancePoint1") && input->GetTagArray()->IsTagPresent(tag + "MeasureDistancePoint2"))
	{
		double point1[3], point2[3];
		albaTagItem *measureTypeTag = input->GetTagArray()->GetTag(tag + "MeasureType");
		albaTagItem *measureLabelTag = input->GetTagArray()->GetTag(tag + "MeasureDistanceLabel");
		albaTagItem *MeasureDistancePoint1Tag = input->GetTagArray()->GetTag(tag + "MeasureDistancePoint1");
		albaTagItem *MeasureDistancePoint2Tag = input->GetTagArray()->GetTag(tag + "MeasureDistancePoint2");

		int nLines = MeasureDistancePoint1Tag->GetNumberOfComponents() / 2;

		// Reload points
		for (int i = 0; i < nLines; i++)
		{
			point1[0] = MeasureDistancePoint1Tag->GetValueAsDouble(i * 2 + 0);
			point1[1] = MeasureDistancePoint1Tag->GetValueAsDouble(i * 2 + 1);
			point1[2] = 0.0;

			point2[0] = MeasureDistancePoint2Tag->GetValueAsDouble(i * 2 + 0);
			point2[1] = MeasureDistancePoint2Tag->GetValueAsDouble(i * 2 + 1);
			point2[2] = 0.0;

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
bool albaInteractor2DMeasure_Distance::Save(albaVME *input, wxString tag)
{
	bool result = false;
	int nLines = GetMeasureCount();

	if (nLines > 0)
	{
		albaTagItem measureTypeTag;
		measureTypeTag.SetName(tag + "MeasureType");
		measureTypeTag.SetNumberOfComponents(nLines);

		albaTagItem measureLabelTag;
		measureLabelTag.SetName(tag + "MeasureDistanceLabel");
		measureLabelTag.SetNumberOfComponents(nLines);

		albaTagItem MeasureDistancePoint1Tag;
		MeasureDistancePoint1Tag.SetName(tag + "MeasureDistancePoint1");
		MeasureDistancePoint1Tag.SetNumberOfComponents(nLines);

		albaTagItem MeasureDistancePoint2Tag;
		MeasureDistancePoint2Tag.SetName(tag + "MeasureDistancePoint2");
		MeasureDistancePoint2Tag.SetNumberOfComponents(nLines);

		for (int i = 0; i < nLines; i++)
		{
			double point1[3], point2[3];
			GetMeasureLinePoints(i, point1, point2);

			measureTypeTag.SetValue(GetTypeName(), i);
			measureLabelTag.SetValue(GetMeasureLabel(i), i);

			MeasureDistancePoint1Tag.SetValue(point1[0], i * 2 + 0);
			MeasureDistancePoint1Tag.SetValue(point1[1], i * 2 + 1);

			MeasureDistancePoint2Tag.SetValue(point2[0], i * 2 + 0);
			MeasureDistancePoint2Tag.SetValue(point2[1], i * 2 + 1);
		}

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureType"))
			input->GetTagArray()->DeleteTag(tag + "MeasureType");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureDistanceLabel"))
			input->GetTagArray()->DeleteTag(tag + "MeasureDistanceLabel");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureDistancePoint1"))
			input->GetTagArray()->DeleteTag(tag + "MeasureDistancePoint1");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureDistancePoint2"))
			input->GetTagArray()->DeleteTag(tag + "MeasureDistancePoint2");

		input->GetTagArray()->SetTag(measureTypeTag);
		input->GetTagArray()->SetTag(measureLabelTag);
		input->GetTagArray()->SetTag(MeasureDistancePoint1Tag);
		input->GetTagArray()->SetTag(MeasureDistancePoint2Tag);

		result = true;
	}

	return result;
}