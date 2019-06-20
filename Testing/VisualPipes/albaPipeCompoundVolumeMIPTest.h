/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeCompoundVolumeMIPTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeCompoundVolumeMIPTest_H__
#define __CPP_UNIT_albaPipeCompoundVolumeMIPTest_H__

#include "albaTest.h"

class albaPipeCompoundVolumeMIPTest : public albaTest
{
public: 

  CPPUNIT_TEST_SUITE( albaPipeCompoundVolumeMIPTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestAllocation );
  CPPUNIT_TEST( TestCreateSceneNode );
  CPPUNIT_TEST( TestGetCurrentScalarVisualPipe );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestAllocation();
  void TestCreateSceneNode();
  void TestGetCurrentScalarVisualPipe();
};

#endif
