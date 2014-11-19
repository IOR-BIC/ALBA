/*=========================================================================

 Program: MAF2Medical
 Module: medOpCreateEditSkeletonTest
 Authors: Simone Brazzale
 
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
  mafString fileName=MAF_DATA_ROOT;
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
