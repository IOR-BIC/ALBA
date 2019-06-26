/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMotionDataTest
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpImporterMotionDataTest_H
#define CPP_UNIT_albaOpImporterMotionDataTest_H

#include "albaTest.h"

class albaOpImporterMotionDataTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpImporterMotionDataTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestOnRawMotionData );  
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestCopy();
    void TestOnRawMotionData();
};

#endif
