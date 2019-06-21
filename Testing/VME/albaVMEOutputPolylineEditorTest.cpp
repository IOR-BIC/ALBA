/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputPolylineEditorTest
 Authors: Daniele Giunchi
 
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
#include "albaVMEOutputPolylineEditorTest.h"

#include "albaVMEOutputPolylineEditor.h"

#include "albaVMEPolylineEditor.h"
#include "albaMatrix.h"
#include "vtkALBASmartPointer.h"

#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPointData.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void albaVMEOutputPolylineEditorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputPolylineEditorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputPolylineEditor outputSurface;
}
//----------------------------------------------------------------------------
void albaVMEOutputPolylineEditorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputPolylineEditor *outputSurface = new albaVMEOutputPolylineEditor();
  cppDEL(outputSurface);
}
//----------------------------------------------------------------------------
void albaVMEOutputPolylineEditorTest::TestGetPolylineData_Update()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  albaVMEPolylineEditor *vmePolylineEditor;
  albaNEW(vmePolylineEditor);
  
  vtkALBASmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(1.0,1.0,1.0);

  vtkALBASmartPointer<vtkCellArray> cells;
  vtkIdType lineids[2];
  lineids[0] = 0;
  lineids[1] = 1;
  cells->InsertNextCell(2,lineids);

  vtkALBASmartPointer<vtkPolyData> polyline;
  polyline->SetPoints(pts);
  polyline->SetLines(cells);
  vmePolylineEditor->SetData(polyline, 0.0);

  albaVMEOutputPolylineEditor *outputPolylineEditor = NULL;
  outputPolylineEditor = albaVMEOutputPolylineEditor::SafeDownCast(vmePolylineEditor->GetOutput());
  outputPolylineEditor->Update();
  m_Result = outputPolylineEditor != NULL && 
             vmePolylineEditor->GetOutput()->GetVTKData() == outputPolylineEditor->GetPolylineData();

  TEST_RESULT;

  albaDEL(vmePolylineEditor);
}