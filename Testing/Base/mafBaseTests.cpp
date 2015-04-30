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

#include "mafBaseTests.h"

#include "mafStringTest.h"
#include "mafTransformTest.h"
#include "mafTransformFrameTest.h"
#include "mafMatrixTest.h"
#include "mafMatrix3x3Test.h"
#include "mafOBBTest.h"
#include "mafDirectoryTest.h"
#include "mafEventSenderTest.h"
#include "mafEventBaseTest.h"
#include "mafEventSourceTest.h"
#include "mafObjectTest.h"
#include "mafIndentTest.h"
#include "mafDirectoryTest.h"
#include "mafTransformBaseTest.h"
#include "mafObserverTest.h"
#include "mafVersionTest.h"
#include "mmuIdFactoryTest.h"
#include "mafVectorTest.h"
#include "mafObjectFactoryTest.h"
#include "vtkMAFToLinearTransformTest.h"
#include "mafReferenceCountedTest.h"
#include "mafLUTLibraryTest.h"
#include "mafObserversListTest.h"
#include "mafDynamicLoaderTest.h"
#include "mafDataChecksumTest.h"
#include "mafVect3dTest.h"
#include "mafEventTest.h"
#include "multiThreaderTest.h"


//--------------------------------------------------------------------------------
mafCxxTypeMacro(mafTestObject);
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
mafCxxTypeMacro(mafFooObject);
//--------------------------------------------------------------------------------

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


	runner.addTest(mafStringTest::suite());
	runner.addTest(mafTransformTest::suite());
	runner.addTest(mafTransformFrameTest::suite());
	runner.addTest(mafMatrixTest::suite());
	runner.addTest(mafMatrix3x3Test::suite());
	runner.addTest(mafOBBTest::suite());
	runner.addTest(mafDirectoryTest::suite());
	runner.addTest(mafEventSenderTest::suite());
	runner.addTest(mafEventBaseTest::suite());
	runner.addTest(mafEventSourceTest::suite());
	runner.addTest(mafObjectTest::suite());
	runner.addTest(mafIndentTest::suite());
	runner.addTest(mafDirectoryTest::suite());
	runner.addTest(mafTransformBaseTest::suite());
	runner.addTest(mafObserverTest::suite());
	runner.addTest(mafVersionTest::suite());
	runner.addTest(mmuIdFactoryTest::suite());
	runner.addTest(mafVectorTest::suite());
	runner.addTest(mafObjectFactoryTest::suite());
	runner.addTest(vtkMAFToLinearTransformTest::suite());
	runner.addTest(mafReferenceCountedTest::suite());
	runner.addTest(mafLUTLibraryTest::suite());
	runner.addTest(mafObserversListTest::suite());
	runner.addTest(mafDynamicLoaderTest::suite());
	runner.addTest(mafDataChecksumTest::suite());
	runner.addTest(mafVect3dTest::suite());
	runner.addTest(mafEventTest::suite());
	runner.addTest(multiThreaderTest::suite());


	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

