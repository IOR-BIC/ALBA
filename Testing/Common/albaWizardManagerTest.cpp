/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardManagerTest
 Authors: Gianluigi Crimi
 
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

#include "albaCommonTests.h"
#include "albaWizardManagerTest.h"
#include "albaWizardSettings.h"
#include "albaWizardManager.h"
#include <cppunit/config/SourcePrefix.h>
#include "albaWizard.h"

//----------------------------------------------------------------------------
void albaWizardManagerTest::AfterTest()
  //----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
}

//----------------------------------------------------------------------------
void albaWizardManagerTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);

  albaWizardManager manager=albaWizardManager();
  
  manager.WarningIfCantUndo(true);
  CPPUNIT_ASSERT(manager.GetWarningIfCantUndo()==true);
  
  manager.WarningIfCantUndo(false);
  CPPUNIT_ASSERT(manager.GetWarningIfCantUndo()==false);

  cppDEL(config);

}



//----------------------------------------------------------------------------
void albaWizardManagerTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{

  wxConfig *config = new wxConfig(wxEmptyString);

  albaWizardManager *manager;

  manager = new albaWizardManager();

  delete  manager;

  cppDEL(config);
}

//----------------------------------------------------------------------------
void albaWizardManagerTest::TestAddWizard()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);

  albaWizard *wizard1=new albaWizard("label1","name1");
  albaWizard *wizard2=new albaWizard("label2","name2");
  albaWizard *wizard3=new albaWizard("label3","name3");
  std::vector<albaWizard *> wizardList;

  albaWizardManager manager=albaWizardManager();

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

