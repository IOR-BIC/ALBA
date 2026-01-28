/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateEditSkeletonTest
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

#include "albaOpCreateEditSkeletonTest.h"
#include "albaOpCreateEditSkeleton.h"
#include "albaOpImporterVTK.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEPolylineGraph.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"

#include <string>
#include <assert.h>
#include "albaVMEPolyline.h"

//-----------------------------------------------------------
void albaOpCreateEditSkeletonTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpCreateEditSkeleton *create=new albaOpCreateEditSkeleton();
  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpCreateEditSkeletonTest::TestAccept() 
//-----------------------------------------------------------
{
  albaOpCreateEditSkeleton *op=new albaOpCreateEditSkeleton();
  albaVMEVolumeGray *volume;
  albaVMEPolylineGraph* graph;
  albaNEW(volume);
  albaNEW(graph);
  graph->ReparentTo(volume);
  
  CPPUNIT_ASSERT(op->Accept(volume));
  CPPUNIT_ASSERT(op->Accept(graph));
  
  albaDEL(graph);
  albaDEL(volume);
  albaDEL(op);
}
//-----------------------------------------------------------
void albaOpCreateEditSkeletonTest::TestOpRun() 
//-----------------------------------------------------------
{
  // import VTK  
  albaOpImporterVTK *importer=new albaOpImporterVTK("importerVTK");
  importer->TestModeOn();
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/PolylineGraph/PolylineGraph.vtk";
  importer->SetFileName(fileName);
  importer->ImportFile();
  albaVMEPolyline *polyline=albaVMEPolyline::SafeDownCast(importer->GetOutput());
	polyline->Update();
	polyline->GetOutput()->Update();

	albaVMEPolylineGraph *graph;
	albaNEW(graph);
	graph->SetData((vtkPolyData*)polyline->GetOutput()->GetVTKData(), 0);

  graph->GetOutput()->GetVTKData()->Update();
  
  CPPUNIT_ASSERT(graph!=NULL);
  CPPUNIT_ASSERT(graph->GetOutput()->GetVTKData()!=NULL);

  int nOfPoints_before = graph->GetOutput()->GetVTKData()->GetNumberOfPoints();

  albaVMEVolumeGray *volume;
  albaNEW(volume);
  graph->ReparentTo(volume);

  // TEST OP ON POLYLINE GRAPH
  albaOpCreateEditSkeleton *create=new albaOpCreateEditSkeleton();
  create->TestModeOn();
  create->SetInput(graph);
  create->OpRun();
  create->OpDo();

  albaVMEPolylineGraph* output = (albaVMEPolylineGraph*) create->GetInput();
  int nOfPoints_after = output->GetOutput()->GetVTKData()->GetNumberOfPoints();

  // op does nothing: input must be same output.
  CPPUNIT_ASSERT(output!=NULL);
  CPPUNIT_ASSERT(nOfPoints_after==nOfPoints_before);

  // TEST OP ON VOLUME
  albaOpCreateEditSkeleton *recreate=new albaOpCreateEditSkeleton();
  recreate->TestModeOn();
  recreate->SetInput(volume);
  recreate->OpRun();
  recreate->OpDo();

  output = (albaVMEPolylineGraph*) recreate->GetInput()->GetChild(0);
  nOfPoints_after =  output->GetOutput()->GetVTKData()->GetNumberOfPoints();

  // op does nothing: input must be same output.
  CPPUNIT_ASSERT(output!=NULL);
  CPPUNIT_ASSERT(nOfPoints_after==nOfPoints_before);

  albaDEL(recreate);
  albaDEL(create);
  albaDEL(volume);
	albaDEL(importer);
	albaDEL(graph);
}
