/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCoreFactoryTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaCoreFactoryTest_H__
#define __CPP_UNIT_albaCoreFactoryTest_H__

#include "albaTest.h"

class albaCoreFactoryTest : public albaTest
{
public:
	void AfterTest();

  CPPUNIT_TEST_SUITE( albaCoreFactoryTest );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetDescription );
	
	// These tests play with the factory to check that everything is working as expected: 
  CPPUNIT_TEST( TestCreateInstance );
	CPPUNIT_TEST( TestGetRegisteredFactories );
	CPPUNIT_TEST( TestRegisterFactory );
	CPPUNIT_TEST( TestUnRegisterAllFactories );
  CPPUNIT_TEST( TestRegisterOverride );
  CPPUNIT_TEST( TestRegisterNewObject );
  CPPUNIT_TEST( TestDisable );
  CPPUNIT_TEST( TestSetEnableFlag );
  CPPUNIT_TEST( TestUnRegisterFactory );
  CPPUNIT_TEST( TestGetArgs );
  CPPUNIT_TEST( TestReHash );
  CPPUNIT_TEST( TestCreateAllInstance );
  CPPUNIT_TEST_SUITE_END();

  protected:
  void TestGetDescription();
  void TestCreateInstance();
	void TestStaticAllocation();
	void TestDynamicAllocation();
	void TestGetRegisteredFactories();
	void TestRegisterFactory();
	void TestUnRegisterAllFactories();
  void TestRegisterOverride();
  void TestRegisterNewObject();
  void TestDisable();
  void TestSetEnableFlag();
  void TestUnRegisterFactory();
  void TestGetArgs();
  void TestReHash();
  void TestCreateAllInstance();
};




#endif
