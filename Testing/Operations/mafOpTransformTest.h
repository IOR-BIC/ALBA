/*=========================================================================

 Program: MAF2
 Module: mafOpTransformTest
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpTransformTEST_H
#define CPP_UNIT_mafOpTransformTEST_H

#include "mafTest.h"

class mafOpTransformTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpTransformTest );
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
