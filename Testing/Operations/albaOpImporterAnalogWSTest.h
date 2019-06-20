/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterAnalogWSTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpImporterAnalogWSTest_H
#define CPP_UNIT_albaOpImporterAnalogWSTest_H

#include "albaTest.h"

class albaOpImporterAnalogWSTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpImporterAnalogWSTest );
  CPPUNIT_TEST( TestDynamicAllocation ); 
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestRead );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestStaticAllocation();
    void TestRead();
};

#endif
