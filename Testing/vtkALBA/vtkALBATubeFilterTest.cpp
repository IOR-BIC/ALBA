/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATubeFilterTest
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "albaDefines.h"
#include "vtkALBALineStripSource.h"

#include "vtkALBATubeFilter.h"
#include "vtkALBATubeFilterTest.h"

#include "vtkTubeFilter.h"

#include "vtkALBASmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkALBACellsFilter.h"

#include <assert.h>


//----------------------------------------------------------------------------
void vtkALBATubeFilterTest::TestInput()
{  
	vtkALBASmartPointer<vtkALBATubeFilter> filter;

	CPPUNIT_ASSERT(filter->GetTotalNumberOfInputConnections() == 0);

	vtkALBASmartPointer<vtkSphereSource> sphere;
	sphere->Update();

	filter->SetInputConnection(sphere->GetOutputPort());
	filter->Update();

	CPPUNIT_ASSERT(filter->GetTotalNumberOfInputConnections() == 1);
}

//----------------------------------------------------------------------------
void vtkALBATubeFilterTest::TestCapping()
{
	vtkALBASmartPointer<vtkALBATubeFilter> filter;

	CPPUNIT_ASSERT(filter->GetCapping() == 0);
	
	filter->CappingOn();
	CPPUNIT_ASSERT(filter->GetCapping() == 1);

	filter->CappingOff();
	CPPUNIT_ASSERT(filter->GetCapping() == 0);

	filter->SetCapping(2);
	CPPUNIT_ASSERT(filter->GetCapping() == 2);
}

//----------------------------------------------------------------------------
void vtkALBATubeFilterTest::TestNormals()
{  
	vtkALBASmartPointer<vtkALBATubeFilter> filter;

	double normals[3];
	filter->GetDefaultNormal(normals);
	CPPUNIT_ASSERT(normals[0] == 0.0 && normals[1] == 0.0 && normals[2] == 1.0);

	filter->UseDefaultNormalOff();
	filter->UseDefaultNormalOn();

	double newNormals[3];
	newNormals[0] = 1.0;
	newNormals[1] = 2.0;
	newNormals[2] = 3.0;
	filter->SetDefaultNormal(newNormals);
	filter->GetDefaultNormal(normals);

	CPPUNIT_ASSERT(normals[0] == newNormals[0] && normals[1] == newNormals[1] && normals[2] == newNormals[2]);
}

//----------------------------------------------------------------------------
void vtkALBATubeFilterTest::TestTCoords()
{
	vtkALBASmartPointer<vtkALBATubeFilter> filter;

	int maxTcoord = filter->GetGenerateTCoordsMaxValue();
	int minTcoord = filter->GetGenerateTCoordsMinValue();

	//
	filter->SetGenerateTCoords(2);
	CPPUNIT_ASSERT(filter->GetGenerateTCoords() == 2);

	filter->SetGenerateTCoords(minTcoord - 1);
	CPPUNIT_ASSERT(filter->GetGenerateTCoords() == minTcoord);

	filter->SetGenerateTCoords(maxTcoord+1);
	CPPUNIT_ASSERT(filter->GetGenerateTCoords() == maxTcoord);
	
	//
	filter->SetGenerateTCoordsToNormalizedLength();
	CPPUNIT_ASSERT(!strcmp(filter->GetGenerateTCoordsAsString(), "GenerateTCoordsFromNormalizedLength"));

	filter->SetGenerateTCoordsToUseLength();
	CPPUNIT_ASSERT(!strcmp(filter->GetGenerateTCoordsAsString(), "GenerateTCoordsFromLength"));

	filter->SetGenerateTCoordsToUseScalars();
	CPPUNIT_ASSERT(!strcmp(filter->GetGenerateTCoordsAsString(), "GenerateTCoordsFromScalar"));
}

