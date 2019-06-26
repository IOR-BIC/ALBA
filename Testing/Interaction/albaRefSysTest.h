/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRefSysTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaRefSysTest_H__
#define __CPP_UNIT_albaRefSysTest_H__

#include "albaTest.h"

class albaRefSysTest : public albaTest
{
  public:
  
  CPPUNIT_TEST_SUITE( albaRefSysTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestCopyConstructor);
  CPPUNIT_TEST(TestSetTypeToCustom);
  CPPUNIT_TEST(TestSetTypeToLocal);
  CPPUNIT_TEST(TestSetTypeToView);
  CPPUNIT_TEST(TestSetTypeToParent);
  CPPUNIT_TEST(TestSetTypeToGlobal);
  CPPUNIT_TEST(TestSetGetType);
  CPPUNIT_TEST(TestSetGetTransform);
  CPPUNIT_TEST(TestSetGetMatrix);
  CPPUNIT_TEST(TestSetGetRenderer);
  CPPUNIT_TEST(TestSetGetVME);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestReset);
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructorDestructor();
    void TestCopyConstructor();    
    void TestSetTypeToCustom();
    void TestSetTypeToLocal();
    void TestSetTypeToView();
    void TestSetTypeToParent();
    void TestSetTypeToGlobal();
    void TestSetGetType();
    void TestSetGetTransform();
    void TestSetGetMatrix();
    void TestSetGetRenderer();
    void TestSetGetVME();
    void TestDeepCopy();
    void TestReset();

};

#endif
