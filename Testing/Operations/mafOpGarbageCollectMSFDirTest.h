/*=========================================================================

 Program: MAF2
 Module: mafOpGarbageCollectMSFDirTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpGarbageCollectMSFDirTest_H__
#define __CPP_UNIT_mafOpGarbageCollectMSFDirTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


/**
  Class Name: mafOpGarbageCollectMSFDirTest
  Test class for mafOpGarbageCollectMSFDir.
*/
class mafOpGarbageCollectMSFDirTest : public CPPUNIT_NS::TestFixture
{
  /** test suite starting */
  CPPUNIT_TEST_SUITE( mafOpGarbageCollectMSFDirTest );

  /** test OpRun */
  CPPUNIT_TEST( TestOpRun );

  /** test Tree Validation with correct tree */
  CPPUNIT_TEST( TestGarbageCollect );
  
  /** test suite ending  */
  CPPUNIT_TEST_SUITE_END();

  protected:

    /** test OpRun */
    void TestOpRun();

    /** test Tree Validation with correct tree */
    void TestGarbageCollect();
};

#endif
