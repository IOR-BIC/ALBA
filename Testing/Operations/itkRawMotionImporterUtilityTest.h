/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaItkRawMotionImporterUtilityTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaItkRawMotionImporterUtilityTest_H
#define CPP_UNIT_albaItkRawMotionImporterUtilityTest_H

#include "albaTest.h"

class albaItkRawMotionImporterUtilityTest : public albaTest
{
public: 

  CPPUNIT_TEST_SUITE( albaItkRawMotionImporterUtilityTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestReadMatrix );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestDynamicAllocation();
  void TestReadMatrix();

  bool m_Result;
};

#endif
