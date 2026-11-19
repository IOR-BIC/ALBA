/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaBehaviorRotateTranslateTest
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
#include "albaBehaviorRotateTranslateTest.h"
#include "albaBehaviorRotateTranslate.h"
#include "albaBehaviorProsthesis.h"
#include "albaBehaviorCutPlane.h"
#include "albaInteractorConstraint.h"
#include "albaRefSys.h"
#include "albaEvent.h"
#include "albaVMEGroup.h"

// Expose the transform chosen by PrepareTransform for a button
#define ALBA_TEST_PREPARE_TRANSFORM(className, baseClass) \
class className : public baseClass \
{ \
public: \
  albaTypeMacro(className, baseClass); \
  bool Prepare(int button) {m_CurrentRotationConstraint = m_RotationConstraint; return PrepareTransform(button);} \
  bool IsRotating() {return m_RotationFlag && !m_TranslationFlag && m_CurrentRotationConstraint == m_RotationConstraint;} \
  bool IsRolling() {return m_RotationFlag && !m_TranslationFlag && m_CurrentRotationConstraint != m_RotationConstraint;} \
  bool IsTranslating() {return m_TranslationFlag && !m_RotationFlag;} \
}; \
albaCxxTypeMacro(className);

ALBA_TEST_PREPARE_TRANSFORM(albaTestRotateTranslateBehavior, albaBehaviorRotateTranslate)
ALBA_TEST_PREPARE_TRANSFORM(albaTestProsthesisBehavior, albaBehaviorProsthesis)
ALBA_TEST_PREPARE_TRANSFORM(albaTestCutPlaneBehavior, albaBehaviorCutPlane)

