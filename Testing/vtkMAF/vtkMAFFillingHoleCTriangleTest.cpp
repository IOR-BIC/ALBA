/*=========================================================================

 Program: MAF2
 Module: vtkMAFFillingHoleCTriangleTest
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
#include "vtkMAFFillingHoleCTriangleTest.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"

//-------------------------------------------------------------------------
void vtkMAFFillingHoleCTriangleTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
	vtkMAFFillingHole::CTriangle *triangle = new vtkMAFFillingHole::CTriangle();
	vtkMAFFillingHole::CTriangle *triangle2 = new vtkMAFFillingHole::CTriangle(0,0,0);

	delete triangle;
	delete triangle2;
}
//-------------------------------------------------------------------------
void vtkMAFFillingHoleCTriangleTest::TestSetEdge()
//-------------------------------------------------------------------------
{
	vtkMAFFillingHole::CTriangle *triangle = new vtkMAFFillingHole::CTriangle();

	int edge0, edge1, edge2;

	edge0 = 0;
	edge1 = 1;
	edge2 = 2;
	
	triangle->SetEdge(edge0, edge1, edge2);
	CPPUNIT_ASSERT( triangle->AEdge[0] == edge0 );
	CPPUNIT_ASSERT( triangle->AEdge[1] == edge1 );
	CPPUNIT_ASSERT( triangle->AEdge[2] == edge2 );

	delete triangle;

}