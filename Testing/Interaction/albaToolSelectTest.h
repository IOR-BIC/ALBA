/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSelectTest
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaToolSelectTest_H__
#define __CPP_UNIT_albaToolSelectTest_H__

#include "albaTest.h"

class albaToolSelectTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaToolSelectTest );
  CPPUNIT_TEST( TestSelect );
  CPPUNIT_TEST( TestNotSelectable );
  CPPUNIT_TEST( TestEnableSelect );
  CPPUNIT_TEST( TestContextMenu );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestSelect();
  void TestNotSelectable();
  void TestEnableSelect();
  void TestContextMenu();
};

#endif
