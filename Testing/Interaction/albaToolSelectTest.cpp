/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSelectTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaToolSelectTest.h"
#include "albaToolSelect.h"
#include "albaEventBase.h"
#include "albaVMEGroup.h"
#include "albaVMEGizmo.h"
#include "albaServiceLocator.h"
#include "albaFakeLogicForTest.h"

//----------------------------------------------------------------------------
void albaToolSelectTest::TestSelect()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> vme;
  albaToolSelect *tool;
  albaNEW(tool);
  albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
  logic->ClearCalls();
  double picked[3] = {1, 2, 3};
  double pos[2] = {10, 20};

  // every button down on a VME asks the logic to select it; the button is not used
  tool->SetPickedVME(vme, picked);
  CPPUNIT_ASSERT(tool->GetPickedVME() == vme.GetPointer());
  CPPUNIT_ASSERT(tool->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(logic->GetNumberOfCalls() == 1 && logic->GetCall(0).testFunction == albaFakeLogicForTest::FKL_VME_SELECT && logic->GetCall(0).vme == vme.GetPointer());
  CPPUNIT_ASSERT(tool->OnMiddleButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(tool->OnRightButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(logic->GetNumberOfCalls() == 3);

  albaDEL(tool);
}
//----------------------------------------------------------------------------
void albaToolSelectTest::TestNotSelectable()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGizmo> gizmo;
  albaToolSelect *tool;
  albaNEW(tool);
  albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
  logic->ClearCalls();
  double pos[2] = {10, 20};

  // nothing picked, or a gizmo: no selection
  tool->SetPickedVME(NULL, NULL);
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(NULL, NULL, pos));
  tool->SetPickedVME(gizmo, NULL);
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(logic->GetNumberOfCalls() == 0);

  albaDEL(tool);
}
//----------------------------------------------------------------------------
void albaToolSelectTest::TestEnableSelect()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> vme;
  albaToolSelect *tool;
  albaNEW(tool);
  albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
  logic->ClearCalls();
  double pos[2] = {10, 20};
  tool->SetPickedVME(vme, NULL);

  CPPUNIT_ASSERT(tool->IsSelectEnabled());
  tool->EnableSelect(false);
  CPPUNIT_ASSERT(!tool->IsSelectEnabled());
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(logic->GetNumberOfCalls() == 0);

  albaDEL(tool);
}
//----------------------------------------------------------------------------
void albaToolSelectTest::TestContextMenu()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> vme;
  albaSmartPointer<albaVMEGizmo> gizmo;
  albaToolSelect *tool;
  albaNEW(tool);
  albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
  logic->ClearCalls();
  double pos[2] = {10, 20};
  albaFakeLogicForTest::testFunctions menu = albaFakeLogicForTest::FKL_SHOW_VIEW_CONTEXTUAL_MENU;

  // the right click shows the context menu of the view, with the VME items only for a selectable VME
  tool->SetPickedVME(vme, NULL);
  tool->OnRightButtonDown(NULL, NULL, pos);
  CPPUNIT_ASSERT(tool->OnRightButtonUp(NULL, NULL, pos));
  CPPUNIT_ASSERT(logic->GetNumberOfCalls(menu) == 1 && logic->GetViewContextualMenuWithVME());
  tool->SetPickedVME(NULL, NULL);
  tool->OnRightButtonDown(NULL, NULL, pos);
  CPPUNIT_ASSERT(tool->OnRightButtonUp(NULL, NULL, pos));
  CPPUNIT_ASSERT(logic->GetNumberOfCalls(menu) == 2 && !logic->GetViewContextualMenuWithVME());
  tool->SetPickedVME(gizmo, NULL);
  tool->OnRightButtonDown(NULL, NULL, pos);
  CPPUNIT_ASSERT(tool->OnRightButtonUp(NULL, NULL, pos));
  CPPUNIT_ASSERT(logic->GetNumberOfCalls(menu) == 3 && !logic->GetViewContextualMenuWithVME());

  // a drag cancels it, and a release without the right button down does not show it
  tool->OnRightButtonDown(NULL, NULL, pos);
  CPPUNIT_ASSERT(!tool->OnDrag(NULL, NULL, pos));
  CPPUNIT_ASSERT(!tool->OnRightButtonUp(NULL, NULL, pos));
  CPPUNIT_ASSERT(!tool->OnRightButtonUp(NULL, NULL, pos));
  CPPUNIT_ASSERT(logic->GetNumberOfCalls(menu) == 3);

  albaDEL(tool);
}
