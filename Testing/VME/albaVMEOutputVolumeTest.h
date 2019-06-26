/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputVolumeTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEOutputVolumeTest_H__
#define __CPP_UNIT_albaVMEOutputVolumeTest_H__

#include "albaTest.h"
/** Test for albaMatrix; Use this suite to trace memory problems */
class albaVMEOutputVolumeTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMEOutputVolumeTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetStructuredData );
  CPPUNIT_TEST( TestGetRectilinearData );
  CPPUNIT_TEST( TestGetUnstructuredData );
  CPPUNIT_TEST( TestSetGetVolumeMaterial );
  CPPUNIT_TEST( TestGetVTKDataTypeAsString_Update );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestGetStructuredData();
  void TestGetRectilinearData();
  void TestGetUnstructuredData();
  void TestSetGetVolumeMaterial();
  void TestGetVTKDataTypeAsString_Update();

	bool m_Result;
};

#endif
