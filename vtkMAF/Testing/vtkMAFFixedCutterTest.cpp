/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFFixedCutterTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:53:13 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2009
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFFixedCutterTest.h"
#include "vtkMAFFixedCutter.h"
#include "vtkCubeSource.h"
#include "vtkPlane.h"
#include "vtkPolyData.h"
#include "vtkMAFSmartPointer.h"

//--------------------------------------------------
void vtkMAFFixedCutterTest::setUp()
//--------------------------------------------------
{

}

//--------------------------------------------------
void vtkMAFFixedCutterTest::tearDown()
//--------------------------------------------------
{

}
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
