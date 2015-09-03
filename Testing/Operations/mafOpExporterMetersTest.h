/*=========================================================================

 Program: MAF2
 Module: mafOpExporterMetersTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpExporterMetersTEST_H
#define CPP_UNIT_mafOpExporterMetersTEST_H

#include "mafTest.h"

class mafOpExporterMetersTest : public mafTest
{
  public:
  CPPUNIT_TEST_SUITE( mafOpExporterMetersTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestOnMetersImporter);
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestFixture();
    void TestOnMetersImporter();

  bool result;
};


#endif
