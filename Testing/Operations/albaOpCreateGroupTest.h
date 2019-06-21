/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateGroupTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpCreateGroupTest_H
#define CPP_UNIT_albaOpCreateGroupTest_H

#include "albaTest.h"

class albaOpCreateGroupTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpCreateGroupTest );
  CPPUNIT_TEST( Test );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void Test();
};

#endif
