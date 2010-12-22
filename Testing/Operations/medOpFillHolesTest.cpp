/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpFillHolesTest.cpp,v $
Language:  C++
Date:      $Date: 2010-12-22 10:22:40 $
Version:   $Revision: 1.1.2.3 $
Authors:   Simone Brazzale
==========================================================================
Copyright (c) 2007
CINECA - Interuniversity Consortium (www.cineca.it)
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
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

  mafString absPathFilename=MED_DATA_ROOT;
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