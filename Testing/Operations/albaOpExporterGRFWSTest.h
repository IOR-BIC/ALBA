/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterGRFWSTest
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpExporterGRFWSTest_H
#define CPP_UNIT_albaOpExporterGRFWSTest_H

#include "albaTest.h"

class albaOpExporterGRFWSTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpExporterGRFWSTest );
  CPPUNIT_TEST( TestDynamicAllocation ); 
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestWrite );
  CPPUNIT_TEST( TestWriteFast );
  CPPUNIT_TEST( TestWriteSingleVector );
  CPPUNIT_TEST( TestWriteSingleVectorFast );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestStaticAllocation();
    void TestWrite();
    void TestWriteFast();
    void TestWriteSingleVector();
    void TestWriteSingleVectorFast();
};

#endif
