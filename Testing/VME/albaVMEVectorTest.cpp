/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVectorTest
 Authors: Paolo Quadrani
 
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
#include "albaVMEVectorTest.h"
#include "albaVMEVector.h"

#include "vtkALBASmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaVMEVectorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEVectorTest::BeforeTest()
//----------------------------------------------------------------------------
{
  albaNEW(m_VmeVector);
}
//----------------------------------------------------------------------------
void albaVMEVectorTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_VmeVector);
}
//----------------------------------------------------------------------------
void albaVMEVectorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEVector *vmeVector;
  albaNEW(vmeVector);
  albaDEL(vmeVector);
}
//----------------------------------------------------------------------------
void albaVMEVectorTest::TestGetVisualPipe()
//----------------------------------------------------------------------------
{
  result = m_VmeVector->GetVisualPipe() == "albaPipeVector";

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMEVectorTest::TestSetData()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(1.0,1.0,1.0);

  vtkALBASmartPointer<vtkCellArray> cells;
  vtkIdType cell[2] = {0,1};
  cells->InsertNextCell(2,cell);

  vtkALBASmartPointer<vtkPolyData> line;
  line->SetPoints(pts);
  line->SetLines(cells);

  result = m_VmeVector->SetData(line,0.0) == ALBA_OK;

  TEST_RESULT;

  vtkALBASmartPointer<vtkPoints> pts_bad_1;
  pts_bad_1->InsertNextPoint(0.0,0.0,0.0);
  pts_bad_1->InsertNextPoint(1.0,1.0,1.0);
  pts_bad_1->InsertNextPoint(2.0,2.0,2.0);

  vtkALBASmartPointer<vtkCellArray> cells_bad_1;
  vtkIdType cell_bad_1[3] = {0,1,2};
  cells_bad_1->InsertNextCell(3,cell_bad_1);

  vtkALBASmartPointer<vtkPolyData> line_bad_1;
  line_bad_1->SetPoints(pts_bad_1);
  line_bad_1->SetLines(cells_bad_1);

  result = m_VmeVector->SetData(line_bad_1,0.0) == ALBA_ERROR;

  TEST_RESULT;

  vtkALBASmartPointer<vtkPoints> pts_bad_2;
  pts_bad_2->InsertNextPoint(0.0,0.0,0.0);
  pts_bad_2->InsertNextPoint(1.0,1.0,1.0);
  pts_bad_2->InsertNextPoint(2.0,2.0,2.0);
  pts_bad_2->InsertNextPoint(3.0,3.0,3.0);

  vtkALBASmartPointer<vtkCellArray> cells_bad_2;
  vtkIdType cell_bad_2_0[2] = {0,1};
	vtkIdType cell_bad_2_1[2] = {1,2};
	vtkIdType cell_bad_2_2[2] = {2,3};
  cells_bad_2->InsertNextCell(2,cell_bad_2_0);
  cells_bad_2->InsertNextCell(2,cell_bad_2_1);
  cells_bad_2->InsertNextCell(2,cell_bad_2_2);

  vtkALBASmartPointer<vtkPolyData> line_bad_2;
  line_bad_2->SetPoints(pts_bad_2);
  line_bad_2->SetLines(cells_bad_2);

  result = m_VmeVector->SetData(line_bad_2,0.0) == ALBA_ERROR;

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMEVectorTest::TestGetModule()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(1.0,0.0,0.0);

  vtkALBASmartPointer<vtkCellArray> cells;
  vtkIdType cell[2] = {0,1};
  cells->InsertNextCell(2,cell);

  vtkALBASmartPointer<vtkPolyData> line;
  line->SetPoints(pts);
  line->SetLines(cells);

  m_VmeVector->SetData(line,0.0);
  m_VmeVector->Update();

  result = m_VmeVector->GetModule() == 1.0;

  TEST_RESULT;
}
