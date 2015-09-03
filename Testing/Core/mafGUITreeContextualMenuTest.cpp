/*=========================================================================

 Program: MAF2
 Module: mafGUITreeContextualMenuTest
 Authors: Paolo Quadrani
 
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
#include "mafGUITreeContextualMenuTest.h"
#include "mafGUITreeContextualMenu.h"

#include "mafViewVTK.h"

#include "mafVMESurface.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafGUITreeContextualMenuTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafGUITreeContextualMenuTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafGUITreeContextualMenu menu;
}

//----------------------------------------------------------------------------
void mafGUITreeContextualMenuTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafGUITreeContextualMenu *menu = new mafGUITreeContextualMenu();
  delete menu;
}

//----------------------------------------------------------------------------
void mafGUITreeContextualMenuTest::TestCreateContextualMenu()
//----------------------------------------------------------------------------
{
  mafViewVTK *view = new mafViewVTK();
  mafVMESurface *vme = mafVMESurface::New();

  // create contextual menu for VME with one existing view.
  mafGUITreeContextualMenu *menu1 = new mafGUITreeContextualMenu();
  menu1->CreateContextualMenu(NULL, view, vme, true);

  result = menu1->FindItem("Save MSF Layout") != wxNOT_FOUND;
  TEST_RESULT;

  // Exists one active view.
//  result = menu1->FindItem("Hide/Show") != wxNOT_FOUND;
//  TEST_RESULT;

  // Contextual menu with view active, but no one VME selected.
  // Should contain only the "Keep tree nodes sorted" menu item.
  mafGUITreeContextualMenu *menu2 = new mafGUITreeContextualMenu();
  menu2->CreateContextualMenu(NULL, view, vme, false);

  result = menu2->FindItem("Save MSF Layout") == wxNOT_FOUND;
  TEST_RESULT;

  // Contextual menu for VME with no one view created.
  mafGUITreeContextualMenu *menu3 = new mafGUITreeContextualMenu();
  menu3->CreateContextualMenu(NULL, NULL, vme, true);

  result = menu3->FindItem("Save MSF Layout") != wxNOT_FOUND;
  TEST_RESULT;

  // No one view created.
  result = menu3->FindItem("Hide/Show") == wxNOT_FOUND;
  TEST_RESULT;

  delete view;
  mafDEL(vme);

  delete menu1;
  delete menu2;
  delete menu3;
}
