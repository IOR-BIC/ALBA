/*=========================================================================

 Program: MAF2
 Module: mafVMEPolylineEditorTest
 Authors: Matteo Giacomoni
 
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
#include "mafVMEPolylineEditorTest.h"
#include "mafVMEPolylineEditor.h"

#include "vtkMAFSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"


//---------------------------------------------------------
void mafVMEPolylineEditorTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafVMEPolylineEditor *editor = NULL;
  mafNEW(editor);
  mafDEL(editor);
}
//---------------------------------------------------------
void mafVMEPolylineEditorTest::TestSetData()
//---------------------------------------------------------
{
  mafVMEPolylineEditor *editor = NULL;
  mafNEW(editor);

  vtkMAFSmartPointer<vtkSphereSource> input1;
  input1->Update();

  int resultSetData;
  resultSetData = editor->SetData(input1->GetOutput(),0.0);
  CPPUNIT_ASSERT( resultSetData == MAF_OK );
  editor->Modified();
  editor->Update();

  vtkPolyData *output1 = vtkPolyData::SafeDownCast(editor->GetOutput()->GetVTKData());
  output1->Update();

  int pointsNumberIn,pointsNumberOut;
  pointsNumberIn = input1->GetOutput()->GetNumberOfPoints();
  pointsNumberOut = output1->GetNumberOfPoints();

  CPPUNIT_ASSERT( pointsNumberIn == pointsNumberOut );

  vtkMAFSmartPointer<vtkPolyData> input2;
  CreateExamplePolydata(input2);
  resultSetData = editor->SetData(input2,0.0);
  CPPUNIT_ASSERT( resultSetData == MAF_OK );
  editor->Modified();
  editor->Update();

  vtkPolyData *output2 = vtkPolyData::SafeDownCast(editor->GetOutput()->GetVTKData());

  pointsNumberIn = input2->GetNumberOfPoints();
  pointsNumberOut = output2->GetNumberOfPoints();

  CPPUNIT_ASSERT( pointsNumberIn == pointsNumberOut );

  mafDEL(editor);
}
//---------------------------------------------------------
void mafVMEPolylineEditorTest::TestSetGetVisualPipe()
//---------------------------------------------------------
{
  mafVMEPolylineEditor *editor = NULL;
  mafNEW(editor);

  mafString pipeIn = "mafPipePolylineGraphEditor";

  editor->SetVisualPipe(pipeIn);

  mafString pipeOut = editor->GetVisualPipe();

  CPPUNIT_ASSERT( pipeIn.Compare(pipeOut) == 0 );

  mafDEL(editor);
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
void mafVMEPolylineEditorTest::CreateExamplePolydata(vtkPolyData *polydata)
//------------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPoints> points;
  vtkMAFSmartPointer<vtkCellArray> lines;

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