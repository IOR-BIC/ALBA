/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkMaskPolyDataFilterTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "vtkMaskPolyDataFilterTest.h"
#include "vtkMaskPolyDataFilter.h"

#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkCylinderSource.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"

//-----------------------------------------------------------
void vtkMaskPolyDataFilterTest::TestStructuredPoints() 
//-----------------------------------------------------------
{
  vtkImageData *volume;
	vtkNEW(volume);

	volume->SetDimensions(3,3,3);
	volume->SetSpacing(2,2,2);
	volume->SetOrigin(0.0,0.0,0.0);
	volume->AllocateScalars();

	for(int count = 0; count < volume->GetNumberOfPoints(); count++)
	{
    volume->GetPointData()->GetScalars()->SetTuple1(count, 10);
	}
	volume->Update();

	
	vtkCylinderSource   *cylinder;
	vtkNEW(cylinder);

  cylinder->SetCenter(1.5,1.5,2.0);
	cylinder->SetRadius(1.5);
	cylinder->SetResolution(16);
	cylinder->SetHeight(4.0);

	cylinder->Update();
		

	//test vtkMaskPolyDataFilter
	vtkMaskPolyDataFilter *maskPolydataFilter;
	vtkNEW(maskPolydataFilter);

	maskPolydataFilter->SetInput(volume);
	maskPolydataFilter->SetDistance(0.0);
	maskPolydataFilter->SetInsideValue(-5);
	maskPolydataFilter->SetInsideOut(0);
 
	maskPolydataFilter->SetMask(cylinder->GetOutput());
	maskPolydataFilter->Update();

	CPPUNIT_ASSERT(((vtkImageData*)maskPolydataFilter->GetOutput())->GetPointData()->GetScalars()->GetTuple1(13) == 10.0);

	vtkDEL(maskPolydataFilter);
	vtkDEL(volume);
	vtkDEL(cylinder);
	

}
//-----------------------------------------------------------
void vtkMaskPolyDataFilterTest::TestRectilinearGrid() 
//-----------------------------------------------------------
{
	vtkRectilinearGrid *volume = vtkRectilinearGrid::New() ;

	// create rectilinear grid data
	vtkFloatArray *xCoordinates = vtkFloatArray::New();
	xCoordinates->SetNumberOfValues(3);
	xCoordinates->SetValue(0, 0.f);
	xCoordinates->SetValue(1, 2.f);
	xCoordinates->SetValue(2, 4.f);


	vtkFloatArray *yCoordinates = vtkFloatArray::New();
	yCoordinates->SetNumberOfValues(3);
	yCoordinates->SetValue(0, 0.f);
	yCoordinates->SetValue(1, 2.f);
	yCoordinates->SetValue(2, 4.f); 

	vtkFloatArray *zCoordinates = vtkFloatArray::New();
	zCoordinates->SetNumberOfValues(3);
	zCoordinates->SetValue(0, 0.f);
	zCoordinates->SetValue(1, 2.f);
	zCoordinates->SetValue(2, 4.f);

	volume->SetDimensions(3,3,3);
	volume->SetXCoordinates(xCoordinates);
	volume->SetYCoordinates(yCoordinates);
	volume->SetZCoordinates(zCoordinates);

	vtkFloatArray *scalars = vtkFloatArray::New();
	scalars->SetName("scalars");

	for(int count = 0; count < volume->GetNumberOfPoints(); count++)
	{
		scalars->InsertNextTuple1(10);
	}
	volume->GetPointData()->AddArray(scalars);
	volume->GetPointData()->SetScalars(scalars);
	volume->Update();


	vtkCylinderSource   *cylinder;
	vtkNEW(cylinder);

	cylinder->SetCenter(1.5,1.5,2.0);
	cylinder->SetRadius(1.5);
	cylinder->SetResolution(16);
	cylinder->SetHeight(4.0);

	cylinder->Update();


	//test vtkMaskPolyDataFilter
	vtkMaskPolyDataFilter *maskPolydataFilter;
	vtkNEW(maskPolydataFilter);

	maskPolydataFilter->SetInput(volume);
	maskPolydataFilter->SetDistance(0.0);
	maskPolydataFilter->SetInsideValue(-5);
	maskPolydataFilter->SetInsideOut(0);

	maskPolydataFilter->SetMask(cylinder->GetOutput());
	maskPolydataFilter->Update();


	CPPUNIT_ASSERT(((vtkRectilinearGrid*)maskPolydataFilter->GetOutput())->GetPointData()->GetScalars()->GetTuple1(13) == 10.0);

	vtkDEL(maskPolydataFilter);
	vtkDEL(volume);
	vtkDEL(cylinder);


}