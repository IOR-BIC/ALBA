/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAProjectVolumeTest
 Authors: Daniele Giunchi, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkALBAProjectVolumeTest.h"
#include "vtkALBAProjectVolume.h"
#include "vtkALBASmartPointer.h"

#include "vtkImageData.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"

int glo_Dimension[3] = { 2,3,4 };

//--------------------------------------------------
void vtkALBAProjectVolumeTest::TestDynamicAllocation()
//--------------------------------------------------
{
  vtkALBASmartPointer<vtkALBAProjectVolume> filter;
  vtkALBAProjectVolume *filter2 = vtkALBAProjectVolume::New();
  filter2->Delete();
}
//--------------------------------------------------
void vtkALBAProjectVolumeTest::TestExecutionProjectionModeToX()
{
	//create imageData with scalars
	vtkImageData *image = CreateNewSPWithScalars();
  
	//Preparing Test Control Data
	vtkALBASmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int y = 0; y < glo_Dimension[1]; y++)
		{
			double acc = 0;
			for (int x = 0; x < glo_Dimension[0]; x++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)glo_Dimension[0]);
		}
  
  //use filter
  vtkALBASmartPointer<vtkALBAProjectVolume> filter;
  filter->SetInputData(image);
  filter->SetProjectionModeToX();
  filter->Update();

  //check Control
  vtkImageData *projectedImage = vtkImageData::SafeDownCast(filter->GetOutput());
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
void vtkALBAProjectVolumeTest::TestExecutionProjectionModeToY()
{
	//create imageData with scalars
	vtkImageData *image = CreateNewSPWithScalars();

	//Preparing Test Control Data
	vtkALBASmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int y = 0; y < glo_Dimension[1]; y++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)glo_Dimension[1]);
		}

  //use filter
  vtkALBASmartPointer<vtkALBAProjectVolume> filter;
  filter->SetInputData(image);
  filter->SetProjectionModeToY();
  filter->Update();

  //check Control
  vtkImageData *projectedImage = (vtkImageData *)filter->GetOutput();
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
void vtkALBAProjectVolumeTest::TestExecutionProjectionModeToZ()
{
	//create imageData with scalars
	vtkImageData *image = CreateNewSPWithScalars();

	//Preparing Test Control Data
	vtkALBASmartPointer<vtkFloatArray> arrayControl;
	for (int y = 0; y < glo_Dimension[1]; y++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int z = 0; z < glo_Dimension[2]; z++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)glo_Dimension[2]);
		}

  //use filter
  vtkALBASmartPointer<vtkALBAProjectVolume> filter;
  filter->SetInputData(image);
  filter->SetProjectionModeToZ();
  filter->Update();

  //check Control
  vtkImageData *projectedImage = (vtkImageData *)filter->GetOutput();
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
void vtkALBAProjectVolumeTest::TestRangeProjectionX()
{
	//create imageData with scalars
	vtkImageData *image = CreateNewSPWithScalars();
	int range[2] = { 0,1 };

	//Preparing Test Control Data
	vtkALBASmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int y = 0; y < glo_Dimension[1]; y++)
		{
			double acc = 0;
			for (int x = range[0]; x < range[1]; x++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)(range[1]-range[0]));
		}

	//use filter
	vtkALBASmartPointer<vtkALBAProjectVolume> filter;
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == false);
	filter->SetInputData(image);
	filter->SetProjectionModeToX();
	filter->ProjectSubRangeOn();
	filter->SetProjectionRange(range);
	filter->Update();

	//check Control
	vtkImageData *projectedImage = (vtkImageData *)filter->GetOutput();
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
void vtkALBAProjectVolumeTest::TestRangeProjectionY()
{
	vtkImageData *image = CreateNewSPWithScalars();
	int range[2] = { 1,3 };

	//Preparing Test Control Data
	vtkALBASmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int y = range[0]; y < range[1]; y++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)(range[1] - range[0]));
		}

	//use filter
	vtkALBASmartPointer<vtkALBAProjectVolume> filter;
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == false);
	filter->SetInputData(image);
	filter->SetProjectionModeToY();
	filter->ProjectSubRangeOn();
	filter->SetProjectionRange(range);
	filter->Update();

	//check Control
	vtkImageData *projectedImage = (vtkImageData *)filter->GetOutput();
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
void vtkALBAProjectVolumeTest::TestRangeProjectionZ()
{
	vtkImageData *image = CreateNewSPWithScalars();
	int range[2] = { 2,3 };

	//Preparing Test Control Data
	vtkALBASmartPointer<vtkFloatArray> arrayControl;
	for (int y = 0; y < glo_Dimension[1]; y++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int z = range[0]; z < range[1]; z++)
				acc += image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)(range[1] - range[0]));
		}

	//use filter
	vtkALBASmartPointer<vtkALBAProjectVolume> filter;
	filter->SetInputData(image);
	filter->SetProjectionModeToZ();
	filter->ProjectSubRangeOn();
	filter->SetProjectionRange(range);
	filter->Update();

	//check Control
	vtkImageData *projectedImage = (vtkImageData *)filter->GetOutput();
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
void vtkALBAProjectVolumeTest::TestExecutionProjectionMaxX()
{
	//create imageData with scalars
	vtkImageData *image = CreateNewSPWithScalars();

	//Preparing Test Control Data
	vtkALBASmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int y = 0; y < glo_Dimension[1]; y++)
		{
			double acc = VTK_DOUBLE_MIN;
			for (int x = 0; x < glo_Dimension[0]; x++)
				acc =  MAX(acc, image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x));
			arrayControl->InsertNextTuple1(acc);
		}

	//use filter
	vtkALBASmartPointer<vtkALBAProjectVolume> filter;
	filter->SetInput(image);
	filter->SetProjectionSideToX();
	filter->SetProjectionModalityToMax();
	filter->Update();

	//check Control
	vtkImageData *projectedImage = vtkImageData::SafeDownCast(filter->GetOutput());
	projectedImage->Update();
	for (int j = 0; j < glo_Dimension[1] * glo_Dimension[2]; j++)
	{
		float value1 = arrayControl->GetTuple1(j);
		float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
		CPPUNIT_ASSERT(value1 == value2);
	}

	CPPUNIT_ASSERT(strcmp(filter->GetProjectionSideAsString(), "X") == 0);
	CPPUNIT_ASSERT(filter->GetProjectionSide() == VTK_PROJECT_FROM_X);

	vtkDEL(image);
}

