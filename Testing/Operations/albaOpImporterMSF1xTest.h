/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMSF1xTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_MMOMSF1XIMPORTERTEST_H
#define CPP_UNIT_MMOMSF1XIMPORTERTEST_H

#include "albaTest.h"

class albaOpImporterMSF1xTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpImporterMSF1xTest );
  CPPUNIT_TEST( Test );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void Test();
};

#endif
