/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Point.cpp
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

#include "albaInteractor2DMeasure_Point.h"
#include "albaInteractor2DMeasure.h"

#include "albaTagArray.h"
#include "albaVME.h"

#include "vtkALBATextActorMeter.h"
#include "vtkActor2D.h"
#include "vtkPointSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DMeasure_Point)

//----------------------------------------------------------------------------
albaInteractor2DMeasure_Point::albaInteractor2DMeasure_Point() : albaInteractor2DMeasure()
{
	m_ShowText = false;
	m_TextSide = -2;

	m_MeasureTypeText = "POINT";
	m_EditConstraint = FREE_EDIT;

	Color color{ 0.4, 0.0, 1, 1.0 };

	SetColorDefault(color.R, color.G, color.B, 0.85);
	SetColorSelection(color.R, color.G, color.B, 1.0);
	SetColorDisable(color.R, color.G, color.B, 0.3);
	SetColorText(color.R, color.G, color.B, 0.5);
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure_Point::~albaInteractor2DMeasure_Point()
{
	// Points
	for (int i = 0; i < m_PointsStackVector.size(); i++)
	{
		cppDEL(m_PointsStackVector[i]);
	}
}

/// RENDERING ////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::DrawNewMeasure(double *wp)
{
	AddMeasure(wp);
	//Call FindAndHighLight to start the edit phase
	FindAndHighlight(wp);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::MoveMeasure(int index, double *point)
{
	EditMeasure(index, point);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::EditMeasure(int index, double *point)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	point[Z] = 0;

	switch (m_EditConstraint)
	{
	case FREE_EDIT: break; // None
	case LOCK_ON_LINE:
		GetPointOnLine(point);
		break;
	}

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	albaString text;
	text.Printf("Point (%.2f, %.2f)", point[X], point[Y]);
	m_Measure2DVector[index].Text = text;

	// Point
	UpdatePointsActor(point);
	// Text
	UpdateTextActor(index, point);

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::FindAndHighlight(double *point)
{
	if (m_CurrMeasure < 0)
		SetAction(ACTION_ADD_MEASURE);

	if (m_EditMeasureEnable)
	{
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			albaActor2dStackHelper *pointsStackVector = m_PointsStackVector[i];
			if(m_Renderer!= pointsStackVector->GetRenderer())
				continue;

			double tmpPoint[3];

			vtkPointSource* pointSource = (vtkPointSource*)pointsStackVector->GetSource();
			pointSource->GetCenter(tmpPoint);

			if (GeometryUtils::DistanceBetweenPoints(point, tmpPoint) < POINT_UPDATE_DISTANCE)
			{
				m_CurrMeasure = i;

				if (m_MoveMeasureEnable)
				{
					pointsStackVector->SetColor(m_Colors[COLOR_EDIT]);
					SetAction(ACTION_MOVE_MEASURE);
				}

				Render();
				return;
			}
		}

		if (m_CurrMeasure >= 0)
		{
			SelectMeasure(-1);
			m_CurrMeasure = -1;
			Render();
		}
	}
}

/// UPDATE ///////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::UpdatePointsActor(double * point, double * point2)
{
	vtkPointSource* pointSource = (vtkPointSource*)m_PointsStackVector[m_CurrMeasure]->GetSource();
	pointSource->SetCenter(point);
	pointSource->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::UpdateTextActor(int index, double *point)
{
	double text_pos[3];
	text_pos[X] = point[X];
	text_pos[Y] = point[Y];
	text_pos[Z] = point[Z];

	text_pos[X] -= m_TextSide *TEXT_H_SHIFT;

	Superclass::UpdateTextActor(index, text_pos);
}

/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::AddMeasure(double *point)
{
	Superclass::AddMeasure(point);

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	int index = m_Measure2DVector.size() - 1;

	albaString text;
	text.Printf("Point (%.2f, %.2f)", point[X], point[Y]);
	m_Measure2DVector[m_Measure2DVector.size() - 1].Text = text;

	// Update Edit Actors
	UpdateEditActors(point);

	//////////////////////////////////////////////////////////////////////////
	// Add Points
	m_PointsStackVector.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));

	//////////Setting proprieties//////////////
	int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

	m_PointsStackVector[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVector[index]->SetColor(m_Colors[col]);

	m_CurrMeasure = index;

	UpdatePointsActor(point);

	//////////////////////////////////////////////////////////////////////////
	albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasureText(GetMeasureCount() - 1)));

	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::RemoveMeasure(int index)
{
	if (m_Renderer && index < GetMeasureCount())
	{
		Superclass::RemoveMeasure(index);

		//////////////////////////////////////////////////////////////////////////
		// Point
		cppDEL(m_PointsStackVector[index]);
		m_PointsStackVector.erase(m_PointsStackVector.begin() + index);

		//////////////////////////////////////////////////////////////////////////
		
		Render();
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::SelectMeasure(int index)
{
	if (GetMeasureCount() > 0)
	{
		// Deselect all
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

			m_PointsStackVector[i]->SetColor(m_Colors[col]);
			SetColor(m_TextActorVector[i], &m_Colors[col]);
		}

		if (index >= 0)
		{
			m_PointsStackVector[index]->SetColor(m_Colors[COLOR_SELECTION]);
			SetColor(m_TextActorVector[index], &m_Colors[COLOR_SELECTION]);

			albaEventMacro(albaEvent(this, ID_MEASURE_SELECTED));
		}

		m_LastSelection = index;
		m_LastEditing = -1;
	}
}

// SET/GET
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::SetPointSize(double size)
{
	m_PointSize = size;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVector[i]->GetProperty()->SetPointSize(m_PointSize);
	}
	Render();
}

/// UTILS ///////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::GetMeasurePoint(int index, double *point)
{
	// Return point value
	if (index >= 0 && index < GetMeasureCount())
	{
		vtkPointSource *pointSource = (vtkPointSource*)m_PointsStackVector[index]->GetSource();
		pointSource->GetCenter(point);
	}
}

/// LOAD/SAVE ///////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_Point::Load(albaVME *input, wxString tag)
{
	if (input->GetTagArray()->IsTagPresent(tag + "MeasurePoint"))
	{
		double point1[3];
		albaTagItem *measureTypeTag = input->GetTagArray()->GetTag(tag + "MeasureType");
		albaTagItem *measureLabelTag = input->GetTagArray()->GetTag(tag + "MeasurePointLabel");
		albaTagItem *measurePointTag = input->GetTagArray()->GetTag(tag + "MeasurePoint");

		int nPoints = measurePointTag->GetNumberOfComponents() / 2;

		// Reload points
		for (int i = 0; i < nPoints; i++)
		{
			point1[X] = measurePointTag->GetValueAsDouble(i * 2 + 0);
			point1[Y] = measurePointTag->GetValueAsDouble(i * 2 + 1);
			point1[Z] = 0.0;

			albaString measureType = measureTypeTag->GetValue(i);
			albaString measureLabel = measureLabelTag->GetValue(i);

			AddMeasure(point1);
			SetMeasureLabel(i, measureLabel);
		}

		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_Point::Save(albaVME *input, wxString tag)
{
	bool result = false;
	int nPoints = GetMeasureCount();

	if (nPoints > 0)
	{
		albaTagItem measureTypeTag;
		measureTypeTag.SetName(tag + "MeasureType");
		measureTypeTag.SetNumberOfComponents(nPoints);

		albaTagItem measureLabelTag;
		measureLabelTag.SetName(tag + "MeasurePointLabel");
		measureLabelTag.SetNumberOfComponents(nPoints);

		albaTagItem measurePointTag;
		measurePointTag.SetName(tag + "MeasurePoint");
		measurePointTag.SetNumberOfComponents(nPoints);

		for (int i = 0; i < nPoints; i++)
		{
			double point1[3];
			GetMeasurePoint(i, point1);

			measureTypeTag.SetValue(GetTypeName(), i);
			measureLabelTag.SetValue(GetMeasureLabel(i), i);

			measurePointTag.SetValue(point1[X], i * 2 + 0);
			measurePointTag.SetValue(point1[Y], i * 2 + 1);
		}

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureType"))
			input->GetTagArray()->DeleteTag(tag + "MeasureType");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasurePointLabel"))
			input->GetTagArray()->DeleteTag(tag + "MeasurePointLabel");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasurePoint"))
			input->GetTagArray()->DeleteTag(tag + "MeasurePoint");

		input->GetTagArray()->SetTag(measureTypeTag);
		input->GetTagArray()->SetTag(measureLabelTag);
		input->GetTagArray()->SetTag(measurePointTag);

		result = true;
	}

	return result;
}

/// CONSTRAINTS
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::LockPointOnLine(double *lineP1, double *lineP2)
{
	m_EditConstraint = LOCK_ON_LINE;

	m_ConstrLineP1[X] = lineP1[X];
	m_ConstrLineP1[Y] = lineP1[Y];
	m_ConstrLineP1[Z] = 0.0;

	m_ConstrLineP2[X] = lineP2[X];
	m_ConstrLineP2[Y] = lineP2[Y];
	m_ConstrLineP2[Z] = 0.0;
}
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Point::GetPointOnLine(double *point)
{
	double minLenght = 5.0;

	double dist1 = GeometryUtils::DistanceBetweenPoints(m_ConstrLineP1, point);
	double dist2 = GeometryUtils::GetPointToLineDistance(point, m_ConstrLineP1, m_ConstrLineP2);

	int sign = (point[X] - m_ConstrLineP1[X] >= 0) ? 1 : -1;

	double dist = sqrt((dist1*dist1) - (dist2*dist2));
	dist = !isnan(dist) ? dist : 0.0;

	double newPoint[3];
	GeometryUtils::FindPointOnLine(newPoint, m_ConstrLineP1, m_ConstrLineP2, dist*sign);

	point[X] = newPoint[X];
	point[Y] = newPoint[Y];
}