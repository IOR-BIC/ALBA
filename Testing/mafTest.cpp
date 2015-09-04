/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardPageTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "mafDecl.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafTest.h"


IMPLEMENT_APP(TestApp)
//--------------------------------------------------------------------------------
bool TestApp::OnInit()
{
	return TRUE;
}

//--------------------------------------------------------------------------------
int TestApp::OnExit()
{
	wxApp::CleanUp();
	return 0;
}

//--------------------------------------------------------------------------------
bool TestApp::Yield(bool onlyIfNeeded){
	return true;
};

//----------------------------------------------------------------------------
void mafTest::setUp()
{
	m_App = new TestApp();  // Instantiate the application class
	m_App->argc = 0;        // set the number of input argument to 0
	m_App->argv = NULL;     // set to NULL the input argument's parameters
	wxTheApp->SetAppName("mafUserTest"); // Set the name for the application
	
	wxLog::EnableLogging();

	//Run Test Specific Stuff
	BeforeTest();
}
//----------------------------------------------------------------------------
void mafTest::tearDown()
{
	//Clean Test Spefic Stuff
	AfterTest();

	cppDEL(m_App);  // Destroy the application
	wxAppConsole::SetInstance(NULL);
}