/*=========================================================================

 Program: MAF2
 Module: mafVMELabeledVolumeTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMELabeledVolumeTest_H__
#define __CPP_UNIT_mafVMELabeledVolumeTest_H__

#include "mafTest.h"

class mafVMELabeledVolumeTest : public mafTest
{
public:

	CPPUNIT_TEST_SUITE( mafVMELabeledVolumeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestVolumeCopy );
	CPPUNIT_TEST( TestGenerateLabeledVolume );
  CPPUNIT_TEST( TestRemoveLabelTag );
  CPPUNIT_TEST( TestSetLabelTag );
	CPPUNIT_TEST( TestDeepCopy );
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestDynamicAllocation();
	void TestVolumeCopy();
	void TestGenerateLabeledVolume();
  void TestRemoveLabelTag();
  void TestSetLabelTag();
  void TestDeepCopy();
};

#endif
