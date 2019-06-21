/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterImageTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpImporterImageTEST_H__
#define __CPP_UNIT_albaOpImporterImageTEST_H__

#include "albaTest.h"

class albaOpImporterImageTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpImporterImageTest );
  CPPUNIT_TEST( Test_Single );
  CPPUNIT_TEST( Test_Multi_No_Volume );
  CPPUNIT_TEST( Test_Multi_Volume );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void Test_Single();
    void Test_Multi_No_Volume();
    void Test_Multi_Volume();
};

#endif
