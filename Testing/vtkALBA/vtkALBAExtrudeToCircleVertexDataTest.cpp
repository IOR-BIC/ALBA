/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAExtrudeToCircleVertexDataTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "albaDefines.h" 

#include <cppunit/config/SourcePrefix.h>
#include "vtkALBAExtrudeToCircleVertexDataTest.h"
#include "vtkALBAExtrudeToCircle.h"
#include "albaConfigure.h"


void vtkALBAExtrudeToCircleVertexDataTest::TestFixture()
{

}

void vtkALBAExtrudeToCircleVertexDataTest::TestConstructorDestructor() 
{
	vtkALBAExtrudeToCircle::VertexData *vertexData = NULL;
	vertexData = new vtkALBAExtrudeToCircle::VertexData();
	CPPUNIT_ASSERT(vertexData);
	cppDEL(vertexData);
	CPPUNIT_ASSERT(vertexData== NULL);
}

void vtkALBAExtrudeToCircleVertexDataTest::TestSetGetCylCoords() 
{
	vtkALBAExtrudeToCircle::VertexData *vertexData = new vtkALBAExtrudeToCircle::VertexData();

	double *getCoords = new double[3];

	getCoords[0] = 5;
	getCoords[1] = 6;
	getCoords[2] = 10;

	vertexData->GetCylCoords(getCoords);

	CPPUNIT_ASSERT(getCoords[0] == 0);
	CPPUNIT_ASSERT(getCoords[1] == 0);
	CPPUNIT_ASSERT(getCoords[2] == 0);

	double getR = 5;
	double getPhi = 6;
	double getZ = 7;

	vertexData->GetCylCoords(&getR, &getPhi, &getZ);

	CPPUNIT_ASSERT(getR == 0);
	CPPUNIT_ASSERT(getPhi == 0);
	CPPUNIT_ASSERT(getZ == 0);

	double *setCoords = new double[3];
	setCoords[0] = -1;
	setCoords[1] = -2;
	setCoords[2] = -3;

	vertexData->SetCylCoords(setCoords);
	vertexData->GetCylCoords(getCoords);

	CPPUNIT_ASSERT(getCoords[0] == -1);
	CPPUNIT_ASSERT(getCoords[1] == -2);
	CPPUNIT_ASSERT(getCoords[2] == -3);

	double r = 5;
	double phi = 10;
	double z = 15;
	vertexData->SetCylCoords(r, phi, z);
	vertexData->GetCylCoords(&r, &phi, &z);

	CPPUNIT_ASSERT(r == 5);
	CPPUNIT_ASSERT(phi == 10);
	CPPUNIT_ASSERT(z == 15);

	cppDEL(vertexData);

	delete [] setCoords;
	delete [] getCoords;
}

void vtkALBAExtrudeToCircleVertexDataTest::TestSetGetCylR() 
{
	vtkALBAExtrudeToCircle::VertexData *vertexData = new vtkALBAExtrudeToCircle::VertexData();

	double testRadius = 5;

	vertexData->SetCylR(testRadius);
	
	double radius = vertexData->GetCylR();

	CPPUNIT_ASSERT(radius == testRadius);

	cppDEL(vertexData);
}

void vtkALBAExtrudeToCircleVertexDataTest::TestSetGetCylPhi() 
{
	vtkALBAExtrudeToCircle::VertexData *vertexData = new vtkALBAExtrudeToCircle::VertexData();

	double cylPhi = 5;

	vertexData->SetCylPhi(cylPhi);

	double getCylPhi;
	getCylPhi = vertexData->GetCylPhi();

	CPPUNIT_ASSERT(getCylPhi == 5);

	cppDEL(vertexData);
}

void vtkALBAExtrudeToCircleVertexDataTest::TestSetGetCylZ() 
{
	vtkALBAExtrudeToCircle::VertexData *vertexData = new vtkALBAExtrudeToCircle::VertexData();

	double cylZ = 5;

	vertexData->SetCylZ(cylZ);

	double getCylZ = -1;
	getCylZ = vertexData->GetCylZ();

	CPPUNIT_ASSERT(getCylZ == 5);

	cppDEL(vertexData);
}

void vtkALBAExtrudeToCircleVertexDataTest::TestSetGetCartCoords() 
{
	vtkALBAExtrudeToCircle::VertexData *vertexData = new vtkALBAExtrudeToCircle::VertexData();

	double *coords = new double[3];
	vertexData->GetCartCoords(coords);

	CPPUNIT_ASSERT(coords[0] == 0);
	CPPUNIT_ASSERT(coords[1] == 0);
	CPPUNIT_ASSERT(coords[2] == 0);

	vertexData->SetCartCoords(5,7,9);
	vertexData->GetCartCoords(coords);
	
	CPPUNIT_ASSERT(coords[0] == 5);
	CPPUNIT_ASSERT(coords[1] == 7);
	CPPUNIT_ASSERT(coords[2] == 9);

	double *newCoords = new double[3];

	newCoords[0] = 5;
	newCoords[1] = 7;
	newCoords[2] = 9;

	vertexData->SetCartCoords(newCoords);
	vertexData->GetCartCoords(coords);

	CPPUNIT_ASSERT(coords[0] == 5);
	CPPUNIT_ASSERT(coords[1] == 7);
	CPPUNIT_ASSERT(coords[2] == 9);

	cppDEL(vertexData);
	
	delete [] coords;
	delete [] newCoords;
}

void vtkALBAExtrudeToCircleVertexDataTest::TestSetGetId()
{
	vtkALBAExtrudeToCircle::VertexData *vertexData = new vtkALBAExtrudeToCircle::VertexData();

	double id = 5;

	vertexData->SetId(id);

	double getId = -1;
	getId = vertexData->GetId();

	CPPUNIT_ASSERT(getId == 5);

	cppDEL(vertexData);
}
