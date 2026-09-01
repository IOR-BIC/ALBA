/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFixTopologyTest
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
#include "vtkALBAFixTopology.h"
#include "vtkALBAFixTopologyTest.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkALBAPoissonSurfaceReconstruction.h"

//-------------------------------------------------------------------------
void vtkALBAFixTopologyTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkALBAFixTopology> filter1;

  vtkALBAFixTopology *filter2;
  vtkNEW(filter2);

  vtkDEL(filter2);
}
//-------------------------------------------------------------------------
void vtkALBAFixTopologyTest::TestExecute()
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPoints> points;
  vtkALBASmartPointer<vtkCellArray> lines;
  vtkALBASmartPointer<vtkPolyData> polydata;

  // coordinates of vertices
   double vertices[7][3] ={
    {0,0,0},
    {0,-1,0},
    {0,-2,0},
    {1,0,0},
    {1,-1,0},
    {2,0,0},
    {2,-1,0},
  };

   // indices of simple cells
   vtkIdType cellids[5][4] = {
     {0,1,4},
     {0,3,4},
     {1,2,4},
     {1,2,4},
     {3,4,6,5},
   };

   // insert points
   for (int i = 0 ;  i < 7 ;  i++)
   {
     points->InsertNextPoint(vertices[i]) ;
   }

   // insert lines and polylines
   lines->InsertNextCell(3, cellids[0]);
   lines->InsertNextCell(3, cellids[1]);
   lines->InsertNextCell(3, cellids[2]);
   lines->InsertNextCell(3, cellids[3]);
   lines->InsertNextCell(4, cellids[4]);

   // put points and cells in polydata
   polydata->SetPoints(points);
   polydata->SetPolys(lines);

   vtkALBASmartPointer<vtkALBAFixTopology> fixTopology;
   fixTopology->SetInputData(polydata);
   fixTopology->Update();

   vtkPolyData *outputFixTopology = fixTopology->GetOutput();

   vtkALBASmartPointer<vtkTriangleFilter> triangleFilter;
   triangleFilter->SetInputData(polydata);
   triangleFilter->Update();
   vtkALBASmartPointer<vtkPolyDataNormals> polyDataNormalsFilter;
   polyDataNormalsFilter->SetInputConnection(triangleFilter->GetOutputPort());
   polyDataNormalsFilter->Update();
   vtkALBASmartPointer<vtkALBAPoissonSurfaceReconstruction> poissonSurfaceReconstructionFilter;
   poissonSurfaceReconstructionFilter->SetInputConnection(polyDataNormalsFilter->GetOutputPort());
   poissonSurfaceReconstructionFilter->Update();

   vtkPolyData *outputPipeLine = poissonSurfaceReconstructionFilter->GetOutput();

   //Check if the two results are equals
   CPPUNIT_ASSERT(outputPipeLine->GetNumberOfPoints() == outputFixTopology->GetNumberOfPoints());
   CPPUNIT_ASSERT(outputPipeLine->GetNumberOfCells() == outputFixTopology->GetNumberOfCells());

   //Check of the points
   for (int i=0;i<outputFixTopology->GetNumberOfPoints();i++)
   {
     double pt1[3],pt2[3];
     outputFixTopology->GetPoint(i,pt1);
     outputPipeLine->GetPoint(i,pt2);

     for (int j=0;j<3;j++)
     {
       CPPUNIT_ASSERT(  pt1[j] == pt2[j] );
     }
     
   }

   //Check of the cells
   for (int i=0;i<outputFixTopology->GetNumberOfCells();i++)
   { 
     vtkCell *cell1 = outputFixTopology->GetCell(i);
     vtkCell *cell2 = outputPipeLine->GetCell(i);

     CPPUNIT_ASSERT( cell1->GetNumberOfPoints() == cell2->GetNumberOfPoints() );

     vtkPoints *pts1 = cell1->GetPoints();
     vtkPoints *pts2 = cell2->GetPoints();

     for (int j=0;j<cell1->GetNumberOfPoints();j++)
     {
       double pt1[3],pt2[3];
       pts1->GetPoint(j,pt1);
       pts2->GetPoint(j,pt2);
       for (int k=0;k<3;k++)
       {
         CPPUNIT_ASSERT(  pt1[j] == pt2[j] );
       }
     }
   }
}