//----------------------------------------------------------------------------
class albaBehaviorRotateTranslateTestListener : public albaObserver // stores the clicks with modifier
//----------------------------------------------------------------------------
{
public:
  albaBehaviorRotateTranslateTestListener() {m_ClickCount = 0; m_ClickKey = -1; m_ClickSender = NULL;};
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
void albaBehaviorRotateTranslateTest::TestRotateTranslateButtons()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaTestRotateTranslateBehavior *behavior;
  albaNEW(behavior);
  behavior->SetVME(group);

  // left button rotates, with CTRL rolls; SHIFT is not used
  CPPUNIT_ASSERT(behavior->Prepare(ALBA_LEFT_BUTTON) && behavior->IsRotating());
  behavior->SetModifierKeys(true, false, false);
  CPPUNIT_ASSERT(behavior->Prepare(ALBA_LEFT_BUTTON) && behavior->IsRolling());
  behavior->EnableRoll(false);
  CPPUNIT_ASSERT(!behavior->Prepare(ALBA_LEFT_BUTTON));
  behavior->SetModifierKeys(false, false, true);
  CPPUNIT_ASSERT(!behavior->Prepare(ALBA_LEFT_BUTTON));

  // middle button translates, not with CTRL or SHIFT; right button not used
  CPPUNIT_ASSERT(!behavior->Prepare(ALBA_MIDDLE_BUTTON));
  behavior->SetModifierKeys(true, false, false);
  CPPUNIT_ASSERT(!behavior->Prepare(ALBA_MIDDLE_BUTTON));
  behavior->SetModifierKeys(false, false, false);
  CPPUNIT_ASSERT(behavior->Prepare(ALBA_MIDDLE_BUTTON) && behavior->IsTranslating());
  CPPUNIT_ASSERT(!behavior->Prepare(ALBA_RIGHT_BUTTON));

  // without a renderer the buttons are not used
  double pos[2] = {10, 20};
  CPPUNIT_ASSERT(!behavior->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnMiddleButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnDrag(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnLeftButtonUp(NULL, NULL, pos));
  CPPUNIT_ASSERT(!behavior->OnMiddleButtonUp(NULL, NULL, pos));

  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaBehaviorRotateTranslateTest::TestRefSysVME()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaSmartPointer<albaVMEGroup> refSys;
  albaBehaviorRotateTranslate *behavior;
  albaNEW(behavior);

  // without VME the constraints cannot be set
  behavior->SetRotationConstraint(albaBehaviorRotateTranslate::X_AXIS);
  CPPUNIT_ASSERT(behavior->GetRotationConstraint()->GetRefSys()->GetType() == albaRefSys::VIEW);

  // the VME gives the default ref sys and pivot
  behavior->SetVME(group);
  albaMatrix *groupMatrix = group->GetOutput()->GetAbsMatrix();
  CPPUNIT_ASSERT(behavior->GetPivotRefSys()->GetType() == albaRefSys::CUSTOM && behavior->GetPivotRefSys()->GetMatrix() == groupMatrix);
  CPPUNIT_ASSERT(behavior->GetRotationConstraint()->GetRefSys()->GetMatrix() == groupMatrix);
  CPPUNIT_ASSERT(behavior->GetTranslationConstraint()->GetRefSys()->GetMatrix() == groupMatrix);

  // the ref sys VME changes ref sys and pivot
  behavior->SetRefSysVME(refSys);
  albaMatrix *refSysMatrix = refSys->GetOutput()->GetAbsMatrix();
  CPPUNIT_ASSERT(behavior->GetPivotRefSys()->GetMatrix() == refSysMatrix);
  CPPUNIT_ASSERT(behavior->GetRotationConstraint()->GetRefSys()->GetMatrix() == refSysMatrix);
  CPPUNIT_ASSERT(behavior->GetTranslationConstraint()->GetRefSys()->GetMatrix() == refSysMatrix);

  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaBehaviorRotateTranslateTest::TestProsthesisButtons()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaTestProsthesisBehavior *behavior;
  albaNEW(behavior);
  behavior->SetVME(group);

  // left button rotates, with CTRL rolls, with SHIFT translates; middle button translates
  CPPUNIT_ASSERT(behavior->Prepare(ALBA_LEFT_BUTTON) && behavior->IsRotating());
  behavior->SetModifierKeys(true, false, false);
  CPPUNIT_ASSERT(behavior->Prepare(ALBA_LEFT_BUTTON) && behavior->IsRolling());
  behavior->SetModifierKeys(false, false, true);
  CPPUNIT_ASSERT(behavior->Prepare(ALBA_LEFT_BUTTON) && behavior->IsTranslating());
  behavior->SetModifierKeys(false, false, false);
  CPPUNIT_ASSERT(behavior->Prepare(ALBA_MIDDLE_BUTTON) && behavior->IsTranslating());
  CPPUNIT_ASSERT(!behavior->Prepare(ALBA_RIGHT_BUTTON));

  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaBehaviorRotateTranslateTest::TestCutPlaneButtons()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaTestCutPlaneBehavior *behavior;
  albaNEW(behavior);
  behavior->SetVME(group);

  // left button rotates, middle button translates, no roll
  CPPUNIT_ASSERT(behavior->Prepare(ALBA_LEFT_BUTTON) && behavior->IsRotating());
  CPPUNIT_ASSERT(behavior->Prepare(ALBA_MIDDLE_BUTTON) && behavior->IsTranslating());
  behavior->SetModifierKeys(true, false, false);
  CPPUNIT_ASSERT(!behavior->Prepare(ALBA_LEFT_BUTTON));
  behavior->SetModifierKeys(false, false, false);

  // with the transform disabled no button is used
  behavior->EnableTransform(false);
  CPPUNIT_ASSERT(!behavior->Prepare(ALBA_LEFT_BUTTON));
  CPPUNIT_ASSERT(!behavior->Prepare(ALBA_MIDDLE_BUTTON));

  albaDEL(behavior);
}
//----------------------------------------------------------------------------
void albaBehaviorRotateTranslateTest::TestCutPlaneClicks()
//----------------------------------------------------------------------------
{
  albaBehaviorCutPlane *behavior;
  albaNEW(behavior);
  albaBehaviorRotateTranslateTestListener listener;
  behavior->SetListener(&listener);
  double pos[2] = {10, 20};

  // disabled by default
  behavior->SetModifierKeys(true, false, false);
  CPPUNIT_ASSERT(!behavior->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(listener.m_ClickCount == 0);

  // CTRL, then SHIFT, which wins over CTRL; also with the transform disabled
  behavior->EnableModifierClicks(true);
  behavior->EnableTransform(false);
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
