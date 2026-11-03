/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFixedCutterTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include <cppunit/config/SourcePrefix.h>

#include "vtkALBAFixedCutterTest.h"
#include "vtkALBAFixedCutter.h"
#include "vtkCubeSource.h"
#include "vtkPlane.h"
#include "vtkPolyData.h"
#include "vtkALBASmartPointer.h"

//--------------------------------------------------
void vtkALBAFixedCutterTest::TestDynamicAllocation()
//--------------------------------------------------
{
  vtkALBASmartPointer<vtkALBAFixedCutter> filter1;
  vtkALBAFixedCutter *filter2 = vtkALBAFixedCutter::New();
  filter2->Delete();
}
//--------------------------------------------------
void vtkALBAFixedCutterTest::TestExecute()
//--------------------------------------------------
{
  vtkALBASmartPointer<vtkALBAFixedCutter> filter;
  vtkALBASmartPointer<vtkPlane> plane;
  vtkALBASmartPointer<vtkCubeSource> cube;

  filter->SetInputConnection(cube->GetOutputPort());
  filter->SetCutFunction(plane);
  filter->Update();

  int numberOfPoints = filter->GetOutput()->GetNumberOfPoints();
  CPPUNIT_ASSERT( numberOfPoints == 8 );

  plane->SetOrigin(10,10,10);

  filter->Update();

  CPPUNIT_ASSERT( filter->GetOutput()->GetNumberOfPoints() == 0 );
}
