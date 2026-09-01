/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFillHolesTest
 Authors: Simone Brazzale
 
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

#include "albaOpFillHolesTest.h"
#include "albaOpFillHoles.h"

#include "albaVMESurface.h"
#include "albaOpImporterVTK.h"

#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"
#include "vtkFeatureEdges.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpFillHolesTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpFillHoles *fill=new albaOpFillHoles();
  albaDEL(fill);
}
//-----------------------------------------------------------
void albaOpFillHolesTest::TestCopy() 
//-----------------------------------------------------------
{
  albaOpFillHoles* fill = new albaOpFillHoles();
  albaOpFillHoles* fill_copy;

  fill_copy = (albaOpFillHoles*)fill->Copy();

  CPPUNIT_ASSERT(fill_copy->GetType()==OPTYPE_OP);

  albaDEL(fill_copy);
  albaDEL(fill);
}
//-----------------------------------------------------------
void albaOpFillHolesTest::TestAccept() 
//-----------------------------------------------------------
{
  albaVMESurface* surface;
  albaNEW(surface);

  albaOpFillHoles* fill = new albaOpFillHoles();
  fill->TestModeOn();
  CPPUNIT_ASSERT(fill->Accept(surface));

  albaDEL(fill);
  albaDEL(surface);
}
//-------------------------------------------------------------------------
void albaOpFillHolesTest::TestOpRun()
//-------------------------------------------------------------------------
{
  albaOpImporterVTK *importerVTK = new albaOpImporterVTK("importerVTK");
  importerVTK->TestModeOn();

  albaString absPathFilename=ALBA_DATA_ROOT;
  absPathFilename<<"/Test_vtkALBAFillingHole/SphereWithHoles.vtk";
  importerVTK->SetFileName(absPathFilename);
  importerVTK->OpRun();

  albaVMESurface *surface = albaVMESurface::SafeDownCast(importerVTK->GetOutput());
  CPPUNIT_ASSERT(surface!=NULL);

  vtkFeatureEdges* fEdge = vtkFeatureEdges::New();
  fEdge->SetInputData((vtkPolyData*)surface->GetOutput()->GetVTKData());
  fEdge->SetBoundaryEdges(true);
  fEdge->SetManifoldEdges(false);
  fEdge->SetNonManifoldEdges(false);
  fEdge->SetFeatureEdges(false);
  fEdge->Update();

  // Testing data has 2 holes with 16 boundary lines and 16 boundary points
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfLines() == 16 );
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfPoints() == 16 );

  albaOpFillHoles* fillOp = new albaOpFillHoles();
  fillOp->SetInput(surface);
  fillOp->TestModeOn();
  fillOp->OpRun();
  fillOp->OpDo();


  fEdge->SetInputData((vtkPolyData*)((albaVMESurface*)fillOp->GetInput())->GetOutput()->GetVTKData());
  fEdge->Update();

  // No operation has been performed (all functions are protected)
  // Testing data has 2 holes with 16 boundary lines and 16 boundary points
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfLines() == 16 );
  CPPUNIT_ASSERT( fEdge->GetOutput()->GetNumberOfPoints() == 16 );

  albaDEL(fillOp);
  vtkDEL(fEdge);
  albaDEL(importerVTK);
}