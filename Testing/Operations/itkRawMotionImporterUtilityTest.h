/*=========================================================================

 Program: MAF2
 Module: mafItkRawMotionImporterUtilityTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafItkRawMotionImporterUtilityTest_H
#define CPP_UNIT_mafItkRawMotionImporterUtilityTest_H

#include "mafTest.h"

class mafItkRawMotionImporterUtilityTest : public mafTest
{
public: 

  CPPUNIT_TEST_SUITE( mafItkRawMotionImporterUtilityTest );
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
