/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medCurvilinearAbscissaOnSkeletonHelperTest.cpp,v $
Language:  C++
Date:      $Date: 2010-11-24 13:43:13 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
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