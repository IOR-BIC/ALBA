/*=========================================================================

 Program: MAF2
 Module: vtkMAFFillingHoleCEdgeTest
 Authors: Eleonora Mambrini
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafString.h"
#include "vtkMAFFillingHole.h"
#include "vtkMAFFillingHoleCEdgeTest.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"

//-------------------------------------------------------------------------
void vtkMAFFillingHoleCEdgeTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
	vtkMAFFillingHole::CEdge *edge1 = new vtkMAFFillingHole::CEdge();
	vtkMAFFillingHole::CEdge *edge2 = new vtkMAFFillingHole::CEdge(0,1);
	vtkMAFFillingHole::CEdge *edge3 = new vtkMAFFillingHole::CEdge(0,1,2,3);

	delete edge1;
	delete edge2;
	delete edge3;
}
//-------------------------------------------------------------------------
void vtkMAFFillingHoleCEdgeTest::TestSetTriangle()
//-------------------------------------------------------------------------
{
	vtkMAFFillingHole::CEdge *edge = new vtkMAFFillingHole::CEdge();

	edge->SetTriangle(0,1);

	CPPUNIT_ASSERT(edge->ATriangle[0] == 0 && edge->ATriangle[1] == 1 );

	delete edge;

	/*int numOfTriangles;

	vtkMAFFillingHole::CTriangle triangle1 = vtkMAFFillingHole::CTriangle()

	double pCoord[3];
	pCoord[0] = pCoord[1] = pCoord[2] = 0;
	vtkMAFFillingHole::CEdge *Edge = new vtkMAFFillingHole::CEdge(pCoord);

	delete Edge;*/

}