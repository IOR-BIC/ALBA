/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMeshTest
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpExporterMeshTest_H__
#define __CPP_UNIT_albaOpExporterMeshTest_H__

#include "albaTest.h"

class albaOpExporterMeshTest : public albaTest
{
  public:
 
  CPPUNIT_TEST_SUITE( albaOpExporterMeshTest );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestExporterMesh );
	CPPUNIT_TEST(TestBackCalculation);
	CPPUNIT_TEST(TestBackCalculationSetValues);
	CPPUNIT_TEST(TestBackCalculationTripleInterval);
	CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestConstructor();
  void TestExporterMesh();
	void TestBackCalculation();
	void TestBackCalculationSetValues();
	void TestBackCalculationTripleInterval();

};

#endif
