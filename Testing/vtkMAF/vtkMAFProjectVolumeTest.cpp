/*=========================================================================

 Program: MAF2
 Module: vtkMAFProjectVolumeTest
 Authors: Daniele Giunchi, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFProjectVolumeTest.h"
#include "vtkMAFProjectVolume.h"
#include "vtkMAFSmartPointer.h"

#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"

int glo_Dimension[3] = { 2,3,4 };

//--------------------------------------------------
void vtkMAFProjectVolumeTest::TestDynamicAllocation()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFProjectVolume> filter;
  vtkMAFProjectVolume *filter2 = vtkMAFProjectVolume::New();
  filter2->Delete();
}
//--------------------------------------------------
void vtkMAFProjectVolumeTest::TestExecutionProjectionModeToX()
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
  vtkMAFSmartPointer<vtkMAFProjectVolume> filter;
  filter->SetInput(image);
  filter->SetProjectionModeToX();
  filter->Update();

  //check Control
  vtkStructuredPoints *projectedImage = vtkStructuredPoints::SafeDownCast(filter->GetOutput());
	projectedImage->Update();
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
void vtkMAFProjectVolumeTest::TestExecutionProjectionModeToY()
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
  vtkMAFSmartPointer<vtkMAFProjectVolume> filter;
  filter->SetInput(image);
  filter->SetProjectionModeToY();
  filter->Update();

  //check Control
  vtkStructuredPoints *projectedImage = (vtkStructuredPoints *)filter->GetOutput();
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
void vtkMAFProjectVolumeTest::TestExecutionProjectionModeToZ()
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
  vtkMAFSmartPointer<vtkMAFProjectVolume> filter;
  filter->SetInput(image);
  filter->SetProjectionModeToZ();
  filter->Update();

  //check Control
  vtkStructuredPoints *projectedImage = (vtkStructuredPoints *)filter->GetOutput();
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

//--------------------------------------------------
void vtkMAFProjectVolumeTest::TestRangeProjectionX()
{
	//create imageData with scalars
	vtkImageData *image = CreateNewSPWithScalars();
	int range[2] = { 0,1 };

	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int y = 0; y < glo_Dimension[1]; y++)
		{
			double acc = 0;
			for (int x = range[0]; x < range[1]; x++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)(range[1]-range[0]));
		}

	//use filter
	vtkMAFSmartPointer<vtkMAFProjectVolume> filter;
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == false);
	filter->SetInput(image);
	filter->SetProjectionModeToX();
	filter->ProjectSubRangeOn();
	filter->SetProjectionRange(range);
	filter->Update();

	//check Control
	vtkStructuredPoints *projectedImage = (vtkStructuredPoints *)filter->GetOutput();
	for (int j = 0; j < glo_Dimension[1] * glo_Dimension[2]; j++)
	{
		float value1 = arrayControl->GetTuple1(j);
		float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
		CPPUNIT_ASSERT(value1 == value2);
	}

	CPPUNIT_ASSERT(filter->GetProjectSubRange() == true);

	vtkDEL(image);
}
//--------------------------------------------------
void vtkMAFProjectVolumeTest::TestRangeProjectionY()
{
	vtkImageData *image = CreateNewSPWithScalars();
	int range[2] = { 1,3 };

	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int y = range[0]; y < range[1]; y++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)(range[1] - range[0]));
		}

	//use filter
	vtkMAFSmartPointer<vtkMAFProjectVolume> filter;
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == false);
	filter->SetInput(image);
	filter->SetProjectionModeToY();
	filter->ProjectSubRangeOn();
	filter->SetProjectionRange(range);
	filter->Update();

	//check Control
	vtkStructuredPoints *projectedImage = (vtkStructuredPoints *)filter->GetOutput();
	for (int j = 0; j < glo_Dimension[0] * glo_Dimension[2]; j++)
	{
		float value1 = arrayControl->GetTuple1(j);
		float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
		CPPUNIT_ASSERT(value1 == value2);
	}
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == true);

	vtkDEL(image);
}
//--------------------------------------------------
void vtkMAFProjectVolumeTest::TestRangeProjectionZ()
{
	vtkImageData *image = CreateNewSPWithScalars();
	int range[2] = { 2,3 };

	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int y = 0; y < glo_Dimension[1]; y++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int z = range[0]; z < range[1]; z++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)(range[1] - range[0]));
		}

	//use filter
	vtkMAFSmartPointer<vtkMAFProjectVolume> filter;
	filter->SetInput(image);
	filter->SetProjectionModeToZ();
	filter->ProjectSubRangeOn();
	filter->SetProjectionRange(range);
	filter->Update();

	//check Control
	vtkStructuredPoints *projectedImage = (vtkStructuredPoints *)filter->GetOutput();
	for (int j = 0; j < glo_Dimension[0] * glo_Dimension[1]; j++)
	{
		float value1 = arrayControl->GetTuple1(j);
		float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
		CPPUNIT_ASSERT(value1 == value2);
	}
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == true);

	vtkDEL(image);
}


//----------------------------------------------------------------------------
vtkImageData * vtkMAFProjectVolumeTest::CreateNewSPWithScalars()
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
