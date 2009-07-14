/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFProjectSPTest.cpp,v $
Language:  C++
Date:      $Date: 2009-07-14 10:05:03 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <cppunit/config/SourcePrefix.h>

#include "mafString.h"

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
  vtkDEL(filter2);
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

  CPPUNIT_ASSERT(mafString(filter->GetProjectionModeAsString()).Equals("X"));
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
  for(int j=0;j<dimension[1]*dimension[2];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }
  CPPUNIT_ASSERT(mafString(filter->GetProjectionModeAsString()).Equals("Y"));
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
  for(int j=0;j<dimension[1]*dimension[2];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedImage->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }
  CPPUNIT_ASSERT(mafString(filter->GetProjectionModeAsString()).Equals("Z"));
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