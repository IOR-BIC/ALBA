/*=========================================================================

 Program: MAF2
 Module: mafVMEVectorTest
 Authors: Paolo Quadrani
 
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
#include "mafVMEVectorTest.h"
#include "mafVMEVector.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafVMEVectorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEVectorTest::BeforeTest()
//----------------------------------------------------------------------------
{
  mafNEW(m_VmeVector);
}
//----------------------------------------------------------------------------
void mafVMEVectorTest::AfterTest()
//----------------------------------------------------------------------------
{
  mafDEL(m_VmeVector);
}
//----------------------------------------------------------------------------
void mafVMEVectorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEVector *vmeVector;
  mafNEW(vmeVector);
  mafDEL(vmeVector);
}
//----------------------------------------------------------------------------
void mafVMEVectorTest::TestGetVisualPipe()
//----------------------------------------------------------------------------
{
  result = m_VmeVector->GetVisualPipe() == "mafPipeVector";

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMEVectorTest::TestSetData()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(1.0,1.0,1.0);

  vtkMAFSmartPointer<vtkCellArray> cells;
  int cell[2] = {0,1};
  cells->InsertNextCell(2,cell);

  vtkMAFSmartPointer<vtkPolyData> line;
  line->SetPoints(pts);
  line->SetLines(cells);
  line->Update();

  result = m_VmeVector->SetData(line,0.0) == MAF_OK;

  TEST_RESULT;

  vtkMAFSmartPointer<vtkPoints> pts_bad_1;
  pts_bad_1->InsertNextPoint(0.0,0.0,0.0);
  pts_bad_1->InsertNextPoint(1.0,1.0,1.0);
  pts_bad_1->InsertNextPoint(2.0,2.0,2.0);

  vtkMAFSmartPointer<vtkCellArray> cells_bad_1;
  int cell_bad_1[3] = {0,1,2};
  cells_bad_1->InsertNextCell(3,cell_bad_1);

  vtkMAFSmartPointer<vtkPolyData> line_bad_1;
  line_bad_1->SetPoints(pts_bad_1);
  line_bad_1->SetLines(cells_bad_1);
  line_bad_1->Update();

  result = m_VmeVector->SetData(line_bad_1,0.0) == MAF_ERROR;

  TEST_RESULT;

  vtkMAFSmartPointer<vtkPoints> pts_bad_2;
  pts_bad_2->InsertNextPoint(0.0,0.0,0.0);
  pts_bad_2->InsertNextPoint(1.0,1.0,1.0);
  pts_bad_2->InsertNextPoint(2.0,2.0,2.0);
  pts_bad_2->InsertNextPoint(3.0,3.0,3.0);

  vtkMAFSmartPointer<vtkCellArray> cells_bad_2;
  int cell_bad_2_0[2] = {0,1};
  int cell_bad_2_1[2] = {1,2};
  int cell_bad_2_2[2] = {2,3};
  cells_bad_2->InsertNextCell(2,cell_bad_2_0);
  cells_bad_2->InsertNextCell(2,cell_bad_2_1);
  cells_bad_2->InsertNextCell(2,cell_bad_2_2);

  vtkMAFSmartPointer<vtkPolyData> line_bad_2;
  line_bad_2->SetPoints(pts_bad_2);
  line_bad_2->SetLines(cells_bad_2);
  line_bad_2->Update();

  result = m_VmeVector->SetData(line_bad_2,0.0) == MAF_ERROR;

  TEST_RESULT;


  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafVMEVectorTest::TestGetModule()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(1.0,0.0,0.0);

  vtkMAFSmartPointer<vtkCellArray> cells;
  int cell[2] = {0,1};
  cells->InsertNextCell(2,cell);

  vtkMAFSmartPointer<vtkPolyData> line;
  line->SetPoints(pts);
  line->SetLines(cells);
  line->Update();

  m_VmeVector->SetData(line,0.0);
  m_VmeVector->GetOutput()->GetVTKData()->Update();
  m_VmeVector->Update();

  result = m_VmeVector->GetModule() == 1.0;

  TEST_RESULT;

  delete wxLog::SetActiveTarget(NULL);
}
