/*=========================================================================

 Program: MAF2
 Module: mafOpImporterRAWVolume_BESTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpImporterRAWVolume_BESTest_H
#define CPP_UNIT_mafOpImporterRAWVolume_BESTest_H

#include "mafTest.h"

class mafOpImporterRAWVolume_BESTest : public mafTest
{

public:

  CPPUNIT_TEST_SUITE( mafOpImporterRAWVolume_BESTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestImport );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestImport();

};

#endif
