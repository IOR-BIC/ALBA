/*=========================================================================

 Program: MAF2
 Module: mafObjectFactoryTest
 Authors: Stefano Perticoni,Marco Petrone,Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFOBJECTFACTORYTEST_H__
#define __CPP_UNIT_MAFOBJECTFACTORYTEST_H__

#include "mafTest.h"

class mafObjectFactoryTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafObjectFactoryTest );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestDynamicAllocation );
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
