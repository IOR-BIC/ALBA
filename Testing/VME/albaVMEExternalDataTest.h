/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEExternalDataTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAVMEEXTERNALDATATEST_H__
#define __CPP_UNIT_ALBAVMEEXTERNALDATATEST_H__

#include "albaTest.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEGeneric.h"


class albaVMEExternalDataTest : public albaTest
{

public:
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaVMEExternalDataTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestSetGetExtension);
  CPPUNIT_TEST(TestSetGetFileName);
  CPPUNIT_TEST(TestSetGetMimeType);
  CPPUNIT_TEST(TestGetAbsoluteFileName);
  CPPUNIT_TEST(TestSetCurrentPath);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST_SUITE_END();

protected:
  
  void TestFixture();
  void TestConstructorDestructor();
  void TestSetGetExtension();
  void TestSetGetFileName();
  void TestSetGetMimeType();
  void TestGetAbsoluteFileName();
  void TestSetCurrentPath();
  void TestDeepCopy();
  void TestEquals();
  
  //create a storage: this also creates a root
  albaVMEStorage *m_Storage1;
  albaVMERoot *m_Storage1Root;

};

#endif
