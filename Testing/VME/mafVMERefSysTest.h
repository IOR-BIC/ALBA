/*=========================================================================

 Program: MAF2
 Module: mafVMERefSysTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMETest_H__
#define __CPP_UNIT_mafVMETest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafVMERefSysTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMERefSysTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAllConstructor );
  CPPUNIT_TEST( TestLandmarkAccept );
  CPPUNIT_TEST( TestDeepCopy );
  CPPUNIT_TEST( TestEquals );
  CPPUNIT_TEST( TestSetGetScaleFactor );
  CPPUNIT_TEST( TestGetSurfaceOutput );
  CPPUNIT_TEST( TestSetMatrix );
  CPPUNIT_TEST( TestGetLocalTimeStamps );
  CPPUNIT_TEST( TestIsAnimated );
  CPPUNIT_TEST( TestGetMaterial );
  CPPUNIT_TEST( TestGetIcon );
  CPPUNIT_TEST( TestGetVisualPipe );
  //CPPUNIT_TEST( TestSetRefSysLink );
  CPPUNIT_TEST( TestGetOriginVME );
  CPPUNIT_TEST( TestGetPoint1VME );
  CPPUNIT_TEST( TestGetPoint2VME );
  CPPUNIT_TEST_SUITE_END();

private:

  void TestFixture();
  void TestDynamicAllocation();
  void TestAllConstructor();
	void TestLandmarkAccept();
  void TestDeepCopy();
  void TestEquals();
  void TestSetGetScaleFactor();
  void TestGetSurfaceOutput();
  void TestSetMatrix();
  void TestGetLocalTimeStamps();
  void TestIsAnimated();
  void TestGetMaterial();
  void TestGetIcon();
  void TestGetVisualPipe();
  //void TestSetRefSysLink();
	void TestGetOriginVME();
  void TestGetPoint1VME();
  void TestGetPoint2VME();

  bool result;
};


#endif

