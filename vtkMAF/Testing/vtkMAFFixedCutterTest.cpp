/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFFixedCutterTest.cpp,v $
Language:  C++
Date:      $Date: 2009-12-17 15:35:00 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2009
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
