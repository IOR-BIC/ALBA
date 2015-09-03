/*=========================================================================

 Program: MAF2
 Module: mafOpExporterGRFWSTest
 Authors: Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpExporterGRFWSTest_H
#define CPP_UNIT_mafOpExporterGRFWSTest_H

#include "mafTest.h"

class mafOpExporterGRFWSTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpExporterGRFWSTest );
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
