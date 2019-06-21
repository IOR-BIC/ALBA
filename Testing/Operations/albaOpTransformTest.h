/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTransformTest
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpTransformTEST_H
#define CPP_UNIT_albaOpTransformTEST_H

#include "albaTest.h"

class albaOpTransformTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpTransformTest );
  CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestAccept );
	CPPUNIT_TEST( TestTransform );
  CPPUNIT_TEST( TestResetMove );
	CPPUNIT_TEST( TestResetScale );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestAccept();
	void TestTransform();
  void TestResetMove();
	void TestResetScale();
};

#endif
