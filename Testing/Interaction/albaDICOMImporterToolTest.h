/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDICOMImporterToolTest
 Authors: Paolo Quadrani, Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDICOMImporterToolTest_H__
#define __CPP_UNIT_albaDICOMImporterToolTest_H__

#include "albaTest.h"

class albaDICOMImporterToolTest : public albaTest
{
public:
  CPPUNIT_TEST_SUITE(albaDICOMImporterToolTest);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestEventsWithoutRenderer);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestConstructorDestructor();
  void TestEventsWithoutRenderer();
};

#endif
