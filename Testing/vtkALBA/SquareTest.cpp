/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: SquareTest
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

#include <cppunit/config/SourcePrefix.h>
#include "SquareTest.h"

#include "vtkALBAPoissonSurfaceReconstruction.h"

//-------------------------------------------------------------------------
void SquareTest::TestCornerIndex()
//-------------------------------------------------------------------------
{
	int counter = 0;
	for(int i=0;i<2;i++){
		for(int j=0;j<2;j++){
			int idx=Square::CornerIndex(i,j);
			switch(counter)
			{
			case 0:
				CPPUNIT_ASSERT(idx == 0);
				break;
			case 1:
				CPPUNIT_ASSERT(idx == 2);
				break;
			case 2:
				CPPUNIT_ASSERT(idx == 1);
				break;
			case 3:
				CPPUNIT_ASSERT(idx == 3);
				break;
			}
			counter++;
		}
	}
}

//-------------------------------------------------------------------------
void SquareTest::FactorCornerIndexTest()
//-------------------------------------------------------------------------
{
	int x,y;
	int idx = 5;
	Square::FactorCornerIndex(idx,x,y);

	CPPUNIT_ASSERT(x == 1);
	CPPUNIT_ASSERT(y == 0);
}
//-------------------------------------------------------------------------
void SquareTest::EdgeIndexTest()
//-------------------------------------------------------------------------
{
	int dir = 1;
	int output = 0;
	output = Square::EdgeIndex(dir,0);
	CPPUNIT_ASSERT(output == 3);
}
//-------------------------------------------------------------------------
void SquareTest::FactorEdgeIndexTest()
//-------------------------------------------------------------------------
{
	int orientation,a;
	int idx = 2;
	Square::FactorEdgeIndex(idx,orientation,a);
	CPPUNIT_ASSERT(orientation == 0);
	CPPUNIT_ASSERT(a == 1);
}

//-------------------------------------------------------------------------
void SquareTest::EdgeCornersTest()
//-------------------------------------------------------------------------
{
	int idx = 3;
	int c1, c2;
	Square::EdgeCorners(idx,c1,c2);
	CPPUNIT_ASSERT(c1 == 0);
	CPPUNIT_ASSERT(c2 == 2);
}

//-------------------------------------------------------------------------
void SquareTest::ReflectEdgeIndexTest()
//-------------------------------------------------------------------------
{
	int idx = 2;
	int edge = 3;
	int outputIndex;
	outputIndex = Square::ReflectEdgeIndex(idx,edge);
	CPPUNIT_ASSERT(outputIndex == 2);
}
//-------------------------------------------------------------------------
void SquareTest::ReflectCornerIndexTest()
//-------------------------------------------------------------------------
{
	int idx = 3;
	int edge = 4;
	int edgeOutput = Square::ReflectCornerIndex(idx,edge);
	CPPUNIT_ASSERT(edgeOutput == 2);
}

