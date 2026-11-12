/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVTKPickerToolTest
 Authors: Josef Kohout, Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVTKPickerToolTest_H__
#define __CPP_UNIT_albaVTKPickerToolTest_H__

#include "albaTest.h"

class albaVTKPickerToolTest : public albaTest
{
public:
  CPPUNIT_TEST_SUITE(albaVTKPickerToolTest);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestSetGetContinuousPicking);
  CPPUNIT_TEST(TestEventsWithoutRenderer);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestConstructorDestructor();
  void TestSetGetContinuousPicking();
  void TestEventsWithoutRenderer();
};

#endif
