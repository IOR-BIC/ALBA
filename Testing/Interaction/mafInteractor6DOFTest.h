/*=========================================================================

 Program: MAF2
 Module: mafInteractor6DOFTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractor6DOFTest_H__
#define __CPP_UNIT_mafInteractor6DOFTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafInteractor6DOFTest : public CPPUNIT_NS::TestFixture
{
public:

	/** CPPUNIT fixture: executed before each test */
	void setUp();

	/** CPPUNIT fixture: executed after each test */
	void tearDown();

	CPPUNIT_TEST_SUITE( mafInteractor6DOFTest );
	CPPUNIT_TEST( TestFixture );
	CPPUNIT_TEST( TestConstructorDestructor );
	CPPUNIT_TEST( TestStartStopInteraction);
	CPPUNIT_TEST( TestSetGetTrackerPoseMatrix);
	CPPUNIT_TEST( TestTrackerSnapshot);
	CPPUNIT_TEST( TestUpdateDeltaTransform);
	CPPUNIT_TEST( TestSetGetTracker);
	CPPUNIT_TEST( TestSetGetIgnoreTriggerEvents);	
	CPPUNIT_TEST( TestIgnoreTriggerEventsOnOff);
	CPPUNIT_TEST( TestSetRenderer);
	CPPUNIT_TEST( TestHideShowAvatar);
 	CPPUNIT_TEST_SUITE_END();

protected:

	void TestFixture(); 
	void TestConstructorDestructor();
	void TestStartStopInteraction();
	void TestSetGetTrackerPoseMatrix();
	void TestTrackerSnapshot();
	void TestUpdateDeltaTransform();
	void TestSetGetTracker();
	void TestSetGetIgnoreTriggerEvents();	
	void TestIgnoreTriggerEventsOnOff();
	void TestSetRenderer();
	void TestHideShowAvatar();

};

#endif
