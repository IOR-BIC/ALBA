/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DMeasure_Template.cpp
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

#include "albaInteractor2DMeasure_Template.h"
#include "albaInteractor2DMeasure.h"

#include "albaGUILutPreset.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEImage.h"
#include "albaVMEOutput.h"

#include "vtkALBATextActorMeter.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkCellArray.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkDataSetMapper.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkPNGReader.h"
#include "vtkPointData.h"
#include "vtkPointSource.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyDataSource.h"
#include "vtkPolygon.h"
#include "vtkProbeFilter.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTexture.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkViewport.h"
#include "vtkLookupTable.h"
#include "defines.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DMeasure_Template)

//----------------------------------------------------------------------------
albaInteractor2DMeasure_Template::albaInteractor2DMeasure_Template() : albaInteractor2DMeasure()
{
	m_ShowText = false;
	m_TextSide = 1;

	m_MeasureTypeText = "TEMPLATE";
	m_EditMeasureEnable = false;
	m_CurrPoint = NO_POINT;
	m_CurrPlane = 0; // XY;

	m_ConfigDir = "";

	Color color{ 0.6, 0.0, 0.6, 1.0 };

	SetColorDefault(color.R, color.G, color.B, 0.85);
	SetColorSelection(color.R, color.G, color.B, 1.0);
	SetColorDisable(color.R, color.G, color.B, 0.3);
	SetColorText(color.R, color.G, color.B, 0.5);

	m_TemplateLookupTable = NULL;

	ShowPoints(false);
	SetMaxMeasures(1);

	m_Angle = 0.0;
}
//----------------------------------------------------------------------------
albaInteractor2DMeasure_Template::~albaInteractor2DMeasure_Template()
{
	// Points and Textures
	for (int i = 0; i < m_PointsStackVectorUL.size(); i++)
	{
		cppDEL(m_PointsStackVectorUL[i]);
		cppDEL(m_PointsStackVectorUR[i]);
		cppDEL(m_PointsStackVectorDR[i]);
		cppDEL(m_PointsStackVectorDL[i]);
		cppDEL(m_PointsStackVectorC[i]);

		m_Renderer->RemoveActor(m_TexturedQuadVector[i]);

		vtkDEL(m_QuadVector[i]);
		vtkDEL(m_TexturedQuadVector[i]);
	}

	vtkDEL(m_TemplateLookupTable);
}

