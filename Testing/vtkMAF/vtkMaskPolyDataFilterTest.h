/*=========================================================================

 Program: MAF2
 Module: vtkMaskPolyDataFilterTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_vtkMaskPolyDataFilterTEST_H
#define CPP_UNIT_vtkMaskPolyDataFilterTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class vtkMaskPolyDataFilterTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( vtkMaskPolyDataFilterTest );
  CPPUNIT_TEST( TestStructuredPoints );
	CPPUNIT_TEST( TestRectilinearGrid );
  CPPUNIT_TEST_SUITE_END();

  protected:
		void TestStructuredPoints();
    void TestRectilinearGrid();
};

#endif
