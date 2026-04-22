/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Ellipse.cpp
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

#include "albaInteractor2DMeasure_Ellipse.h"
#include "albaInteractor2DMeasure.h"

#include "albaTagArray.h"
#include "albaVME.h"

#include "vtkALBATextActorMeter.h"
#include "vtkALBAEllipseSource.h"
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
#include "albaVect3d.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DMeasure_Ellipse)

//----------------------------------------------------------------------------
albaInteractor2DMeasure_Ellipse::albaInteractor2DMeasure_Ellipse() : albaInteractor2DMeasure()
{
	m_ShowText = false;
	m_TextSide = 1;

	m_MeasureTypeText = "CENTER_POINT";

	m_CurrPoint = NO_POINT;
	m_CTRLPressed = false;
	m_CenterPointSize = 5.0;

	m_LastPoint[0] = m_LastPoint[1] = m_LastPoint[2] = 0.0;

	Color color{ 0.6, 0.0, 0.6, 1.0 };

	SetColorDefault(color.R, color.G, color.B, 0.65);
	SetColorSelection(color.R, color.G, color.B, 1.0);
	SetColorDisable(color.R, color.G, color.B, 0.3);
	SetColorText(color.R, color.G, color.B, 0.5);
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure_Ellipse::~albaInteractor2DMeasure_Ellipse()
{
	GetLogicManager()->UnRegisterForKeyEvents(this);

	// Lines and Points
	for (int i = 0; i < m_EllipseStackVector.size(); i++)
	{
		cppDEL(m_PointsStackVectorL[i]);
		cppDEL(m_PointsStackVectorR[i]);
		cppDEL(m_PointsStackVectorC[i]);		
		cppDEL(m_EllipseStackVector[i]);
	}
}

/// RENDERING ////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::DrawNewMeasure(double * wp)
{
	AddMeasure(wp, wp);
	//Call FindAndHighLight to start the edit phase
	FindAndHighlight(wp);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::MoveMeasure(int index, double * point)
{
	if (index < 0)
		return;

	double point1[3];
	double point2[3];
	
	GetMeasurePoints(index, point1, point2);

	if (!m_MovingMeasure)
	{
		m_OldLineP1[X] = point1[X] - m_StartMousePosition[X];
		m_OldLineP1[Y] = point1[Y] - m_StartMousePosition[Y];
		m_OldLineP1[Z] = point1[Z] - m_StartMousePosition[Z];

		m_OldLineP2[X] = point2[X] - m_StartMousePosition[X];
		m_OldLineP2[Y] = point2[Y] - m_StartMousePosition[Y];
		m_OldLineP2[Z] = point2[Z] - m_StartMousePosition[Z];

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

	// Points
	UpdatePointsActor(tmp_pos1, tmp_pos2);
	// Epplise
	UpdateEllipseActor(tmp_pos1, tmp_pos2);
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
void albaInteractor2DMeasure_Ellipse::EditMeasure(int index, double *point)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	m_MovingMeasure = true;

	double point1[3];
	double point2[3];

	GetMeasurePoints(index, point1, point2);


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
	text.Printf("%.2f mm", DistanceBetweenPoints(point1, point2));
	//m_MeasureTextVector[index] = text;
	m_Measure2DVector[index].Text = text;

	// Points
	UpdatePointsActor(point1, point2);
	// Ellipse
	UpdateEllipseActor(point1, point2);
	// Text
	UpdateTextActor(point1, point2);

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));

	//////////////////////////////////////////////////////////////////////////
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::RotateMeasure(int index, double *point)
{
	albaVect3d center, p1, p2, currPoint, v1, v2;

	GetCenter(m_CurrMeasure, center.GetVect());
	GetMeasurePoints(m_CurrMeasure, p1.GetVect(), p2.GetVect());

	currPoint = m_CurrPoint == POINT_1 ? p1 : p2;

	v1 = currPoint - center;
	v2 = albaVect3d(point) - center;

	int A, B, C;

	if (m_CurrPlane == 0) { A = 0; B = 1; C = 2; } //XY
	else if (m_CurrPlane == 1) { A = 1; B = 2; C = 0; } //YZ
	else if (m_CurrPlane == 2) { A = 0; B = 2; C = 1; } //XZ

	// Calculate angle between v1 and v2 in the current plane
	
	double angle1 = atan2(v1[B], v1[A]);
	double angle2 = atan2(v2[B], v2[A]);
	double angle = angle2 - angle1;

	// Update ellipse rotation
	vtkALBAEllipseSource *ellipseSource = (vtkALBAEllipseSource *)m_EllipseStackVector[m_CurrMeasure]->GetSourceAlgorithm();
	ellipseSource->SetTheta(ellipseSource->GetTheta() + angle);

	// Create transform for rotation
	vtkTransform *transform = vtkTransform::New();
	transform->Translate(center[0], center[1], center[2]);

	// Set rotation axis based on current plane
	double rotAxis[3] = { 0.0, 0.0, 1.0 }; // Default: Z-axis (XY plane)
	if (m_CurrPlane == 1) // YZ plane
		rotAxis[0] = 1.0, rotAxis[2] = 0.0; // X-axis
	else if (m_CurrPlane == 2) // XZ plane
		rotAxis[1] = 1.0, rotAxis[2] = 0.0; // Y-axis

	transform->RotateWXYZ( vtkMath::DegreesFromRadians(angle), rotAxis[0], rotAxis[1], rotAxis[2]);
	transform->Translate(-center[0], -center[1], -center[2]);
	transform->Update();

	// Apply transformation to both points
	double newP1[3], newP2[3];
	transform->TransformPoint(p1.GetVect(), newP1);
	transform->TransformPoint(p2.GetVect(), newP2);

	// Update visual elements
	UpdatePointsActor(newP1, newP2);
	UpdateEllipseActor(newP1, newP2);
	UpdateTextActor(newP1, newP2);

	// Clean up
	vtkDEL(transform);
	
	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));

	//////////////////////////////////////////////////////////////////////////
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::FindAndHighlight(double * point)
{
	if (m_CurrMeasure < 0)
		SetAction(ACTION_ADD_MEASURE);

	m_LastPoint[0] = point[0];
	m_LastPoint[1] = point[1];
	m_LastPoint[2] = point[2];

	if (m_EditMeasureEnable && GetAction()!=albaInteractor2DMeasure::ACTION_ROTATING_MEASURE)
	{
		SetUpdateDistance(PixelSizeInWorld()*4.0);
		
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			if (m_Renderer != m_PointsStackVectorL[i]->GetRenderer())
				continue;

			double point1[3], point2[3];
			GetMeasurePoints(i, point1, point2);
			
			double centerPoint[3];
			GetMidPoint(centerPoint, point1, point2);

			double radius = vtkMath::Distance2BetweenPoints(point2, centerPoint);

			if (DistancePointToLine(point, point1, point2) < m_PointUpdateDist)
			{
				SelectMeasure(i); 

				double p1Dist = DistanceBetweenPoints(point,point1);
				double p2Dist = DistanceBetweenPoints(point,point2);
				double p1p2Dist = DistanceBetweenPoints(point1, point2);
				double minDist = MIN(m_PointUpdateDist, (p1p2Dist/3.0));
				
				if ((p1Dist < p2Dist) && (p1Dist <= minDist))
				{
					if(m_CTRLPressed)
						SetAction(ACTION_ROTATE_MEASURE);
					else
						SetAction(ACTION_EDIT_MEASURE);
					m_CurrPoint = POINT_1;
					m_PointsStackVectorL[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else if (p2Dist <= minDist)
				{
					if (m_CTRLPressed)
						SetAction(ACTION_ROTATE_MEASURE);
					else
						SetAction(ACTION_EDIT_MEASURE);
					m_CurrPoint = POINT_2;
					m_PointsStackVectorR[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else if (vtkMath::Distance2BetweenPoints(centerPoint, point) < m_PointUpdateDist2)
				{
					SetAction(ACTION_MOVE_MEASURE);
					m_CurrPoint = POINT_1;
					m_PointsStackVectorC[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				Render();
				return;
			}
		}

		if (m_CurrMeasure >= 0)
		{
			SelectMeasure(-1);
			m_CurrPoint = NO_POINT;
			Render();
		}
	}
}

/// UPDATE ///////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::UpdatePointsActor(double * point1, double * point2)
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
void albaInteractor2DMeasure_Ellipse::UpdateEllipseActor(double * point1, double * point2)
{

	int A, B, C;
	vtkALBAEllipseSource *ellipseSource = (vtkALBAEllipseSource *)m_EllipseStackVector[m_CurrMeasure]->GetSourceAlgorithm();

	if (m_CurrPlane == 0) { A = 0; B = 1; C = 2; } //XY
	else if (m_CurrPlane == 1) { A = 1; B = 2; C = 0; } //YZ
	else if (m_CurrPlane == 2) { A = 0; B = 2; C = 1; } //XZ

	double angle = ellipseSource->GetTheta();


	double midPoint[3]; 
	GetMidPoint(midPoint, point1, point2);

	// Create transform for rotation
	vtkTransform *transform = vtkTransform::New();
	transform->Translate(midPoint[0], midPoint[1], midPoint[2]);

	// Set rotation axis based on current plane
	double rotAxis[3] = { 0.0, 0.0, 1.0 }; // Default: Z-axis (XY plane)
	if (m_CurrPlane == 1) // YZ plane
		rotAxis[0] = 1.0, rotAxis[2] = 0.0; // X-axis
	else if (m_CurrPlane == 2) // XZ plane
		rotAxis[1] = 1.0, rotAxis[2] = 0.0; // Y-axis

	//Rotating back to obtain ortho Axiss
	transform->RotateWXYZ(-vtkMath::DegreesFromRadians(angle), rotAxis[0], rotAxis[1], rotAxis[2]);
	transform->Translate(-midPoint[0], -midPoint[1], -midPoint[2]);
	transform->Update();

	// Apply transformation to both points
	double unRotatedP1[3], unRotatedP2[3];
	transform->TransformPoint(point1, unRotatedP1);
	transform->TransformPoint(point2, unRotatedP2);



	double majorAxis = (unRotatedP1[A] - unRotatedP2[A]) / 2;
	double minorAxis = (unRotatedP1[B] - unRotatedP2[B]) / 2;


	ellipseSource->SetPlane(m_CurrPlane);
	ellipseSource->SetMajorAxis(fabs(majorAxis));
	ellipseSource->SetMinorAxis(fabs(minorAxis));
	ellipseSource->SetCenter(midPoint);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::UpdateTextActor(double * point1, double * point2)
{
	double text_pos[3];
	GetMidPoint(text_pos, point1, point2);

	text_pos[X] += m_TextSide *TEXT_W_SHIFT;

	Superclass::UpdateTextActor(m_CurrMeasure, text_pos);
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::Enable(bool enable /*= true*/)
{
	if (enable)
		GetLogicManager()->RegisterForKeyEvents(this);
	else
		GetLogicManager()->UnRegisterForKeyEvents(this);

	Superclass::Enable(enable);
}

/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::AddMeasure(double *point1, double *point2)
{
	if (GetMeasureCount() > 0)
	{
		int index = GetMeasureCount() - 1;
		double oldPoint1[3], oldPoint2[3];
		GetMeasurePoints(index, oldPoint1, oldPoint2);

		bool hasSameRenderer = (m_Renderer == m_Measure2DVector[index].Renderer);

		if (DistanceBetweenPoints(oldPoint1, oldPoint2)<m_PointUpdateDist)
		{
			if (!hasSameRenderer) return;

			SelectMeasure(index);
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
	text.Printf("(%.2f, %.2f) - Radius %.2f mm", point1[X], point1[Y], DistanceBetweenPoints(point1, point2));
	m_Measure2DVector[index].Text = text;

	// Update Edit Actors
	UpdateEditActors(point1, point2);

	// Add Points
	m_PointsStackVectorL.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorR.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorC.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));

	// Add Ellipse
	m_EllipseStackVector.push_back(new albaActor2dStackHelper(vtkALBAEllipseSource::New(), m_Renderer));
	
	//////////Setting mapper/actors/proprieties//////////////
	int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;
	
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

	// Ellipse
	m_EllipseStackVector[index]->GetProperty()->SetLineWidth(m_LineWidth);
	m_EllipseStackVector[index]->SetColor(m_Colors[col]);

	vtkALBAEllipseSource *ellipseSource = (vtkALBAEllipseSource *)m_EllipseStackVector[index]->GetSourceAlgorithm();
	ellipseSource->SetResolution(60);

	//////////////////////////////////////////////////////////////////////////

	SelectMeasure(index);

	UpdatePointsActor(point1, point2);
	UpdateEllipseActor(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasureText(m_CurrMeasure)));

	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::RemoveMeasure(int index)
{
	if (m_Renderer && index < GetMeasureCount())
	{
		Superclass::RemoveMeasure(index);

		//////////////////////////////////////////////////////////////////////////
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

		// Ellipse
		cppDEL(m_EllipseStackVector[index]);
		m_EllipseStackVector.erase(m_EllipseStackVector.begin() + index);
	
		Render();
	}	
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::SelectMeasure(int index)
{
	if (index != m_CurrMeasure && GetMeasureCount() > 0)
	{
		m_CurrMeasure = index;
		m_LastEditing = -1;

		// Deselect all
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

			m_PointsStackVectorL[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorR[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorC[i]->SetColor(m_Colors[col]);
			m_EllipseStackVector[i]->SetColor(m_Colors[col]);
			SetColor(m_TextActorVector[i], &m_Colors[col]);
		}

		if (index >= 0)
		{
			m_PointsStackVectorL[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorR[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorC[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_EllipseStackVector[index]->SetColor(m_Colors[COLOR_SELECTION]);
			SetColor(m_TextActorVector[index], &m_Colors[COLOR_SELECTION]);

			albaEventMacro(albaEvent(this, ID_MEASURE_SELECTED));
		}
	}
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::Show(bool show)
{
	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVectorL[i]->SetVisibility(show);
		m_PointsStackVectorR[i]->SetVisibility(show);
		m_PointsStackVectorC[i]->SetVisibility(show);
		m_EllipseStackVector[i]->SetVisibility(show);
		m_TextActorVector[i]->SetVisibility(show && m_ShowText);
	}

	Render();
}

// SET/GET
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::SetPointSize(double size)
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
void albaInteractor2DMeasure_Ellipse::SetCenterPointSize(double size)
{
	m_CenterPointSize = size;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVectorC[i]->GetProperty()->SetPointSize(m_CenterPointSize);
	}
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::SetLineWidth(double width)
{
	m_LineWidth = width;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_EllipseStackVector[i]->GetProperty()->SetLineWidth(m_LineWidth);
	}
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::GetMeasurePoints(int index, double *point1, double *point2)
{
	vtkPointSource *pointSourceL = (vtkPointSource *)m_PointsStackVectorL[index]->GetSourceAlgorithm();
	pointSourceL->GetCenter(point1);
	vtkPointSource *pointSourceR = (vtkPointSource *)m_PointsStackVectorR[index]->GetSourceAlgorithm();
	pointSourceR->GetCenter(point2);
}

/// UTILS ///////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::GetCenter(int index, double *center)
{
	// Return center points values
	if (index >= 0 && index < GetMeasureCount())
	{
		vtkALBAEllipseSource* ellipseSource = (vtkALBAEllipseSource*)m_EllipseStackVector[index]->GetSourceAlgorithm();
		ellipseSource->GetCenter(center);
	}
}

/// LOAD/SAVE ///////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_Ellipse::Load(albaVME *input, wxString tag)
{
	if (input->GetTagArray()->IsTagPresent(tag + "MeasureCenterPoint1") && input->GetTagArray()->IsTagPresent(tag + "MeasureCenterPoint2"))
	{
		double point1[3], point2[3];
		albaTagItem *measureTypeTag = input->GetTagArray()->GetTag(tag + "MeasureType");
		albaTagItem *measureLabelTag = input->GetTagArray()->GetTag(tag + "MeasureCenterLabel");
		albaTagItem *measureCenterPoint1Tag = input->GetTagArray()->GetTag(tag + "MeasureCenterPoint1");
		albaTagItem *measureCenterPoint2Tag = input->GetTagArray()->GetTag(tag + "MeasureCenterPoint2");
		albaTagItem *measureAngleTag = input->GetTagArray()->GetTag(tag + "MeasureAngle");
		 
		/**TODO: Read MeasureAngle*/
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
bool albaInteractor2DMeasure_Ellipse::Save(albaVME *input, wxString tag)
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
			GetMeasurePoints(i, point1, point2);

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

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Ellipse::OnEvent(albaEventBase *event)
{
	if (event->GetId() == KEY_PRESSED)
	{
		bool isCTRL = ((albaEvent *)event)->GetArg() == WXK_CONTROL;
		if (isCTRL)
		{
			m_CTRLPressed = true;
			FindAndHighlight(m_LastPoint);
		}
	}
	else if (event->GetId() == KEY_RELEASED)
	{
		bool isCTRL = ((albaEvent *)event)->GetArg() == WXK_CONTROL;
		if (isCTRL)
		{
			m_CTRLPressed = false;
			FindAndHighlight(m_LastPoint);
		}
	}
	else
		Superclass::OnEvent(event);
}
