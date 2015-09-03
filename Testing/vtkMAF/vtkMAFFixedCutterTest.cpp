/*=========================================================================

 Program: MAF2
 Module: vtkMAFFixedCutterTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFFixedCutterTest.h"
#include "vtkMAFFixedCutter.h"
#include "vtkCubeSource.h"
#include "vtkPlane.h"
#include "vtkPolyData.h"
#include "vtkMAFSmartPointer.h"

//--------------------------------------------------
void vtkMAFFixedCutterTest::TestDynamicAllocation()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFFixedCutter> filter1;
  vtkMAFFixedCutter *filter2 = vtkMAFFixedCutter::New();
  filter2->Delete();
}
//--------------------------------------------------
void vtkMAFFixedCutterTest::TestExecute()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFFixedCutter> filter;
  vtkMAFSmartPointer<vtkPlane> plane;
  vtkMAFSmartPointer<vtkCubeSource> cube;

  filter->SetInput((vtkDataSet*)cube->GetOutput());
  filter->SetCutFunction(plane);
  filter->Update();

  CPPUNIT_ASSERT( filter->GetOutput()->GetNumberOfPoints() == 4 );

  plane->SetOrigin(10,10,10);

  filter->Update();

  //If the cutter has any points , the filter should place a single point in default position
  CPPUNIT_ASSERT( filter->GetOutput()->GetNumberOfPoints() == 1 );

  CPPUNIT_ASSERT( filter->GetOutput()->GetPoint(0)[0] == 0 );
  CPPUNIT_ASSERT( filter->GetOutput()->GetPoint(0)[1] == 0 );
  CPPUNIT_ASSERT( filter->GetOutput()->GetPoint(0)[2] == 0 );
}
