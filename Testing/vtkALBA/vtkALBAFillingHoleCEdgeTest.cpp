/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFillingHoleCEdgeTest
 Authors: Eleonora Mambrini
 
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

#include <cppunit/config/SourcePrefix.h>
#include "albaString.h"
#include "vtkALBAFillingHole.h"
#include "vtkALBAFillingHoleCEdgeTest.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"

//-------------------------------------------------------------------------
void vtkALBAFillingHoleCEdgeTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
	vtkALBAFillingHole::CEdge *edge1 = new vtkALBAFillingHole::CEdge();
	vtkALBAFillingHole::CEdge *edge2 = new vtkALBAFillingHole::CEdge(0,1);
	vtkALBAFillingHole::CEdge *edge3 = new vtkALBAFillingHole::CEdge(0,1,2,3);

	delete edge1;
	delete edge2;
	delete edge3;
}
//-------------------------------------------------------------------------
void vtkALBAFillingHoleCEdgeTest::TestSetTriangle()
//-------------------------------------------------------------------------
{
	vtkALBAFillingHole::CEdge *edge = new vtkALBAFillingHole::CEdge();

	edge->SetTriangle(0,1);

	CPPUNIT_ASSERT(edge->ATriangle[0] == 0 && edge->ATriangle[1] == 1 );

	delete edge;

	/*int numOfTriangles;

	vtkALBAFillingHole::CTriangle triangle1 = vtkALBAFillingHole::CTriangle()

	double pCoord[3];
	pCoord[0] = pCoord[1] = pCoord[2] = 0;
	vtkALBAFillingHole::CEdge *Edge = new vtkALBAFillingHole::CEdge(pCoord);

	delete Edge;*/

}