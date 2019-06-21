/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMELabeledVolumeTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMELabeledVolumeTest_H__
#define __CPP_UNIT_albaVMELabeledVolumeTest_H__

#include "albaTest.h"

class albaVMELabeledVolumeTest : public albaTest
{
public:

	CPPUNIT_TEST_SUITE( albaVMELabeledVolumeTest );
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
