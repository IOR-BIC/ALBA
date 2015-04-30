/*=========================================================================

 Program: MAF2
 Module: mafNodeFactoryTest
 Authors: Stefano Perticoni, Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFNODEFACTORYTEST_H__
#define __CPP_UNIT_MAFNODEFACTORYTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>



class mafNodeFactoryTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( mafNodeFactoryTest );
  CPPUNIT_TEST( TestCreateNodeInstance );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestCreateNodeInstance();
};




#endif
