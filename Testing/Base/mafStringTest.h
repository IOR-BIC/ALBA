/*=========================================================================

 Program: MAF2
 Module: mafStringTest
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFSTRINGTEST_H__
#define __CPP_UNIT_MAFSTRINGTEST_H__

#include "mafTest.h"


class mafStringTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafStringTest );
  CPPUNIT_TEST( Test );
  CPPUNIT_TEST_SUITE_END();

  protected:
    /** Test multiple string features with a single test */
    void Test();
};

#endif
