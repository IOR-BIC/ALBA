/*=========================================================================

 Program: MAF2
 Module: mafGUIGizmoInterfaceTest
 Authors: Stefano Perticoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafDecl.h"
#include "mafGUIGizmoInterfaceTest.h"
#include "mafGUIGizmoInterface.h"

void mafGUIGizmoInterfaceTest::TestFixture()
{

}


void mafGUIGizmoInterfaceTest::TestConstructorDestructor()
{
	mafGUIGizmoInterface *dummyGizmoInterface = new mafGUIGizmoInterface(NULL);
	cppDEL(dummyGizmoInterface);
}

void mafGUIGizmoInterfaceTest::TestOnEvent()
{
	mafGUIGizmoInterface *dummyGizmoInterface = new mafGUIGizmoInterface(NULL);
	// simple call since this method is doing nothing...
	dummyGizmoInterface->OnEvent(NULL);
	cppDEL(dummyGizmoInterface);	
}

void mafGUIGizmoInterfaceTest::TestGetGui()
{
	mafGUIGizmoInterface *dummyGizmoInterface = new mafGUIGizmoInterface(NULL);
	mafGUI *gui = dummyGizmoInterface->GetGui();
	// no gui is built by default so:
	CPPUNIT_ASSERT(gui == NULL);
	cppDEL(dummyGizmoInterface);	
}

void mafGUIGizmoInterfaceTest::TestEnableWidgets()
{
	mafGUIGizmoInterface *dummyGizmoInterface = new mafGUIGizmoInterface(NULL);
	// maybe an iVar registering widget enabling could be added to mafGUIGizmoInterface...
	// simple call since this method is doing nothing...
	dummyGizmoInterface->EnableWidgets(true);
	dummyGizmoInterface->EnableWidgets(false);
	cppDEL(dummyGizmoInterface);	
}

void mafGUIGizmoInterfaceTest::TestTestModeOn()
{
	mafGUIGizmoInterface *dummyGizmoInterface = new mafGUIGizmoInterface(NULL);
	bool defaultTestMode = dummyGizmoInterface->GetTestMode();

	CPPUNIT_ASSERT(defaultTestMode == false);

	dummyGizmoInterface->TestModeOn();
	bool testMode = dummyGizmoInterface->GetTestMode();
	CPPUNIT_ASSERT(testMode == true);

	cppDEL(dummyGizmoInterface);	
}

void mafGUIGizmoInterfaceTest::TestTestModeOff()
{
	mafGUIGizmoInterface *dummyGizmoInterface = new mafGUIGizmoInterface(NULL);
	bool defaultTestMode = dummyGizmoInterface->GetTestMode();
	CPPUNIT_ASSERT(defaultTestMode == false);

	dummyGizmoInterface->TestModeOff();
	bool testMode = dummyGizmoInterface->GetTestMode();
	CPPUNIT_ASSERT(testMode == false);

	cppDEL(dummyGizmoInterface);	
}

void mafGUIGizmoInterfaceTest::TestGetTestMode()
{
	mafGUIGizmoInterface *dummyGizmoInterface = new mafGUIGizmoInterface(NULL);
	bool defaultTestMode = dummyGizmoInterface->GetTestMode();
	
	// default test mode must be false
	CPPUNIT_ASSERT(defaultTestMode == false);

	dummyGizmoInterface->TestModeOn();
	bool testMode = dummyGizmoInterface->GetTestMode();
	CPPUNIT_ASSERT(testMode = true);

	cppDEL(dummyGizmoInterface);
}