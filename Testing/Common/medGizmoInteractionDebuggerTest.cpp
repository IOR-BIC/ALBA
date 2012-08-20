/*=========================================================================

 Program: MAF2Medical
 Module: medGizmoInteractionDebuggerTest
 Authors: Stefano Perticoni
 
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

#include "medGizmoInteractionDebuggerTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include "mafGizmoSlice.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "medGizmoInteractionDebugger.h"
#include "medVMEPolylineGraph.h"
#include "vtkMAFSmartPointer.h"
#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellarray.h"
#include "vtkAxes.h"
#include "vtkTubeFilter.h"

void medGizmoInteractionDebuggerTest::setUp()
{
	//// Create the polyline graph constrain

	m_PolylineGraph =NULL;
	mafNEW(m_PolylineGraph);

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

	m_PolylineGraph->SetDataByDetaching(polyline,-1);

	vtkDEL(polyline);
	vtkDEL(pts_arr);
	vtkDEL(pts);
	vtkDEL(lines);
	
	//// Create the dummy input vme


	m_Root = NULL;
	m_GizmoInputSurface = NULL;

	int returnValue = -1;

	CPPUNIT_ASSERT(m_GizmoInputSurface == NULL);
	mafNEW(m_GizmoInputSurface);

	vtkMAFSmartPointer<vtkAxes> axes;
	axes->SetScaleFactor(2.5);

	vtkMAFSmartPointer<vtkTubeFilter> tube;
	tube->SetInput(axes->GetOutput());
	tube->SetRadius(0.5);
	tube->SetNumberOfSides(20);

	m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,mafVMEGeneric::MAF_VME_REFERENCE_DATA);

	CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


	mafNEW(m_Root);
	m_GizmoInputSurface->ReparentTo(m_Root);

	CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);

}

void medGizmoInteractionDebuggerTest::tearDown()
{
	mafDEL(m_PolylineGraph);
	mafDEL(m_GizmoInputSurface);
	mafDEL(m_Root); 

}

void medGizmoInteractionDebuggerTest::TestFixture()
{

}


void medGizmoInteractionDebuggerTest::TestConstructorDestructor()
{
	medGizmoInteractionDebugger *gizmoInteractioDebugger = NULL;
	gizmoInteractioDebugger = new medGizmoInteractionDebugger(m_GizmoInputSurface, NULL, "dummy", true);

	CPPUNIT_ASSERT(gizmoInteractioDebugger);
	cppDEL(gizmoInteractioDebugger);
}



