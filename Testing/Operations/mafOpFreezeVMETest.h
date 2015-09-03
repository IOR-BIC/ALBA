/*=========================================================================

 Program: MAF2
 Module: mafOpFreezeVMETest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpFreezeVMETEST_H
#define CPP_UNIT_mafOpFreezeVMETEST_H

#include "mafTest.h"

class mafOpFreezeVMETest : public mafTest
{
	CPPUNIT_TEST_SUITE( mafOpFreezeVMETest );
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
