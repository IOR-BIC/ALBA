/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExtractGeometryTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpExtractGeometryTest_H
#define CPP_UNIT_albaOpExtractGeometryTest_H

#include "albaTest.h"

class albaOpExtractGeometryTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpExtractGeometryTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestCopy );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST_SUITE_END();

  protected:
		void TestDynamicAllocation();
    void TestCopy();
    void TestAccept();
};

#endif
