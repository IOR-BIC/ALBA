/*=========================================================================

 Program: MAF2
 Module: mafOpClassicICPRegistrationTest
 Authors: Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpClassicICPRegistrationTest_H
#define CPP_UNIT_mafOpClassicICPRegistrationTest_H

#include "mafTest.h"

class mafOpClassicICPRegistrationTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpClassicICPRegistrationTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestCopy );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOpDo );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestCopy();
  void TestAccept();
  void TestOpDo();
};

#endif
