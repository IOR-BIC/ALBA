/*=========================================================================

 Program: MAF2
 Module: vtkMAFPolyDataMirrorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFPolyDataMirrorTEST_H__
#define __CPP_UNIT_vtkMAFPolyDataMirrorTEST_H__

#include "mafTest.h"

//-----------------------------------------------------
// forward references:
//-----------------------------------------------------

class vtkPolyData;

class vtkMAFPolyDataMirrorTest : public mafTest
{
	public:
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( vtkMAFPolyDataMirrorTest );
	CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestMirrorX );
  CPPUNIT_TEST( TestMirrorY );
  CPPUNIT_TEST( TestMirrorZ );
  //CPPUNIT_TEST( TestFlipNormals );
  CPPUNIT_TEST_SUITE_END();

  protected:
	void TestDynamicAllocation();
	void TestMirrorX();
  void TestMirrorY();
  void TestMirrorZ();
  void TestFlipNormals();

	vtkPolyData *m_TestPolyData;
};

#endif