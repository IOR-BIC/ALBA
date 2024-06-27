/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Line.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaInteractor2DMeasure_Line.h"
#include "albaInteractor2DMeasure.h"

#include "albaTagArray.h"
#include "albaVME.h"

#include "vtkALBATextActorMeter.h"
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
albaCxxTypeMacro(albaInteractor2DMeasure_Line)

//----------------------------------------------------------------------------
albaInteractor2DMeasure_Line::albaInteractor2DMeasure_Line() : albaInteractor2DMeasure()
{
	m_ShowText = false;
	m_TextSide = 1;

	m_MeasureTypeText = "LINE";

	m_CurrPoint = NO_POINT;

	m_LineExtensionLength = 1.0;
	m_LineStipplePattern = 0xFFFF;

	Color color{ 0.4, 0.4, 1, 1.0 };

	SetColorDefault(color.R, color.G, color.B, 0.85);
	SetColorSelection(color.R, color.G, color.B, 1.0);
	SetColorDisable(color.R, color.G, color.B, 0.3);
	SetColorText(color.R, color.G, color.B, 0.5);
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure_Line::~albaInteractor2DMeasure_Line()
{
	// Lines
	for (int i = 0; i < m_LineStackVector.size(); i++)
	{
		cppDEL(m_LineStackVector[i]);
		cppDEL(m_LineStackVectorL[i]);
		cppDEL(m_LineStackVectorR[i]);
		cppDEL(m_PointsStackVectorL[i]);
		cppDEL(m_PointsStackVectorR[i]);
	}
}

/// RENDERING ////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::DrawNewMeasure(double * wp)
{
	AddMeasure(wp, wp);
	//Call FindAndHighLight to start the edit phase
	FindAndHighlight(wp);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::MoveMeasure(int index, double * point)
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

	// Line
	UpdateLineActors(tmp_pos1, tmp_pos2);
	// Points
	UpdatePointsActor(tmp_pos1, tmp_pos2);
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
void albaInteractor2DMeasure_Line::EditMeasure(int index, double *point)
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
	text.Printf("Distance %.2f mm", DistanceBetweenPoints(point1, point2));
	//m_MeasureTextVector[index] = text;
	m_Measure2DVector[index].Text = text;

	// Line
	UpdateLineActors(point1, point2);
	// Points
	UpdatePointsActor(point1, point2);
	// Text
	UpdateTextActor(point1, point2);

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::FindAndHighlight(double * point)
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
void albaInteractor2DMeasure_Line::UpdatePointsActor(double * point1, double * point2)
{
	//left
	vtkPointSource* pointSourceL = (vtkPointSource*)m_PointsStackVectorL[m_CurrMeasure]->GetSource();
	pointSourceL->SetCenter(point1);
	pointSourceL->Update();

	//right
	vtkPointSource* pointSourceR = (vtkPointSource*)m_PointsStackVectorR[m_CurrMeasure]->GetSource();
	pointSourceR->SetCenter(point2);
	pointSourceR->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::UpdateLineActors(double * point1, double * point2)
{
	vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[m_CurrMeasure]->GetSource();

	lineSource->SetPoint1(point1);
	lineSource->SetPoint2(point2);
	lineSource->Update();

	UpdateLR(lineSource, (vtkLineSource*)m_LineStackVectorL[m_CurrMeasure]->GetSource(), (vtkLineSource*)m_LineStackVectorR[m_CurrMeasure]->GetSource());
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::UpdateLR(vtkLineSource *line, vtkLineSource *lineL, vtkLineSource *lineR)
{
	double p1[3], p2[3], lp[3], rp[3], pDiff[3];

	line->GetPoint1(p1);
	line->GetPoint2(p2);

	pDiff[X] = p2[X] - p1[X];
	pDiff[Y] = p2[Y] - p1[Y];
	pDiff[Z] = p2[Z] - p1[Z];

	lp[X] = p1[X] - pDiff[X] * m_LineExtensionLength;
	lp[Y] = p1[Y] - pDiff[Y] * m_LineExtensionLength;
	lp[Z] = p1[Z] - pDiff[Z] * m_LineExtensionLength;

	rp[X] = p2[X] + pDiff[X] * m_LineExtensionLength;
	rp[Y] = p2[Y] + pDiff[Y] * m_LineExtensionLength;
	rp[Z] = p2[Z] + pDiff[Z] * m_LineExtensionLength;

	lineL->SetPoint1(lp);
	lineL->SetPoint2(p1);

	lineR->SetPoint1(p2);
	lineR->SetPoint2(rp);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::UpdateTextActor(double * point1, double * point2)
{
	double text_pos[3];
	GetMidPoint(text_pos, point1, point2);

	text_pos[X] += m_TextSide *TEXT_W_SHIFT;
	text_pos[Y] -= m_TextSide *TEXT_H_SHIFT;

	Superclass::UpdateTextActor(m_CurrMeasure, text_pos);
}


/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::AddMeasure(double *point1, double *point2)
{
	if (GetMeasureCount() > 0)
	{
		int index = GetMeasureCount() - 1;
		double oldPoint1[3], oldPoint2[3];
		GetMeasureLinePoints(index, oldPoint1, oldPoint2);

		bool hasSameRenderer = (m_Renderer == m_Measure2DVector[index].Renderer);

		if (DistanceBetweenPoints(oldPoint1,oldPoint2)<POINT_UPDATE_DISTANCE)
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
	text.Printf("%.2f mm", DistanceBetweenPoints(point1, point2));
	m_Measure2DVector[index].Text = text;
	
	// Update Edit Actors
	UpdateEditActors(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Add Line
	m_LineStackVector.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));
	m_LineStackVectorR.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));
	m_LineStackVectorL.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));

	// Add Points
	m_PointsStackVectorL.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorR.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));

	//////////Setting proprieties//////////////
	int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

	m_LineStackVector[index]->SetColor(m_Colors[col]);
	m_LineStackVector[index]->GetProperty()->SetLineWidth(m_LineWidth);
	m_LineStackVector[index]->GetProperty()->SetLineStipplePattern(m_LineStipplePattern);

	m_LineStackVectorL[index]->SetColor(m_Colors[col]);
	m_LineStackVectorL[index]->GetProperty()->SetLineStipplePattern(0xf0f0);
	m_LineStackVectorL[index]->GetProperty()->SetLineWidth(m_LineWidth);
	m_LineStackVectorR[index]->SetColor(m_Colors[col]);
	m_LineStackVectorR[index]->GetProperty()->SetLineStipplePattern(0xf0f0);
	m_LineStackVectorR[index]->GetProperty()->SetLineWidth(m_LineWidth);

	//---Points---
	//Left
	m_PointsStackVectorL[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorL[index]->SetColor(m_Colors[col]);

	//Right
	m_PointsStackVectorR[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorR[index]->SetColor(m_Colors[col]);

	m_CurrMeasure = index;

	UpdateLineActors(point1, point2);
	UpdatePointsActor(point1, point2);
	UpdateTextActor(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasureText(GetMeasureCount() - 1)));

	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::RemoveMeasure(int index)
{
	if (m_Renderer && index < GetMeasureCount())
	{
		Superclass::RemoveMeasure(index);

		//////////////////////////////////////////////////////////////////////////
		// Line
		cppDEL(m_LineStackVector[index]);
		m_LineStackVector.erase(m_LineStackVector.begin() + index);
		//Left
		cppDEL(m_LineStackVectorL[index]);
		m_LineStackVectorL.erase(m_LineStackVectorL.begin() + index);
		//Right
		cppDEL(m_LineStackVectorR[index]);
		m_LineStackVectorR.erase(m_LineStackVectorR.begin() + index);

		//POINTS
		//Left
		cppDEL(m_PointsStackVectorL[index]);
		m_PointsStackVectorL.erase(m_PointsStackVectorL.begin() + index);
		//Right
		cppDEL(m_PointsStackVectorR[index]);
		m_PointsStackVectorR.erase(m_PointsStackVectorR.begin() + index);
	
		Render();
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::SelectMeasure(int index)
{
	if (GetMeasureCount() > 0)
	{
		// Deselect all
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

			m_LineStackVector[i]->SetColor(m_Colors[col]);
			m_LineStackVectorL[i]->SetColor(m_Colors[col]);
			m_LineStackVectorR[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorL[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorR[i]->SetColor(m_Colors[col]);
			SetColor(m_TextActorVector[i], &m_Colors[col]);
		}

		if (index >= 0)
		{
			m_LineStackVector[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_LineStackVectorL[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_LineStackVectorR[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorL[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorR[index]->SetColor(m_Colors[COLOR_SELECTION]);
			SetColor(m_TextActorVector[index], &m_Colors[COLOR_SELECTION]);

			albaEventMacro(albaEvent(this, ID_MEASURE_SELECTED));
		}

		m_LastSelection = index;
		m_LastEditing = -1;
	}
}

// SET/GET
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::SetPointSize(double size)
{
	m_PointSize = size;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVectorR[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorL[i]->GetProperty()->SetPointSize(m_PointSize);
	}
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::SetLineWidth(double width)
{
	m_LineWidth = width;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_LineStackVector[i]->GetProperty()->SetLineWidth(m_LineWidth);
		m_LineStackVectorL[i]->GetProperty()->SetLineWidth(m_LineWidth);
		m_LineStackVectorR[i]->GetProperty()->SetLineWidth(m_LineWidth);
	}
	Render();
}

//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::Show(bool show)
{
	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVectorL[i]->SetVisibility(show);
		m_PointsStackVectorR[i]->SetVisibility(show);
		m_LineStackVector[i]->SetVisibility(show);
		m_LineStackVectorL[i]->SetVisibility(show);
		m_LineStackVectorR[i]->SetVisibility(show);

		m_TextActorVector[i]->SetVisibility(show && m_ShowText);
	}

	Render();
}

/// UTILS ///////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Line::GetMeasureLinePoints(int index, double *point1, double *point2)
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
bool albaInteractor2DMeasure_Line::Load(albaVME *input, wxString tag)
{
	if (input->GetTagArray()->IsTagPresent(tag + "MeasureLinePoint1") && input->GetTagArray()->IsTagPresent(tag + "MeasureLinePoint2"))
	{
		double point1[3], point2[3];
		albaTagItem *measureTypeTag = input->GetTagArray()->GetTag(tag + "MeasureType");
		albaTagItem *measureLabelTag = input->GetTagArray()->GetTag(tag + "MeasureLineLabel");
		albaTagItem *measureLinePoint1Tag = input->GetTagArray()->GetTag(tag + "MeasureLinePoint1");
		albaTagItem *measureLinePoint2Tag = input->GetTagArray()->GetTag(tag + "MeasureLinePoint2");

		int nLines = measureLinePoint1Tag->GetNumberOfComponents() / 2;

		m_CurrentRenderer = m_Renderer;

		// Reload points
		for (int i = 0; i < nLines; i++)
		{
			point1[X] = measureLinePoint1Tag->GetValueAsDouble(i * 3 + 0);
			point1[Y] = measureLinePoint1Tag->GetValueAsDouble(i * 3 + 1);
			point1[Z] = measureLinePoint1Tag->GetValueAsDouble(i * 3 + 2);

			point2[X] = measureLinePoint2Tag->GetValueAsDouble(i * 3 + 0);
			point2[Y] = measureLinePoint2Tag->GetValueAsDouble(i * 3 + 1);
			point2[Z] = measureLinePoint2Tag->GetValueAsDouble(i * 3 + 2);

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
bool albaInteractor2DMeasure_Line::Save(albaVME *input, wxString tag)
{
	bool result = false;
	int nLines = GetMeasureCount();

	if (nLines > 0)
	{
		albaTagItem measureTypeTag;
		measureTypeTag.SetName(tag + "MeasureType");
		measureTypeTag.SetNumberOfComponents(nLines);

		albaTagItem measureLabelTag;
		measureLabelTag.SetName(tag + "MeasureLineLabel");
		measureLabelTag.SetNumberOfComponents(nLines);

		albaTagItem measureLinePoint1Tag;
		measureLinePoint1Tag.SetName(tag + "MeasureLinePoint1");
		measureLinePoint1Tag.SetNumberOfComponents(nLines);

		albaTagItem measureLinePoint2Tag;
		measureLinePoint2Tag.SetName(tag + "MeasureLinePoint2");
		measureLinePoint2Tag.SetNumberOfComponents(nLines);

		for (int i = 0; i < nLines; i++)
		{
			double point1[3], point2[3];
			GetMeasureLinePoints(i, point1, point2);

			measureTypeTag.SetValue(GetTypeName(), i);
			measureLabelTag.SetValue(GetMeasureLabel(i), i);

			measureLinePoint1Tag.SetValue(point1[X], i * 3 + 0);
			measureLinePoint1Tag.SetValue(point1[Y], i * 3 + 1);
			measureLinePoint1Tag.SetValue(point1[Z], i * 3 + 2);

			measureLinePoint2Tag.SetValue(point2[X], i * 3 + 0);
			measureLinePoint2Tag.SetValue(point2[Y], i * 3 + 1);
			measureLinePoint2Tag.SetValue(point2[Z], i * 3 + 2);
		}

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureType"))
			input->GetTagArray()->DeleteTag(tag + "MeasureType");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureLineLabel"))
			input->GetTagArray()->DeleteTag(tag + "MeasureLineLabel");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureLinePoint1"))
			input->GetTagArray()->DeleteTag(tag + "MeasureLinePoint1");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureLinePoint2"))
			input->GetTagArray()->DeleteTag(tag + "MeasureLinePoint2");

		input->GetTagArray()->SetTag(measureTypeTag);
		input->GetTagArray()->SetTag(measureLabelTag);
		input->GetTagArray()->SetTag(measureLinePoint1Tag);
		input->GetTagArray()->SetTag(measureLinePoint2Tag);

		result = true;
	}

	return result;
}