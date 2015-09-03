/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputSurfaceTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEOutputSurfaceTest_H__
#define __CPP_UNIT_mafVMEOutputSurfaceTest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafVMEOutputSurfaceTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEOutputSurfaceTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetSurfaceData );
  CPPUNIT_TEST( TestSetGetTexture );
  CPPUNIT_TEST( TestSetGetMaterial );
  CPPUNIT_TEST( TestGetNumberOfTrainglesUpdate );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestGetSurfaceData();
  void TestSetGetTexture();
  void TestSetGetMaterial();
  void TestGetNumberOfTrainglesUpdate();

	bool m_Result;
};

#endif
