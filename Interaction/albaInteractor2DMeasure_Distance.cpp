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

	double dist = DistanceBetweenPoints(point1, point2);

	if (dist >= m_MinDistance)
	{
		albaString text;
		text.Printf("Distance %.2f mm", dist);
		m_Measure2DVector[index].Text = text;
		m_Measure2DVector[index].Value = dist;

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
			albaActor2dStackHelper *lineStackVector = m_LineStackVector[i];
			if (m_Renderer != lineStackVector->GetRenderer())
				continue;

			double linePoint1[3], linePoint2[3];

			vtkLineSource* lineSource = (vtkLineSource*)lineStackVector->GetSource();

			lineSource->GetPoint1(linePoint1);
			lineSource->GetPoint2(linePoint2);
			double dis = DistancePointToLine(point, linePoint1, linePoint2);
			//albaLogMessage("Dist %f", dis);

			if (DistancePointToLine(point, linePoint1, linePoint2) < POINT_UPDATE_DISTANCE)
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
	double tickLenght = DistanceBetweenPoints(point1, point2) * 0.05;
	tickLenght = tickLenght == 0.0 ? 0.5 : tickLenght;

	if (m_TickLenght > 0.0) tickLenght = m_TickLenght;

	//////////////////////////////////////////////////////////////////////////

	tickLenght = 0.1;

	double  pDiff[3] = { point2[X] - point1[X], point2[Y] - point1[Y], point2[Z] - point1[Z] };

	// tickL
	double tick1Point1[3]{ point1[X] - pDiff[X] * tickLenght, point1[Y] - pDiff[Y] * tickLenght,  point1[Z] - pDiff[Z] * tickLenght };
	double tick1Point2[3]{ point1[X] + pDiff[X] * tickLenght, point1[Y] + pDiff[Y] * tickLenght,  point1[Z] + pDiff[Z] * tickLenght };

	// tickR
	double tick2Point1[3]{ point2[X] - pDiff[X] * tickLenght, point2[Y] - pDiff[Y] * tickLenght,  point2[Z] - pDiff[Z] * tickLenght };
	double tick2Point2[3]{ point2[X] + pDiff[X] * tickLenght, point2[Y] + pDiff[Y] * tickLenght,  point2[Z] + pDiff[Z] * tickLenght };

	double angle = (M_PI / 2) - GetAngle(tick1Point1, point2, point1);

	RotatePoint(tick1Point1, point1, angle);
	RotatePoint(tick1Point2, point1, angle);
	RotatePoint(tick2Point1, point2, angle);
	RotatePoint(tick2Point2, point2, angle);

// 	Color blue = { 0,0,1,1 };
// 	Color red = { 1,0,0,1 };
// 	m_TickStackVectorL[m_CurrMeasure]->SetColor(blue);
// 	m_TickStackVectorR[m_CurrMeasure]->SetColor(red);

	vtkLineSource* tickSourceL = (vtkLineSource*)m_TickStackVectorL[m_CurrMeasure]->GetSource();
	tickSourceL->SetPoint1(tick1Point1);
	tickSourceL->SetPoint2(tick1Point2);
	tickSourceL->Update();
	
	vtkLineSource* tickSourceR = (vtkLineSource*)m_TickStackVectorR[m_CurrMeasure]->GetSource();
	tickSourceR->SetPoint1(tick2Point1);
	tickSourceR->SetPoint2(tick2Point2);
	tickSourceR->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Distance::UpdateTextActor(double * point1, double * point2)
{
	double text_pos[3];
	GetMidPoint(text_pos, point1, point2);

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

	if (DistanceBetweenPoints(point1, point2) < m_MinDistance)
	{
		point2[X] += m_MinDistance;
	}

	Superclass::AddMeasure(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	int index = m_Measure2DVector.size() - 1;

	albaString text;
	double dist = DistanceBetweenPoints(point1, point2);
	text.Printf("Distance %.2f mm", dist);
	m_Measure2DVector[index].Text = text;
	m_Measure2DVector[index].Value = dist;

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

		m_CurrentRenderer = m_Renderer;

		// Reload points
		for (int i = 0; i < nLines; i++)
		{
			point1[X] = MeasureDistancePoint1Tag->GetValueAsDouble(i * 3 + 0);
			point1[Y] = MeasureDistancePoint1Tag->GetValueAsDouble(i * 3 + 1);
			point1[Z] = MeasureDistancePoint1Tag->GetValueAsDouble(i * 3 + 2);

			point2[X] = MeasureDistancePoint2Tag->GetValueAsDouble(i * 3 + 0);
			point2[Y] = MeasureDistancePoint2Tag->GetValueAsDouble(i * 3 + 1);
			point2[Z] = MeasureDistancePoint2Tag->GetValueAsDouble(i * 3 + 2);

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

			MeasureDistancePoint1Tag.SetValue(point1[X], i * 3 + 0);
			MeasureDistancePoint1Tag.SetValue(point1[Y], i * 3 + 1);
			MeasureDistancePoint1Tag.SetValue(point1[Z], i * 3 + 2);

			MeasureDistancePoint2Tag.SetValue(point2[X], i * 3 + 0);
			MeasureDistancePoint2Tag.SetValue(point2[Y], i * 3 + 1);
			MeasureDistancePoint2Tag.SetValue(point2[Z], i * 3 + 2);
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