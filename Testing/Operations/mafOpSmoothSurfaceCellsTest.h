/*=========================================================================

 Program: MAF2
 Module: mafOpSmoothSurfaceCellsTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpSmoothSurfaceCellsTest_H
#define CPP_UNIT_mafOpSmoothSurfaceCellsTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class mafOpSmoothSurfaceCellsTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( mafOpSmoothSurfaceCellsTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestSetDiameter );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST( TestSetSeed );
  CPPUNIT_TEST( TestMarkCells );
  CPPUNIT_TEST( TestSmoothCells );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestAccept();
  void TestSetDiameter();
  void TestOpRun();
  void TestSetSeed();
  void TestMarkCells();
  void TestSmoothCells();
};

#endif
