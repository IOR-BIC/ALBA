/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterLandmarkTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpImporterLandmarkTEST_H
#define CPP_UNIT_albaOpImporterLandmarkTEST_H

#include "albaTest.h"

class albaOpImporterLandmarkTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpImporterLandmarkTest );
  CPPUNIT_TEST( Test );
  CPPUNIT_TEST( TestTimeVariant );
  CPPUNIT_TEST( TestUnTag );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void Test();
    void TestTimeVariant();
    void TestUnTag();
};

#endif
