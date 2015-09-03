/*=========================================================================

 Program: MAF2
 Module: mafVMEVolumeLargeTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEVolumeLargeTest_H__
#define __CPP_UNIT_mafVMEVolumeLargeTest_H__

#include "mafTest.h"

class mafVMEVolumeLargeTest : public mafTest
{
public: 
  CPPUNIT_TEST_SUITE( mafVMEVolumeLargeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetIcon );
  CPPUNIT_TEST( TestSetLargeData );
  CPPUNIT_TEST( TestSetGetFileNameTest );
  CPPUNIT_TEST( TestUnRegister );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestGetIcon();
	void TestSetLargeData();
	void TestSetGetFileNameTest();
	void TestUnRegister();
};

#endif
