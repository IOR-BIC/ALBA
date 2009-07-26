/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFProjectRGTest.cpp,v $
Language:  C++
Date:      $Date: 2009-07-26 08:17:36 $
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

#include "vtkMAFProjectRGTest.h"
#include "vtkMAFProjectRG.h"
#include "vtkMAFSmartPointer.h"

#include "vtkPointData.h"
#include "vtkFloatArray.h"



//--------------------------------------------------
void vtkMAFProjectRGTest::setUp()
//--------------------------------------------------
{
 
}

//--------------------------------------------------
void vtkMAFProjectRGTest::tearDown()
//--------------------------------------------------
{

}
//--------------------------------------------------
void vtkMAFProjectRGTest::TestDynamicAllocation()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFProjectRG> filter;
  vtkMAFProjectRG *filter2 = vtkMAFProjectRG::New();
  vtkDEL(filter2);
}
//--------------------------------------------------
void vtkMAFProjectRGTest::TestExecutionProjectionModeToX()
//--------------------------------------------------
{
  //create RG with scalars
  vtkMAFSmartPointer<vtkRectilinearGrid> rg;

  vtkFloatArray *vx = vtkFloatArray::New();
  vtkFloatArray *vy = vtkFloatArray::New();
  vtkFloatArray *vz = vtkFloatArray::New();

  int Dimension[3] = {2,2,2};

  vx->SetNumberOfValues(Dimension[0]);
  vy->SetNumberOfValues(Dimension[1]);
  vz->SetNumberOfValues(Dimension[2]);

  for (int ix = 0; ix < Dimension[0]; ix++)
    vx->SetValue(ix, 0.+ ((double)ix));
  for (int iy = 0; iy < Dimension[1]; iy++)
    vy->SetValue(iy, 0.+ ((double)iy));
  for (int iz = 0; iz < Dimension[2]; iz++)
    vz->SetValue(iz, 0.+ ((double)iz));

  rg->SetDimensions(Dimension);
  rg->SetXCoordinates(vx);
  rg->SetYCoordinates(vy);
  rg->SetZCoordinates(vz);
  
  vx->Delete();
  vy->Delete();
  vz->Delete();
  

  int i = 0;
  int size = Dimension[0] * Dimension[1] * Dimension[2];
  vtkMAFSmartPointer<vtkFloatArray> array;
  array->Allocate(8);
  
  for(; i < size; i++)
  {
    array->SetTuple1(i,i);
  }
  array->Modified();
  rg->GetPointData()->SetScalars(array);
  rg->Update();
  
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
  arrayControl->InsertNextTuple1(1./Dimension[0]);
  arrayControl->InsertNextTuple1(5./Dimension[0]);
  arrayControl->InsertNextTuple1(9./Dimension[0]);
  arrayControl->InsertNextTuple1(13./Dimension[0]);

  
  //use filter
  vtkMAFSmartPointer<vtkMAFProjectRG> filter;
  filter->SetInput(rg);
  filter->SetProjectionModeToX();
  filter->Update();

  //check Control
  vtkRectilinearGrid *projectedRG = filter->GetOutput();
  for(int j=0;j<Dimension[1]*Dimension[2];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedRG->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }

  CPPUNIT_ASSERT(mafString(filter->GetProjectionModeAsString()).Equals("X"));
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_X );
}
//--------------------------------------------------
void vtkMAFProjectRGTest::TestExecutionProjectionModeToY()
//--------------------------------------------------
{
  //create RG with scalars
  vtkMAFSmartPointer<vtkRectilinearGrid> rg;

  vtkFloatArray *vx = vtkFloatArray::New();
  vtkFloatArray *vy = vtkFloatArray::New();
  vtkFloatArray *vz = vtkFloatArray::New();

  int Dimension[3] = {2,2,2};

  vx->SetNumberOfValues(Dimension[0]);
  vy->SetNumberOfValues(Dimension[1]);
  vz->SetNumberOfValues(Dimension[2]);

  for (int ix = 0; ix < Dimension[0]; ix++)
    vx->SetValue(ix, 0.+ ((double)ix));
  for (int iy = 0; iy < Dimension[1]; iy++)
    vy->SetValue(iy, 0.+ ((double)iy));
  for (int iz = 0; iz < Dimension[2]; iz++)
    vz->SetValue(iz, 0.+ ((double)iz));

  rg->SetDimensions(Dimension);
  rg->SetXCoordinates(vx);
  rg->SetYCoordinates(vy);
  rg->SetZCoordinates(vz);

  vx->Delete();
  vy->Delete();
  vz->Delete();


  int i = 0;
  int size = Dimension[0] * Dimension[1] * Dimension[2];
  vtkMAFSmartPointer<vtkFloatArray> array;
  array->Allocate(8);

  for(; i < size; i++)
  {
    array->SetTuple1(i,i);
  }
  array->Modified();
  rg->GetPointData()->SetScalars(array);
  rg->Update();

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
  arrayControl->InsertNextTuple1(2./Dimension[1]);
  arrayControl->InsertNextTuple1(4./Dimension[1]);
  arrayControl->InsertNextTuple1(10./Dimension[1]);
  arrayControl->InsertNextTuple1(12./Dimension[1]);

  //use filter
  vtkMAFSmartPointer<vtkMAFProjectRG> filter;
  filter->SetInput(rg);
  filter->SetProjectionModeToY();
  filter->Update();

  //check Control
  vtkRectilinearGrid *projectedRG = filter->GetOutput();
  for(int j=0;j<Dimension[0]*Dimension[2];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedRG->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }
  CPPUNIT_ASSERT(mafString(filter->GetProjectionModeAsString()).Equals("Y"));
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_Y );
}
//--------------------------------------------------
void vtkMAFProjectRGTest::TestExecutionProjectionModeToZ()
//--------------------------------------------------
{
  //create RG with scalars
  vtkMAFSmartPointer<vtkRectilinearGrid> rg;

  vtkFloatArray *vx = vtkFloatArray::New();
  vtkFloatArray *vy = vtkFloatArray::New();
  vtkFloatArray *vz = vtkFloatArray::New();

  int Dimension[3] = {2,2,2};

  vx->SetNumberOfValues(Dimension[0]);
  vy->SetNumberOfValues(Dimension[1]);
  vz->SetNumberOfValues(Dimension[2]);

  for (int ix = 0; ix < Dimension[0]; ix++)
    vx->SetValue(ix, 0.+ ((double)ix));
  for (int iy = 0; iy < Dimension[1]; iy++)
    vy->SetValue(iy, 0.+ ((double)iy));
  for (int iz = 0; iz < Dimension[2]; iz++)
    vz->SetValue(iz, 0.+ ((double)iz));

  rg->SetDimensions(Dimension);
  rg->SetXCoordinates(vx);
  rg->SetYCoordinates(vy);
  rg->SetZCoordinates(vz);

  vx->Delete();
  vy->Delete();
  vz->Delete();


  int i = 0;
  int size = Dimension[0] * Dimension[1] * Dimension[2];
  vtkMAFSmartPointer<vtkFloatArray> array;
  array->Allocate(8);

  for(; i < size; i++)
  {
    array->SetTuple1(i,i);
  }
  array->Modified();
  rg->GetPointData()->SetScalars(array);
  rg->Update();

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
  arrayControl->InsertNextTuple1(4./Dimension[2]);
  arrayControl->InsertNextTuple1(6./Dimension[2]);
  arrayControl->InsertNextTuple1(8./Dimension[2]);
  arrayControl->InsertNextTuple1(10./Dimension[2]);

  

  //use filter
  vtkMAFSmartPointer<vtkMAFProjectRG> filter;
  filter->SetInput(rg);
  filter->SetProjectionModeToZ();
  filter->Update();

  //check Control
  vtkRectilinearGrid *projectedRG = filter->GetOutput();
  for(int j=0;j<Dimension[0]*Dimension[1];j++)
  {
    float value1 = arrayControl->GetTuple1(j);
    float value2 = projectedRG->GetPointData()->GetScalars()->GetTuple1(j);
    CPPUNIT_ASSERT(value1 == value2);
  }
  CPPUNIT_ASSERT(mafString(filter->GetProjectionModeAsString()).Equals("Z"));
  CPPUNIT_ASSERT(filter->GetProjectionMode() == VTK_PROJECT_FROM_Z );
}
//--------------------------------------------------
void vtkMAFProjectRGTest::TestPrintSelf()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFProjectRG> filter;
  filter->SetProjectionModeToX();
  std::cout;
  filter->PrintSelf(cout, 2);
  
  
}