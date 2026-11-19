/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSelectToolTest
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
#include "albaSelectToolTest.h"
#include "albaSelectTool.h"
#include "albaEventBase.h"
#include "albaVMEGroup.h"
#include "albaVMEGizmo.h"

//----------------------------------------------------------------------------
class albaSelectToolTestListener : public albaObserver // stores the events of the select tool
//----------------------------------------------------------------------------
{
public:
  albaSelectToolTestListener() {Clear();};
  void Clear() {m_SelectCount = m_MenuCount = 0; m_Data = NULL; m_Channel = -1;};
  virtual void OnEvent(albaEventBase *event)
  {
    if (event->GetId() == VME_SELECT)
      m_SelectCount++;
    else if (event->GetId() == SHOW_VIEW_CONTEXTUAL_MENU)
      m_MenuCount++;
    m_Data = event->GetData();
    m_Channel = event->GetChannel();
  };

  int    m_SelectCount;
  int    m_MenuCount;
  void  *m_Data;
  albaID m_Channel;
};

//----------------------------------------------------------------------------
void albaSelectToolTest::TestSelect()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> vme;
  albaSelectTool *tool;
  albaNEW(tool);
  albaSelectToolTestListener listener;
  tool->SetListener(&listener);
  double picked[3] = {1, 2, 3};
  double pos[2] = {10, 20};

  // every button down on a VME selects it, on MCH_UP with the VME as data; the button is not used
  tool->SetPickedVME(vme, picked);
  CPPUNIT_ASSERT(tool->GetPickedVME() == vme.GetPointer());
  CPPUNIT_ASSERT(tool->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(listener.m_SelectCount == 1 && listener.m_Data == vme.GetPointer() && listener.m_Channel == MCH_UP);
  CPPUNIT_ASSERT(tool->OnMiddleButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(tool->OnRightButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(tool->OnTrackerButtonDown(NULL, ALBA_LEFT_BUTTON, NULL));
  CPPUNIT_ASSERT(listener.m_SelectCount == 4);

  albaDEL(tool);
}
//----------------------------------------------------------------------------
void albaSelectToolTest::TestNotSelectable()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGizmo> gizmo;
  albaSelectTool *tool;
  albaNEW(tool);
  albaSelectToolTestListener listener;
  tool->SetListener(&listener);
  double pos[2] = {10, 20};

  // nothing picked, or a gizmo: no selection
  tool->SetPickedVME(NULL, NULL);
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(NULL, NULL, pos));
  tool->SetPickedVME(gizmo, NULL);
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(listener.m_SelectCount == 0);

  albaDEL(tool);
}
//----------------------------------------------------------------------------
void albaSelectToolTest::TestEnableSelect()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> vme;
  albaSelectTool *tool;
  albaNEW(tool);
  albaSelectToolTestListener listener;
  tool->SetListener(&listener);
  double pos[2] = {10, 20};
  tool->SetPickedVME(vme, NULL);

  CPPUNIT_ASSERT(tool->IsSelectEnabled());
  tool->EnableSelect(false);
  CPPUNIT_ASSERT(!tool->IsSelectEnabled());
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(listener.m_SelectCount == 0);

  albaDEL(tool);
}
//----------------------------------------------------------------------------
void albaSelectToolTest::TestContextMenu()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> vme;
  albaSelectTool *tool;
  albaNEW(tool);
  albaSelectToolTestListener listener;
  tool->SetListener(&listener);
  double pos[2] = {10, 20};

  // the right click shows the context menu with the picked VME, also when nothing is picked
  tool->SetPickedVME(vme, NULL);
  tool->OnRightButtonDown(NULL, NULL, pos);
  CPPUNIT_ASSERT(tool->OnRightButtonUp(NULL, NULL, pos));
  CPPUNIT_ASSERT(listener.m_MenuCount == 1 && listener.m_Data == vme.GetPointer());
  tool->SetPickedVME(NULL, NULL);
  tool->OnRightButtonDown(NULL, NULL, pos);
  CPPUNIT_ASSERT(tool->OnRightButtonUp(NULL, NULL, pos));
  CPPUNIT_ASSERT(listener.m_MenuCount == 2 && listener.m_Data == NULL);

  // a drag cancels it, and a release without the right button down does not show it
  tool->OnRightButtonDown(NULL, NULL, pos);
  CPPUNIT_ASSERT(!tool->OnDrag(NULL, NULL, pos));
  CPPUNIT_ASSERT(!tool->OnRightButtonUp(NULL, NULL, pos));
  CPPUNIT_ASSERT(!tool->OnRightButtonUp(NULL, NULL, pos));
  CPPUNIT_ASSERT(listener.m_MenuCount == 2);

  albaDEL(tool);
}
