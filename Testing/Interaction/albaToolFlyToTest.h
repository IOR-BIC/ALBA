/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolFlyToTest
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaToolFlyToTest_H__
#define __CPP_UNIT_albaToolFlyToTest_H__

#include "albaTest.h"

class albaToolFlyToTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaToolFlyToTest );
  CPPUNIT_TEST( TestNoPick );
  CPPUNIT_TEST( TestFlyTo );
  CPPUNIT_TEST( TestFlyAway );
  CPPUNIT_TEST( TestButtons );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestNoPick();
  void TestFlyTo();
  void TestFlyAway();
  void TestButtons();
};

#endif
