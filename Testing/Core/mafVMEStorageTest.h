/*=========================================================================

 Program: MAF2
 Module: mafVMEStorageTest
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEStorageTest_H__
#define __CPP_UNIT_mafVMEStorageTest_H__

#include "mafTest.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEGeneric.h"


class mafVMEStorageTest : public mafTest
{
  public: 

    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();


    // CPPUNIT test suite

    CPPUNIT_TEST_SUITE( mafVMEStorageTest );
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
    mafVMEStorage *m_Storage;

    // helper variables 
    mafVMERoot *m_StorageRoot;
    mafVMEGeneric *m_RootVme;

};


#endif
