/*=========================================================================
Program:  ALBA (Agile Library for Biomedical Applications)
Module:    albaActor2dStackHelper.cpp
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

#include "albaActor2dStackHelper.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"
#include "vtkPolyDataSource.h"
#include "vtkProperty2D.h"


//----------------------------------------------------------------------------
albaActor2dStackHelper::albaActor2dStackHelper(vtkPolyDataSource *source, vtkRenderer *renderer)
{
	m_Source = source;
	m_Renderer = renderer;
	m_Mapper = vtkPolyDataMapper2D::New();
	m_Actor = vtkActor2D::New();

	//Mapper
	vtkCoordinate *coordinate = vtkCoordinate::New();
	coordinate->SetCoordinateSystemToWorld();
	m_Mapper->SetInput(source->GetOutput());
	m_Mapper->SetTransformCoordinate(coordinate);

	//Actor
	m_Actor->SetMapper(m_Mapper);

	//Renderer
	m_Renderer->AddActor2D(m_Actor);

	vtkDEL(coordinate);
}


//----------------------------------------------------------------------------
albaActor2dStackHelper::~albaActor2dStackHelper()
{
	m_Renderer->RemoveActor2D(m_Actor);
	vtkDEL(m_Actor);
	vtkDEL(m_Mapper);
	vtkDEL(m_Source);
}

//----------------------------------------------------------------------------
vtkProperty2D* albaActor2dStackHelper::GetProperty()
{
	return m_Actor->GetProperty();
}

//----------------------------------------------------------------------------
void albaActor2dStackHelper::SetColor(Color col)
{
	vtkProperty2D* property = m_Actor->GetProperty();
	property->SetColor(col.R, col.G, col.B);
	property->SetOpacity(col.Alpha);
}

//----------------------------------------------------------------------------
void albaActor2dStackHelper::SetVisibility(bool show)
{
	m_Actor->SetVisibility(show);
}