/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCurvilinearAbscissaOnSkeletonHelperTest
 Authors: Daniele Giunchi
 
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

#include "vtkSystemIncludes.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkDoubleArray.h"
#include "vtkCellArray.h"

#include "albaCurvilinearAbscissaOnSkeletonHelperTest.h"
#include "albaVMEPolylineGraph.h"
#include "albaEvent.h"
#include "albaVME.h"
#include "albaCurvilinearAbscissaOnSkeletonHelper.h"

//----------------------------------------------------------------------------
void albaCurvilinearAbscissaOnSkeletonHelperTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	albaCurvilinearAbscissaOnSkeletonHelper *helper = new albaCurvilinearAbscissaOnSkeletonHelper(NULL,NULL,true);
	cppDEL(helper);
}
//----------------------------------------------------------------------------
void albaCurvilinearAbscissaOnSkeletonHelperTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	//albaCurvilinearAbscissaOnSkeletonHelper helper;
}
//----------------------------------------------------------------------------
void albaCurvilinearAbscissaOnSkeletonHelperTest::TestSetGetConstraintPolylineGraph()
//----------------------------------------------------------------------------
{
  albaCurvilinearAbscissaOnSkeletonHelper *helper = new albaCurvilinearAbscissaOnSkeletonHelper(NULL,NULL,true);

  //// Create and set the polyline graph
  albaVMEPolylineGraph *input;
  albaNEW(input);

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
  albaDEL(input);
  cppDEL(helper);
}
//----------------------------------------------------------------------------
void albaCurvilinearAbscissaOnSkeletonHelperTest::SetGetCurvilinearAbscissa()
//----------------------------------------------------------------------------
{
  albaVMEGeneric *input_vme;
  albaNEW(input_vme);

  albaCurvilinearAbscissaOnSkeletonHelper *helper = new albaCurvilinearAbscissaOnSkeletonHelper(input_vme,NULL,true);

  //// Create and set the polyline graph
  albaVMEPolylineGraph *input;
  albaNEW(input);

  vtkPolyData *polyline;
  vtkDoubleArray *pts_arr;
  vtkPoints *pts;
  vtkCellArray *lines;

  vtkNEW(polyline);
  vtkNEW(pts_arr);
  vtkNEW(pts);
  vtkNEW(lines);

  pts_arr->SetNumberOfComponents(3);
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
  CPPUNIT_ASSERT(helper->GetCurvilinearAbscissa() == 1);

  albaDEL(input);
  cppDEL(helper);
  albaDEL(input_vme);
}
//----------------------------------------------------------------------------
void albaCurvilinearAbscissaOnSkeletonHelperTest::SetGetActiveBranchId()
//----------------------------------------------------------------------------
{
  albaVMEGeneric *input_vme;
  albaNEW(input_vme);

  albaCurvilinearAbscissaOnSkeletonHelper *helper = new albaCurvilinearAbscissaOnSkeletonHelper(input_vme,NULL,true);

  //// Create and set the polyline graph
  albaVMEPolylineGraph *input;
  albaNEW(input);

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

  albaDEL(input);
  cppDEL(helper);
  albaDEL(input_vme);
}
//----------------------------------------------------------------------------
void albaCurvilinearAbscissaOnSkeletonHelperTest::TestMoveOnSkeleton()
//----------------------------------------------------------------------------
{
  albaVMEGeneric *input_vme;
  albaNEW(input_vme);

  albaCurvilinearAbscissaOnSkeletonHelper *helper = new albaCurvilinearAbscissaOnSkeletonHelper(input_vme,NULL,true);

  //// Create and set the polyline graph
  albaVMEPolylineGraph *input;
  albaNEW(input);

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
  
  albaMatrix *m = new albaMatrix();
  m->Identity();
  albaEvent *e = new albaEvent(this,ID_TRANSFORM,m);


  CPPUNIT_ASSERT(helper->GetCurvilinearAbscissa() == 0);

  cppDEL(m);
  cppDEL(e);

  albaDEL(input);
  cppDEL(helper);
  albaDEL(input_vme);
}