/*=========================================================================

 Program: MAF2
 Module: mafPipeCompoundVolumeVRTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeCompoundVolumeVRTest_H__
#define __CPP_UNIT_mafPipeCompoundVolumeVRTest_H__

#include "mafTest.h"

class mafPipeCompoundVolumeVRTest : public mafTest
{
public: 

  CPPUNIT_TEST_SUITE( mafPipeCompoundVolumeVRTest );
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