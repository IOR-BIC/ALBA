/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include "albaDecl.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaGUIWizardTest.h"
#include "albaGUIWizard.h"
#include "albaGUIFrame.h"
#include "albaGUIWizardPage.h"
#include "wx/module.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaGUIWizardTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_Win = new albaGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//----------------------------------------------------------------------------
void albaGUIWizardTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();
	delete m_Win;
}
//----------------------------------------------------------------------------
void albaGUIWizardTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);

  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardTest::TestSetFirstPage()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);

  albaGUIWizardPage *page = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);

  wizard->SetFirstPage(page);

  CPPUNIT_ASSERT(page == wizard->GetFirstPage());

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardTest::TestEnableChangePageOn()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);

  albaGUIWizardPage *page = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);

  wizard->EnableChangePageOn();

  CPPUNIT_ASSERT( wizard->GetEnableChangePage() == true );

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardTest::TestEnableChangePageOff()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);

  wizard->EnableChangePageOff();

  CPPUNIT_ASSERT( wizard->GetEnableChangePage() == false );

  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardTest::TestRun()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);

  albaGUIWizardPage *page = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);

  wizard->SetFirstPage(page);

  /** Seems impossible to test method Run() */
  //wizard->Run();

  delete page;
  delete wizard;
}
