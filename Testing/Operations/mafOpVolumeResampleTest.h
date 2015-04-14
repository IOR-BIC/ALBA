/*=========================================================================

 Program: MAF2
 Module: mafOpVolumeResampleTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MMOVOLUMERESAMPLETEST_H__
#define __CPP_UNIT_MMOVOLUMERESAMPLETEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>



class mafOpVolumeResampleTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( mafOpVolumeResampleTest );
  CPPUNIT_TEST( TestBase );
	CPPUNIT_TEST( TestVMELocalBounds );
	CPPUNIT_TEST( TestVME4DBounds );
	CPPUNIT_TEST( TestVMEBounds );
	CPPUNIT_TEST( TestScalarRange );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestBase();
		void TestVMELocalBounds();
		void TestVME4DBounds();
		void TestVMEBounds();
		void TestScalarRange();
};

#endif
