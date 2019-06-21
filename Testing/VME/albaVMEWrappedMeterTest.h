/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEWrappedMeterTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEWrappedMeterTest_H__
#define __CPP_UNIT_albaVMEWrappedMeterTest_H__

#include "albaTest.h"

class albaVMEWrappedMeterTest : public albaTest
{
	CPPUNIT_TEST_SUITE( albaVMEWrappedMeterTest );
  CPPUNIT_TEST( TestWithGenericVME );
	CPPUNIT_TEST( TestWithGenericVMEWithMiddlePoints );
	CPPUNIT_TEST( TestWithGenericLandmark );
	CPPUNIT_TEST( TestWithGenericLandmarkWithMiddlePoints );
	
	//CPPUNIT_TEST( TestLineDistanceWithGenericVME );
	//CPPUNIT_TEST( TestLineAngleWithGenericVME );

	//CPPUNIT_TEST( TestLineDistanceWithLandmark );
	//CPPUNIT_TEST( TestLineAngleWithLandmark );

  CPPUNIT_TEST( TestWrappedGeometry);
	
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestWithGenericVME();
	void TestWithGenericVMEWithMiddlePoints();
	void TestWithGenericLandmark();
	void TestWithGenericLandmarkWithMiddlePoints();

	/*void TestLineDistanceWithGenericVME();
	void TestLineAngleWithGenericVME();

	void TestLineDistanceWithLandmark();
	void TestLineAngleWithLandmark();*/

  void TestWrappedGeometry();
};


#endif