//--------------------------------------------------
void vtkALBAProjectVolumeTest::TestExecutionProjectionMaxY()
{
	//create imageData with scalars
	vtkImageData *image = CreateNewSPWithScalars();

	//Preparing Test Control Data
	vtkALBASmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = VTK_DOUBLE_MIN;
			for (int y = 0; y < glo_Dimension[1]; y++)
				acc = MAX(acc,image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x));
			arrayControl->InsertNextTuple1(acc);
		}

	//use filter
	vtkALBASmartPointer<vtkALBAProjectVolume> filter;
	filter->SetInput(image);
	filter->SetProjectionSideToY();
	filter->SetProjectionModalityToMax();
	filter->Update();

	//check Control
	vtkImageData *projectedImage = (vtkImageData *)filter->GetOutput();
	for (int j = 0; j < glo_Dimension[0] * glo_Dimension[2]; j++)
	{
		float value1 = arrayControl->GetTuple1(j);
		float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
		CPPUNIT_ASSERT(value1 == value2);
	}
	CPPUNIT_ASSERT(strcmp(filter->GetProjectionSideAsString(), "Y") == 0);
	CPPUNIT_ASSERT(filter->GetProjectionSide() == VTK_PROJECT_FROM_Y);

	vtkDEL(image);
}
//--------------------------------------------------
void vtkALBAProjectVolumeTest::TestExecutionProjectionMaxZ()
{
	//create imageData with scalars
	vtkImageData *image = CreateNewSPWithScalars();

	//Preparing Test Control Data
	vtkALBASmartPointer<vtkFloatArray> arrayControl;
	for (int y = 0; y < glo_Dimension[1]; y++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = VTK_DOUBLE_MIN;
			for (int z = 0; z < glo_Dimension[2]; z++)
				acc = MAX(acc,image->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x));
			arrayControl->InsertNextTuple1(acc);
		}

	//use filter
	vtkALBASmartPointer<vtkALBAProjectVolume> filter;
	filter->SetInput(image);
	filter->SetProjectionSideToZ();
	filter->SetProjectionModalityToMax();
	filter->Update();

	//check Control
	vtkImageData *projectedImage = (vtkImageData *)filter->GetOutput();
	for (int j = 0; j < glo_Dimension[0] * glo_Dimension[1]; j++)
	{
		float value1 = arrayControl->GetTuple1(j);
		float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
		CPPUNIT_ASSERT(value1 == value2);
	}
	CPPUNIT_ASSERT(strcmp(filter->GetProjectionSideAsString(), "Z") == 0);
	CPPUNIT_ASSERT(filter->GetProjectionSide() == VTK_PROJECT_FROM_Z);

	vtkDEL(image);
}

//----------------------------------------------------------------------------
vtkImageData * vtkALBAProjectVolumeTest::CreateNewSPWithScalars()
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
	vtkALBASmartPointer<vtkFloatArray> floatArray;
	floatArray->Allocate(size);

	for (; i < size; i++)
	{
		floatArray->SetTuple1(i, i);
	}
 	floatArray->Modified();
 	image->GetPointData()->SetScalars(floatArray);

	return image;
}
