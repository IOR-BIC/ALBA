/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoInteractionDebuggerTest
 Authors: Stefano Perticoni
 
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

#include "albaGizmoInteractionDebuggerTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include "albaGizmoSlice.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "albaGizmoInteractionDebugger.h"
#include "albaVMEPolylineGraph.h"
#include "vtkALBASmartPointer.h"
#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellarray.h"
#include "vtkAxes.h"
#include "vtkTubeFilter.h"

void albaGizmoInteractionDebuggerTest::BeforeTest()
{
	//// Create the polyline graph constrain
	m_PolylineGraph =NULL;
	albaNEW(m_PolylineGraph);

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
	albaNEW(m_GizmoInputSurface);

	vtkALBASmartPointer<vtkAxes> axes;
	axes->SetScaleFactor(2.5);

	vtkALBASmartPointer<vtkTubeFilter> tube;
	tube->SetInputConnection(axes->GetOutputPort());
	tube->SetRadius(0.5);
	tube->SetNumberOfSides(20);
	tube->Update();

	m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);

	CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


	albaNEW(m_Root);
	m_GizmoInputSurface->ReparentTo(m_Root);

	CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);

}

void albaGizmoInteractionDebuggerTest::AfterTest()
{
	albaDEL(m_PolylineGraph);
	albaDEL(m_GizmoInputSurface);
	albaDEL(m_Root); 
}

void albaGizmoInteractionDebuggerTest::TestFixture()
{

}


void albaGizmoInteractionDebuggerTest::TestConstructorDestructor()
{
	albaGizmoInteractionDebugger *gizmoInteractioDebugger = NULL;
	gizmoInteractioDebugger = new albaGizmoInteractionDebugger(m_GizmoInputSurface, NULL, "dummy", true);

	CPPUNIT_ASSERT(gizmoInteractioDebugger);
	cppDEL(gizmoInteractioDebugger);
}



