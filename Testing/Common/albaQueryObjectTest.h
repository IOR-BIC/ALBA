/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaQueryObjectTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaQueryObjectTest_H__
#define __CPP_UNIT_albaQueryObjectTest_H__

#include "albaTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

/**
class name : albaQueryObjectTest
      Test class for albaQueryObject
*/
class albaQueryObjectTest : public albaTest
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( albaQueryObjectTest );
  /** macro for test TestDynamicAllocation */
	CPPUNIT_TEST( TestDynamicAllocation );
  /** macro for test TestStaticAllocation */
	CPPUNIT_TEST( TestStaticAllocation );
  /** macro for test TestGetValueAsString */
	CPPUNIT_TEST( TestGetValueAsString );
  /** macro for test TestGetValueAsInt */
	CPPUNIT_TEST( TestGetValueAsInt );
  /** macro for test TestGetValueAsLong */
  CPPUNIT_TEST( TestGetValueAsLong );
  /** macro for test TestGetValueAsFloat */
  CPPUNIT_TEST( TestGetValueAsFloat );
  /** macro for test TestGetValueAsDouble */
  CPPUNIT_TEST( TestGetValueAsDouble );
  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:
	/** Dynamic allocation test */
	void TestDynamicAllocation();
  /** Static allocation test */
	void TestStaticAllocation();
  /** Test if value is a string */
	void TestGetValueAsString();
  /** Test if value is a int */
  void TestGetValueAsInt();
  /** Test if value is a long */
  void TestGetValueAsLong();
  /** Test if value is a float */
  void TestGetValueAsFloat();
  /** Test if value is a double */
	void TestGetValueAsDouble();
};


#endif
