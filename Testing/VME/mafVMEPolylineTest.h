/*=========================================================================

 Program: MAF2
 Module: mafVMEPolylineTest
 Authors: Daniele Giunchi & Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFVMEPOLYLINETEST_H__
#define __CPP_UNIT_MAFVMEPOLYLINETEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkPolyData;

class mafVMEPolylineTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( mafVMEPolylineTest );
	CPPUNIT_TEST( TestSetData );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestSetData();
	double CalculateLengthSubstitute(vtkPolyData *data);
};

#endif
