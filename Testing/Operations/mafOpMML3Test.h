/*=========================================================================

 Program: MAF2
 Module: mafOpMML3Test
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpMML3Test_H__
#define __CPP_UNIT_mafOpMML3Test_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "vtkDataSet.h"
#include "vtkTransform.h"

class mafOpMML3Test : public CPPUNIT_NS::TestFixture
{
public:
  CPPUNIT_TEST_SUITE( mafOpMML3Test );
  CPPUNIT_TEST( TestDynamicAllocation);
  CPPUNIT_TEST( TestOpDo);
  CPPUNIT_TEST( TestOpUndo);
  CPPUNIT_TEST( TestAccept);
  CPPUNIT_TEST( TestCopy);
  CPPUNIT_TEST( TestAcceptVMELandmark);
  CPPUNIT_TEST( TestAcceptVMESurface);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestOpDo();
  void TestOpUndo();
  void TestAccept();
  void TestCopy();
  void TestAcceptVMELandmark();
  void TestAcceptVMESurface();
};

#endif
