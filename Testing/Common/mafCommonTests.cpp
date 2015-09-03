/*=========================================================================

 Program: MAF2
 Module: mafWizardWaitOpTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafCommonTests.h"

#include "mafQueryObjectTest.h"
#include "mafResultQueryAbstractHandlerTest.h"
#include "mafRelationalDatabaseAbstractConnectorTest.h"
#include "mafCurvilinearAbscissaOnSkeletonHelperTest.h"
#include "mafHTMLTemplateParserTest.h"
#include "mafHTMLTemplateParserBlockTest.h"
#include "mafWizardBlockInformationTest.h"
#include "mafWizardBlockOperationTest.h"
#include "mafWizardBlockSelectionTest.h"
#include "mafWizardBlockYesNoSelectionTest.h"
#include "mafWizardBlockTest.h"
#include "mafWizardBlockTypeCheckTest.h"
#include "mafWizardBlockVMESelectionTest.h"
#include "mafWizardManagerTest.h"
#include "mafWizardSettingsTest.h"
#include "mafWizardWaitOpTest.h"
#include "mafWizardTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


//Main Test Executor
int
	main( int argc, char* argv[] )
{

	// Create the event manager and test controller
	CPPUNIT_NS::TestResult controller;

	// Add a listener that collects test result
	CPPUNIT_NS::TestResultCollector result;
	controller.addListener( &result );        

	// Add a listener that print dots as test run.
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener( &progress );      

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;




	runner.addTest(mafQueryObjectTest::suite());
	runner.addTest(mafResultQueryAbstractHandlerTest::suite());
	runner.addTest(mafRelationalDatabaseAbstractConnectorTest::suite());
	runner.addTest(mafCurvilinearAbscissaOnSkeletonHelperTest::suite());
	runner.addTest(mafHTMLTemplateParserTest::suite());
	runner.addTest(mafHTMLTemplateParserBlockTest::suite());
	runner.addTest(mafWizardBlockInformationTest::suite());
	runner.addTest(mafWizardBlockOperationTest::suite());
	runner.addTest(mafWizardBlockSelectionTest::suite());
	runner.addTest(mafWizardBlockYesNoSelectionTest::suite());
	runner.addTest(mafWizardBlockTest::suite());
	runner.addTest(mafWizardBlockTypeCheckTest::suite());
	runner.addTest(mafWizardBlockVMESelectionTest::suite());
	runner.addTest(mafWizardManagerTest::suite());
	runner.addTest(mafWizardSettingsTest::suite());
	runner.addTest(mafWizardWaitOpTest::suite());
	runner.addTest(mafWizardTest::suite());

	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

