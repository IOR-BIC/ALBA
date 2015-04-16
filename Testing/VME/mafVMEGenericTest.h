/*=========================================================================

 Program: MAF2
 Module: mafVMEGenericTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFVMEGENERICTEST_H__
#define __CPP_UNIT_MAFVMEGENERICTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafVMERoot.h"
#include "mafVMEGeneric.h"


class mafVMEGenericTest : public CPPUNIT_NS::TestFixture
{

  public:
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( mafVMEGenericTest );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestSetData );
    CPPUNIT_TEST( TestGetVisualPipe );
      CPPUNIT_TEST_SUITE_END();

protected:
    void TestDynamicAllocation();
    void TestSetData();
    void TestGetVisualPipe();

};

#endif