/// RENDERING ////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::DrawNewMeasure(double * wp)
{
	AddMeasure(wp, wp, wp, wp);
	//Call FindAndHighLight to start the edit phase
	FindAndHighlight(wp);
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::MoveMeasure(int index, double * point)
{
	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::EditMeasure(int index, double *point)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	m_MovingMeasure = true;

	double pointUL[3], pointUR[3], pointDR[3], pointDL[3];
	GetMeasurePoints(index, pointUL, pointUR, pointDR, pointDL);

	if (m_CurrPoint == POINT_1)
	{
		pointUL[X] = point[X];
		pointUL[Y] = point[Y];
		pointUL[Z] = point[Z];
	}
	else if (m_CurrPoint == POINT_2)
	{
		pointUR[X] = point[X];
		pointUR[Y] = point[Y];
		pointUR[Z] = point[Z];
	}
	else if (m_CurrPoint == POINT_3)
	{
		pointDR[X] = point[X];
		pointDR[Y] = point[Y];
		pointDR[Z] = point[Z];
	}
	else if (m_CurrPoint == POINT_4)
	{
		pointDL[X] = point[X];
		pointDL[Y] = point[Y];
		pointDL[Z] = point[Z];
	}

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////

	// Points
	UpdatePointsActor(pointUL, pointUR, pointDR, pointDL);
	// Texture
	UpdateTexureActor(pointUL, pointUR, pointDR, pointDL);

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));

	//////////////////////////////////////////////////////////////////////////
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::FindAndHighlight(double * point)
{
	if (m_CurrMeasure < 0)
		SetAction(ACTION_ADD_MEASURE);

	if (m_EditMeasureEnable)
	{
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			double pointUL[3], pointUR[3], pointDR[3], pointDL[3];
			GetMeasurePoints(i, pointUL, pointUR, pointDR, pointDL);

			//if (GeometryUtils::DistancePointToLine(point, linePoint1, linePoint2) < POINT_UPDATE_DISTANCE)
			{
				//SelectMeasure(i);

				if (vtkMath::Distance2BetweenPoints(pointUL, point) < POINT_UPDATE_DISTANCE_2)
				{
					SelectMeasure(i);

					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_1;
					m_PointsStackVectorUL[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else if (vtkMath::Distance2BetweenPoints(pointUR, point) < POINT_UPDATE_DISTANCE_2)
				{
					SelectMeasure(i);

					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_2;
					m_PointsStackVectorUR[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else if (vtkMath::Distance2BetweenPoints(pointDR, point) < POINT_UPDATE_DISTANCE_2)
				{
					SelectMeasure(i);

					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_3;
					m_PointsStackVectorDR[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
				else if (vtkMath::Distance2BetweenPoints(pointDL, point) < POINT_UPDATE_DISTANCE_2)
				{
					SelectMeasure(i);

					SetAction(ACTION_EDIT_MEASURE);
					m_CurrMeasure = i;
					m_CurrPoint = POINT_4;
					m_PointsStackVectorDL[i]->SetColor(m_Colors[COLOR_EDIT]);
				}
// 				else if (vtkMath::Distance2BetweenPoints(centerPoint, point) < POINT_UPDATE_DISTANCE_2)
// 				{
// 					SetAction(ACTION_MOVE_MEASURE);
// 					m_CurrMeasure = i;
// 					m_CurrPoint = POINT_1;
// 					m_PointsStackVectorC[i]->SetColor(m_Colors[COLOR_EDIT]);
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
void albaInteractor2DMeasure_Template::UpdatePointsActor(double * pointUL, double * pointUR, double * pointDR, double * pointDL)
{
	// Up Left
	vtkPointSource* pointSourceUL = (vtkPointSource*)m_PointsStackVectorUL[m_CurrMeasure]->GetSource();
	pointSourceUL->SetCenter(pointUL);
	pointSourceUL->Update();

	// Up Right
	vtkPointSource* pointSourceUR = (vtkPointSource*)m_PointsStackVectorUR[m_CurrMeasure]->GetSource();
	pointSourceUR->SetCenter(pointUR);
	pointSourceUR->Update();

	// Down Right
	vtkPointSource* pointSourceDR = (vtkPointSource*)m_PointsStackVectorDR[m_CurrMeasure]->GetSource();
	pointSourceDR->SetCenter(pointDR);
	pointSourceDR->Update();
	
	// Down Left
	vtkPointSource* pointSourceDL = (vtkPointSource*)m_PointsStackVectorDL[m_CurrMeasure]->GetSource();
	pointSourceDL->SetCenter(pointDL);
	pointSourceDL->Update();

	// Center
	double pointC[3];
	GetCenter(m_CurrMeasure, pointC);

	vtkPointSource* pointSourceC = (vtkPointSource*)m_PointsStackVectorC[m_CurrMeasure]->GetSource();
	pointSourceC->SetCenter(pointC);
	pointSourceC->Update();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::UpdateTexureActor(double * pointUL, double * pointUR, double * pointDR, double * pointDL)
{
	double pointC[3];
	GetCenter(m_CurrMeasure, pointC);

	//////////////////////////////////////////////////////////////////////////

	m_Renderer->RemoveActor(m_TexturedQuadVector[m_CurrMeasure]);

	m_QuadVector[m_CurrMeasure]->GetPoints()->SetPoint(0, pointC);
	m_QuadVector[m_CurrMeasure]->GetPoints()->SetPoint(1, pointUL);
	m_QuadVector[m_CurrMeasure]->GetPoints()->SetPoint(2, pointUR);
	m_QuadVector[m_CurrMeasure]->GetPoints()->SetPoint(3, pointDR);
	m_QuadVector[m_CurrMeasure]->GetPoints()->SetPoint(4, pointDL);
	m_QuadVector[m_CurrMeasure]->GetPoints()->SetPoint(5, pointUL);
	m_QuadVector[m_CurrMeasure]->Update();

	m_Renderer->AddActor(m_TexturedQuadVector[m_CurrMeasure]);
}

/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::AddMeasure(double * pointUL, double * pointUR, double * pointDR, double * pointDL)
{
	if (GetMeasureCount() > 0)
	{
		int index = GetMeasureCount() - 1;
		double oldPoint1[3], oldPoint2[3], oldPoint3[3], oldPoint4[3];
		GetMeasurePoints(index, oldPoint1, oldPoint2, oldPoint3, oldPoint4);

		if (DistanceBetweenPoints(oldPoint1, oldPoint2) < POINT_UPDATE_DISTANCE)
		{
			m_CurrMeasure = index;
			m_CurrPoint = POINT_2;
			EditMeasure(index, pointUL);
			return;
		}
	}

	Superclass::AddMeasure(pointUL, pointDR);

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	int index = m_Measure2DVector.size() - 1;

	// Update Edit Actors
	UpdateEditActors(pointUL, pointDR);

	//////////////////////////////////////////////////////////////////////////

	// Add Points
	m_PointsStackVectorUL.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorUR.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorDR.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorDL.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));
	m_PointsStackVectorC.push_back(new albaActor2dStackHelper(GetNewPointSource(), m_Renderer));

	// Add Texture

	// Create a plane - Points
	int nPoints = 6;// 10;

	vtkPoints *points;
	vtkNEW(points);
	points->SetNumberOfPoints(nPoints);

	double midPoint[3];
	GetMidPoint(midPoint, pointUL, pointDR);

	points->SetPoint(0, midPoint);
	points->SetPoint(1, pointUL);
	points->SetPoint(2, pointUR);
	points->SetPoint(3, pointDR);
	points->SetPoint(4, pointDL);
	points->SetPoint(5, pointUL);

	// Polygon
	vtkPolygon *polygon;
	vtkNEW(polygon);
	polygon->GetPointIds()->SetNumberOfIds(nPoints); // Make Polygon

	for (int i = 0; i < nPoints; i++)
		polygon->GetPointIds()->SetId(i, i);

	// CellArray - polygons
	vtkCellArray *polygons;
	vtkNEW(polygons);
	polygons->InsertNextCell(polygon);

	// Texture Coordinates
	vtkFloatArray *textureCoordinates;
	vtkNEW(textureCoordinates);
	textureCoordinates->SetName("TextureCoordinates");
	textureCoordinates->SetNumberOfComponents(2);
	textureCoordinates->InsertTuple2(0, 0.5, 0.5);
	textureCoordinates->InsertTuple2(1, 0.0, 0.0);
	textureCoordinates->InsertTuple2(2, 1.0, 0.0);
	textureCoordinates->InsertTuple2(3, 1.0, 1.0);
	textureCoordinates->InsertTuple2(4, 0.0, 1.0);
	textureCoordinates->InsertTuple2(5, 0.0, 0.0);

	// PolyData - Quad
	m_QuadVector.push_back(vtkPolyData::New());
	m_QuadVector[index]->SetPoints(points);
	m_QuadVector[index]->SetPolys(polygons);
	m_QuadVector[index]->GetPointData()->SetTCoords(textureCoordinates);

	// Apply the texture
	vtkTexture *texture;
	vtkNEW(texture);
	//texture->SetQualityTo32Bit();
	texture->RepeatOff();
	texture->InterpolateOff();

	wxString configDir = GetConfigDirectory().c_str();

	if (!m_ConfigDir.IsEmpty())
		configDir = m_ConfigDir;	

	// Read the image which will be the texture
	vtkPNGReader *imageReader;
	vtkNEW(imageReader);
	albaString m_TextureName = "Empty";
	imageReader->SetFileName(configDir + "\\Templates\\" + m_TextureName + ".png");
	imageReader->Update();

	texture->SetInput(imageReader->GetOutput());

	// Mapper
	vtkPolyDataMapper *quadMapper;
	vtkNEW(quadMapper);
	quadMapper->SetInput(m_QuadVector[index]);

	// Actor
	m_TexturedQuadVector.push_back(vtkActor::New());
	m_TexturedQuadVector[index]->SetPosition(-1, -1, 0);
	m_TexturedQuadVector[index]->SetMapper(quadMapper);
	m_TexturedQuadVector[index]->SetTexture(texture);

	m_Renderer->AddActor(m_TexturedQuadVector[index]);

	vtkDEL(imageReader);
	vtkDEL(points);
	vtkDEL(polygons);
	vtkDEL(polygon);
	vtkDEL(textureCoordinates);
	vtkDEL(texture);
	vtkDEL(quadMapper);

	//////////Setting mapper/actors/proprieties//////////////
	int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

	//---Points---
	// Up Left
	m_PointsStackVectorUL[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorUL[index]->SetColor(m_Colors[col]);

	// Up Right
	m_PointsStackVectorUR[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorUR[index]->SetColor(m_Colors[col]);

	// Down Right
	m_PointsStackVectorDR[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorDR[index]->SetColor(m_Colors[col]);

	// Down Left
	m_PointsStackVectorDL[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorDL[index]->SetColor(m_Colors[col]);

	// Center
	m_PointsStackVectorC[index]->GetProperty()->SetPointSize(m_PointSize);
	m_PointsStackVectorC[index]->SetColor(m_Colors[col]);

	//////////////////////////////////////////////////////////////////////////

	m_CurrMeasure = index;

	ShowPoints(false);

	UpdatePointsActor(pointUL, pointUR, pointDR, pointDL);
	UpdateTexureActor(pointUL, pointUR, pointDR, pointDL);

	//////////////////////////////////////////////////////////////////////////
	albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasureText(m_CurrMeasure)));

	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::EditPoints(int index, double * pointUL, double * pointUR, double * pointDR, double * pointDL)
{
	if (index < 0 || index >= GetMeasureCount())
		return;

	m_MovingMeasure = true;
	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////

	// Points
	UpdatePointsActor(pointUL, pointUR, pointDR, pointDL);
	// Texture
	UpdateTexureActor(pointUL, pointUR, pointDR, pointDL);

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));

	//////////////////////////////////////////////////////////////////////////
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::EditTextureCoord(int index, double * pointUL, double * pointUR, double * pointDR, double * pointDL)
{
	// Texture Coordinates
	vtkFloatArray *textureCoordinates;
	vtkNEW(textureCoordinates);
	textureCoordinates->SetName("TextureCoordinates");
	textureCoordinates->SetNumberOfComponents(2);

	double center[2]{ (pointDR[X] + pointUL[X]) / 2, (pointDR[Y] + pointUL[Y]) / 2 };

	textureCoordinates->InsertTuple2(0, center[X], center[Y]);
	textureCoordinates->InsertTuple2(1, pointUL[X], pointUL[Y]);
	textureCoordinates->InsertTuple2(2, pointUR[X], pointUR[Y]);
	textureCoordinates->InsertTuple2(3, pointDR[X], pointDR[Y]);
	textureCoordinates->InsertTuple2(4, pointDL[X], pointDL[Y]);
	textureCoordinates->InsertTuple2(5, pointUL[X], pointUL[Y]);

	// PolyData - Quad
	m_QuadVector[index]->GetPointData()->SetTCoords(textureCoordinates);
	m_QuadVector[m_CurrMeasure]->Update();

	vtkDEL(textureCoordinates);

	albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_MeasureValue));

	//////////////////////////////////////////////////////////////////////////
	Render();
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::RemoveMeasure(int index)
{
	if (m_Renderer && index < GetMeasureCount())
	{
		Superclass::RemoveMeasure(index);

		//////////////////////////////////////////////////////////////////////////

		// POINTS
		// Up Left
		cppDEL(m_PointsStackVectorUL[index]);
		m_PointsStackVectorUL.erase(m_PointsStackVectorUL.begin() + index);

		// Up Right
		cppDEL(m_PointsStackVectorUR[index]);
		m_PointsStackVectorUR.erase(m_PointsStackVectorUR.begin() + index);

		// Down Right
		cppDEL(m_PointsStackVectorDR[index]);
		m_PointsStackVectorDR.erase(m_PointsStackVectorDR.begin() + index);
		
		// Down Left
		cppDEL(m_PointsStackVectorDL[index]);
		m_PointsStackVectorDL.erase(m_PointsStackVectorDL.begin() + index);

		// Center
		cppDEL(m_PointsStackVectorC[index]);
		m_PointsStackVectorC.erase(m_PointsStackVectorC.begin() + index);
		
		// Texture
		m_Renderer->RemoveActor(m_TexturedQuadVector[index]);

		vtkDEL(m_QuadVector[index]);
		m_QuadVector.erase(m_QuadVector.begin() + index);

		vtkDEL(m_TexturedQuadVector[index]);
		m_TexturedQuadVector.erase(m_TexturedQuadVector.begin() + index);
		
		Render();
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::SelectMeasure(int index)
{
	if (GetMeasureCount() > 0)
	{
		// Deselect all
		for (int i = 0; i < GetMeasureCount(); i++)
		{
			int col = m_IsEnabled ? COLOR_DEFAULT : COLOR_DISABLE;

			m_PointsStackVectorUL[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorUR[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorDR[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorDL[i]->SetColor(m_Colors[col]);
			m_PointsStackVectorC[i]->SetColor(m_Colors[col]);
		}

		if (index >= 0)
		{
			m_PointsStackVectorUL[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorUR[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorDR[index]->SetColor(m_Colors[COLOR_SELECTION]);			
			m_PointsStackVectorDL[index]->SetColor(m_Colors[COLOR_SELECTION]);
			m_PointsStackVectorC[index]->SetColor(m_Colors[COLOR_SELECTION]);

			albaEventMacro(albaEvent(this, ID_MEASURE_SELECTED));
		}

		m_LastSelection = index;
		m_LastEditing = -1;
	}
}

// SET/GET
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::SetPointSize(double size)
{
	m_PointSize = size;

	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVectorUL[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorUR[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorDR[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorDL[i]->GetProperty()->SetPointSize(m_PointSize);
		m_PointsStackVectorC[i]->GetProperty()->SetPointSize(m_PointSize);
	}
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::SetTexture(vtkImageData *imageData)
{
	// Apply the texture
	vtkTexture *texture;
	vtkNEW(texture);
	//texture->SetQualityTo32Bit();
	texture->RepeatOff();
	texture->InterpolateOff();

	if (m_TemplateLookupTable)
	{
		texture->MapColorScalarsThroughLookupTableOn();
		texture->SetLookupTable((vtkLookupTable *)m_TemplateLookupTable);
	}

	texture->SetInput(imageData);
	
	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_TexturedQuadVector[i]->SetTexture(texture);
	}

	vtkDEL(texture);
}

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::SetTextureOpacity(double opacity)
{
	m_TexturedQuadVector[0]->GetProperty()->SetOpacity(opacity);

	//////////////////////////////////////////////////////////////////////////
	Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::SetTextureColor(double r,double g, double b)
{
	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_TexturedQuadVector[i]->GetProperty()->SetColor(r,g,b);
	}
	Render();
} 

//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::GetMeasurePoints(int index, double * pointUL, double * pointUR, double * pointDR, double * pointDL)
{
	// Return points values
	if (index >= 0 && index < GetMeasureCount())
	{
		// Up Left
		vtkPointSource* pointSourceUL = (vtkPointSource*)m_PointsStackVectorUL[m_CurrMeasure]->GetSource();
		pointSourceUL->GetCenter(pointUL);

		// Up Right
		vtkPointSource* pointSourceUR = (vtkPointSource*)m_PointsStackVectorUR[m_CurrMeasure]->GetSource();
		pointSourceUR->GetCenter(pointUR);

		// Down Right
		vtkPointSource* pointSourceDR = (vtkPointSource*)m_PointsStackVectorDR[m_CurrMeasure]->GetSource();
		pointSourceDR->GetCenter(pointDR);

		// Down Left
		vtkPointSource* pointSourceDL = (vtkPointSource*)m_PointsStackVectorDL[m_CurrMeasure]->GetSource();
		pointSourceDL->GetCenter(pointDL);
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::GetCenter(int index, double *center)
{
	// Return center points values
	if (index >= 0 && index < GetMeasureCount())
	{
		double pointUL[3], pointDR[3];

		// Up Left
		vtkPointSource* pointSourceUL = (vtkPointSource*)m_PointsStackVectorUL[m_CurrMeasure]->GetSource();
		pointSourceUL->GetCenter(pointUL);

		// Down Right
		vtkPointSource* pointSourceDR = (vtkPointSource*)m_PointsStackVectorDR[m_CurrMeasure]->GetSource();
		pointSourceDR->GetCenter(pointDR);

		center[X] = (pointUL[X] + pointDR[X]) / 2;
		center[Y] = (pointUL[Y] + pointDR[Y]) / 2;
		center[Z] = (pointUL[Z] + pointDR[Z]) / 2;
	}
}

//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::Show(bool show)
{
	//ShowPoints(show);
	ShowTexture(show);
}
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::ShowPoints(bool show)
{
	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_PointsStackVectorUL[i]->SetVisibility(show);
		m_PointsStackVectorUR[i]->SetVisibility(show);
		m_PointsStackVectorDR[i]->SetVisibility(show);
		m_PointsStackVectorDL[i]->SetVisibility(show);
		m_PointsStackVectorC[i]->SetVisibility(show);
	}
	Render();
}
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::ShowTexture(bool show)
{
	for (int i = 0; i < GetMeasureCount(); i++)
	{
		m_TexturedQuadVector[i]->SetVisibility(show);

		m_TextActorVector[i]->SetVisibility(show && m_ShowText);
	}
	Render();
}

/// UTILS ///////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::Rotate(double angle, double *origin)
{
	if (GetMeasureCount() > 0)
	{
		if (origin == NULL)
		{
			double center[3]{ 0.0, 0.0 ,0.0 };
			GetCenter(0, center);

			m_TexturedQuadVector[0]->SetOrigin(center);
		}
		else
		{
			m_TexturedQuadVector[0]->SetOrigin(origin);
		}

		m_TexturedQuadVector[0]->RotateZ(-m_Angle);
		m_TexturedQuadVector[0]->RotateZ(angle);

		m_Angle = angle;

		//////////////////////////////////////////////////////////////////////////
		Render();
	}
}

//---------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::CropImage(albaVMEImage *vmeImage)
{
	if (vmeImage)
	{
		double p1[3], p2[3], p3[3], p4[3];
		GetMeasurePoints(0, p1, p2, p3, p4);
		double croppingBoxBounds[6]{ p1[X],p3[X],p1[Y],p3[Y],p1[Z],p3[Z] };

		CropImage(vmeImage, croppingBoxBounds);
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DMeasure_Template::CropImage(albaVMEImage *vmeImage, double *bounds)
{
	if (vmeImage)
	{
		vtkImageData *inputSP;
		vtkNEW(inputSP);
		inputSP->DeepCopy(vtkImageData::SafeDownCast(vmeImage->GetOutput()->GetVTKData()));
		inputSP->Update();

		int voi_dim[6];
		int original_vol_dim[6];
		double inputBounds[6];

		vmeImage->GetOutput()->GetBounds(inputBounds);
		inputSP->GetExtent(original_vol_dim);

		double spc[3];
		inputSP->GetSpacing(spc);
		double m_XSpacing = spc[X];
		double m_YSpacing = spc[Y];
		double m_ZSpacing = spc[Z];

		double xBoundLength, yBoundLength, zBoundLength;
		xBoundLength = fabs(inputBounds[1] - inputBounds[0]);
		yBoundLength = fabs(inputBounds[3] - inputBounds[2]);
		zBoundLength = fabs(inputBounds[5] - inputBounds[4]);

		double tolerance = 1e-3; //base tolerance

		if (fabs(bounds[0] - inputBounds[0]) / xBoundLength < tolerance)
		{
			voi_dim[0] = original_vol_dim[0];
		}
		else
		{
			voi_dim[0] = ceil((bounds[0] - inputBounds[0]) / (m_XSpacing));
		}

		if (fabs(bounds[1] - inputBounds[1]) / xBoundLength < tolerance)
		{
			voi_dim[1] = original_vol_dim[1];
		}
		else
		{
			voi_dim[1] = floor((bounds[1] - inputBounds[0]) / m_XSpacing);
		}

		if (fabs(bounds[2] - inputBounds[2]) / yBoundLength < tolerance)
		{
			voi_dim[2] = original_vol_dim[2];
		}
		else
		{
			voi_dim[2] = ceil((bounds[2] - inputBounds[2]) / (m_YSpacing));
		}

		if (fabs(bounds[3] - inputBounds[3]) / yBoundLength < tolerance)
		{
			voi_dim[3] = original_vol_dim[3];
		}
		else
		{
			voi_dim[3] = floor((bounds[3] - inputBounds[2]) / m_YSpacing);
		}

		if (fabs(bounds[4] - inputBounds[4]) / zBoundLength < tolerance)
		{
			voi_dim[4] = original_vol_dim[4];
		}
		else
		{
			voi_dim[4] = ceil((bounds[4] - inputBounds[4]) / (m_ZSpacing));
		}

		if (fabs(bounds[5] - inputBounds[5]) / zBoundLength < tolerance)
		{
			voi_dim[5] = original_vol_dim[5];
		}
		else
		{
			voi_dim[5] = floor((bounds[5] - inputBounds[4]) / m_ZSpacing);
		}
		
		double in_org[3];
		inputSP->GetOrigin(in_org);

		// using the vtkALBASmartPointer allows you to don't mind the object Delete
		vtkALBASmartPointer<vtkImageData> v_esp;
		v_esp->SetOrigin(in_org[X] + voi_dim[0] * m_XSpacing,
			in_org[Y] + voi_dim[2] * m_YSpacing,
			in_org[Z] + voi_dim[4] * m_ZSpacing);
		v_esp->SetSpacing(m_XSpacing, m_YSpacing, m_ZSpacing);
		v_esp->SetDimensions(voi_dim[1] - voi_dim[0] + 1,
			voi_dim[3] - voi_dim[2] + 1,
			voi_dim[5] - voi_dim[4] + 1);
		v_esp->Modified();

		vtkALBASmartPointer<vtkProbeFilter> probeFilter;
		probeFilter->SetInput(v_esp);
		probeFilter->SetSource(inputSP);
		probeFilter->Update();

		vtkImageData *outputSP;
		vtkNEW(outputSP);
		outputSP->DeepCopy(probeFilter->GetOutput());
		outputSP->Update();
		
		// Updated LookupTable
		if (m_TemplateLookupTable == NULL)
			vtkNEW(m_TemplateLookupTable);

		lutPreset(4, m_TemplateLookupTable);

		double range[2];

		inputSP->GetScalarRange(range);
		m_TemplateLookupTable->SetTableRange(range);
		m_TemplateLookupTable->Build();

		SetTexture(outputSP);

		vtkDEL(inputSP);
		vtkDEL(outputSP);
	}
}


/// LOAD/SAVE ///////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_Template::Load(albaVME *input, wxString tag)
{
	if (input->GetTagArray()->IsTagPresent(tag + "MeasureTemplatePoint1") 
		&& input->GetTagArray()->IsTagPresent(tag + "MeasureTemplatePoint2")
		&& input->GetTagArray()->IsTagPresent(tag + "MeasureTemplatePoint3")
		&& input->GetTagArray()->IsTagPresent(tag + "MeasureTemplatePoint4"))
	{
		double pointUL[3], pointUR[3], pointDR[3], pointDL[3];
		albaTagItem *measureTypeTag = input->GetTagArray()->GetTag(tag + "MeasureType");
		albaTagItem *measureLabelTag = input->GetTagArray()->GetTag(tag + "MeasureTemplateLabel");
		albaTagItem *measureTemplatePoint1Tag = input->GetTagArray()->GetTag(tag + "MeasureTemplatePoint1");
		albaTagItem *measureTemplatePoint2Tag = input->GetTagArray()->GetTag(tag + "MeasureTemplatePoint2");
		albaTagItem *measureTemplatePoint3Tag = input->GetTagArray()->GetTag(tag + "MeasureTemplatePoint3");
		albaTagItem *measureTemplatePoint4Tag = input->GetTagArray()->GetTag(tag + "MeasureTemplatePoint4");

		int nTemplates = measureTemplatePoint1Tag->GetNumberOfComponents() / 2;

		// Reload points
		for (int i = 0; i < nTemplates; i++)
		{
			pointUL[X] = measureTemplatePoint1Tag->GetValueAsDouble(i * 3 + 0);
			pointUL[Y] = measureTemplatePoint1Tag->GetValueAsDouble(i * 3 + 1);
			pointUL[Z] = measureTemplatePoint1Tag->GetValueAsDouble(i * 3 + 2);

			pointUR[X] = measureTemplatePoint2Tag->GetValueAsDouble(i * 3 + 0);
			pointUR[Y] = measureTemplatePoint2Tag->GetValueAsDouble(i * 3 + 1);
			pointUR[Z] = measureTemplatePoint2Tag->GetValueAsDouble(i * 3 + 2);

			pointDR[X] = measureTemplatePoint3Tag->GetValueAsDouble(i * 3 + 0);
			pointDR[Y] = measureTemplatePoint3Tag->GetValueAsDouble(i * 3 + 1);
			pointDR[Z] = measureTemplatePoint3Tag->GetValueAsDouble(i * 3 + 2);

			pointDL[X] = measureTemplatePoint4Tag->GetValueAsDouble(i * 3 + 0);
			pointDL[Y] = measureTemplatePoint4Tag->GetValueAsDouble(i * 3 + 1);
			pointDL[Z] = measureTemplatePoint4Tag->GetValueAsDouble(i * 3 + 2);

			albaString measureType = measureTypeTag->GetValue(i);
			albaString measureLabel = measureLabelTag->GetValue(i);

			AddMeasure(pointUL, pointUR, pointDR, pointDL);
			SetMeasureLabel(i, measureLabel);
		}

		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
bool albaInteractor2DMeasure_Template::Save(albaVME *input, wxString tag)
{
	bool result = false;
	int nTemplates = GetMeasureCount();

	if (nTemplates > 0)
	{
		albaTagItem measureTypeTag;
		measureTypeTag.SetName(tag + "MeasureType");
		measureTypeTag.SetNumberOfComponents(nTemplates);

		albaTagItem measureLabelTag;
		measureLabelTag.SetName(tag + "MeasureTemplateLabel");
		measureLabelTag.SetNumberOfComponents(nTemplates);

		albaTagItem measureTemplatePoint1Tag;
		measureTemplatePoint1Tag.SetName(tag + "MeasureTemplatePoint1");
		measureTemplatePoint1Tag.SetNumberOfComponents(nTemplates);

		albaTagItem measureTemplatePoint2Tag;
		measureTemplatePoint2Tag.SetName(tag + "MeasureTemplatePoint2");
		measureTemplatePoint2Tag.SetNumberOfComponents(nTemplates);

		albaTagItem measureTemplatePoint3Tag;
		measureTemplatePoint3Tag.SetName(tag + "MeasureTemplatePoint3");
		measureTemplatePoint3Tag.SetNumberOfComponents(nTemplates);

		albaTagItem measureTemplatePoint4Tag;
		measureTemplatePoint4Tag.SetName(tag + "MeasureTemplatePoint4");
		measureTemplatePoint4Tag.SetNumberOfComponents(nTemplates);

		for (int i = 0; i < nTemplates; i++)
		{
			double pointUL[3], pointUR[3], pointDR[3], pointDL[3];
			GetMeasurePoints(i, pointUL, pointUR, pointDR, pointDL);

			measureTypeTag.SetValue(GetTypeName(), i);
			measureLabelTag.SetValue(GetMeasureLabel(i), i);

			measureTemplatePoint1Tag.SetValue(pointUL[X], i * 3 + 0);
			measureTemplatePoint1Tag.SetValue(pointUL[Y], i * 3 + 1);
			measureTemplatePoint1Tag.SetValue(pointUL[Z], i * 3 + 2);

			measureTemplatePoint2Tag.SetValue(pointUR[X], i * 3 + 0);
			measureTemplatePoint2Tag.SetValue(pointUR[Y], i * 3 + 1);
			measureTemplatePoint2Tag.SetValue(pointUR[Z], i * 3 + 2);

			measureTemplatePoint3Tag.SetValue(pointDR[X], i * 3 + 0);
			measureTemplatePoint3Tag.SetValue(pointDR[Y], i * 3 + 1);
			measureTemplatePoint3Tag.SetValue(pointDR[Z], i * 3 + 2);

			measureTemplatePoint4Tag.SetValue(pointDL[X], i * 3 + 0);
			measureTemplatePoint4Tag.SetValue(pointDL[Y], i * 3 + 1);
			measureTemplatePoint4Tag.SetValue(pointDL[Z], i * 3 + 2);
		}

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureType"))
			input->GetTagArray()->DeleteTag(tag + "MeasureType");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureTemplateLabel"))
			input->GetTagArray()->DeleteTag(tag + "MeasureLabel");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureTemplatePoint1"))
			input->GetTagArray()->DeleteTag(tag + "MeasureTemplatePoint1");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureTemplatePoint2"))
			input->GetTagArray()->DeleteTag(tag + "MeasureTemplatePoint2");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureTemplatePoint3"))
			input->GetTagArray()->DeleteTag(tag + "MeasureTemplatePoint3");

		if (input->GetTagArray()->IsTagPresent(tag + "MeasureTemplatePoint4"))
			input->GetTagArray()->DeleteTag(tag + "MeasureTemplatePoint4");

		input->GetTagArray()->SetTag(measureTypeTag);
		input->GetTagArray()->SetTag(measureLabelTag);
		input->GetTagArray()->SetTag(measureTemplatePoint1Tag);
		input->GetTagArray()->SetTag(measureTemplatePoint2Tag);
		input->GetTagArray()->SetTag(measureTemplatePoint3Tag);
		input->GetTagArray()->SetTag(measureTemplatePoint4Tag);

		result = true;
	}

	return result;
}

//---------------------------------------------------------------------------
wxString albaInteractor2DMeasure_Template::GetConfigDirectory()
{
	//getting the Config directory
	wxString config_dir = albaGetApplicationDirectory().c_str();

#ifdef _DEBUG
	config_dir = ALBA_SOURCE_DIR;
	config_dir += "\\Installer";
#endif
	config_dir += "\\Config";

	for (unsigned int i = 0; i < config_dir.Length(); i++)
	{
		if (config_dir[i] == '/')
			config_dir[i] = '\\';
	}

	return config_dir;
}