﻿/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_LineDistance.cpp
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

#include "albaInteractor2DMeasure_LineDistance.h"
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
#include "albaVect3d.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DMeasure_LineDistance)

//----------------------------------------------------------------------------
albaInteractor2DMeasure_LineDistance::albaInteractor2DMeasure_LineDistance() : albaInteractor2DMeasure()
{
	m_ShowText = false;
	m_TextSide = 1;

	m_MeasureTypeText = "LINE_DISTANCE";

	m_CurrPoint = NO_POINT;
	m_AddModeCompleted = true;

	m_LineExtensionLength = 1.0;
	m_LineStipplePattern = 0xFFFF;

	Color color{ 0.4, 0.4, 1, 1.0 };

	SetColorDefault(color.R, color.G, color.B, 0.85);
	SetColorSelection(color.R, color.G, color.B, 1.0);
	SetColorDisable(color.R, color.G, color.B, 0.3);
	SetColorText(color.R, color.G, color.B, 0.5);
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure_LineDistance::~albaInteractor2DMeasure_LineDistance()
{
	// Lines
	for (int i = 0; i < m_LineStackVector.size(); i++)
	{
		cppDEL(m_LineStackVector[i]);
		cppDEL(m_LineStackVectorB[i]);
		cppDEL(m_LineStackVectorPerp[i]);
		cppDEL(m_PointsStackVectorL[i]);
		cppDEL(m_PointsStackVectorR[i]);
	}
}

/// RENDERING ////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_LineDistance::DrawNewMeasure(double * wp)
{
	AddMeasure(wp, wp);

	//Call FindAndHighLight to start the edit phase
	FindAndHighlight(wp);
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_LineDistance::MoveMeasure(int index, double * point)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	if (!m_Measure2DVector[index].Active)
		return;

	double linePoint1[3];
	double linePoint2[3];

	vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[index]->GetSource();
	lineSource->GetPoint1(linePoint1);
	lineSource->GetPoint2(linePoint2);

	if (!m_MoveLineB)
	{
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

		double tmp_pos1[3] = { point[X] + m_OldLineP1[X] , point[Y] + m_OldLineP1[Y] , point[Z] + m_OldLineP1[Z] };
		double tmp_pos2[3] = { point[X] + m_OldLineP2[X] , point[Y] + m_OldLineP2[Y] , point[Z] + m_OldLineP2[Z] };

		m_MeasureValue = DistanceBetweenPoints(tmp_pos1, tmp_pos2);

		UpdateLineActors(tmp_pos1, tmp_pos2);
		// Points
		UpdatePointsActor(tmp_pos1, tmp_pos2);
	}
	else
	{
		m_Distances[index] = DistancePointToLine(point, linePoint1, linePoint2);
		m_Distances[index] *= PointUpDownLine(point, linePoint1, linePoint2);

		m_CurrMeasure = index;
		UpdateLineActors(linePoint1, linePoint2);
	}

	//called on mouse up  
	if (!m_DraggingLeft)
	{
		m_ActorAdded = false;
	}

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	double dist = GetDistance(index);
	UpdateMeasure(index, dist);

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_LineDistance::EditMeasure(int index, double *point)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	if (!m_Measure2DVector[index].Active)
		return;

	m_MovingMeasure = true;

	double point1[3], point2[3];
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
	double dist = GetDistance(index);
	UpdateMeasure(index, dist);

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
void albaInteractor2DMeasure_LineDistance::FindAndHighlight(double * point)
{
	if (m_CurrMeasure < 0)
		SetAction(ACTION_ADD_MEASURE);

	if (m_EditMeasureEnable)
	{
		SetUpdateDistance(PixelSizeInWorld()*4.0);

		for (int i = 0; i < GetMeasureCount(); i++)
		{
			double linePoint1[3], linePoint2[3];
			double lineBPoint1[3], lineBPoint2[3];

			vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[i]->GetSource();
			lineSource->GetPoint1(linePoint1);
			lineSource->GetPoint2(linePoint2);

			vtkLineSource* lineSourceB = (vtkLineSource*)m_LineStackVectorB[i]->GetSource();
			lineSourceB->GetPoint1(lineBPoint1);
			lineSourceB->GetPoint2(lineBPoint2);

			double l1Dist = DistancePointToLine(point, linePoint1, linePoint2);
			double l2Dist = DistancePointToLine(point, lineBPoint1, lineBPoint2);
			if ((l1Dist < l2Dist) && (l1Dist < m_PointUpdateDist))
			{
				SelectMeasure(i);

				if (m_Measure2DVector[i].Active)
				{
					double p1Dist = DistanceBetweenPoints(point,linePoint1);
					double p2Dist = DistanceBetweenPoints(point,linePoint2);
					double p1p2Dist = DistanceBetweenPoints(linePoint1, linePoint2);
					double minDist = MIN(m_PointUpdateDist, (p1p2Dist/3.0));

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
							m_LineStackVector[i]->SetColor(m_Colors[COLOR_EDIT]);
							SetAction(ACTION_MOVE_MEASURE);
						}

						m_MoveLineB = false;
					}
				}

				Render();
				return;
			}
			else if (l2Dist < m_PointUpdateDist)
			{
				SelectMeasure(i);

				if (m_Measure2DVector[i].Active)
				{
					m_CurrMeasure = i;
					if (m_MoveMeasureEnable)
					{
						m_LineStackVectorB[i]->SetColor(m_Colors[COLOR_EDIT]);
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

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_LineDistance::UpdateMeasure(int index, double measure)
{
	albaString text;
	text.Printf("Distance %.2f mm", measure);
	m_Measure2DVector[index].Text = text;
	m_Measure2DVector[index].Value = measure;
}

/// UPDATE ///////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_LineDistance::UpdatePointsActor(double * point1, double * point2)
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
void albaInteractor2DMeasure_LineDistance::UpdateLineActors(double * point1, double * point2)
{
	double p1[3], p2[3], lBp1[3], lBp2[3], delta;
	vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[m_CurrMeasure]->GetSource();
	vtkLineSource* lineSourceB = (vtkLineSource*)m_LineStackVectorB[m_CurrMeasure]->GetSource();
	vtkLineSource* lineSourcePerp = (vtkLineSource*)m_LineStackVectorPerp[m_CurrMeasure]->GetSource();

	lineSource->GetPoint1(p1);
	lineSource->GetPoint2(p2);
	lineSourceB->GetPoint1(lBp1);
	lineSourceB->GetPoint2(lBp2);

	lineSource->SetPoint1(point1);
	lineSource->SetPoint2(point2);
	lineSource->Update();

	if (m_SecondLineAdded[m_CurrMeasure])
	{
		GetParallelLine(lBp1, lBp2, point1, point2, m_Distances[m_CurrMeasure]);

		lineSourceB->SetPoint1(lBp1);
		lineSourceB->SetPoint2(lBp2);
		lineSourceB->Update();

		double m1[3], m2[3];
		m1[X] = (p1[X] + p2[X]) / 2.0;
		m1[Y] = (p1[Y] + p2[Y]) / 2.0;
		m1[Z] = (p1[Z] + p2[Z]) / 2.0;

		m2[X] = (lBp1[X] + lBp2[X]) / 2.0;
		m2[Y] = (lBp1[Y] + lBp2[Y]) / 2.0;
		m2[Z] = (lBp1[Z] + lBp2[Z]) / 2.0;

		lineSourcePerp->SetPoint1(m1);
		lineSourcePerp->SetPoint2(m2);
		lineSourcePerp->Update();
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_LineDistance::UpdateTextActor(double * point1, double * point2)
{
	double text_pos[3];
	GetMidPoint(text_pos, point1, point2);

	text_pos[X] += m_TextSide *TEXT_W_SHIFT;
	text_pos[Y] -= m_TextSide *TEXT_H_SHIFT;

	Superclass::UpdateTextActor(m_CurrMeasure, text_pos);
}


/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_LineDistance::AddMeasure(double *point1, double *point2)
{
	if (GetMeasureCount() > 0)
	{
		int index = GetMeasureCount() - 1;
		double oldPoint1[3], oldPoint2[3];
		GetMeasureLinePoints(index, oldPoint1, oldPoint2);

		bool hasSameRenderer = (m_Renderer == m_Measure2DVector[index].Renderer);

		if (DistancePointToLine(point1,oldPoint1, oldPoint2) < m_PointUpdateDist)
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
		else if (m_SecondLineAdded[index] == false)
		{
			if (!hasSameRenderer) return;

			//Adding the second line no need to add a new measure.
			m_SecondLineAdded[index] = true;
			albaVect3d l1P1, l1P2;

			vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[index]->GetSource();
			vtkLineSource* lineSourceB = (vtkLineSource*)m_LineStackVectorB[index]->GetSource();

			l1P1 = lineSource->GetPoint1();
			l1P2 = lineSource->GetPoint2();

			m_Distances[index] = DistancePointToLine(point1, l1P1.GetVect(), l1P2.GetVect());
			m_Distances[index] *= PointUpDownLine(point1, l1P1.GetVect(), l1P2.GetVect());

			m_CurrMeasure = index;
			UpdateLineActors(l1P1.GetVect(), l1P2.GetVect());

			m_CurrMeasure = -1;
			m_AddModeCompleted = true;
			ActivateMeasure(-1, true);
			return;
		}
	}

	if (m_AddModeCompleted)
	{
		Superclass::AddMeasure(point1, point2);

		//////////////////////////////////////////////////////////////////////////
		// Update Measure
		int index = m_Measure2DVector.size() - 1;
		double dist = GetDistance(index);
		
		UpdateMeasure(index, dist);

		// Update Edit Actors
		UpdateEditActors(point1, point2);

		//////////////////////////////////////////////////////////////////////////
		// Add Line
		m_LineStackVector.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));
		m_LineStackVectorB.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));
		m_LineStackVectorPerp.push_back(new albaActor2dStackHelper(vtkLineSource::New(), m_Renderer));

		// Add Points
		m_PointsStackVectorL.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
		m_PointsStackVectorR.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));

		m_SecondLineAdded.push_back(false);
		m_Distances.push_back(0);

		//////////Setting proprieties//////////////
		int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

		m_LineStackVector[index]->SetColor(m_Colors[col]);
		m_LineStackVector[index]->GetProperty()->SetLineWidth(m_LineWidth);
		m_LineStackVector[index]->GetProperty()->SetLineStipplePattern(m_LineStipplePattern);

		m_LineStackVectorB[index]->SetColor(m_Colors[col]);
		m_LineStackVectorB[index]->GetProperty()->SetLineWidth(m_LineWidth);
		m_LineStackVectorB[index]->GetProperty()->SetLineStipplePattern(m_LineStipplePattern);

		m_LineStackVectorPerp[index]->SetColor(m_Colors[COLOR_DISABLE]);
		m_LineStackVectorPerp[index]->GetProperty()->SetLineWidth(m_LineWidth);
		m_LineStackVectorPerp[index]->GetProperty()->SetLineStipplePattern(0xf0f0);

		//---Points---
		//Left
		m_PointsStackVectorL[index]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorL[index]->SetColor(m_Colors[col]);

		//Right
		m_PointsStackVectorR[index]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorR[index]->SetColor(m_Colors[col]);

		m_CurrMeasure = index;
		m_AddModeCompleted = false;

		ActivateMeasure(-1, false);
		ActivateMeasure(index);

		UpdateLineActors(point1, point2);
		UpdatePointsActor(point1, point2);
	}
	//////////////////////////////////////////////////////////////////////////
	albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasureText(GetMeasureCount() - 1)));

	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_LineDistance::RemoveMeasure(int index)
{
	if (m_Renderer && index < GetMeasureCount())
	{
		Superclass::RemoveMeasure(index);

		//////////////////////////////////////////////////////////////////////////
		// LINES
		cppDEL(m_LineStackVector[index]);
		m_LineStackVector.erase(m_LineStackVector.begin() + index);
		//Left
		cppDEL(m_LineStackVectorB[index]);
		m_LineStackVectorB.erase(m_LineStackVectorB.begin() + index);

		cppDEL(m_LineStackVectorPerp[index]);
		m_LineStackVectorPerp.erase(m_LineStackVectorPerp.begin() + index);

		//POINTS
		//Left
		cppDEL(m_PointsStackVectorL[index]);
		m_PointsStackVectorL.erase(m_PointsStackVectorL.begin() + index);
		//Right
		cppDEL(m_PointsStackVectorR[index]);
		m_PointsStackVectorR.erase(m_PointsStackVectorR.begin() + index);

		m_Distances.erase(m_Distances.begin() + index);

		m_SecondLineAdded.erase(m_SecondLineAdded.begin() + index);

		Render();
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_LineDistance::SelectMeasure(int index)
{
	if (GetMeasureCount() > 0)
	{
		// Deselect all
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

			m_LineStackVector[i]->SetColor(m_Colors[col]);
			m_LineStackVectorB[i]->SetColor(m_Colors[col]);
			m_LineStackVectorPerp[i]->SetColor(m_Colors[COLOR_DISABLE]);

			m_PointsStackVectorL[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorR[i]->SetColor(m_Colors[col]);
			SetColor(m_TextActorVector[i], &m_Colors[col]);
		}

		if (index >= 0 && m_Measure2DVector[index].Active)
		{
			m_LineStackVector[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_LineStackVectorB[index]->SetColor(m_Colors[COLOR_SELECTION]);
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
void albaInteractor2DMeasure_LineDistance::SetPointSize(double size)
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
void albaInteractor2DMeasure_LineDistance::SetLineWidth(double width)
{
	m_LineWidth = width;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_LineStackVector[i]->GetProperty()->SetLineWidth(m_LineWidth);
		m_LineStackVectorB[i]->GetProperty()->SetLineWidth(m_LineWidth);
		m_LineStackVectorPerp[i]->GetProperty()->SetLineWidth(m_LineWidth);
	}
	Render();
}

//---------------------------------------------------------------------------
void albaInteractor2DMeasure_LineDistance::Show(bool show)
{
	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_LineStackVector[i]->SetVisibility(show);
		m_LineStackVectorB[i]->SetVisibility(show);

		m_PointsStackVectorR[i]->SetVisibility(show);
		m_PointsStackVectorL[i]->SetVisibility(show);
		m_LineStackVectorPerp[i]->SetVisibility(show);

		m_TextActorVector[i]->SetVisibility(show && m_ShowText);
	}

	Render();
}

/// UTILS ///////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_LineDistance::GetMeasureLinePoints(int index, double *point1, double *point2)
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
bool albaInteractor2DMeasure_LineDistance::Load(albaVME *input, wxString tag)
{
	if (input->GetTagArray()->IsTagPresent(tag + "MeasureLineDistancePoint1") && input->GetTagArray()->IsTagPresent(tag + "MeasureLineDistancePoint1"))
	{
		double point1[3], point2[3];
		albaTagItem *measureTypeTag = input->GetTagArray()->GetTag(tag + "MeasureType");
		albaTagItem *measureLabelTag = input->GetTagArray()->GetTag(tag + "MeasureLineDistanceLabel");
		albaTagItem *measureLinePoint1Tag = input->GetTagArray()->GetTag(tag + "MeasureLineDistancePoint1");
		albaTagItem *measureLinePoint2Tag = input->GetTagArray()->GetTag(tag + "MeasureLineDistancePoint2");
		albaTagItem *measureLineDistanceTag = input->GetTagArray()->GetTag(tag + "MeasureLineDistance");

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
			m_CurrMeasure = m_Distances.size() - 1;
			m_SecondLineAdded[m_CurrMeasure] = true;
			m_Distances[m_CurrMeasure] = measureLineDistanceTag->GetValueAsDouble(i);
			UpdateLineActors(point1, point2);
			m_CurrMeasure = -1;
			SetMeasureLabel(i, measureLabel);
		}

		Render();

		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_LineDistance::Save(albaVME *input, wxString tag)
{
	bool result = false;
	int nLines = GetMeasureCount();

	if (nLines > 0)
	{
		albaTagItem measureTypeTag;
		measureTypeTag.SetName(tag + "MeasureType");
		measureTypeTag.SetNumberOfComponents(nLines);

		albaTagItem measureLabelTag;
		measureLabelTag.SetName(tag + "MeasureLineDistanceLabel");
		measureLabelTag.SetNumberOfComponents(nLines);

		albaTagItem measureLinePoint1Tag;
		measureLinePoint1Tag.SetName(tag + "MeasureLineDistancePoint1");
		measureLinePoint1Tag.SetNumberOfComponents(nLines);

		albaTagItem measureLinePoint2Tag;
		measureLinePoint2Tag.SetName(tag + "MeasureLineDistancePoint2");
		measureLinePoint2Tag.SetNumberOfComponents(nLines);

		albaTagItem measureLineDistanceTag;
		measureLineDistanceTag.SetName(tag + "MeasureLineDistance");
		measureLineDistanceTag.SetNumberOfComponents(nLines);


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

			measureLineDistanceTag.SetValue(m_Distances[i], i);
		}

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureType"))
			input->GetTagArray()->DeleteTag(tag + "MeasureType");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureLineLabel"))
			input->GetTagArray()->DeleteTag(tag + "MeasureLineLabel");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureLinePoint1"))
			input->GetTagArray()->DeleteTag(tag + "MeasureLinePoint1");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureLinePoint2"))
			input->GetTagArray()->DeleteTag(tag + "MeasureLinePoint2");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureLineDistance"))
			input->GetTagArray()->DeleteTag(tag + "MeasureLineDistance");

		input->GetTagArray()->SetTag(measureTypeTag);
		input->GetTagArray()->SetTag(measureLabelTag);
		input->GetTagArray()->SetTag(measureLinePoint1Tag);
		input->GetTagArray()->SetTag(measureLinePoint2Tag);
		input->GetTagArray()->SetTag(measureLineDistanceTag);

		result = true;
	}

	return result;
}

//----------------------------------------------------------------------------
double albaInteractor2DMeasure_LineDistance::GetDistance(int idx)
{
	if (idx >= 0 && idx < m_Distances.size())
		return fabs(m_Distances[idx]);
	else
		return 0;
}
