/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBALandmarkCloudOutlineCornerFilterTest
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>
#include <cppunit/TestAssert.h>

#include "vtkALBASmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkALBACellsFilter.h"
#include "vtkALBALandmarkCloudOutlineCornerFilter.h"
#include "vtkALBALandmarkCloudOutlineCornerFilterTest.h"

#include <assert.h>
#include "vtkCellArray.h"

//----------------------------------------------------------------------------
void vtkALBALandmarkCloudOutlineCornerFilterTest::TestInput()
{  
	vtkALBASmartPointer<vtkALBALandmarkCloudOutlineCornerFilter> filter;

	int numberOfInputs = filter->GetTotalNumberOfInputConnections();
	CPPUNIT_ASSERT(numberOfInputs == 0);

	vtkALBASmartPointer<vtkSphereSource> sphere;
	sphere->Update();

	filter->SetInputConnection(sphere->GetOutputPort());
	filter->Update();

	numberOfInputs = filter->GetTotalNumberOfInputConnections();
	CPPUNIT_ASSERT(numberOfInputs == 1);
}

//----------------------------------------------------------------------------
void vtkALBALandmarkCloudOutlineCornerFilterTest::TestGetSet()
{
	vtkALBASmartPointer<vtkALBALandmarkCloudOutlineCornerFilter> filter;

	// Default Values
	CPPUNIT_ASSERT(filter->GetCornerFactor() == 0.2);
	CPPUNIT_ASSERT(filter->GetCloudRadius() == 1.0);

	// Set CloudRadius
	double newCloudRadius = 1.5;
	filter->SetCloudRadius(newCloudRadius);
	CPPUNIT_ASSERT(filter->GetCloudRadius() == newCloudRadius);

	// Set CornerFactor
	filter->SetCornerFactor(0.001); // = min value
	CPPUNIT_ASSERT(filter->GetCornerFactor() == 0.001);

	filter->SetCornerFactor(0.000001); // < min value
	CPPUNIT_ASSERT(filter->GetCornerFactor() == 0.001);

	filter->SetCornerFactor(0.3); // > min value & < max value
	CPPUNIT_ASSERT(filter->GetCornerFactor() == 0.3);

	filter->SetCornerFactor(0.5); // = max value
	CPPUNIT_ASSERT(filter->GetCornerFactor() == 0.5);

	filter->SetCornerFactor(0.6); // > max value
	CPPUNIT_ASSERT(filter->GetCornerFactor() == 0.5);
}

//----------------------------------------------------------------------------
void vtkALBALandmarkCloudOutlineCornerFilterTest::TestBounds()
{  
	bool result = true;

	vtkALBASmartPointer<vtkALBALandmarkCloudOutlineCornerFilter> filter;

	double outputBounds[6];

	filter->GetOutput()->GetBounds(outputBounds);

	for (int i = 0; i < 6; i++)
	{
		if (abs(outputBounds[i]) != 1.0)
		{
			result = false;
			break;
		}
	}

	CPPUNIT_ASSERT(result);

	//

	vtkALBASmartPointer<vtkPolyData> pointPD;
	vtkALBASmartPointer<vtkCellArray> vertices;
	vtkALBASmartPointer<vtkPoints> points;
	
	points->SetNumberOfPoints(10);
	double coord[3];

	for (vtkIdType i = 0; i<10; i++)
	{
		coord[0] = i * 1.5;
		coord[1] = i *-2.5;
		coord[2] = i * 3.5;
		points->SetPoint(i, coord);
		vertices->InsertNextCell(1, &i);
	}

	pointPD->SetPoints(points);
	pointPD->SetVerts(vertices);

	double inputBounds[6];
	pointPD->GetBounds(inputBounds);

	filter->SetInputData(pointPD);
	filter->Update();
	filter->GetOutput()->GetBounds(outputBounds);
	
	result = true;
	double cloudRadius = filter->GetCloudRadius();

	for (int i = 0; i < 6; i += 2)
		inputBounds[i] -= cloudRadius;
	for (int i = 1; i < 6; i += 2)
		inputBounds[i] += cloudRadius;
	
	for (int i=0; i<6; i++)
	{
		if (inputBounds[i] != outputBounds[i])
		{
			result = false;
			break;
		}
	}

	CPPUNIT_ASSERT(result);
}
