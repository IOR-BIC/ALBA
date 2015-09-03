/*=========================================================================

 Program: MAF2
 Module: mafOpImporterRAWImagesTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpImporterRAWImagesTEST_H
#define CPP_UNIT_mafOpImporterRAWImagesTEST_H

#include "mafTest.h"

class mafOpImporterRAWImagesTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpImporterRAWImagesTest );
  CPPUNIT_TEST( Test );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void Test();
};

#endif
