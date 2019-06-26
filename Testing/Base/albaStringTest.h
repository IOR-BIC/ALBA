/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaStringTest
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBASTRINGTEST_H__
#define __CPP_UNIT_ALBASTRINGTEST_H__

#include "albaTest.h"


class albaStringTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaStringTest );
  CPPUNIT_TEST( Test );
  CPPUNIT_TEST_SUITE_END();

  protected:
    /** Test multiple string features with a single test */
    void Test();
};

#endif
