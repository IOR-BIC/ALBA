/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaFlyToToolTest
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaFlyToToolTest_H__
#define __CPP_UNIT_albaFlyToToolTest_H__

#include "albaTest.h"

class albaFlyToToolTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaFlyToToolTest );
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
