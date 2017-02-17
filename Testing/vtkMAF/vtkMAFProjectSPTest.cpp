/*=========================================================================

 Program: MAF2
 Module: vtkMAFProjectSPTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFProjectSPTest.h"
#include "vtkMAFProjectSP.h"
#include "vtkMAFSmartPointer.h"

#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"

int glo_Dimension[3] = { 2,2,2 };

//--------------------------------------------------
void vtkMAFProjectSPTest::TestDynamicAllocation()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFProjectSP> filter;
  vtkMAFProjectSP *filter2 = vtkMAFProjectSP::New();
  filter2->Delete();
}
//--------------------------------------------------
void vtkMAFProjectSPTest::TestExecutionProjectionModeToX()
//--------------------------------------------------
{
	//create imageData with scalars
	vtkImageData *image = CreateNewSPWithScalars();
  
	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int y = 0; y < glo_Dimension[1]; y++)
		{
			double acc = 0;
			for (int x = 0; x < glo_Dimension[0]; x++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)glo_Dimension[0]);
		}
  
  //use filter
  vtkMAFSmartPointer<vtkMAFProjectSP> filter;
  filter->SetInput(image);
  filter->SetProjectionModeToX();
  filter->Update();

  //check Control
  vtkStructuredPoints *projectedImage = filter->GetOutput();
  for(int j=0;j<glo_Dimension[1]*glo_Dimension[2];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }

  CPPUNIT_ASSERT(strcmp(filter->GetProjectionModeAsString(),"X")==0);
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_X );

	vtkDEL(image);
}
//--------------------------------------------------
void vtkMAFProjectSPTest::TestExecutionProjectionModeToY()
//--------------------------------------------------
{
	//create imageData with scalars
	vtkImageData *image = CreateNewSPWithScalars();

	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int y = 0; y < glo_Dimension[1]; y++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)glo_Dimension[1]);
		}

  //use filter
  vtkMAFSmartPointer<vtkMAFProjectSP> filter;
  filter->SetInput(image);
  filter->SetProjectionModeToY();
  filter->Update();

  //check Control
  vtkStructuredPoints *projectedImage = filter->GetOutput();
  for(int j=0;j<glo_Dimension[0]*glo_Dimension[2];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }
  CPPUNIT_ASSERT(strcmp(filter->GetProjectionModeAsString(),"Y")==0);
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_Y );

	vtkDEL(image);
}
//--------------------------------------------------
void vtkMAFProjectSPTest::TestExecutionProjectionModeToZ()
//--------------------------------------------------
{
	//create imageData with scalars
	vtkImageData *image = CreateNewSPWithScalars();

	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int y = 0; y < glo_Dimension[1]; y++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int z = 0; z < glo_Dimension[2]; z++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)glo_Dimension[2]);
		}  

  //use filter
  vtkMAFSmartPointer<vtkMAFProjectSP> filter;
  filter->SetInput(image);
  filter->SetProjectionModeToZ();
  filter->Update();

  //check Control
  vtkStructuredPoints *projectedImage = filter->GetOutput();
  for(int j=0;j<glo_Dimension[0]*glo_Dimension[1];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }
  CPPUNIT_ASSERT(strcmp(filter->GetProjectionModeAsString(),"Z")==0);
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_Z );

	vtkDEL(image);
}

//----------------------------------------------------------------------------
vtkImageData * vtkMAFProjectSPTest::CreateNewSPWithScalars()
{
	//create imageData with scalars
	vtkImageData *image;
	vtkNEW(image);
	double spacing[3] = { 1. ,1. ,1. };
	image->SetDimensions(glo_Dimension);
	image->SetSpacing(spacing);
	image->SetScalarTypeToFloat();

	int i = 0;
	int size = glo_Dimension[0] * glo_Dimension[1] * glo_Dimension[2];
	vtkMAFSmartPointer<vtkFloatArray> array;
	array->Allocate(size);

	for (; i < size; i++)
	{
		array->SetTuple1(i, i);
	}
	array->Modified();
	image->GetPointData()->SetScalars(array);
	image->Update();

	return image;
}
