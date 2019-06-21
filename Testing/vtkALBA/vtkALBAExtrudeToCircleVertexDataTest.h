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

#ifndef __CPP_UNIT_vtkALBAExtrudeToCircleVertexDataTest_H__
#define __CPP_UNIT_vtkALBAExtrudeToCircleVertexDataTest_H__

#include "albaTest.h"
#include "vtkALBAExtrudeToCircle.h"

//------------------------------------------------------------------------------
// Test class for vtkALBAExtrudeToCircle::VertexData
//------------------------------------------------------------------------------
class vtkALBAExtrudeToCircleVertexDataTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( vtkALBAExtrudeToCircleVertexDataTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
	CPPUNIT_TEST( TestSetGetCylCoords );
	CPPUNIT_TEST( TestSetGetCylR );
	CPPUNIT_TEST( TestSetGetCylPhi );
	CPPUNIT_TEST( TestSetGetCylZ );
	CPPUNIT_TEST( TestSetGetCartCoords );
	CPPUNIT_TEST( TestSetGetId );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestConstructorDestructor();
	void TestSetGetCylCoords();
	void TestSetGetCylR();
	void TestSetGetCylPhi();
	void TestSetGetCylZ();
	void TestSetGetCartCoords();
	void TestSetGetId();

};

#endif
