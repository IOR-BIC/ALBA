/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaResultQueryAbstractHandlerTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaResultQueryAbstractHandlerTest_H__
#define __CPP_UNIT_albaResultQueryAbstractHandlerTest_H__

#include "albaTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

/** 
class name: albaResultQueryAbstractHandlerTest
  Test class for albaResultQueryAbstractHandler
*/
class albaResultQueryAbstractHandlerTest : public albaTest
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( albaResultQueryAbstractHandlerTest );
  /** macro for test TestDynamicAllocation */
	CPPUNIT_TEST( TestDynamicAllocation );
  /** macro for test TestStaticAllocation */
	CPPUNIT_TEST( TestStaticAllocation );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetResultAsStringMatrix );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetColumnsTypeInformationAsStringVector );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetColumnsNameInformationAsStringVector );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetResultAsObjectsMatrix );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetNumberOfRecords );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetNumberOfFields );

  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Dynamic allocation test */
	void TestDynamicAllocation();
  /** Static allocation test */
	void TestStaticAllocation();

  /**Get result as string matrix */
  void TestGetResultAsStringMatrix();
  /** Get column type as list of string */
  void TestGetColumnsTypeInformationAsStringVector();
  /** Get column name as list of string */
  void TestGetColumnsNameInformationAsStringVector();
  /** Get result as string matrix */
  void TestGetResultAsObjectsMatrix();
  /** Get number of records (rows) */
  void TestGetNumberOfRecords();
  /** Get number of fields (columns) */
  void TestGetNumberOfFields();

};


#endif
