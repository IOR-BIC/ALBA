/*=========================================================================

 Program: MAF2
 Module: vtkMAFRayCastCleanerTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFRayCastCleanerTEST_H__
#define __CPP_UNIT_vtkMAFRayCastCleanerTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>



class vtkMAFRayCastCleanerTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( vtkMAFRayCastCleanerTest );
  CPPUNIT_TEST( TestFilter );
  CPPUNIT_TEST( TestFilterMR );
  CPPUNIT_TEST( TestSetterGetter );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFilter();
  void TestFilterMR();
  void TestSetterGetter();
  void TestDynamicAllocation();
};

#endif