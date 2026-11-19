/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionBehaviorTest
 Authors: Alberto Losi
 
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
#include "albaInteractionBehaviorTest.h"
#include "albaInteractionBehavior.h"
#include "albaEvent.h"
#include "vtkRenderer.h"

//----------------------------------------------------------------------------
class albaInteractionBehaviorDummy : public albaInteractionBehavior // concrete class for test
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaInteractionBehaviorDummy, albaInteractionBehavior);

  /** The left button sends the click with modifier, if enabled */
  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) {return SendModifierClick();}
};

albaCxxTypeMacro(albaInteractionBehaviorDummy);

//----------------------------------------------------------------------------
class albaInteractionBehaviorTestListener : public albaObserver // stores the clicks with modifier
//----------------------------------------------------------------------------
{
public:
  albaInteractionBehaviorTestListener() {m_ClickCount = 0; m_ClickKey = -1; m_ClickSender = NULL;};
  virtual void OnEvent(albaEventBase *event)
  {
    albaEvent *e = albaEvent::SafeDownCast(event);
    if (e && e->GetId() == ID_CLICK_ON_VME)
    {
      m_ClickCount++;
      m_ClickKey = e->GetArg();
      m_ClickSender = e->GetSender();
    }
  };

  int m_ClickCount;
  long m_ClickKey;
  void *m_ClickSender;
};

//----------------------------------------------------------------------------
void albaInteractionBehaviorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaInteractionBehaviorTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaInteractionBehaviorDummy *behavior;
  albaNEW(behavior);
  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaInteractionBehaviorTest::TestSetGetRenderer()
//----------------------------------------------------------------------------
{
  vtkRenderer *renderer;
  vtkNEW(renderer);

  albaInteractionBehaviorDummy *behavior;
  albaNEW(behavior);

  // renderer and view start empty (they belong to albaInteractionAgent)
  CPPUNIT_ASSERT(behavior->GetRenderer() == NULL);
  CPPUNIT_ASSERT(behavior->GetView() == NULL);
  CPPUNIT_ASSERT(behavior->IsA("albaInteractionTool"));

  behavior->SetRendererAndView(renderer, NULL);
  CPPUNIT_ASSERT(behavior->GetRenderer() == renderer);

  vtkDEL(renderer);
  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaInteractionBehaviorTest::TestButtonsNotUsed()
//----------------------------------------------------------------------------
{
  albaInteractionBehaviorDummy *behavior;
  albaNEW(behavior);
  double pos[2] = {10, 20};

  // the methods not redefined leave the buttons to the camera
  CPPUNIT_ASSERT(!behavior->OnMiddleButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnRightButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnDrag(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnLeftButtonUp(NULL, NULL, pos));

  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaInteractionBehaviorTest::TestModifierClick()
//----------------------------------------------------------------------------
{
  albaInteractionBehaviorDummy *behavior;
  albaNEW(behavior);
  albaInteractionBehaviorTestListener listener;
  behavior->SetListener(&listener);
  double pos[2] = {10, 20};

  // disabled by default
  behavior->SetModifierKeys(true, false, false);
  CPPUNIT_ASSERT(!behavior->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(listener.m_ClickCount == 0);

  // CTRL, then SHIFT, which wins over CTRL
  behavior->EnableModifierClicks(true);
  CPPUNIT_ASSERT(behavior->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(listener.m_ClickCount == 1 && listener.m_ClickKey == ALBA_CTRL_KEY && listener.m_ClickSender == behavior);
  behavior->SetModifierKeys(true, false, true);
  CPPUNIT_ASSERT(behavior->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(listener.m_ClickCount == 2 && listener.m_ClickKey == ALBA_SHIFT_KEY);

  // ALT alone or no modifiers: no click
  behavior->SetModifierKeys(false, true, false);
  CPPUNIT_ASSERT(!behavior->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(listener.m_ClickCount == 2);

  albaDEL(behavior);
}
