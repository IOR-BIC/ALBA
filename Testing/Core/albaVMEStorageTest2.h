/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEStorageTest2
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEStorageTest2_H__
#define __CPP_UNIT_albaVMEStorageTest2_H__

#include "albaTest.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEGeneric.h"

#include "vtkSphereSource.h"
#include "vtkConeSource.h"

/** Test used to isolate leaks in storage related classes; see albaVMEStorageTest for a store/restore
example with synthetic generated time varying data*/
class albaVMEStorageTest2 : public albaTest
{
  public: 

    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    // CPPUNIT test suite
    CPPUNIT_TEST_SUITE( albaVMEStorageTest2 );
    CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
    CPPUNIT_TEST( TestStoreRestoreTagArray );
    CPPUNIT_TEST(TestRestoreOnNewStorage);
    CPPUNIT_TEST(TestRestoreOnSameStorage);
    CPPUNIT_TEST_SUITE_END();

 private:
    
    void TestFixture();

    void TestRestoreOnNewStorage();
    void TestRestoreOnSameStorage();
    void TestStoreRestoreTagArray();

    //create a storage: this also creates a root
    albaVMEStorage *m_Storage1;
    albaVMERoot *m_Storage1Root;

    // helper objects
    vtkSphereSource *m_SphereSource;
    vtkConeSource *m_ConeSource;
    albaTimeStamp m_CurrentTime;

};


#endif
