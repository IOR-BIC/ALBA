/*=========================================================================

 Program: MAF2Medical
 Module: medWizardManagerTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "medWizardManagerTest.h"
#include "medWizardSettings.h"
#include "medWizardManager.h"
#include <cppunit/config/SourcePrefix.h>
#include "medWizard.h"
// TEST

// ===============================================================================
class TestApp : public wxApp
  // ===============================================================================
{
public:
  bool OnInit();
  int  OnExit();
};
DECLARE_APP(TestApp)

IMPLEMENT_APP(TestApp)
  //--------------------------------------------------------------------------------
  bool TestApp::OnInit()
  //--------------------------------------------------------------------------------
{
  return TRUE;
}
//--------------------------------------------------------------------------------
int TestApp::OnExit()
  //--------------------------------------------------------------------------------
{
  wxApp::CleanUp();
  return 0;
}
// ===============================================================================



//----------------------------------------------------------------------------
void medWizardManagerTest::setUp()
  //----------------------------------------------------------------------------
{
  m_App = new TestApp();  // Instantiate the application class
  m_App->argc = 0;        // set the number of input argument to 0
  m_App->argv = NULL;     // set to NULL the input argument's parameters
  wxTheApp->SetAppName("medGUISettingsAdvancedTest"); // Set the name for the application
}

//----------------------------------------------------------------------------
void medWizardManagerTest::tearDown()
  //----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
  cppDEL(m_App);
}

//----------------------------------------------------------------------------
void medWizardManagerTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);

  medWizardManager manager=medWizardManager();
  
  manager.WarningIfCantUndo(true);
  CPPUNIT_ASSERT(manager.GetWarningIfCantUndo()==true);
  
  manager.WarningIfCantUndo(false);
  CPPUNIT_ASSERT(manager.GetWarningIfCantUndo()==false);

  cppDEL(config);

}



//----------------------------------------------------------------------------
void medWizardManagerTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{

  wxConfig *config = new wxConfig(wxEmptyString);

  medWizardManager *manager;

  manager = new medWizardManager();

  delete  manager;

  cppDEL(config);
}

//----------------------------------------------------------------------------
void medWizardManagerTest::TestAddWizard()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);

  medWizard *wizard1=new medWizard("label1","name1");
  medWizard *wizard2=new medWizard("label2","name2");
  medWizard *wizard3=new medWizard("label3","name3");
  std::vector<medWizard *> wizardList;

  medWizardManager manager=medWizardManager();

  //wizard will be deleted from wizard manager
  manager.WizardAdd(wizard1,"wizard1");
  manager.WizardAdd(wizard2,"wizard2");
  manager.WizardAdd(wizard3,"wizard3");

  wizardList=manager.GetWizardList();

  CPPUNIT_ASSERT(wizardList[0]->GetName()=="name1");
  CPPUNIT_ASSERT(wizardList[1]->GetLabel()=="label2");
  CPPUNIT_ASSERT(wizardList[2]->GetMenuPath()=="wizard3");

  cppDEL(config);
}

