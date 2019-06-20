/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterAnalogWSTest
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpExporterAnalogWSTest_H
#define CPP_UNIT_albaOpExporterAnalogWSTest_H

#include "albaTest.h"

class albaOpExporterAnalogWSTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpExporterAnalogWSTest );
  CPPUNIT_TEST( TestDynamicAllocation ); 
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestWrite );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestStaticAllocation();
    void TestWrite();
};

#endif
