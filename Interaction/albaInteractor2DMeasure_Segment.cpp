﻿/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Segment.cpp
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

#include "albaInteractor2DMeasure_Segment.h"
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
albaCxxTypeMacro(albaInteractor2DMeasure_Segment)

//----------------------------------------------------------------------------
albaInteractor2DMeasure_Segment::albaInteractor2DMeasure_Segment() : albaInteractor2DMeasure_Distance()
{
	m_ShowText = false;
	m_TextSide = 1;

	m_MeasureTypeText = "SEGMENT";

	m_CurrPoint = NO_POINT;

	Color color{ 1, 0.4, 1, 1.0 };

	SetColorDefault(color.R, color.G, color.B, 0.85);
	SetColorSelection(color.R, color.G, color.B, 1.0);
	SetColorDisable(color.R, color.G, color.B, 0.3);
	SetColorText(color.R, color.G, color.B, 0.5);
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure_Segment::~albaInteractor2DMeasure_Segment(){}

/// RENDERING ////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Segment::EditMeasure(int index, double *point)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	m_MovingMeasure = true;

	double point1[3];
	double point2[3];

	vtkLineSource* lineSource = (vtkLineSource*)m_LineStackVector[index]->GetSource();
	lineSource->GetPoint1(point1);
	lineSource->GetPoint2(point2);

	if (point1[X] > point2[X]) // Swap points
	{
		std::swap(point1, point2);
		m_CurrPoint = (m_CurrPoint == POINT_1) ? POINT_2 : POINT_1;
	}

	if (m_CurrPoint == POINT_1)
	{
		double dist1 = DistanceBetweenPoints(point1, point);
		double dist2 = DistancePointToLine(point, point1, point2);

		int sign = (point[X]-point1[X] >= 0) ? 1 : -1;

		double dist = sqrt((dist1*dist1) - (dist2*dist2));
		dist = !isnan(dist) ? dist : 0.0;

		double newPoint[3];
		FindPointOnLine(newPoint, point1, point2, dist*sign);

		if (newPoint[X] < point2[X])
		{
			point1[X] = newPoint[X];
			point1[Y] = newPoint[Y];
			point1[Z] = newPoint[Z];
		}
	}
	else if (m_CurrPoint == POINT_2)
	{
		double dist1 = DistanceBetweenPoints(point1, point);
		double dist2 = DistancePointToLine(point, point1, point2);
		double len = DistanceBetweenPoints(point1, point2);

		int sign = (point[X] - point1[X] >= 0) ? 1 : -1;

		double dist = sqrt((dist1*dist1) - (dist2*dist2));
		dist = !isnan(dist) ? dist : 0.0;

		double newPoint[3];
		FindPointOnLine(newPoint, point1, point2, dist*sign);

		if (newPoint[X] > point1[X])
		{
			point2[X] = newPoint[X];
			point2[Y] = newPoint[Y];
			point2[Z] = newPoint[Z];
		}
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

/// LOAD/SAVE ////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_Segment::Load(albaVME *input, wxString tag)
{
	if (input->GetTagArray()->IsTagPresent(tag + "MeasureSegmentPoint1") && input->GetTagArray()->IsTagPresent(tag + "MeasureSegmentPoint2"))
	{
		double point1[3], point2[3];
		albaTagItem *measureTypeTag = input->GetTagArray()->GetTag(tag + "MeasureType");
		albaTagItem *measureLabelTag = input->GetTagArray()->GetTag(tag + "MeasureSegmentLabel");
		albaTagItem *MeasureSegmentPoint1Tag = input->GetTagArray()->GetTag(tag + "MeasureSegmentPoint1");
		albaTagItem *MeasureSegmentPoint2Tag = input->GetTagArray()->GetTag(tag + "MeasureSegmentPoint2");

		int nLines = MeasureSegmentPoint1Tag->GetNumberOfComponents() / 3;

		m_CurrentRenderer = m_Renderer;

		// Reload points
		for (int i = 0; i < nLines; i++)
		{
			point1[X] = MeasureSegmentPoint1Tag->GetValueAsDouble(i * 3 + 0);
			point1[Y] = MeasureSegmentPoint1Tag->GetValueAsDouble(i * 3 + 1);
			point1[Z] = MeasureSegmentPoint1Tag->GetValueAsDouble(i * 3 + 2);

			point2[X] = MeasureSegmentPoint2Tag->GetValueAsDouble(i * 3 + 0);
			point2[Y] = MeasureSegmentPoint2Tag->GetValueAsDouble(i * 3 + 1);
			point2[Z] = MeasureSegmentPoint2Tag->GetValueAsDouble(i * 3 + 2);

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
bool albaInteractor2DMeasure_Segment::Save(albaVME *input, wxString tag)
{
	bool result = false;
	int nLines = GetMeasureCount();

	if (nLines > 0)
	{
		albaTagItem measureTypeTag;
		measureTypeTag.SetName(tag + "MeasureType");
		measureTypeTag.SetNumberOfComponents(nLines);

		albaTagItem measureLabelTag;
		measureLabelTag.SetName(tag + "MeasureSegmentLabel");
		measureLabelTag.SetNumberOfComponents(nLines);

		albaTagItem MeasureSegmentPoint1Tag;
		MeasureSegmentPoint1Tag.SetName(tag + "MeasureSegmentPoint1");
		MeasureSegmentPoint1Tag.SetNumberOfComponents(nLines);

		albaTagItem MeasureSegmentPoint2Tag;
		MeasureSegmentPoint2Tag.SetName(tag + "MeasureSegmentPoint2");
		MeasureSegmentPoint2Tag.SetNumberOfComponents(nLines);

		for (int i = 0; i < nLines; i++)
		{
			double point1[3], point2[3];
			GetMeasureLinePoints(i, point1, point2);

			measureTypeTag.SetValue(GetTypeName(), i);
			measureLabelTag.SetValue(GetMeasureLabel(i), i);

			MeasureSegmentPoint1Tag.SetValue(point1[X], i * 3 + 0);
			MeasureSegmentPoint1Tag.SetValue(point1[Y], i * 3 + 1);
			MeasureSegmentPoint1Tag.SetValue(point1[Z], i * 3 + 2);

			MeasureSegmentPoint2Tag.SetValue(point2[X], i * 3 + 0);
			MeasureSegmentPoint2Tag.SetValue(point2[Y], i * 3 + 1);
			MeasureSegmentPoint2Tag.SetValue(point2[Z], i * 3 + 2);
		}

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureType"))
			input->GetTagArray()->DeleteTag(tag + "MeasureType");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureSegmentLabel"))
			input->GetTagArray()->DeleteTag(tag + "MeasureSegmentLabel");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureSegmentPoint1"))
			input->GetTagArray()->DeleteTag(tag + "MeasureSegmentPoint1");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureSegmentPoint2"))
			input->GetTagArray()->DeleteTag(tag + "MeasureSegmentPoint2");

		input->GetTagArray()->SetTag(measureTypeTag);
		input->GetTagArray()->SetTag(measureLabelTag);
		input->GetTagArray()->SetTag(MeasureSegmentPoint1Tag);
		input->GetTagArray()->SetTag(MeasureSegmentPoint2Tag);

		result = true;
	}

	return result;
}