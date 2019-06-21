/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpLabelExtractorTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpLabelExtractorTest_H__
#define __CPP_UNIT_albaOpLabelExtractorTest_H__

#include "albaTest.h"

class albaOpLabelExtractorTest : public albaTest
{
public:

	CPPUNIT_TEST_SUITE( albaOpLabelExtractorTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestLabelRG );
  CPPUNIT_TEST( TestLabelSP );
  CPPUNIT_TEST( TestLabelSmoothRG );
  CPPUNIT_TEST( TestLabelSmoothSP );
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestDynamicAllocation();
	void TestLabelRG();
  void TestLabelSP();
  void TestLabelSmoothRG();
  void TestLabelSmoothSP();
};

#endif
