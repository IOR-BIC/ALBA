/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpCreateEditSkeletonTest.cpp,v $
Language:  C++
Date:      $Date: 2011-01-28 12:04:19 $
Version:   $Revision: 1.1.2.1 $
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

#include "medOpCreateEditSkeletonTest.h"
#include "medOpCreateEditSkeleton.h"
#include "medOpImporterVTK.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"
#include "medVMEPolylineGraph.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpCreateEditSkeletonTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpCreateEditSkeleton *create=new medOpCreateEditSkeleton();
  mafDEL(create);
}
//-----------------------------------------------------------
void medOpCreateEditSkeletonTest::TestAccept() 
//-----------------------------------------------------------
{
  medOpCreateEditSkeleton *op=new medOpCreateEditSkeleton();
  mafVMEVolumeGray *volume;
  medVMEPolylineGraph* graph;
  mafNEW(volume);
  mafNEW(graph);
  graph->ReparentTo(volume);
  
  CPPUNIT_ASSERT(op->Accept(volume));
  CPPUNIT_ASSERT(op->Accept(graph));
  
  mafDEL(graph);
  mafDEL(volume);
  mafDEL(op);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpCreateEditSkeletonTest::TestOpRun() 
//-----------------------------------------------------------
{
  // import VTK  
  medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
  importer->TestModeOn();
  mafString fileName=MED_DATA_ROOT;
  fileName<<"/PolylineGraph/PolylineGraph.vtk";
  importer->SetFileName(fileName);
  importer->ImportVTK();
  medVMEPolylineGraph *graph=medVMEPolylineGraph::SafeDownCast(importer->GetOutput());
  graph->GetOutput()->GetVTKData()->Update();
  
  CPPUNIT_ASSERT(graph!=NULL);
  CPPUNIT_ASSERT(graph->GetOutput()->GetVTKData()!=NULL);

  int nOfPoints_before = graph->GetOutput()->GetVTKData()->GetNumberOfPoints();

  mafVMEVolumeGray *volume;
  mafNEW(volume);
  graph->ReparentTo(volume);

  // TEST OP ON POLYLINE GRAPH
  medOpCreateEditSkeleton *create=new medOpCreateEditSkeleton();
  create->TestModeOn();
  create->SetInput(graph);
  create->OpRun();
  create->OpDo();

  medVMEPolylineGraph* output = (medVMEPolylineGraph*) create->GetInput();
  int nOfPoints_after = output->GetOutput()->GetVTKData()->GetNumberOfPoints();

  // op does nothing: input must be same output.
  CPPUNIT_ASSERT(output!=NULL);
  CPPUNIT_ASSERT(nOfPoints_after==nOfPoints_before);

  // TEST OP ON VOLUME
  medOpCreateEditSkeleton *recreate=new medOpCreateEditSkeleton();
  recreate->TestModeOn();
  recreate->SetInput(volume);
  recreate->OpRun();
  recreate->OpDo();

  output = (medVMEPolylineGraph*) recreate->GetInput()->GetChild(0);
  nOfPoints_after =  output->GetOutput()->GetVTKData()->GetNumberOfPoints();

  // op does nothing: input must be same output.
  CPPUNIT_ASSERT(output!=NULL);
  CPPUNIT_ASSERT(nOfPoints_after==nOfPoints_before);

  mafDEL(recreate);
  mafDEL(create);
  mafDEL(volume);
	mafDEL(importer);
}
