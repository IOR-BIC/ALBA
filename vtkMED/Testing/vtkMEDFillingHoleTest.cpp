/*=========================================================================

 Program: MAF2Medical
 Module: vtkMEDFillingHoleTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafString.h"
#include "vtkMEDFillingHole.h"
#include "vtkMEDFillingHoleTest.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"

//-------------------------------------------------------------------------
void vtkMEDFillingHoleTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMEDFillingHole> filter1;

  vtkMEDFillingHole *filter2;
  vtkNEW(filter2);

  vtkDEL(filter2);
}
//-------------------------------------------------------------------------
void vtkMEDFillingHoleTest::TestSetFillAHole()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyDataReader> r;
  mafString filename=MED_DATA_ROOT;
  filename<<"/Tes_vtkMEDFillingHole/SphereWithHoles.vtk";
  r->SetFileName(filename.GetCStr());
  r->Update();

  vtkMAFSmartPointer<vtkFeatureEdges> fEdge;
  fEdge->SetInput(r->GetOutput());
  fEdge->SetBoundaryEdges(TRUE);
  fEdge->SetManifoldEdges(FALSE);
  fEdge->SetNonManifoldEdges(FALSE);
  fEdge->SetFeatureEdges(FALSE);
  fEdge->Update();

  //Testing data has 2 holes with 16 boundary lines and 16 boundary points
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfLines() == 16 );
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfPoints() == 16 );

  vtkMAFSmartPointer<vtkMEDFillingHole> filter;
  filter->SetInput(r->GetOutput());
  filter->SetFillAHole(10);
  filter->Update();

  fEdge->SetInput(filter->GetOutput());
  fEdge->Update();

  //The Id of the boundary point set in the filter method isn't an Id of a boundary point of an edge
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfLines() == 16 );
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfPoints() == 16 );

  filter->SetFillAHole(0);
  filter->Update();

  fEdge->SetInput(filter->GetOutput());
  fEdge->Update();

  //The Id of the boundary point set in the filter method is an Id of a boundary point of an edge
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfLines() == 6 );
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfPoints() == 6 );

}
//-------------------------------------------------------------------------
void vtkMEDFillingHoleTest::TestSetFillAllHole()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyDataReader> r;
  mafString filename=MED_DATA_ROOT;
  filename<<"/Tes_vtkMEDFillingHole/SphereWithHoles.vtk";
  r->SetFileName(filename.GetCStr());
  r->Update();

  vtkMAFSmartPointer<vtkFeatureEdges> fEdge;
  fEdge->SetInput(r->GetOutput());
  fEdge->SetBoundaryEdges(TRUE);
  fEdge->SetManifoldEdges(FALSE);
  fEdge->SetNonManifoldEdges(FALSE);
  fEdge->SetFeatureEdges(FALSE);
  fEdge->Update();

  //Testing data has 2 holes with 16 boundary lines and 16 boundary points
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfLines() == 16 );
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfPoints() == 16 );

  vtkMAFSmartPointer<vtkMEDFillingHole> filter;
  filter->SetInput(r->GetOutput());
  filter->SetFillAllHole();
  filter->Update();

  fEdge->SetInput(filter->GetOutput());
  fEdge->Update();

  //The Id of the boundary point set in the filter method isn't an Id of a boundary point of an edge
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfLines() == 0 );

}
//-------------------------------------------------------------------------
void vtkMEDFillingHoleTest::TestGetLastPatch()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyDataReader> r;
  mafString filename=MED_DATA_ROOT;
  filename<<"/Tes_vtkMEDFillingHole/SphereWithHoles.vtk";
  r->SetFileName(filename.GetCStr());
  r->Update();

  vtkMAFSmartPointer<vtkFeatureEdges> fEdge;
  fEdge->SetInput(r->GetOutput());
  fEdge->SetBoundaryEdges(TRUE);
  fEdge->SetManifoldEdges(FALSE);
  fEdge->SetNonManifoldEdges(FALSE);
  fEdge->SetFeatureEdges(FALSE);
  fEdge->Update();

  //Testing data has 2 holes with 16 boundary lines and 16 boundary points
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfLines() == 16 );
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfPoints() == 16 );

  vtkMAFSmartPointer<vtkMEDFillingHole> filter;
  filter->SetInput(r->GetOutput());
  filter->SetFillAHole(0);
  filter->Update();

  vtkPolyData *patch = filter->GetLastPatch();
  patch->Update();

  vtkPolyData *closedSphere = filter->GetOutput();
  closedSphere->Update();

  vtkMAFSmartPointer<vtkAppendPolyData> append;
  append->SetInput(r->GetOutput());
  append->AddInput(patch);
  append->Update();

  vtkMAFSmartPointer<vtkCleanPolyData> clean;
  clean->SetInput(append->GetOutput());
  clean->Update();

  CPPUNIT_ASSERT( clean->GetOutput()->GetNumberOfPoints() == filter->GetOutput()->GetNumberOfPoints() );
  CPPUNIT_ASSERT( clean->GetOutput()->GetNumberOfCells() == filter->GetOutput()->GetNumberOfCells() );

  for (int i=0;i<clean->GetOutput()->GetNumberOfPoints();i++)
  {
    double pt1[3],pt2[3];
    clean->GetOutput()->GetPoint(i,pt1);
    filter->GetOutput()->GetPoint(i,pt2);

    CPPUNIT_ASSERT(pt1[0] == pt2[0] && pt1[1] == pt2[1] && pt1[2] == pt2[2]);
  }
}