//----------------------------------------------------------------------------
void vtkALBATubeFilterTest::TestRadius()
{
	vtkALBASmartPointer<vtkALBATubeFilter> filter;

	double maxRadius = filter->GetRadiusMaxValue(); // Dafault 1.0
	double minRadius = filter->GetRadiusMinValue(); // Dafault 0.0

	CPPUNIT_ASSERT(filter->GetRadius() == 0.5); // Dafault 0.5

	filter->SetRadius(1.0);
	CPPUNIT_ASSERT(filter->GetRadius() == 1.0);

	filter->SetRadius(maxRadius + 1.0);
	CPPUNIT_ASSERT(filter->GetRadius() == maxRadius);

	filter->SetRadius(minRadius - 1.0);
	CPPUNIT_ASSERT(filter->GetRadius() == minRadius);

	//
	CPPUNIT_ASSERT(filter->GetRadiusFactor() == 10.0); // Default 10.0
	
	filter->SetRadiusFactor(5.0);
	CPPUNIT_ASSERT(filter->GetRadiusFactor() == 5.0);

	//
	CPPUNIT_ASSERT(filter->GetVaryRadius() == 0); // Default 0

	int maxVaryRadius = filter->GetVaryRadiusMaxValue(); // Default 3
	int minVaryRadius = filter->GetVaryRadiusMinValue(); // Default 0

	filter->SetVaryRadius(1);
	CPPUNIT_ASSERT(filter->GetVaryRadius() == 1);

	filter->SetVaryRadius(maxVaryRadius + 1);
	CPPUNIT_ASSERT(filter->GetVaryRadius() == maxVaryRadius);

	filter->SetVaryRadius(minVaryRadius - 1);
	CPPUNIT_ASSERT(filter->GetVaryRadius() == minVaryRadius);

	//
	filter->SetVaryRadiusToVaryRadiusByVector();
	CPPUNIT_ASSERT(!strcmp(filter->GetVaryRadiusAsString(),"VaryRadiusByVector"));

	filter->SetVaryRadiusToVaryRadiusByScalar();
	CPPUNIT_ASSERT(!strcmp(filter->GetVaryRadiusAsString(), "VaryRadiusByScalar"));

	filter->SetVaryRadiusToVaryRadiusByAbsoluteScalar();
	CPPUNIT_ASSERT(!strcmp(filter->GetVaryRadiusAsString(), "VaryRadiusByAbsoluteScalar"));

	filter->SetVaryRadiusToVaryRadiusOff();
	CPPUNIT_ASSERT(!strcmp(filter->GetVaryRadiusAsString(), "VaryRadiusOff"));
}

//----------------------------------------------------------------------------
void vtkALBATubeFilterTest::TestSides()
{
	vtkALBASmartPointer<vtkALBATubeFilter> filter;
	vtkALBASmartPointer<vtkALBALineStripSource> lineStrip;
	
	CPPUNIT_ASSERT(filter->GetNumberOfSides() == 3); // Default 3

	int nPoints = 5;

	double point[3];
	point[1] = 0.0;
	point[2] = 0.0;

	for (int i = 0; i < nPoints; i++)
	{
		point[0] = i * 0.2;
		lineStrip->AddPoint(point);
	}

	lineStrip->Update();

	filter->SetInputConnection(lineStrip->GetOutputPort());
	filter->Update();

	CPPUNIT_ASSERT(filter->GetOutput()->GetNumberOfPoints() == nPoints * filter->GetNumberOfSides());
	
	filter->SetNumberOfSides(5);
	filter->Update();

	CPPUNIT_ASSERT(filter->GetOutput()->GetNumberOfPoints() == nPoints * filter->GetNumberOfSides());
}

//----------------------------------------------------------------------------
void vtkALBATubeFilterTest::TestOldVersion()
{
	vtkALBASmartPointer<vtkALBATubeFilter> newFilter;
	vtkALBASmartPointer<vtkTubeFilter> oldFilter;

	vtkALBASmartPointer<vtkALBALineStripSource> newLineStrip;
	vtkALBASmartPointer<vtkALBALineStripSource> oldLineStrip;

	double point[3];
	point[1] = 0.0;
	point[2] = 0.0;

	for (int i=0; i<5; i++)
	{
		point[0] = i * 0.2;
		newLineStrip->AddPoint(point);
	}

	newLineStrip->Update();

	newFilter->SetInputConnection(newLineStrip->GetOutputPort());
	newFilter->Update();
	oldFilter->SetInputConnection(newLineStrip->GetOutputPort());
	oldFilter->Update();
	
	int n1 = newFilter->GetOutput()->GetNumberOfPoints();
	int n2 = oldFilter->GetOutput()->GetNumberOfPoints();

	CPPUNIT_ASSERT(n1 == n2);

	//
	newLineStrip->ClearPoints();

	for (int i = 0; i < 5; i++)
	{
		point[0] = i * 0.2;

		oldLineStrip->AddPoint(point);

		newLineStrip->AddPoint(point);
		if (i == 2)
			newLineStrip->AddPoint(point); // Add the same point
	}

	newLineStrip->Update();
	oldLineStrip->Update();

	newFilter->SetInputConnection(newLineStrip->GetOutputPort());
	newFilter->Update();
	oldFilter->SetInputConnection(oldLineStrip->GetOutputPort());
	oldFilter->Update();

	// newTubeFilter ignore repeated points
	n1 = newFilter->GetOutput()->GetNumberOfPoints();
	n2 = oldFilter->GetOutput()->GetNumberOfPoints();

	CPPUNIT_ASSERT(n1 == n2);
}
