/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardTest
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

#include "albaWizardTest.h"
#include "albaWizard.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void albaWizardTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  albaWizard wizard=albaWizard("Test Wizard","testWizard");

  CPPUNIT_ASSERT(wizard.GetName()=="testWizard");

  CPPUNIT_ASSERT(wizard.GetLabel()=="Test Wizard");

  wizard.SetMenuPath("TestMenu");
  CPPUNIT_ASSERT(wizard.GetMenuPath()=="TestMenu");

  wizard.ShowProgressBar(true);
  CPPUNIT_ASSERT(wizard.GetShowProgressBar()==true);
  
}

//----------------------------------------------------------------------------
void albaWizardTest::TestDynamicAllocation()
  //----------------------------------------------------------------------------
{
  albaWizard *wizard;

  wizard = new albaWizard("block");

  delete  wizard;
}

