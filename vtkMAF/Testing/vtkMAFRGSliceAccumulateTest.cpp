/*=========================================================================

 Program: MAF2
 Module: vtkMAFRGSliceAccumulateTest
 Authors: Matrteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "vtkMAFRGSliceAccumulateTest.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFRGSliceAccumulate.h"
#include "vtkImageData.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"

//--------------------------------------------------
void vtkMAFRGSliceAccumulateTest::setUp()
//--------------------------------------------------
{

}
//--------------------------------------------------
void vtkMAFRGSliceAccumulateTest::tearDown()
//--------------------------------------------------
{

}
//--------------------------------------------------
void vtkMAFRGSliceAccumulateTest::TestFixture()
//--------------------------------------------------
{

}
//---------------------------------------------------------
void vtkMAFRGSliceAccumulateTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  vtkMAFRGSliceAccumulate *accumulate = NULL;
  accumulate = vtkMAFRGSliceAccumulate::New();
  accumulate->Delete();
}
//---------------------------------------------------------
void vtkMAFRGSliceAccumulateTest::TestSetSlice()
//---------------------------------------------------------
{
  vtkMAFSmartPointer<vtkImageData> slice1;
  slice1->SetOrigin(0.0,0.0,1.0);
  slice1->SetSpacing(1.0,1.0,1.0);
  slice1->SetDimensions(10,10,1);
  vtkMAFSmartPointer<vtkFloatArray> scalar1;
  float value = 0.0;
  for(int i=0;i<100;i++)
  {
    scalar1->InsertNextTuple1(value);
  }
  scalar1->SetName("scalar");
  slice1->GetPointData()->AddArray(scalar1);
  slice1->GetPointData()->SetActiveScalars("scalar");
  slice1->Update();

  vtkMAFSmartPointer<vtkImageData> slice2;
  slice2->SetOrigin(0.0,0.0,2.0);
  slice2->SetSpacing(1.0,1.0,1.0);
  slice2->SetDimensions(10,10,1);
  vtkMAFSmartPointer<vtkFloatArray> scalar2;
  value = 1.0;
  for(int i=0;i<100;i++)
  {
    scalar2->InsertNextTuple1(value);
  }
  scalar2->SetName("scalar");
  slice2->GetPointData()->AddArray(scalar2);
  slice2->GetPointData()->SetActiveScalars("scalar");
  slice2->Update();


  vtkMAFSmartPointer<vtkImageData> slice3;
  slice3->SetOrigin(0.0,0.0,3.0);
  slice3->SetSpacing(1.0,1.0,1.0);
  slice3->SetDimensions(10,10,1);
  vtkMAFSmartPointer<vtkFloatArray> scalar3;
  value = 2.0;
  for(int i=0;i<100;i++)
  {
    scalar3->InsertNextTuple1(value);
  }
  scalar3->SetName("scalar");
  slice3->GetPointData()->AddArray(scalar3);
  slice3->GetPointData()->SetActiveScalars("scalar");
  slice3->Update();

  vtkMAFSmartPointer<vtkMAFRGSliceAccumulate> accumulate;
  accumulate->SetNumberOfSlices(3);
  accumulate->SetSlice(0,slice1);
  accumulate->SetSlice(1,slice2);
  accumulate->SetSlice(2,slice3);
  accumulate->Update();

  vtkRectilinearGrid *output = accumulate->GetOutput();
  //output->DeepCopy(accumulate->GetOutput());
  output->Update();

  output->GetPointData()->Update();

  vtkFloatArray *scalarsOutput = vtkFloatArray::SafeDownCast(output->GetPointData()->GetScalars());

  CPPUNIT_ASSERT( scalarsOutput->GetNumberOfTuples() == 300 );

  for(int i=0;i<scalarsOutput->GetNumberOfTuples();i++)
  {
    if( i<100 )
    {
      CPPUNIT_ASSERT( scalarsOutput->GetTuple1(i) == 0.0);
    }
    else if( i<200 )
    {
      CPPUNIT_ASSERT( scalarsOutput->GetTuple1(i) == 1.0);
    }
    else if( i<300 )
    {
      CPPUNIT_ASSERT( scalarsOutput->GetTuple1(i) == 2.0);
    }
  }

}
//---------------------------------------------------------
void vtkMAFRGSliceAccumulateTest::TestAddSlice()
//---------------------------------------------------------
{
  vtkMAFSmartPointer<vtkImageData> slice1;
  slice1->SetOrigin(0.0,0.0,1.0);
  slice1->SetSpacing(1.0,1.0,1.0);
  slice1->SetDimensions(10,10,1);
  vtkMAFSmartPointer<vtkFloatArray> scalar1;
  float value = 0.0;
  for(int i=0;i<100;i++)
  {
    scalar1->InsertNextTuple1(value);
  }
  scalar1->SetName("scalar");
  slice1->GetPointData()->AddArray(scalar1);
  slice1->GetPointData()->SetActiveScalars("scalar");
  slice1->Update();

  vtkMAFSmartPointer<vtkImageData> slice2;
  slice2->SetOrigin(0.0,0.0,2.0);
  slice2->SetSpacing(1.0,1.0,1.0);
  slice2->SetDimensions(10,10,1);
  vtkMAFSmartPointer<vtkFloatArray> scalar2;
  value = 1.0;
  for(int i=0;i<100;i++)
  {
    scalar2->InsertNextTuple1(value);
  }
  scalar2->SetName("scalar");
  slice2->GetPointData()->AddArray(scalar2);
  slice2->GetPointData()->SetActiveScalars("scalar");
  slice2->Update();


  vtkMAFSmartPointer<vtkImageData> slice3;
  slice3->SetOrigin(0.0,0.0,3.0);
  slice3->SetSpacing(1.0,1.0,1.0);
  slice3->SetDimensions(10,10,1);
  vtkMAFSmartPointer<vtkFloatArray> scalar3;
  value = 2.0;
  for(int i=0;i<100;i++)
  {
    scalar3->InsertNextTuple1(value);
  }
  scalar3->SetName("scalar");
  slice3->GetPointData()->AddArray(scalar3);
  slice3->GetPointData()->SetActiveScalars("scalar");
  slice3->Update();

  vtkMAFSmartPointer<vtkMAFRGSliceAccumulate> accumulate;
  //accumulate->SetNumberOfSlices(3);
  accumulate->AddSlice(slice2);
  accumulate->AddSlice(slice1);
  accumulate->AddSlice(slice3);
  accumulate->Update();

  vtkRectilinearGrid *output = accumulate->GetOutput();
  //output->DeepCopy(accumulate->GetOutput());
  output->Update();

  output->GetPointData()->Update();

  vtkFloatArray *scalarsOutput = vtkFloatArray::SafeDownCast(output->GetPointData()->GetScalars());

  CPPUNIT_ASSERT( scalarsOutput->GetNumberOfTuples() == 300 );

  for(int i=0;i<scalarsOutput->GetNumberOfTuples();i++)
  {
    if( i<100 )
    {
      CPPUNIT_ASSERT( scalarsOutput->GetTuple1(i) == 1.0);
    }
    else if( i<200 )
    {
      CPPUNIT_ASSERT( scalarsOutput->GetTuple1(i) == 0.0);
    }
    else if( i<300 )
    {
      CPPUNIT_ASSERT( scalarsOutput->GetTuple1(i) == 2.0);
    }
  }
}