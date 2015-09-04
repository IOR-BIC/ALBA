/*=========================================================================

 Program: MAF2
 Module: mafVMEExternalDataTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFVMEEXTERNALDATATEST_H__
#define __CPP_UNIT_MAFVMEEXTERNALDATATEST_H__

#include "mafTest.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEGeneric.h"


class mafVMEExternalDataTest : public mafTest
{

public:
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafVMEExternalDataTest );
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
  mafVMEStorage *m_Storage1;
  mafVMERoot *m_Storage1Root;

};

#endif
