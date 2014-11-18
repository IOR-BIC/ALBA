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

//--------------------------------------------------
void vtkMAFProjectSPTest::setUp()
//--------------------------------------------------
{
 
}

//--------------------------------------------------
void vtkMAFProjectSPTest::tearDown()
//--------------------------------------------------
{

}
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
  vtkMAFSmartPointer<vtkImageData> image;
  int dimension[3] = {2,2,2};
  double spacing[3] = {1. ,1. ,1.};
  image->SetDimensions(dimension);
  image->SetSpacing(spacing);
  image->SetScalarTypeToFloat();

  int i = 0;
  int size = dimension[0] * dimension[1] * dimension[2];
  vtkMAFSmartPointer<vtkFloatArray> array;
  array->Allocate(8);
  
  for(; i < size; i++)
  {
    array->SetTuple1(i,i);
  }
  array->Modified();
  image->GetPointData()->SetScalars(array);
  image->Update();
  
  // calculate projection over three base axis
  // the xy planes are:
  /*
  z = 0
  0 1 
  2 3
 
  */

  /*
  z = 1
  4 5
  6 7
  */


  
  //the projection along x is:
  /*
  x proj
  1 5
  9 13
  must divided by dim[0]
  */

  vtkMAFSmartPointer<vtkFloatArray> arrayControl;
  arrayControl->InsertNextTuple1(1./dimension[0]);
  arrayControl->InsertNextTuple1(5./dimension[0]);
  arrayControl->InsertNextTuple1(9./dimension[0]);
  arrayControl->InsertNextTuple1(13./dimension[0]);

  
  //use filter
  vtkMAFSmartPointer<vtkMAFProjectSP> filter;
  filter->SetInput(image);
  filter->SetProjectionModeToX();
  filter->Update();

  //check Control
  vtkStructuredPoints *projectedImage = filter->GetOutput();
  for(int j=0;j<dimension[1]*dimension[2];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }

  CPPUNIT_ASSERT(strcmp(filter->GetProjectionModeAsString(),"X")==0);
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_X );
}
//--------------------------------------------------
void vtkMAFProjectSPTest::TestExecutionProjectionModeToY()
//--------------------------------------------------
{
  //create imageData with scalars
  vtkMAFSmartPointer<vtkImageData> image;
  int dimension[3] = {2,2,2};
  double spacing[3] = {1. ,1. ,1.};
  image->SetDimensions(dimension);
  image->SetSpacing(spacing);
  image->SetScalarTypeToFloat();

  int i = 0;
  int size = dimension[0] * dimension[1] * dimension[2];
  vtkMAFSmartPointer<vtkFloatArray> array;
  array->Allocate(8);

  for(; i < size; i++)
  {
    array->SetTuple1(i,i);
  }
  array->Modified();
  image->GetPointData()->SetScalars(array);
  image->Update();

  // calculate projection over three base axis
  // the xy planes are:
  /*
  z = 0
  0 1 
  2 3

  */

  /*
  z = 1
  4 5
  6 7
  */

  //the projection along y is:
  /*
  y proj
  2  4
  10 12
  must divided by dim[1]
  */

  vtkMAFSmartPointer<vtkFloatArray> arrayControl;
  arrayControl->InsertNextTuple1(2./dimension[1]);
  arrayControl->InsertNextTuple1(4./dimension[1]);
  arrayControl->InsertNextTuple1(10./dimension[1]);
  arrayControl->InsertNextTuple1(12./dimension[1]);

  //use filter
  vtkMAFSmartPointer<vtkMAFProjectSP> filter;
  filter->SetInput(image);
  filter->SetProjectionModeToY();
  filter->Update();

  //check Control
  vtkStructuredPoints *projectedImage = filter->GetOutput();
  for(int j=0;j<dimension[0]*dimension[2];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }
  CPPUNIT_ASSERT(strcmp(filter->GetProjectionModeAsString(),"Y")==0);
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_Y );
}
//--------------------------------------------------
void vtkMAFProjectSPTest::TestExecutionProjectionModeToZ()
//--------------------------------------------------
{
  //create imageData with scalars
  vtkMAFSmartPointer<vtkImageData> image;
  int dimension[3] = {2,2,2};
  double spacing[3] = {1. ,1. ,1.};
  image->SetDimensions(dimension);
  image->SetSpacing(spacing);
  image->SetScalarTypeToFloat();

  int i = 0;
  int size = dimension[0] * dimension[1] * dimension[2];
  vtkMAFSmartPointer<vtkFloatArray> array;
  array->Allocate(8);

  for(; i < size; i++)
  {
    array->SetTuple1(i,i);
  }
  array->Modified();
  image->GetPointData()->SetScalars(array);
  image->Update();

  // calculate projection over three base axis
  // the xy planes are:
  /*
  z = 0
  0 1 
  2 3

  */

  /*
  z = 1
  4 5
  6 7
  */


  //the projection along z is:
  /*
  z proj
  4  6
  8 10
  must divided by dim[2]
  */

  vtkMAFSmartPointer<vtkFloatArray> arrayControl;
  arrayControl->InsertNextTuple1(4./dimension[2]);
  arrayControl->InsertNextTuple1(6./dimension[2]);
  arrayControl->InsertNextTuple1(8./dimension[2]);
  arrayControl->InsertNextTuple1(10./dimension[2]);

  

  //use filter
  vtkMAFSmartPointer<vtkMAFProjectSP> filter;
  filter->SetInput(image);
  filter->SetProjectionModeToZ();
  filter->Update();

  //check Control
  vtkStructuredPoints *projectedImage = filter->GetOutput();
  for(int j=0;j<dimension[0]*dimension[1];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }
  CPPUNIT_ASSERT(strcmp(filter->GetProjectionModeAsString(),"Z")==0);
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_Z );
}
//--------------------------------------------------
void vtkMAFProjectSPTest::TestPrintSelf()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFProjectSP> filter;
  filter->SetProjectionModeToX();
  std::cout;
  filter->PrintSelf(cout, 2);
  
  
}