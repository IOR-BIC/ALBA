/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFreezeVMETest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpFreezeVMETEST_H
#define CPP_UNIT_albaOpFreezeVMETEST_H

#include "albaTest.h"

class albaOpFreezeVMETest : public albaTest
{
	CPPUNIT_TEST_SUITE( albaOpFreezeVMETest );
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestFreezeVMESurfaceParametric );
	CPPUNIT_TEST( TestFreezeVMEMeter );
	CPPUNIT_TEST( TestFreezeVMEWrappedMeter );
	CPPUNIT_TEST( TestFreezeVMESlicer );
	CPPUNIT_TEST( TestFreezeVMEProber );
  CPPUNIT_TEST( TestFreezeVMEProfileSpline );
  CPPUNIT_TEST( TestFreezeVMERefSys );
	CPPUNIT_TEST_SUITE_END();

protected:

  void TestDynamicAllocation();
  void TestStaticAllocation();
	void TestFreezeVMESurfaceParametric();
	void TestFreezeVMEMeter();
	void TestFreezeVMEWrappedMeter();
	void TestFreezeVMESlicer();
	void TestFreezeVMEProber();
  void TestFreezeVMEProfileSpline();
  void TestFreezeVMERefSys();
};

#endif
