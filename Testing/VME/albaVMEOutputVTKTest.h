/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputVTKTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEOutputVTKTest_H__
#define __CPP_UNIT_albaVMEOutputVTKTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaVMEOutputVTKTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMEOutputVTKTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestGetVTKData );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestGetVTKData();

	bool m_Result;
};

#endif
