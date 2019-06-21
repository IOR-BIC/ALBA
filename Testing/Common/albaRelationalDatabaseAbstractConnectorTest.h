/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRelationalDatabaseAbstractConnectorTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaRelationalDatabaseAbstractConnectorTest_H__
#define __CPP_UNIT_albaRelationalDatabaseAbstractConnectorTest_H__

#include "albaTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

/** 
class name: albaRelationalDatabaseAbstractConnectorTest
  Test class for albaRelationalDatabaseAbstractConnector
*/
class albaRelationalDatabaseAbstractConnectorTest : public albaTest
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( albaRelationalDatabaseAbstractConnectorTest );
  /** macro for test TestDynamicAllocation */
	CPPUNIT_TEST( TestDynamicAllocation );
  /** macro for test TestStaticAllocation */
	CPPUNIT_TEST( TestStaticAllocation );
  /** macro for test TestSetGetRelationalDatabaseInformation */
  CPPUNIT_TEST( TestSetGetRelationalDatabaseInformation );
  /** macro for test TestSetGetQuery */
  CPPUNIT_TEST( TestSetGetQuery );
  /** macro for test TestIsFailed */
  CPPUNIT_TEST( TestIsFailed );
  /** macro for test TestGetNumberOfRecords */
  CPPUNIT_TEST( TestGetNumberOfRecords );
  /** macro for test TestGetNumberOfFields */
  CPPUNIT_TEST( TestGetNumberOfFields );
  /** macro for test TestGetQueryResultAsStringMatrix */
  CPPUNIT_TEST( TestGetQueryResultAsStringMatrix );
  /** macro for test TestGetColumnsTypeAsStringVector */
  CPPUNIT_TEST( TestGetColumnsTypeAsStringVector );
  /** macro for test TestGetColumnsNameAsStringVector */
  CPPUNIT_TEST( TestGetColumnsNameAsStringVector );
  /** macro for test TestGetQueryResultAsObjectsMatrix */
  CPPUNIT_TEST( TestGetQueryResultAsObjectsMatrix );
  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Dynamic allocation test */
	void TestDynamicAllocation();
  /** Static allocation test */
	void TestStaticAllocation();
  /** test method for SetRelationalDatabaseInformation GetRelationalDatabaseInformation*/
  void TestSetGetRelationalDatabaseInformation();
  /** test method for SetQuery & GetQuery*/
  void TestSetGetQuery();
  /** test method for IsFailed*/
  void TestIsFailed();
  /** test method for GetNumberOfRecords*/
  void TestGetNumberOfRecords();
  /** test method for GetNumberOfFields*/
  void TestGetNumberOfFields();
  /** test method for GetQueryResultAsStringMatrix*/
  void TestGetQueryResultAsStringMatrix();
  /** test method for GetColumnsTypeAsStringVector*/
  void TestGetColumnsTypeAsStringVector();
  /** test method for GetColumnsNameAsStringVector*/
  void TestGetColumnsNameAsStringVector();
  /** test method for GetQueryResultAsObjectsMatrix*/
  void TestGetQueryResultAsObjectsMatrix();

};



#endif
