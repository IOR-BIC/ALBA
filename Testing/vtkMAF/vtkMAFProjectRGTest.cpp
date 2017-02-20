/*=========================================================================

 Program: MAF2
 Module: vtkMAFProjectRGTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFProjectRGTest.h"
#include "vtkMAFProjectRG.h"
#include "vtkMAFSmartPointer.h"

#include "vtkPointData.h"
#include "vtkFloatArray.h"
static int glo_Dimension[3] = { 2,3,4 };


//--------------------------------------------------
void vtkMAFProjectRGTest::TestDynamicAllocation()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFProjectRG> filter;
  vtkMAFProjectRG *filter2 = vtkMAFProjectRG::New();
  filter2->Delete();
}
//--------------------------------------------------
void vtkMAFProjectRGTest::TestExecutionProjectionModeToX()
//--------------------------------------------------
{
	vtkRectilinearGrid *rg=CreateNewRGWithScalars();
	
	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int y = 0; y < glo_Dimension[1]; y++)
		{
			double acc = 0;
			for (int x = 0; x < glo_Dimension[0]; x++)
				acc += rg->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0]*glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)glo_Dimension[0]);
		}
	 
  //use filter
  vtkMAFSmartPointer<vtkMAFProjectRG> filter;
  filter->SetInput(rg);
  filter->SetProjectionModeToX();
  filter->Update();

  //check Control
  vtkRectilinearGrid *projectedRG = filter->GetOutput();
  for(int j=0;j<glo_Dimension[1]*glo_Dimension[2];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedRG->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }

  CPPUNIT_ASSERT(strcmp(filter->GetProjectionModeAsString(),"X")==0);
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_X );

	vtkDEL(rg);
}


//--------------------------------------------------
void vtkMAFProjectRGTest::TestExecutionProjectionModeToY()
//--------------------------------------------------
{
	vtkRectilinearGrid *rg = CreateNewRGWithScalars();

	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int y = 0; y < glo_Dimension[1]; y++)
				acc += rg->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)glo_Dimension[1]);
		}

  //use filter
  vtkMAFSmartPointer<vtkMAFProjectRG> filter;
  filter->SetInput(rg);
  filter->SetProjectionModeToY();
  filter->Update();

  //check Control
  vtkRectilinearGrid *projectedRG = filter->GetOutput();
  for(int j=0;j<glo_Dimension[0]* glo_Dimension[2];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedRG->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }
  CPPUNIT_ASSERT(strcmp(filter->GetProjectionModeAsString(),"Y")==0);
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_Y );

	vtkDEL(rg);
}
//--------------------------------------------------
void vtkMAFProjectRGTest::TestExecutionProjectionModeToZ()
//--------------------------------------------------
{
	vtkRectilinearGrid *rg = CreateNewRGWithScalars();

	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int y = 0; y < glo_Dimension[1]; y++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int z = 0; z < glo_Dimension[2]; z++)
				acc += rg->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)glo_Dimension[2]);
		}

  //use filter
  vtkMAFSmartPointer<vtkMAFProjectRG> filter;
  filter->SetInput(rg);
  filter->SetProjectionModeToZ();
  filter->Update();

  //check Control
  vtkRectilinearGrid *projectedRG = filter->GetOutput();
  for(int j=0;j<glo_Dimension[0]* glo_Dimension[1];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedRG->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }
  CPPUNIT_ASSERT(strcmp(filter->GetProjectionModeAsString(),"Z")==0);
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_Z );

	vtkDEL(rg);
}

//--------------------------------------------------
void vtkMAFProjectRGTest::TestRangeProjectionX()
//--------------------------------------------------
{
	vtkRectilinearGrid *rg = CreateNewRGWithScalars();
	int range[2] = { 0,1 };

	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int y = 0; y < glo_Dimension[1]; y++)
		{
			double acc = 0;
			for (int x = range[0]; x < range[1]; x++)
				acc += rg->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)(range[1]-range[0]));
		}

	//use filter
	vtkMAFSmartPointer<vtkMAFProjectRG> filter;
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == false);
	filter->SetInput(rg);
	filter->SetProjectionModeToX();
	filter->ProjectSubRangeOn();
	filter->SetProjectionRange(range);
	filter->Update();

	//check Control
	vtkRectilinearGrid *projectedRG = filter->GetOutput();
	for (int j = 0; j < glo_Dimension[1] * glo_Dimension[2]; j++)
	{
		float value1 = arrayControl->GetTuple1(j);
		float value2 = projectedRG->GetPointData()->GetScalars()->GetTuple1(j);
		CPPUNIT_ASSERT(value1 == value2);
	}
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == true);

	vtkDEL(rg);
}


//--------------------------------------------------
void vtkMAFProjectRGTest::TestRangeProjectionY()
{
	vtkRectilinearGrid *rg = CreateNewRGWithScalars();
	int range[2] = { 1,3 };
	
	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int z = 0; z < glo_Dimension[2]; z++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int y = range[0]; y < range[1]; y++)
				acc += rg->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)(range[1] - range[0]));
		}

	//use filter
	vtkMAFSmartPointer<vtkMAFProjectRG> filter;
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == false);
	filter->SetInput(rg);
	filter->SetProjectionModeToY();
	filter->ProjectSubRangeOn();
	filter->SetProjectionRange(range);
	filter->Update();

	//check Control
	vtkRectilinearGrid *projectedRG = filter->GetOutput();
	for (int j = 0; j < glo_Dimension[0] * glo_Dimension[2]; j++)
	{
		float value1 = arrayControl->GetTuple1(j);
		float value2 = projectedRG->GetPointData()->GetScalars()->GetTuple1(j);
		CPPUNIT_ASSERT(value1 == value2);
	}
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == true);

	vtkDEL(rg);
}
//--------------------------------------------------
void vtkMAFProjectRGTest::TestRangeProjectionZ()
//--------------------------------------------------
{
	vtkRectilinearGrid *rg = CreateNewRGWithScalars();
	int range[2] = { 2,3 };

	//Preparing Test Control Data
	vtkMAFSmartPointer<vtkFloatArray> arrayControl;
	for (int y = 0; y < glo_Dimension[1]; y++)
		for (int x = 0; x < glo_Dimension[0]; x++)
		{
			double acc = 0;
			for (int z = range[0]; z < range[1]; z++)
				acc += rg->GetPointData()->GetScalars()->GetTuple1(z*glo_Dimension[0] * glo_Dimension[1] + y*glo_Dimension[0] + x);
			arrayControl->InsertNextTuple1(acc / (double)(range[1] - range[0]));
		}

	//use filter
	vtkMAFSmartPointer<vtkMAFProjectRG> filter;
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == false);
	filter->SetInput(rg);
	filter->SetProjectionModeToZ();
	filter->ProjectSubRangeOn();
	filter->SetProjectionRange(range);
	filter->Update();

	//check Control
	vtkRectilinearGrid *projectedRG = filter->GetOutput();
	for (int j = 0; j < glo_Dimension[0] * glo_Dimension[1]; j++)
	{
		float value1 = arrayControl->GetTuple1(j);
		float value2 = projectedRG->GetPointData()->GetScalars()->GetTuple1(j);
		CPPUNIT_ASSERT(value1 == value2);
	}
	CPPUNIT_ASSERT(filter->GetProjectSubRange() == true);

	vtkDEL(rg);
}


//----------------------------------------------------------------------------
vtkRectilinearGrid *vtkMAFProjectRGTest::CreateNewRGWithScalars()
{
	//create RG with scalars
	vtkRectilinearGrid *rg;
	vtkNEW(rg);

	vtkFloatArray *vx = vtkFloatArray::New();
	vtkFloatArray *vy = vtkFloatArray::New();
	vtkFloatArray *vz = vtkFloatArray::New();

	vx->SetNumberOfValues(glo_Dimension[0]);
	vy->SetNumberOfValues(glo_Dimension[1]);
	vz->SetNumberOfValues(glo_Dimension[2]);

	for (int ix = 0; ix < glo_Dimension[0]; ix++)
		vx->SetValue(ix, 0. + ((double)ix));
	for (int iy = 0; iy < glo_Dimension[1]; iy++)
		vy->SetValue(iy, 0. + ((double)iy));
	for (int iz = 0; iz < glo_Dimension[2]; iz++)
		vz->SetValue(iz, 0. + ((double)iz));

	rg->SetDimensions(glo_Dimension);
	rg->SetXCoordinates(vx);
	rg->SetYCoordinates(vy);
	rg->SetZCoordinates(vz);

	vx->Delete();
	vy->Delete();
	vz->Delete();


	int i = 0;
	int size = glo_Dimension[0] * glo_Dimension[1] * glo_Dimension[2];
	vtkMAFSmartPointer<vtkFloatArray> array;
	array->Allocate(size);

	for (; i < size; i++)
	{
		array->SetTuple1(i, i);
	}
	array->Modified();
	rg->GetPointData()->SetScalars(array);
	rg->Update();

	return rg;
}

