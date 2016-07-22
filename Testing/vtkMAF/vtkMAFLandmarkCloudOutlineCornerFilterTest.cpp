/*=========================================================================

 Program: MAF2
 Module: vtkMAFLandmarkCloudOutlineCornerFilterTest
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkMAFCellsFilter.h"
#include "vtkMAFLandmarkCloudOutlineCornerFilter.h"
#include "vtkMAFLandmarkCloudOutlineCornerFilterTest.h"

#include <assert.h>
#include "vtkPointSetSource.h"
#include "vtkCellArray.h"

//----------------------------------------------------------------------------
void vtkMAFLandmarkCloudOutlineCornerFilterTest::TestInput()
{  
	vtkMAFSmartPointer<vtkMAFLandmarkCloudOutlineCornerFilter> filter;

	int numberOfInputs = filter->GetNumberOfInputs();
	CPPUNIT_ASSERT(numberOfInputs == 0);

	vtkMAFSmartPointer<vtkSphereSource> sphere;
	sphere->Update();

	filter->SetInput(sphere->GetOutput());
	filter->Update();

	numberOfInputs = filter->GetNumberOfInputs();
	CPPUNIT_ASSERT(numberOfInputs == 1);
}

//----------------------------------------------------------------------------
void vtkMAFLandmarkCloudOutlineCornerFilterTest::TestGetSet()
{
	vtkMAFSmartPointer<vtkMAFLandmarkCloudOutlineCornerFilter> filter;

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
void vtkMAFLandmarkCloudOutlineCornerFilterTest::TestBounds()
{  
	bool result = true;

	vtkMAFSmartPointer<vtkMAFLandmarkCloudOutlineCornerFilter> filter;

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

	vtkMAFSmartPointer<vtkPolyData> pointPD;
	vtkMAFSmartPointer<vtkCellArray> vertices;
	vtkMAFSmartPointer<vtkPoints> points;
	
	points->SetNumberOfPoints(10);
	double coord[3];

	for (int i = 0; i<10; i++)
	{
		coord[0] = i * 1.5;
		coord[1] = i *-2.5;
		coord[2] = i * 3.5;
		points->SetPoint(i, coord);
		vertices->InsertNextCell(1, &i);
	}

	pointPD->SetPoints(points);
	pointPD->SetVerts(vertices);
	pointPD->Update();

	double inputBounds[6];
	pointPD->GetBounds(inputBounds);

	filter->SetInput(pointPD);
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
