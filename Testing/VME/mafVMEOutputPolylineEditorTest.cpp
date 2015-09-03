/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputPolylineEditorTest
 Authors: Daniele Giunchi
 
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
#include "mafVMEOutputPolylineEditorTest.h"

#include "mafVMEOutputPolylineEditor.h"

#include "mafVMEPolylineEditor.h"
#include "mafMatrix.h"
#include "vtkMAFSmartPointer.h"

#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPointData.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void mafVMEOutputPolylineEditorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputPolylineEditorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputPolylineEditor outputSurface;
}
//----------------------------------------------------------------------------
void mafVMEOutputPolylineEditorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputPolylineEditor *outputSurface = new mafVMEOutputPolylineEditor();
  cppDEL(outputSurface);
}
//----------------------------------------------------------------------------
void mafVMEOutputPolylineEditorTest::TestGetPolylineData_Update()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  mafVMEPolylineEditor *vmePolylineEditor;
  mafNEW(vmePolylineEditor);
  
  vtkMAFSmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(1.0,1.0,1.0);

  vtkMAFSmartPointer<vtkCellArray> cells;
  vtkIdType lineids[2];
  lineids[0] = 0;
  lineids[1] = 1;
  cells->InsertNextCell(2,lineids);

  vtkMAFSmartPointer<vtkPolyData> polyline;
  polyline->SetPoints(pts);
  polyline->SetLines(cells);
  vmePolylineEditor->SetData(polyline, 0.0);

  mafVMEOutputPolylineEditor *outputPolylineEditor = NULL;
  outputPolylineEditor = mafVMEOutputPolylineEditor::SafeDownCast(vmePolylineEditor->GetOutput());
  outputPolylineEditor->Update();
  m_Result = outputPolylineEditor != NULL && 
             vmePolylineEditor->GetOutput()->GetVTKData() == outputPolylineEditor->GetPolylineData();

  TEST_RESULT;

  mafDEL(vmePolylineEditor);
}