/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFillingHoleCTriangleTest
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
#include "vtkALBAFillingHoleCTriangleTest.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"

//-------------------------------------------------------------------------
void vtkALBAFillingHoleCTriangleTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
	vtkALBAFillingHole::CTriangle *triangle = new vtkALBAFillingHole::CTriangle();
	vtkALBAFillingHole::CTriangle *triangle2 = new vtkALBAFillingHole::CTriangle(0,0,0);

	delete triangle;
	delete triangle2;
}
//-------------------------------------------------------------------------
void vtkALBAFillingHoleCTriangleTest::TestSetEdge()
//-------------------------------------------------------------------------
{
	vtkALBAFillingHole::CTriangle *triangle = new vtkALBAFillingHole::CTriangle();

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