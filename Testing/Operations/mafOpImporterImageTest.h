/*=========================================================================

 Program: MAF2
 Module: mafOpImporterImageTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpImporterImageTEST_H__
#define __CPP_UNIT_mafOpImporterImageTEST_H__

#include "mafTest.h"

class mafOpImporterImageTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpImporterImageTest );
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
