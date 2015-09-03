/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardTest
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
#include "mafGUIWizardTest.h"
#include "mafGUIWizard.h"
#include "mafGUIFrame.h"
#include "mafGUIWizardPage.h"
#include "wx/module.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafGUIWizardTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_Win = new mafGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();
	delete m_Win;
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);

  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::TestSetFirstPage()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);

  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  wizard->SetFirstPage(page);

  CPPUNIT_ASSERT(page == wizard->GetFirstPage());

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::TestEnableChangePageOn()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);

  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  wizard->EnableChangePageOn();

  CPPUNIT_ASSERT( wizard->GetEnableChangePage() == true );

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::TestEnableChangePageOff()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);

  wizard->EnableChangePageOff();

  CPPUNIT_ASSERT( wizard->GetEnableChangePage() == false );

  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::TestRun()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);

  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  wizard->SetFirstPage(page);

  /** Seems impossible to test method Run() */
  //wizard->Run();

  delete page;
  delete wizard;
}
