/*=========================================================================

 Program: MAF2
 Module: mafOpEditNormalsTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpEditNormalsTest_H
#define CPP_UNIT_mafOpEditNormalsTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class mafOpEditNormalsTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( mafOpEditNormalsTest );
	CPPUNIT_TEST( Test );
	CPPUNIT_TEST( TestUndo1 );
	CPPUNIT_TEST( TestUndo2 );
	CPPUNIT_TEST_SUITE_END();

protected:
	void Test();
	void TestUndo1();
	void TestUndo2();
};

#endif
