/*=========================================================================

 Program: MAF2
 Module: mafVMEPolylineGraphTest
 Authors: Roberto Mucci
 
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
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkCell.h"
#include "vtkCell.h"
#include "mafVMEPolylineGraph.h"
#include "mafVMEPolylineGraphTest.h"
#include "wx/wx.h"
#include <fstream>

static bool ExtractModel   = true;
static bool CleanModel     = false;

//----------------------------------------------------------------------------
void mafVMEPolylineGraphTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_Polydata = vtkPolyData::New() ;
  CreateExamplePolydata();
}

//----------------------------------------------------------------------------
void mafVMEPolylineGraphTest::AfterTest()
//----------------------------------------------------------------------------
{
  m_Polydata->Delete() ;
  m_Polydata = NULL ;
}

//---------------------------------------------------------
void mafVMEPolylineGraphTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafVMEPolylineGraph *graph = NULL;
  mafNEW(graph);
  mafDEL(graph);
}

//---------------------------------------------------------
void mafVMEPolylineGraphTest::TestSetData()
//---------------------------------------------------------
{
  mafVMEPolylineGraph *graph = NULL;
  mafNEW(graph);

  graph->SetData(m_Polydata, 0);

  double bounds[6];
  graph->GetOutput()->GetBounds(bounds);

  CPPUNIT_ASSERT(fabs(bounds[0] - 0.00) < 0.001 && (bounds[1] - 16.00) < 0.001 && (bounds[2]- 0) < 0.001 
                  && (bounds[3] - 12.00) < 0.001 && (bounds[4]- 0) < 0.001 && (bounds[5] - 0) < 0.001);


  mafDEL(graph);
}

//------------------------------------------------------------------------------
void mafVMEPolylineGraphTest::CreateExamplePolydata()
//------------------------------------------------------------------------------
{
  vtkPoints *points = vtkPoints::New() ;
  vtkCellArray *lines = vtkCellArray::New() ;

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
  m_Polydata->SetPoints(points) ;
  m_Polydata->SetLines(lines) ;
  points->Delete() ;
  lines->Delete() ;
}
