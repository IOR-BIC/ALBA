/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoInterfaceGUITest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 

//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaDecl.h"
#include "albaGizmoInterfaceGUITest.h"
#include "albaGizmoInterfaceGUI.h"

void albaGizmoInterfaceGUITest::TestFixture()
{

}


void albaGizmoInterfaceGUITest::TestConstructorDestructor()
{
	albaGizmoInterfaceGUI *dummyGizmoInterface = new albaGizmoInterfaceGUI(NULL);
	cppDEL(dummyGizmoInterface);
}

void albaGizmoInterfaceGUITest::TestOnEvent()
{
	albaGizmoInterfaceGUI *dummyGizmoInterface = new albaGizmoInterfaceGUI(NULL);
	// simple call since this method is doing nothing...
	dummyGizmoInterface->OnEvent(NULL);
	cppDEL(dummyGizmoInterface);	
}

void albaGizmoInterfaceGUITest::TestGetGui()
{
	albaGizmoInterfaceGUI *dummyGizmoInterface = new albaGizmoInterfaceGUI(NULL);
	albaGUI *gui = dummyGizmoInterface->GetGui();
	// no gui is built by default so:
	CPPUNIT_ASSERT(gui == NULL);
	cppDEL(dummyGizmoInterface);	
}

void albaGizmoInterfaceGUITest::TestEnableWidgets()
{
	albaGizmoInterfaceGUI *dummyGizmoInterface = new albaGizmoInterfaceGUI(NULL);
	// maybe an iVar registering widget enabling could be added to albaGizmoInterfaceGUI...
	// simple call since this method is doing nothing...
	dummyGizmoInterface->EnableWidgets(true);
	dummyGizmoInterface->EnableWidgets(false);
	cppDEL(dummyGizmoInterface);	
}

void albaGizmoInterfaceGUITest::TestTestModeOn()
{
	albaGizmoInterfaceGUI *dummyGizmoInterface = new albaGizmoInterfaceGUI(NULL);
	bool defaultTestMode = dummyGizmoInterface->GetTestMode();

	CPPUNIT_ASSERT(defaultTestMode == false);

	dummyGizmoInterface->TestModeOn();
	bool testMode = dummyGizmoInterface->GetTestMode();
	CPPUNIT_ASSERT(testMode == true);

	cppDEL(dummyGizmoInterface);	
}

void albaGizmoInterfaceGUITest::TestTestModeOff()
{
	albaGizmoInterfaceGUI *dummyGizmoInterface = new albaGizmoInterfaceGUI(NULL);
	bool defaultTestMode = dummyGizmoInterface->GetTestMode();
	CPPUNIT_ASSERT(defaultTestMode == false);

	dummyGizmoInterface->TestModeOff();
	bool testMode = dummyGizmoInterface->GetTestMode();
	CPPUNIT_ASSERT(testMode == false);

	cppDEL(dummyGizmoInterface);	
}

void albaGizmoInterfaceGUITest::TestGetTestMode()
{
	albaGizmoInterfaceGUI *dummyGizmoInterface = new albaGizmoInterfaceGUI(NULL);
	bool defaultTestMode = dummyGizmoInterface->GetTestMode();
	
	// default test mode must be false
	CPPUNIT_ASSERT(defaultTestMode == false);

	dummyGizmoInterface->TestModeOn();
	bool testMode = dummyGizmoInterface->GetTestMode();
	CPPUNIT_ASSERT(testMode = true);

	cppDEL(dummyGizmoInterface);
}