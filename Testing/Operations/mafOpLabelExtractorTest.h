/*=========================================================================

 Program: MAF2
 Module: mafOpLabelExtractorTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpLabelExtractorTest_H__
#define __CPP_UNIT_mafOpLabelExtractorTest_H__

#include "mafTest.h"

class mafOpLabelExtractorTest : public mafTest
{
public:

	CPPUNIT_TEST_SUITE( mafOpLabelExtractorTest );
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
