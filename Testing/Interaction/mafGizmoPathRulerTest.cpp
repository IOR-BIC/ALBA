/*=========================================================================

 Program: MAF2
 Module: mafGizmoPathRulerTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafGizmoPathRulerTest.h"
#include "mafString.h"
#include <iostream>

#include "mafGizmoPathRuler.h"
#include "mafVMERoot.h"
#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMEOutputPolyline.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "mafVMEGizmo.h"
#include "mafAbsMatrixPipe.h"

void mafGizmoPathRulerTest::BeforeTest()
{
	CreateTestData();
}

void mafGizmoPathRulerTest::AfterTest()
{
  mafDEL(m_TestPolyline);
  mafDEL(m_Root);
}

void mafGizmoPathRulerTest::CreateTestData()
//----------------------------------------------------------------------------
{
  m_Root = NULL;
  m_TestPolyline = NULL;

  vtkMAFSmartPointer<vtkPolyData> polydata;
  vtkMAFSmartPointer<vtkPoints> points;
  vtkMAFSmartPointer<vtkCellArray> cells;

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

  int pointId[2];
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
  mafNEW(m_TestPolyline);

  returnValue = m_TestPolyline->SetData(vtkPolyData::SafeDownCast(polydata),0.0,mafVMEGeneric::MAF_VME_REFERENCE_DATA);
  CPPUNIT_ASSERT(returnValue == MAF_OK);

  m_TestPolyline->Modified();
  m_TestPolyline->Update();

  mafNEW(m_Root);
  m_TestPolyline->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_TestPolyline->GetPolylineOutput() != NULL);
}

void mafGizmoPathRulerTest::TestConstructor()
{
  mafGizmoPathRuler *gp = new mafGizmoPathRuler(m_Root, NULL);
  cppDEL(gp);
}

void mafGizmoPathRulerTest::TestSetCurvilinearAbscissa()
{

  vtkPolyData* vtkData = vtkPolyData::SafeDownCast(m_TestPolyline->GetOutput()->GetVTKData());
  
  int npoints = vtkData->GetNumberOfPoints();
  // this fail!
  // CPPUNIT_ASSERT(npoints == 5);
  
  // it needs an update to succeed!
  
  npoints = vtkData->GetNumberOfPoints();
  CPPUNIT_ASSERT(npoints == 5);

  mafGizmoPathRuler *gp = new mafGizmoPathRuler(m_Root, NULL);
  
  gp->SetConstraintPolyline(m_TestPolyline);
  gp->SetCurvilinearAbscissa(1);

  mafMatrix matrix;
  matrix = gp->GetOutput(0)->GetAbsMatrixPipe()->GetMatrix();

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
  matrix = gp->GetOutput(0)->GetAbsMatrixPipe()->GetMatrix();
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
  matrix = gp->GetOutput(0)->GetAbsMatrixPipe()->GetMatrix();
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

void mafGizmoPathRulerTest::TestFixture()
{
	
}