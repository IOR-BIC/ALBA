/*=========================================================================

 Program: MAF2Medical
 Module: medCurvilinearAbscissaOnSkeletonHelperTest
 Authors: Daniele Giunchi
 
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

#include "vtkSystemIncludes.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkDoubleArray.h"
#include "vtkCellArray.h"

#include "medCurvilinearAbscissaOnSkeletonHelperTest.h"
#include "medVMEPolylineGraph.h"
#include "mafEvent.h"
#include "mafVME.h"
#include "medCurvilinearAbscissaOnSkeletonHelper.h"

//----------------------------------------------------------------------------
void medCurvilinearAbscissaOnSkeletonHelperTest::setUp()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
void medCurvilinearAbscissaOnSkeletonHelperTest::tearDown()
//----------------------------------------------------------------------------
{
	delete wxLog::SetActiveTarget(NULL);
}

//----------------------------------------------------------------------------
void medCurvilinearAbscissaOnSkeletonHelperTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	medCurvilinearAbscissaOnSkeletonHelper *helper = new medCurvilinearAbscissaOnSkeletonHelper(NULL,NULL,true);
	cppDEL(helper);
}
//----------------------------------------------------------------------------
void medCurvilinearAbscissaOnSkeletonHelperTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	//medCurvilinearAbscissaOnSkeletonHelper helper;
}
//----------------------------------------------------------------------------
void medCurvilinearAbscissaOnSkeletonHelperTest::TestSetGetConstraintPolylineGraph()
//----------------------------------------------------------------------------
{
  medCurvilinearAbscissaOnSkeletonHelper *helper = new medCurvilinearAbscissaOnSkeletonHelper(NULL,NULL,true);

  //// Create and set the polyline graph
  medVMEPolylineGraph *input;
  mafNEW(input);

  vtkPolyData *polyline;
  vtkDoubleArray *pts_arr;
  vtkPoints *pts;
  vtkCellArray *lines;

  vtkNEW(polyline);
  vtkNEW(pts_arr);
  vtkNEW(pts);
  vtkNEW(lines);

  pts_arr->SetNumberOfComponents(3);
  pts_arr->SetNumberOfTuples(3);
  pts_arr->InsertNextTuple3(1,1,1);
  pts_arr->InsertNextTuple3(0,0,0);
  pts_arr->InsertNextTuple3(-1,-1,-1);
  pts->SetData(pts_arr);

  lines->InsertNextCell(2);
  lines->InsertCellPoint(0);
  lines->InsertCellPoint(1);
  lines->InsertNextCell(2);
  lines->InsertCellPoint(1);
  lines->InsertCellPoint(2);

  polyline->SetPoints(pts);
  polyline->SetLines(lines);

  input->SetDataByDetaching(polyline,-1);

  vtkDEL(polyline);
  vtkDEL(pts_arr);
  vtkDEL(pts);
  vtkDEL(lines);
  ////

  helper->SetConstraintPolylineGraph(input);
  CPPUNIT_ASSERT(helper->GetSetConstraintPolylineGraph() == input);
  mafDEL(input);
  cppDEL(helper);
}
//----------------------------------------------------------------------------
void medCurvilinearAbscissaOnSkeletonHelperTest::SetGetCurvilinearAbscissa()
//----------------------------------------------------------------------------
{
  mafVMEGeneric *input_vme;
  mafNEW(input_vme);

  medCurvilinearAbscissaOnSkeletonHelper *helper = new medCurvilinearAbscissaOnSkeletonHelper(input_vme,NULL,true);

  //// Create and set the polyline graph
  medVMEPolylineGraph *input;
  mafNEW(input);

  vtkPolyData *polyline;
  vtkDoubleArray *pts_arr;
  vtkPoints *pts;
  vtkCellArray *lines;

  vtkNEW(polyline);
  vtkNEW(pts_arr);
  vtkNEW(pts);
  vtkNEW(lines);

  pts_arr->SetNumberOfComponents(3);
  pts_arr->SetNumberOfTuples(3);
  pts_arr->InsertNextTuple3(1,1,1);
  pts_arr->InsertNextTuple3(0,0,0);
  pts_arr->InsertNextTuple3(-1,-1,-1);
  pts->SetData(pts_arr);
  
  lines->InsertNextCell(2);
  lines->InsertCellPoint(0);
  lines->InsertCellPoint(1);
  lines->InsertNextCell(2);
  lines->InsertCellPoint(1);
  lines->InsertCellPoint(2);

  polyline->SetPoints(pts);
  polyline->SetLines(lines);

  input->SetDataByDetaching(polyline,0);

  vtkDEL(polyline);
  vtkDEL(pts_arr);
  vtkDEL(pts);
  vtkDEL(lines);
  ////
  helper->SetConstraintPolylineGraph(input);
  helper->SetCurvilinearAbscissa(0,1.);
  CPPUNIT_ASSERT(helper->GetCurvilinearAbscissa() == 0);

  mafDEL(input);
  cppDEL(helper);
  mafDEL(input_vme);
}
//----------------------------------------------------------------------------
void medCurvilinearAbscissaOnSkeletonHelperTest::SetGetActiveBranchId()
//----------------------------------------------------------------------------
{
  mafVMEGeneric *input_vme;
  mafNEW(input_vme);

  medCurvilinearAbscissaOnSkeletonHelper *helper = new medCurvilinearAbscissaOnSkeletonHelper(input_vme,NULL,true);

  //// Create and set the polyline graph
  medVMEPolylineGraph *input;
  mafNEW(input);

  vtkPolyData *polyline;
  vtkDoubleArray *pts_arr;
  vtkPoints *pts;
  vtkCellArray *lines;

  vtkNEW(polyline);
  vtkNEW(pts_arr);
  vtkNEW(pts);
  vtkNEW(lines);

  pts_arr->SetNumberOfComponents(3);
  pts_arr->SetNumberOfTuples(3);
  pts_arr->InsertNextTuple3(1,1,1);
  pts_arr->InsertNextTuple3(0,0,0);
  pts_arr->InsertNextTuple3(-1,-1,-1);
  pts->SetData(pts_arr);

  lines->InsertNextCell(2);
  lines->InsertCellPoint(0);
  lines->InsertCellPoint(1);
  lines->InsertNextCell(2);
  lines->InsertCellPoint(1);
  lines->InsertCellPoint(2);

  polyline->SetPoints(pts);
  polyline->SetLines(lines);

  input->SetDataByDetaching(polyline,0);

  vtkDEL(polyline);
  vtkDEL(pts_arr);
  vtkDEL(pts);
  vtkDEL(lines);
  ////

  helper->SetConstraintPolylineGraph(input);
  helper->SetCurvilinearAbscissa(0,0);
  CPPUNIT_ASSERT(helper->GetActiveBranchId() == 0);

  mafDEL(input);
  cppDEL(helper);
  mafDEL(input_vme);
}
//----------------------------------------------------------------------------
void medCurvilinearAbscissaOnSkeletonHelperTest::TestMoveOnSkeleton()
//----------------------------------------------------------------------------
{
  mafVMEGeneric *input_vme;
  mafNEW(input_vme);

  medCurvilinearAbscissaOnSkeletonHelper *helper = new medCurvilinearAbscissaOnSkeletonHelper(input_vme,NULL,true);

  //// Create and set the polyline graph
  medVMEPolylineGraph *input;
  mafNEW(input);

  vtkPolyData *polyline;
  vtkDoubleArray *pts_arr;
  vtkPoints *pts;
  vtkCellArray *lines;

  vtkNEW(polyline);
  vtkNEW(pts_arr);
  vtkNEW(pts);
  vtkNEW(lines);

  pts_arr->SetNumberOfComponents(3);
  pts_arr->SetNumberOfTuples(3);
  pts_arr->InsertNextTuple3(1,1,1);
  pts_arr->InsertNextTuple3(0,0,0);
  pts_arr->InsertNextTuple3(-1,-1,-1);
  pts->SetData(pts_arr);

  lines->InsertNextCell(2);
  lines->InsertCellPoint(0);
  lines->InsertCellPoint(1);
  lines->InsertNextCell(2);
  lines->InsertCellPoint(1);
  lines->InsertCellPoint(2);

  polyline->SetPoints(pts);
  polyline->SetLines(lines);

  input->SetDataByDetaching(polyline,0);

  vtkDEL(polyline);
  vtkDEL(pts_arr);
  vtkDEL(pts);
  vtkDEL(lines);
  ////

  helper->SetConstraintPolylineGraph(input);
  helper->SetCurvilinearAbscissa(0,0);
  
  mafMatrix *m = new mafMatrix();
  m->Identity();
  mafEvent *e = new mafEvent(this,ID_TRANSFORM,m);


  CPPUNIT_ASSERT(helper->GetCurvilinearAbscissa() == 0);

  cppDEL(m);
  cppDEL(e);

  mafDEL(input);
  cppDEL(helper);
  mafDEL(input_vme);
}