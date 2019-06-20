/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: TriangulationTest
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
#include "TriangulationTest.h"

#include "vtkALBAPoissonSurfaceReconstruction.h"

//-------------------------------------------------------------------------
void TriangulationTest::TestConstructorDestructor()
//-------------------------------------------------------------------------
{
	Triangulation<float> triangulationFloat;
	Triangulation<double> triangulationDouble;
	Triangulation<int> triangulationInt;
}


//-------------------------------------------------------------------------
void TriangulationTest::TestFactor()
//-------------------------------------------------------------------------
{
	Triangulation<double> triangulation;

	Point3D<double> p0;
	Point3D<double> p1;
	Point3D<double> p2;

	int pointId0 = 0;
    int pointId1 = 1;
    int pointId2 = 2;

	triangulation.points.push_back(p0);
	triangulation.points.push_back(p1);
	triangulation.points.push_back(p2);

	triangulation.AddTriangle(0,1,2);

	int returnValue = triangulation.Factor(0,pointId0,pointId1,pointId2);

	CPPUNIT_ASSERT(returnValue == 1);
}

//-------------------------------------------------------------------------
void TriangulationTest::TestArea()
//-------------------------------------------------------------------------
{
	Triangulation<double> triangulation;

	Point3D<double> p0;
	Point3D<double> p1;
	Point3D<double> p2;

	int pointId0 = 0;
	int pointId1 = 1;
	int pointId2 = 2;

	triangulation.points.push_back(p0);
	triangulation.points.push_back(p1);
	triangulation.points.push_back(p2);

	triangulation.AddTriangle(0,1,2);

	double area = triangulation.Area();

	CPPUNIT_ASSERT(area == 0);

	area = triangulation.Area(0);

	CPPUNIT_ASSERT(area == 0);

	area = triangulation.Area(0,1,2);

	CPPUNIT_ASSERT(area == 0);
}

//-------------------------------------------------------------------------
void TriangulationTest::TestFlipMinimize()
//-------------------------------------------------------------------------
{
	Triangulation<double> triangulation;

	std::vector<Point3D<double> >		points;

	double pos[3] = {0,0,1};

	Point3D<double> p0;
	Point3D<double> p1;
	Point3D<double> p2;

	int pointId0 = 0;
	int pointId1 = 1;
	int pointId2 = 2;

	triangulation.points.push_back(p0);
	triangulation.points.push_back(p1);
	triangulation.points.push_back(p2);

    triangulation.AddTriangle(0,1,2);

    int result = triangulation.FlipMinimize(0);

	CPPUNIT_ASSERT(result == 0);	
}
//-------------------------------------------------------------------------
void TriangulationTest::TestAddTriangle()
//-------------------------------------------------------------------------
{
	Triangulation<double> triangulation;

	Point3D<double> p0;
	Point3D<double> p1;
	Point3D<double> p2;

	const int pointId0 = 0;
	const int pointId1 = 1;
	const int pointId2 = 2;

	triangulation.points.push_back(p0);
	triangulation.points.push_back(p1);
	triangulation.points.push_back(p2);

	triangulation.AddTriangle(0,1,2);

	int result = triangulation.AddTriangle(pointId0, pointId1, pointId2);

	assert(result == 0);
}

