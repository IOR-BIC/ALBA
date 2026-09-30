/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPolylineEditorTest
 Authors: Matteo Giacomoni
 
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
#include "albaVMEPolylineEditorTest.h"
#include "albaVMEPolylineEditor.h"

#include "vtkALBASmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"


//---------------------------------------------------------
void albaVMEPolylineEditorTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  albaVMEPolylineEditor *editor = NULL;
  albaNEW(editor);
  albaDEL(editor);
}
//---------------------------------------------------------
void albaVMEPolylineEditorTest::TestSetData()
//---------------------------------------------------------
{
  albaVMEPolylineEditor *editor = NULL;
  albaNEW(editor);

  vtkALBASmartPointer<vtkSphereSource> input1;
  input1->Update();

  int resultSetData;
  resultSetData = editor->SetData(input1->GetOutput(),0.0);
  CPPUNIT_ASSERT( resultSetData == ALBA_OK );
  editor->Modified();
  editor->Update();

  vtkPolyData *output1 = vtkPolyData::SafeDownCast(editor->GetOutput()->GetVTKData());

  int pointsNumberIn,pointsNumberOut;
  pointsNumberIn = input1->GetOutput()->GetNumberOfPoints();
  pointsNumberOut = output1->GetNumberOfPoints();

  CPPUNIT_ASSERT( pointsNumberIn == pointsNumberOut );

  vtkALBASmartPointer<vtkPolyData> input2;
  CreateExamplePolydata(input2);
  resultSetData = editor->SetData(input2,0.0);
  CPPUNIT_ASSERT( resultSetData == ALBA_OK );
  editor->Modified();
  editor->Update();

  vtkPolyData *output2 = vtkPolyData::SafeDownCast(editor->GetOutput()->GetVTKData());

  pointsNumberIn = input2->GetNumberOfPoints();
  pointsNumberOut = output2->GetNumberOfPoints();

  CPPUNIT_ASSERT( pointsNumberIn == pointsNumberOut );

  albaDEL(editor);
}
//---------------------------------------------------------
void albaVMEPolylineEditorTest::TestSetGetVisualPipe()
//---------------------------------------------------------
{
  albaVMEPolylineEditor *editor = NULL;
  albaNEW(editor);

  albaString pipeIn = "albaPipePolylineGraphEditor";

  editor->SetVisualPipe(pipeIn);

  albaString pipeOut = editor->GetVisualPipe();

  CPPUNIT_ASSERT( pipeIn.Compare(pipeOut) == 0 );

  albaDEL(editor);
}
//------------------------------------------------------------------------------
/* Create example polydata.  This is a connected tree of lines and polylines.

                        17
                        |
                        16
                        |
                        15 - 18 - 19 - 20 - 21
                        |
                        14
   2                    |
  /                     13
 /                      |                        
1 - 3 - 6 - 7 - 8 - 9 - 10 - 11 - 12
|   |\
|   | \
0   5  4

*/  
void albaVMEPolylineEditorTest::CreateExamplePolydata(vtkPolyData *polydata)
//------------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPoints> points;
  vtkALBASmartPointer<vtkCellArray> lines;

  int i ;

  // coordinates of vertices
  static double vertices[22][3] ={
    {0,0,0},
    {1,2,0},
    {2,4,0},
    {2,1,0},
    {3,1,0},
    {3,0,0},

    {3,2,0},
    {4,1,0},
    {5,2,0},
    {6,1,0},
    {7,2,0},
    {8,1,0},
    {9,2,0},

    {7,4,0},
    {9,6,0},
    {10,8,0},
    {13,10,0},
    {14,12,0},

    {11,9,0},
    {12,8,0},
    {13,8,0},
    {16,10,0}
  } ;

  // indices of simple lines and polylines
  static vtkIdType lineids[7][10] = {
    {0,1},
    {1,2},
    {3,4},
    {3,5},
    {1, 3, 6, 7, 8, 9, 10, 11, 12},
    {10, 13, 14, 15, 16, 17},
    {15, 18, 19, 20, 21}
  };

  // insert points
  for (i = 0 ;  i < 22 ;  i++)
    points->InsertNextPoint(vertices[i]) ;

  // insert lines and polylines
  lines->InsertNextCell(2, lineids[0]) ;
  lines->InsertNextCell(2, lineids[1]) ;
  lines->InsertNextCell(2, lineids[2]) ;
  lines->InsertNextCell(2, lineids[3]) ;
  lines->InsertNextCell(9, lineids[4]) ;
  lines->InsertNextCell(6, lineids[5]) ;
  lines->InsertNextCell(5, lineids[6]) ;

  // put points and cells in polydata
  polydata->SetPoints(points) ;
  polydata->SetLines(lines) ;

}