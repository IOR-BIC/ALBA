/*=========================================================================

 Program: MAF2
 Module: vtkDicomUnPackerTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_VTKDICOMUNPACKERTEST_H__
#define __CPP_UNIT_VTKDICOMUNPACKERTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>



class vtkDicomUnPackerTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( vtkDicomUnPackerTest );
  CPPUNIT_TEST( ReadTest );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void ReadTest();
};

#endif