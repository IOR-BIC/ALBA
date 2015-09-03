/*=========================================================================

 Program: MAF2
 Module: mafOpImporterRAWVolumeTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_MMORAWIMPORTERVOLUMETEST_H
#define CPP_UNIT_MMORAWIMPORTERVOLUMETEST_H

#include "mafTest.h"

class mafOpImporterRAWVolumeTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpImporterRAWVolumeTest );
  CPPUNIT_TEST( Test );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void Test();
};

#endif
