/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPadTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDeviceButtonsPadTest_H__
#define __CPP_UNIT_albaDeviceButtonsPadTest_H__

#include "albaTest.h"

class albaDeviceButtonsPadTest : public albaTest
{
 public:

 CPPUNIT_TEST_SUITE( albaDeviceButtonsPadTest );
 CPPUNIT_TEST( TestFixture );
 CPPUNIT_TEST( TestConstructorDestructor );
 CPPUNIT_TEST( TestSetGetNumberOfButtons );
 CPPUNIT_TEST( TestSetGetButtonState );
 CPPUNIT_TEST_SUITE_END();

 protected:

   void TestFixture();
   void TestConstructorDestructor();
   void TestSetGetNumberOfButtons();
	  void CreateTestData();
   void TestSetGetButtonState();

};

#endif
