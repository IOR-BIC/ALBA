/*=========================================================================

 Program: MAF2
 Module: mafOpCreateLabeledVolumeTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpCreateLabeledVolumeTest_H
#define CPP_UNIT_mafOpCreateLabeledVolumeTest_H

#include "mafTest.h"

class mafOpCreateLabeledVolumeTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpCreateLabeledVolumeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestCopy );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST( TestOpDo );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestCopy();
  void TestAccept();
  void TestOpRun();
  void TestOpDo();
};

#endif
