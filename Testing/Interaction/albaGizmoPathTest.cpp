/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoPathTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaGizmoPathTest.h"
#include "albaString.h"
#include <iostream>

#include "albaGizmoPath.h"
#include "albaVMERoot.h"
#include "albaSmartPointer.h"
#include "vtkALBASmartPointer.h"
#include "albaVMEOutputPolyline.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "albaVMEGizmo.h"
#include "albaAbsMatrixPipe.h"

void albaGizmoPathTest::BeforeTest()
{
	CreateTestData();
}

void albaGizmoPathTest::AfterTest()
{
  albaDEL(m_TestPolyline);
  albaDEL(m_Root);
}

void albaGizmoPathTest::CreateTestData()
//----------------------------------------------------------------------------
{
  m_Root = NULL;
  m_TestPolyline = NULL;

  vtkALBASmartPointer<vtkPolyData> polydata;
  vtkALBASmartPointer<vtkPoints> points;
  vtkALBASmartPointer<vtkCellArray> cells;

  double point[3];
  // point 1
  point[0] = 0.0;
  point[1] = 0.0;
  point[2] = 0.0;
  points->InsertNextPoint(point);

  // point 2
  point[0] = 1.0;
  point[1] = 0.0;
  point[2] = 0.0;
  points->InsertNextPoint(point);

  // point 3
  point[0] = 1.0;
  point[1] = 1.0;
  point[2] = 0.0;
  points->InsertNextPoint(point);

  // point 4
  point[0] = 2.0;
  point[1] = 1.0;
  point[2] = 0.0;
  points->InsertNextPoint(point);

  // point 5
  point[0] = 3.0;
  point[1] = 1.0;
  point[2] = 0.0;
  points->InsertNextPoint(point);

  points->Modified();

  CPPUNIT_ASSERT(points->GetNumberOfPoints() == 5);

  vtkIdType pointId[2];
  for(int i = 0; i< points->GetNumberOfPoints();i++)
  {
    if (i > 0)
    {             
      pointId[0] = i - 1;
      pointId[1] = i;
      cells->InsertNextCell(2 , pointId);  
    }
  }

  polydata->SetPoints(points);
  polydata->SetLines(cells);
  polydata->Modified();

  // try to set this data to the volume
  int returnValue = -1;

  CPPUNIT_ASSERT(m_TestPolyline == NULL);
  albaNEW(m_TestPolyline);

  returnValue = m_TestPolyline->SetData(vtkPolyData::SafeDownCast(polydata),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
  CPPUNIT_ASSERT(returnValue == ALBA_OK);

  m_TestPolyline->Modified();
  m_TestPolyline->Update();

  albaNEW(m_Root);
  m_TestPolyline->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_TestPolyline->GetPolylineOutput() != NULL);
}

void albaGizmoPathTest::TestConstructor()
{
  albaGizmoPath *gp = new albaGizmoPath(m_Root, NULL, "test vme");
  cppDEL(gp);
}

void albaGizmoPathTest::TestSetCurvilinearAbscissa()
{

  vtkPolyData* vtkData = vtkPolyData::SafeDownCast(m_TestPolyline->GetOutput()->GetVTKData());
  
  int npoints = vtkData->GetNumberOfPoints();
  // this fail!
  // CPPUNIT_ASSERT(npoints == 5);
  
  // it needs an update to succeed!
  
  npoints = vtkData->GetNumberOfPoints();
  CPPUNIT_ASSERT(npoints == 5);

  albaGizmoPath *gp = new albaGizmoPath(m_Root, NULL, "test gizmo path");
  
  gp->SetConstraintPolyline(m_TestPolyline);
  gp->SetCurvilinearAbscissa(1);

  albaMatrix matrix;
  matrix = gp->GetOutput()->GetAbsMatrixPipe()->GetMatrix();

  matrix.Print(cout) ;

  CPPUNIT_ASSERT(matrix.GetElement(0,0) == 1);
  CPPUNIT_ASSERT(matrix.GetElement(1,0) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(2,0) == 0);

  CPPUNIT_ASSERT(matrix.GetElement(0,1) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(1,1) == 1);
  CPPUNIT_ASSERT(matrix.GetElement(2,1) == 0);

  CPPUNIT_ASSERT(matrix.GetElement(0,2) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(1,2) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(2,2) == 1);

  CPPUNIT_ASSERT(matrix.GetElement(0,3) == 1);
  CPPUNIT_ASSERT(matrix.GetElement(1,3) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(2,3) == 0);

  gp->SetCurvilinearAbscissa(1.5);
  matrix = gp->GetOutput()->GetAbsMatrixPipe()->GetMatrix();
  matrix.Print(cout) ;

  CPPUNIT_ASSERT(matrix.GetElement(0,0) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(1,0) == 1);
  CPPUNIT_ASSERT(matrix.GetElement(2,0) == 0);

  CPPUNIT_ASSERT(matrix.GetElement(0,1) == -1);
  CPPUNIT_ASSERT(matrix.GetElement(1,1) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(2,1) == 0);

  CPPUNIT_ASSERT(matrix.GetElement(0,2) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(1,2) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(2,2) == 1);

  CPPUNIT_ASSERT(matrix.GetElement(0,3) == 1);
  CPPUNIT_ASSERT(matrix.GetElement(1,3) == 0.5);
  CPPUNIT_ASSERT(matrix.GetElement(2,3) == 0);

  gp->SetCurvilinearAbscissa(2.5);
  matrix = gp->GetOutput()->GetAbsMatrixPipe()->GetMatrix();
  matrix.Print(cout) ;

  CPPUNIT_ASSERT(matrix.GetElement(0,0) == 1);
  CPPUNIT_ASSERT(matrix.GetElement(1,0) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(2,0) == 0);

  CPPUNIT_ASSERT(matrix.GetElement(0,1) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(1,1) == 1);
  CPPUNIT_ASSERT(matrix.GetElement(2,1) == 0);

  CPPUNIT_ASSERT(matrix.GetElement(0,2) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(1,2) == 0);
  CPPUNIT_ASSERT(matrix.GetElement(2,2) == 1);

  CPPUNIT_ASSERT(matrix.GetElement(0,3) == 1.5);
  CPPUNIT_ASSERT(matrix.GetElement(1,3) == 1);
  CPPUNIT_ASSERT(matrix.GetElement(2,3) == 0);

  cppDEL(gp);
  
}

void albaGizmoPathTest::TestFixture()
{
	
}