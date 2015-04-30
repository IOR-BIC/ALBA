/*=========================================================================

 Program: MAF2
 Module: mafVMEStorageTest2
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEStorageTest2_H__
#define __CPP_UNIT_mafVMEStorageTest2_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEGeneric.h"

#include "vtkSphereSource.h"
#include "vtkConeSource.h"

/** Test used to isolate leaks in storage related classes; see mafVMEStorageTest for a store/restore
example with synthetic generated time varying data*/
class mafVMEStorageTest2 : public CPPUNIT_NS::TestFixture
{
  public: 

    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    // CPPUNIT test suite
    CPPUNIT_TEST_SUITE( mafVMEStorageTest2 );
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
    mafVMEStorage m_Storage1;
    mafVMERoot *m_Storage1Root;

    // helper objects
    vtkSphereSource *m_SphereSource;
    vtkConeSource *m_ConeSource;
    mafTimeStamp m_CurrentTime;

};


#endif
