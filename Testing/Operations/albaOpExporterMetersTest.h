/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMetersTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpExporterMetersTEST_H
#define CPP_UNIT_albaOpExporterMetersTEST_H

#include "albaTest.h"

class albaOpExporterMetersTest : public albaTest
{
  public:
  CPPUNIT_TEST_SUITE( albaOpExporterMetersTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestOnMetersImporter);
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestFixture();
    void TestOnMetersImporter();

  bool result;
};


#endif
