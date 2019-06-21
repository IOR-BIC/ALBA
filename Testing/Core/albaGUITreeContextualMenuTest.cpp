/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITreeContextualMenuTest
 Authors: Paolo Quadrani
 
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
#include "albaGUITreeContextualMenuTest.h"
#include "albaGUITreeContextualMenu.h"

#include "albaViewVTK.h"

#include "albaVMESurface.h"
#include "albaOpManager.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaGUITreeContextualMenuTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaGUITreeContextualMenuTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaGUITreeContextualMenu menu;
}

//----------------------------------------------------------------------------
void albaGUITreeContextualMenuTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaGUITreeContextualMenu *menu = new albaGUITreeContextualMenu();
  delete menu;
}

//----------------------------------------------------------------------------
void albaGUITreeContextualMenuTest::TestCreateContextualMenu()
//----------------------------------------------------------------------------
{
  albaViewVTK *view = new albaViewVTK();
  albaVMESurface *vme = albaVMESurface::New();

	albaOpManager *opManager = new albaOpManager();

  // create contextual menu for VME with one existing view.
  albaGUITreeContextualMenu *menu1 = new albaGUITreeContextualMenu();
	menu1->SetOpManager(opManager);
  menu1->CreateContextualMenu(NULL, view, vme, true);

 
  // Exists one active view.
//  result = menu1->FindItem("Hide/Show") != wxNOT_FOUND;
//  TEST_RESULT;

  // Contextual menu with view active, but no one VME selected.
  // Should contain only the "Keep tree nodes sorted" menu item.
  albaGUITreeContextualMenu *menu2 = new albaGUITreeContextualMenu();
	menu2->SetOpManager(opManager);
  menu2->CreateContextualMenu(NULL, view, vme, false);
	

  // Contextual menu for VME with no one view created.
  albaGUITreeContextualMenu *menu3 = new albaGUITreeContextualMenu();
	menu3->SetOpManager(opManager);
  menu3->CreateContextualMenu(NULL, NULL, vme, true);
	

  // No one view created.
  result = menu3->FindItem("Hide/Show") == wxNOT_FOUND;
  TEST_RESULT;

  delete view;
  albaDEL(vme);

  delete menu1;
  delete menu2;
  delete menu3;

	cppDEL(opManager);
}
