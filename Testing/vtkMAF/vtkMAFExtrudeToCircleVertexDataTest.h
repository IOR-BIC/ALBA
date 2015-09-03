/*=========================================================================

 Program: MAF2
 Module: vtkMAFExtrudeToCircleVertexDataTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFExtrudeToCircleVertexDataTest_H__
#define __CPP_UNIT_vtkMAFExtrudeToCircleVertexDataTest_H__

#include "mafTest.h"
#include "vtkMAFExtrudeToCircle.h"

//------------------------------------------------------------------------------
// Test class for vtkMAFExtrudeToCircle::VertexData
//------------------------------------------------------------------------------
class vtkMAFExtrudeToCircleVertexDataTest : public mafTest
{
public:

  CPPUNIT_TEST_SUITE( vtkMAFExtrudeToCircleVertexDataTest );
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
