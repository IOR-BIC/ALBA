/*=========================================================================

 Program: MAF2
 Module: mafOpExporterLandmarkTest
 Authors: Daniele Giunchi, Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpExporterLandmarkTEST_H
#define CPP_UNIT_mafOpExporterLandmarkTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class mafOpExporterLandmarkTest : public CPPUNIT_NS::TestFixture
{

public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafOpExporterLandmarkTest );
  //CPPUNIT_TEST( TestOnVmeRawMotionData ); ///< this test is leaked for vmerawmotiondata
	CPPUNIT_TEST( TestOnLandmarkImporter );
  CPPUNIT_TEST( TestMultipleExports );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestOnVmeRawMotionData();
		void TestOnLandmarkImporter();
    void TestMultipleExports();
};

#endif