/*=========================================================================

 Program: MAF2
 Module: mafPipeCompoundVolumeFixedScalarsTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeCompoundVolumeFixedScalarsTest_H__
#define __CPP_UNIT_mafPipeCompoundVolumeFixedScalarsTest_H__

#include "mafTest.h"

class mafPipeCompoundVolumeFixedScalarsTest : public mafTest
{
public: 

  CPPUNIT_TEST_SUITE( mafPipeCompoundVolumeFixedScalarsTest );
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
