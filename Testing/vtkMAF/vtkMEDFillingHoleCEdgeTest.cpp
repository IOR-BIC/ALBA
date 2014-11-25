/*=========================================================================

 Program: MAF2Medical
 Module: vtkMEDFillingHoleCEdgeTest
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
#include "vtkMEDFillingHole.h"
#include "vtkMEDFillingHoleCEdgeTest.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"

//-------------------------------------------------------------------------
void vtkMEDFillingHoleCEdgeTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
	vtkMEDFillingHole::CEdge *edge1 = new vtkMEDFillingHole::CEdge();
	vtkMEDFillingHole::CEdge *edge2 = new vtkMEDFillingHole::CEdge(0,1);
	vtkMEDFillingHole::CEdge *edge3 = new vtkMEDFillingHole::CEdge(0,1,2,3);

	delete edge1;
	delete edge2;
	delete edge3;
}
//-------------------------------------------------------------------------
void vtkMEDFillingHoleCEdgeTest::TestSetTriangle()
//-------------------------------------------------------------------------
{
	vtkMEDFillingHole::CEdge *edge = new vtkMEDFillingHole::CEdge();

	edge->SetTriangle(0,1);

	CPPUNIT_ASSERT(edge->ATriangle[0] == 0 && edge->ATriangle[1] == 1 );

	delete edge;

	/*int numOfTriangles;

	vtkMEDFillingHole::CTriangle triangle1 = vtkMEDFillingHole::CTriangle()

	double pCoord[3];
	pCoord[0] = pCoord[1] = pCoord[2] = 0;
	vtkMEDFillingHole::CEdge *Edge = new vtkMEDFillingHole::CEdge(pCoord);

	delete Edge;*/

}