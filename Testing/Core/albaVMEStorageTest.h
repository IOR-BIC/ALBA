/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEStorageTest
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEStorageTest_H__
#define __CPP_UNIT_albaVMEStorageTest_H__

#include "albaTest.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEGeneric.h"


class albaVMEStorageTest : public albaTest
{
  public: 

    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();


    // CPPUNIT test suite

    CPPUNIT_TEST_SUITE( albaVMEStorageTest );
    CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
    CPPUNIT_TEST(TestSave);
    CPPUNIT_TEST(TestSaveAs);
    CPPUNIT_TEST(TestSaveAndReload);
    CPPUNIT_TEST(TestTreeEditAndGarbageCollection);
    CPPUNIT_TEST_SUITE_END();

   
 private:
    
    void TestFixture();
    void TestSaveAndReload();
    void TestSave();
    void TestSaveAs();
    void TestTreeEditAndGarbageCollection();

    void CreateVMETestTree();

    // storage to test
    albaVMEStorage *m_Storage;

    // helper variables 
    albaVMERoot *m_StorageRoot;
    albaVMEGeneric *m_RootVme;

};


#endif
