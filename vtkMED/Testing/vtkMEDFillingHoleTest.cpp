/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDFillingHoleTest.cpp,v $
Language:  C++
Date:      $Date: 2010-06-14 12:25:05 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
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
