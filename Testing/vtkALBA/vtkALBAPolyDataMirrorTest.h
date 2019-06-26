/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPolyDataMirrorTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAPolyDataMirrorTEST_H__
#define __CPP_UNIT_vtkALBAPolyDataMirrorTEST_H__

#include "albaTest.h"

//-----------------------------------------------------
// forward references:
//-----------------------------------------------------

class vtkPolyData;

class vtkALBAPolyDataMirrorTest : public albaTest
{
	public:
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( vtkALBAPolyDataMirrorTest );
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