/*=========================================================================

 Program: MAF2
 Module: mafWizardBlockOperationTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafWizardBlockOperationTest_H__
#define __CPP_UNIT_mafWizardBlockOperationTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------


/** 
class name: mafWizardBlockOperationTest
  Test class for mafWizardBlockOperation
*/
class mafWizardBlockOperationTest : public CPPUNIT_NS::TestFixture
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( mafWizardBlockOperationTest );

  /** macro for test TestSetGetVector */
  CPPUNIT_TEST( TestSetterGetter );

  /** macro for test TestSetGetVector */
  CPPUNIT_TEST( TestShowHideLists );

  /** macro for test Dynamic allocation */
  CPPUNIT_TEST( TestDynamicAllocation );

  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Test Setter and Getter methods */
  void TestSetterGetter();

  /** Test Constructors and destructor */
	void TestShowHideLists();

  /** Test Dynamic allocation */
  void TestDynamicAllocation();
};




#endif
