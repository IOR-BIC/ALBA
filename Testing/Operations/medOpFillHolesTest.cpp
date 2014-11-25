/*=========================================================================

 Program: MAF2Medical
 Module: medOpFillHolesTest
 Authors: Simone Brazzale
 
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

#include "medOpFillHolesTest.h"
#include "medOpFillHoles.h"

#include "mafVMESurface.h"
#include "medOpImporterVTK.h"

#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"
#include "vtkFeatureEdges.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpFillHolesTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpFillHoles *fill=new medOpFillHoles();
  mafDEL(fill);
}
//-----------------------------------------------------------
void medOpFillHolesTest::TestCopy() 
//-----------------------------------------------------------
{
  medOpFillHoles* fill = new medOpFillHoles();
  medOpFillHoles* fill_copy;

  fill_copy = (medOpFillHoles*)fill->Copy();

  CPPUNIT_ASSERT(fill_copy->GetType()==OPTYPE_OP);

  mafDEL(fill_copy);
  mafDEL(fill);
}
//-----------------------------------------------------------
void medOpFillHolesTest::TestAccept() 
//-----------------------------------------------------------
{
  mafVMESurface* surface;
  mafNEW(surface);

  medOpFillHoles* fill = new medOpFillHoles();
  fill->TestModeOn();
  CPPUNIT_ASSERT(fill->Accept(surface));

  mafDEL(fill);
  mafDEL(surface);
}
//-------------------------------------------------------------------------
void medOpFillHolesTest::TestOpRun()
//-------------------------------------------------------------------------
{
  medOpImporterVTK *importerVTK = new medOpImporterVTK("importerVTK");
  importerVTK->TestModeOn();

  mafString absPathFilename=MAF_DATA_ROOT;
  absPathFilename<<"/Tes_vtkMEDFillingHole/SphereWithHoles.vtk";
  importerVTK->SetFileName(absPathFilename);
  importerVTK->OpRun();

  mafVMESurface *surface = mafVMESurface::SafeDownCast(importerVTK->GetOutput());
  CPPUNIT_ASSERT(surface!=NULL);

  vtkFeatureEdges* fEdge = vtkFeatureEdges::New();
  fEdge->SetInput((vtkPolyData*)surface->GetOutput()->GetVTKData());
  fEdge->SetBoundaryEdges(TRUE);
  fEdge->SetManifoldEdges(FALSE);
  fEdge->SetNonManifoldEdges(FALSE);
  fEdge->SetFeatureEdges(FALSE);
  fEdge->Update();

  // Testing data has 2 holes with 16 boundary lines and 16 boundary points
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfLines() == 16 );
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfPoints() == 16 );

  medOpFillHoles* fillOp = new medOpFillHoles();
  fillOp->SetInput(surface);
  fillOp->TestModeOn();
  fillOp->OpRun();
  fillOp->OpDo();

  ((mafVMESurface*)fillOp->GetInput())->GetOutput()->GetVTKData()->Update();

  fEdge->SetInput((vtkPolyData*)((mafVMESurface*)fillOp->GetInput())->GetOutput()->GetVTKData());
  fEdge->Update();

  // No operation has been performed (all functions are protected)
  // Testing data has 2 holes with 16 boundary lines and 16 boundary points
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfLines() == 16 );
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfPoints() == 16 );

  mafDEL(fillOp);
  vtkDEL(fEdge);
  mafDEL(importerVTK);

  delete wxLog::SetActiveTarget(NULL);
}