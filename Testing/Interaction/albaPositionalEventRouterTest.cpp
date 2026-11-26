/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPositionalEventRouterTest
 Authors: Stefano Perticoni
 
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
#include "albaPositionalEventRouterTest.h"
#include "albaPositionalEventRouter.h"
#include "albaRWIBase.h"
#include "albaVMESurface.h"
#include "albaView.h"
#include "albaEvent.h"
#include "albaBehaviorTransform.h"
#include "albaTransform.h"
#include "albaInteractionTool.h"
#include "albaToolCamera.h"
#include "albaToolSelect.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"

// the mouse events the tests send to the PER
enum TEST_MOUSE_EVENTS
{
  TEST_BUTTON_DOWN,
  TEST_BUTTON_UP,
  TEST_MOUSE_MOVE,
  TEST_MOUSE_WHEEL,
};

//----------------------------------------------------------------------------
class albaFakePickView : public albaView // view counting the picking performed on it
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaFakePickView, albaView);

  albaFakePickView() : albaView("fakePickView")
  {
    m_PickCount = 0;
    m_FakeVme = NULL;
    m_FakePosition[0] = 1.0; m_FakePosition[1] = 2.0; m_FakePosition[2] = 3.0;
    m_LastPickX = m_LastPickY = -1;
  };

  virtual void SetBackgroundColor(wxColor color) {};

  virtual bool Pick(int x, int y)
  {
    m_PickCount++;
    m_LastPickX = x;
    m_LastPickY = y;
    m_PickedVME = m_FakeVme;
    m_PickedPosition[0] = m_FakePosition[0]; m_PickedPosition[1] = m_FakePosition[1]; m_PickedPosition[2] = m_FakePosition[2];
    return m_FakeVme != NULL;
  };

  int      m_PickCount;
  int      m_LastPickX;
  int      m_LastPickY;
  albaVME *m_FakeVme;
  double   m_FakePosition[3];
};

albaCxxTypeMacro(albaFakePickView);

//----------------------------------------------------------------------------
class albaPERTestTransformListener : public albaObserver // stores the picked position sent at mouse down
//----------------------------------------------------------------------------
{
public:
  albaPERTestTransformListener() {m_MouseDownCount = 0; m_PickedPosition[0] = m_PickedPosition[1] = m_PickedPosition[2] = 0.0;};
  virtual void OnEvent(albaEventBase *event)
  {
    albaEvent *e = albaEvent::SafeDownCast(event);
    if (e && e->GetId() == ID_TRANSFORM && e->GetArg() == albaBehaviorTransform::MOUSE_ACTION_DOWN)
    {
      m_MouseDownCount++;
      albaTransform::GetPosition(*albaMatrix::SafeDownCast(e->GetMafObject()), m_PickedPosition);
    }
  };

  int    m_MouseDownCount;
  double m_PickedPosition[3];
};

//----------------------------------------------------------------------------
class albaPERTestTool : public albaInteractionTool // counts the calls and uses the events as configured
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaPERTestTool, albaInteractionTool);

  albaPERTestTool()
  {
    m_UseLeft = m_UseRight = m_UseMove = m_UseWheel = false;
    m_UseDrag = true;
    m_LeftDown = m_LeftUp = m_RightDown = m_RightUp = m_Drag = m_Move = m_Wheel = m_Push = m_Pop = 0;
    m_LastPos[0] = m_LastPos[1] = m_LastRotation = 0;
  };

  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) {m_LeftDown++; return m_UseLeft;};
  virtual bool OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) {m_LeftUp++; return true;};
  virtual bool OnRightButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) {m_RightDown++; return m_UseRight;};
  virtual bool OnRightButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) {m_RightUp++; return true;};
  virtual bool OnDrag(albaView *view, vtkRenderer *ren, double pos[2]) {m_Drag++; return m_UseDrag;};
  virtual bool OnMove(albaView *view, vtkRenderer *ren, double pos[2]) {m_Move++; m_LastPos[0] = pos[0]; m_LastPos[1] = pos[1]; return m_UseMove;};
  virtual bool OnWheel(albaView *view, vtkRenderer *ren, double pos[2], double rotation) {m_Wheel++; m_LastRotation = rotation; return m_UseWheel;};
  virtual void OnPush() {m_Push++;};
  virtual void OnPop() {m_Pop++;};

  bool m_UseLeft, m_UseRight, m_UseMove, m_UseWheel, m_UseDrag;
  int m_LeftDown, m_LeftUp, m_RightDown, m_RightUp, m_Drag, m_Move, m_Wheel, m_Push, m_Pop;
  double m_LastPos[2], m_LastRotation;
};

albaCxxTypeMacro(albaPERTestTool);

//----------------------------------------------------------------------------
class albaPERTestModalTool : public albaPERTestTool // the button down receives the button up, as when it opens a modal dialog
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaPERTestModalTool, albaPERTestTool);

  albaPERTestModalTool() {m_PER = NULL; m_RWI = NULL; m_UseLeft = true;};

  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
  {
    m_LeftDown++;
    m_PER->OnLeftButtonUp(m_RWI, pos);
    return m_UseLeft;
  };

  albaPositionalEventRouter *m_PER;
  albaRWIBase *m_RWI;
};

albaCxxTypeMacro(albaPERTestModalTool);

//----------------------------------------------------------------------------
class albaPERTestStoppingListener : public albaObserver // releases the button as soon as the transform starts
//----------------------------------------------------------------------------
{
public:
  albaPERTestStoppingListener() {m_Transform = NULL; m_MouseUpCount = 0;};
  virtual void OnEvent(albaEventBase *event)
  {
    albaEvent *e = albaEvent::SafeDownCast(event);
    if (e == NULL || e->GetId() != ID_TRANSFORM)
      return;
    double pos[2] = {10, 20};
    if (e->GetArg() == albaBehaviorTransform::MOUSE_ACTION_DOWN)
      m_Transform->OnLeftButtonUp(NULL, NULL, pos);
    else if (e->GetArg() == albaBehaviorTransform::MOUSE_ACTION_UP)
      m_MouseUpCount++;
  };

  albaBehaviorTransform *m_Transform;
  int m_MouseUpCount;
};

//----------------------------------------------------------------------------
class albaPERTestBehavior : public albaInteractionBehavior // records the calls of the PER
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaPERTestBehavior, albaInteractionBehavior);

  albaPERTestBehavior() {m_UseLeft = true; m_Interacting = false; m_Drag = 0;};

  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) {m_Interacting = m_UseLeft; return m_UseLeft;};
  virtual bool OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) {m_Interacting = false; return true;};
  virtual bool OnDrag(albaView *view, vtkRenderer *ren, double pos[2]) {m_Drag++; return true;};

  double *GetPickedPosition() {return m_PickedPosition;};

  bool m_UseLeft, m_Interacting;
  int m_Drag;
};

albaCxxTypeMacro(albaPERTestBehavior);

//----------------------------------------------------------------------------
class albaPERTestMouse // the render window sending the mouse events to the PER
//----------------------------------------------------------------------------
{
public:
  albaPERTestMouse() {m_RWI = albaRWIBase::New();};
  ~albaPERTestMouse() {m_RWI->Delete();};

  albaRWIBase *m_RWI;  ///< created without a window: the renderer under the mouse is NULL
};

//----------------------------------------------------------------------------
static void SendToPER(albaPositionalEventRouter *per, albaPERTestMouse *mouse, albaID id, int button = ALBA_LEFT_BUTTON, double rotation = 0)
//----------------------------------------------------------------------------
{
  // the render window sets the modifier keys and calls the method of the event, at the position (10, 20)
  double pos[2] = {10, 20};
  per->SetModifierKeys(false, false, false);
  switch (id)
  {
    case TEST_BUTTON_DOWN:
      switch (button)
      {
        case ALBA_LEFT_BUTTON:   per->OnLeftButtonDown(mouse->m_RWI, pos);   break;
        case ALBA_MIDDLE_BUTTON: per->OnMiddleButtonDown(mouse->m_RWI, pos); break;
        case ALBA_RIGHT_BUTTON:  per->OnRightButtonDown(mouse->m_RWI, pos);  break;
      }
      break;
    case TEST_BUTTON_UP:
      switch (button)
      {
        case ALBA_LEFT_BUTTON:   per->OnLeftButtonUp(mouse->m_RWI, pos);   break;
        case ALBA_MIDDLE_BUTTON: per->OnMiddleButtonUp(mouse->m_RWI, pos); break;
        case ALBA_RIGHT_BUTTON:  per->OnRightButtonUp(mouse->m_RWI, pos);  break;
      }
      break;
    case TEST_MOUSE_MOVE: per->OnMouseMove(mouse->m_RWI, pos); break;
    case TEST_MOUSE_WHEEL:   per->OnMouseWheel(mouse->m_RWI, pos, rotation); break;
  }
}

//----------------------------------------------------------------------------
static void SelectView(albaPERTestMouse *mouse, albaView *view)
//----------------------------------------------------------------------------
{
  mouse->m_RWI->SetView(view);
}

void albaPositionalEventRouterTest::TestFixture()
{

}

void albaPositionalEventRouterTest::TestConstructorDestructor()
{
  albaPositionalEventRouter *mip = albaPositionalEventRouter::New();
  CPPUNIT_ASSERT(mip);
  mip->Delete();
}

void albaPositionalEventRouterTest::TestEnableSelect()
{
  albaPositionalEventRouter *mip = albaPositionalEventRouter::New();
  CPPUNIT_ASSERT(mip);

  // the selection belongs to the select tool of the PER
  CPPUNIT_ASSERT(mip->GetSelectTool()->IsSelectEnabled());
  mip->EnableSelect(false);
  CPPUNIT_ASSERT(!mip->GetSelectTool()->IsSelectEnabled());

  mip->Delete();
}

void albaPositionalEventRouterTest::TestIsInteracting()
{
  albaPositionalEventRouter *mip = albaPositionalEventRouter::New();
  CPPUNIT_ASSERT(mip);

  bool isInteracting = mip->IsInteracting();
  CPPUNIT_ASSERT(isInteracting == false);
  mip->Delete();
}

void albaPositionalEventRouterTest::TestOnEvent()
{
  // mostly to test for leaks: a move without view and without tools
  albaPositionalEventRouter *mip = albaPositionalEventRouter::New();
  CPPUNIT_ASSERT(mip);
  albaPERTestMouse *mouse = new albaPERTestMouse();

  SendToPER(mip, mouse, TEST_MOUSE_MOVE);
  CPPUNIT_ASSERT(!mip->IsInteracting());

  delete mouse;
  mip->Delete();
}

void albaPositionalEventRouterTest::TestGetCameraTool()
{
  albaPositionalEventRouter *mip = albaPositionalEventRouter::New();

  albaToolCamera *camera = mip->GetCameraTool();
  CPPUNIT_ASSERT(camera != NULL);

  // the camera tool is not part of the tool stack
  CPPUNIT_ASSERT(mip->GetNumberOfTools() == 0);

  mip->Delete();
}

void albaPositionalEventRouterTest::TestButtonDownPicksAtPosition()
{
  albaFakePickView *view = new albaFakePickView();
  albaPERTestMouse *mouse = new albaPERTestMouse();
  SelectView(mouse, view);
  albaVMESurface *vme = albaVMESurface::New();
  view->m_FakeVme = vme;
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();

  // the button down picks the view at its position, and the select tool gets the picking
  SendToPER(per, mouse, TEST_BUTTON_DOWN, ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(view->m_PickCount == 1);
  CPPUNIT_ASSERT(view->m_LastPickX == 10 && view->m_LastPickY == 20);
  CPPUNIT_ASSERT(per->GetSelectTool()->GetPickedVME() == vme);

  // the drags, the button up and the wheel do not pick
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  SendToPER(per, mouse, TEST_BUTTON_UP, ALBA_RIGHT_BUTTON);
  SendToPER(per, mouse, TEST_MOUSE_WHEEL);
  CPPUNIT_ASSERT(view->m_PickCount == 1);

  // nothing picked: the select tool gets no VME
  view->m_FakeVme = NULL;
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(view->m_PickCount == 2);
  CPPUNIT_ASSERT(per->GetSelectTool()->GetPickedVME() == NULL);

  per->Delete();
  vme->Delete();
  delete mouse;
  delete view;
}
void albaPositionalEventRouterTest::TestButtonDownPicksOnce()
{
  albaFakePickView *view = new albaFakePickView();
  albaPERTestMouse *mouse = new albaPERTestMouse();
  SelectView(mouse, view);

  // the picked VME has a behavior, like gizmos
  albaPERTestBehavior *behavior;
  albaNEW(behavior);
  albaVMESurface *vme = albaVMESurface::New();
  vme->SetBehavior(behavior);
  view->m_FakeVme = vme;

  albaPositionalEventRouter *per = albaPositionalEventRouter::New();

  SendToPER(per, mouse, TEST_BUTTON_DOWN);

  // the select tool and the behavior share the same picking
  CPPUNIT_ASSERT(per->GetSelectTool()->GetPickedVME() == vme);
  CPPUNIT_ASSERT(behavior->m_Interacting);
  CPPUNIT_ASSERT(per->GetInteractingTool() == behavior);
  CPPUNIT_ASSERT(!per->GetCameraTool()->IsInteracting());
  CPPUNIT_ASSERT(view->m_PickCount == 1);

  SendToPER(per, mouse, TEST_BUTTON_UP);

  CPPUNIT_ASSERT(!behavior->m_Interacting);
  CPPUNIT_ASSERT(view->m_PickCount == 1);

  // the PER gives the picked position to the behavior
  double *pickedPos = behavior->GetPickedPosition();
  CPPUNIT_ASSERT(pickedPos[0] == 1.0 && pickedPos[1] == 2.0 && pickedPos[2] == 3.0);

  // a button the behavior does not use goes to the camera, and the drags do not reach the behavior
  behavior->m_UseLeft = false;
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  CPPUNIT_ASSERT(per->IsInteracting() && per->GetInteractingTool() == NULL);
  CPPUNIT_ASSERT(per->GetCameraTool()->IsInteracting());
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  CPPUNIT_ASSERT(behavior->m_Drag == 0);
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(!per->GetCameraTool()->IsInteracting());

  per->Delete();
  vme->SetBehavior(NULL);
  vme->Delete();
  albaDEL(behavior);
  delete mouse;
  delete view;
}

void albaPositionalEventRouterTest::TestGenericMouseUsesButtonDownPick()
{
  albaFakePickView *view = new albaFakePickView();
  albaPERTestMouse *mouse = new albaPERTestMouse();
  SelectView(mouse, view);
  albaVMESurface *vme = albaVMESurface::New();
  view->m_FakeVme = vme;

  albaBehaviorTransform *generic = albaBehaviorTransform::New();
  albaPERTestTransformListener listener;
  generic->SetListener(&listener);
  generic->SetRendererAndView(NULL, view);

  // the button down uses the position picked by the PER: no picking on the view
  double pickedPos[3] = {7.0, 8.0, 9.0};
  generic->SetPickedPosition(pickedPos);
  generic->OnButtonDownAction(10, 20);

  CPPUNIT_ASSERT(view->m_PickCount == 0);
  CPPUNIT_ASSERT(listener.m_MouseDownCount == 1);
  CPPUNIT_ASSERT(listener.m_PickedPosition[0] == 7.0 && listener.m_PickedPosition[1] == 8.0 && listener.m_PickedPosition[2] == 9.0);

  generic->Delete();
  vme->Delete();
  delete mouse;
  delete view;
}

void albaPositionalEventRouterTest::TestTransformBehaviorButtons()
{
  albaBehaviorTransform *transform = albaBehaviorTransform::New();
  double pos[2] = {10, 20};

  // without a renderer the transform does not start, and drags and button up are not used
  CPPUNIT_ASSERT(!transform->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!transform->OnDrag(NULL, NULL, pos));
  CPPUNIT_ASSERT(!transform->OnLeftButtonUp(NULL, NULL, pos));

  // the middle and right buttons are not used
  CPPUNIT_ASSERT(!transform->OnMiddleButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!transform->OnRightButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!transform->OnMiddleButtonUp(NULL, NULL, pos));

  transform->Delete();
}

void albaPositionalEventRouterTest::TestEventTracing()
{
  albaFakePickView *view = new albaFakePickView();
  albaPERTestMouse *mouse = new albaPERTestMouse();
  SelectView(mouse, view);

  albaPERTestBehavior *behavior;
  albaNEW(behavior);
  albaVMESurface *vme = albaVMESurface::New();
  vme->SetBehavior(behavior);
  view->m_FakeVme = vme;

  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  CPPUNIT_ASSERT(!per->GetEventTracing());

  // tracing disabled: nothing is traced
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(std::string(per->GetLastEventTrace()).empty());

  // the button down selects the picked VME and starts its behavior
  per->SetEventTracing(true);
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  std::string trace = per->GetLastEventTrace();
  CPPUNIT_ASSERT(trace.find("LeftButtonDown ctrl=0 alt=0 shift=0") != std::string::npos);
  CPPUNIT_ASSERT(trace.find("Select, Behavior albaPERTestBehavior") != std::string::npos);

  // the button up goes to the behavior that started the interaction
  SendToPER(per, mouse, TEST_BUTTON_UP);
  trace = per->GetLastEventTrace();
  CPPUNIT_ASSERT(trace.find("-> Behavior albaPERTestBehavior") != std::string::npos);

  // a move without interaction is not traced
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  CPPUNIT_ASSERT(trace == per->GetLastEventTrace());

  per->SetEventTracing(false);

  per->Delete();
  vme->SetBehavior(NULL);
  vme->Delete();
  albaDEL(behavior);
  delete mouse;
  delete view;
}

void albaPositionalEventRouterTest::TestPushPopTool()
{
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestTool *bottom;
  albaNEW(bottom);
  albaPERTestTool *top;
  albaNEW(top);

  per->PushTool(bottom);
  per->PushTool(top);
  CPPUNIT_ASSERT(per->GetNumberOfTools() == 2);
  CPPUNIT_ASSERT(bottom->m_Push == 1 && top->m_Push == 1);

  // a tool is pushed only once, NULL is ignored
  per->PushTool(bottom);
  per->PushTool(NULL);
  CPPUNIT_ASSERT(per->GetNumberOfTools() == 2);
  CPPUNIT_ASSERT(bottom->m_Push == 1);

  // a tool can be popped from any position
  per->PopTool(bottom);
  CPPUNIT_ASSERT(per->GetNumberOfTools() == 1);
  CPPUNIT_ASSERT(bottom->m_Pop == 1);
  per->PopTool(bottom);
  CPPUNIT_ASSERT(bottom->m_Pop == 1);

  // the PER pops the remaining tools when deleted
  per->Delete();
  CPPUNIT_ASSERT(top->m_Pop == 1);

  albaDEL(top);
  albaDEL(bottom);
}

void albaPositionalEventRouterTest::TestToolInteraction()
{
  albaFakePickView *view = new albaFakePickView();
  albaPERTestMouse *mouse = new albaPERTestMouse();
  SelectView(mouse, view);

  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestTool *tool;
  albaNEW(tool);
  per->PushTool(tool);
  per->SetEventTracing(true);

  // the tool uses the left button down: it interacts with the mouse until the left button is released
  tool->m_UseLeft = true;
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  CPPUNIT_ASSERT(std::string(per->GetLastEventTrace()).find("-> Tool albaPERTestTool") != std::string::npos);
  CPPUNIT_ASSERT(per->IsInteracting() && per->GetInteractingTool() == tool);
  SendToPER(per, mouse, TEST_MOUSE_MOVE);

  // the other buttons pressed in the meantime go to the same tool, and do not end the interaction
  SendToPER(per, mouse, TEST_BUTTON_DOWN, ALBA_RIGHT_BUTTON);
  SendToPER(per, mouse, TEST_BUTTON_UP, ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(tool->m_RightDown == 1 && tool->m_RightUp == 1);
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  CPPUNIT_ASSERT(tool->m_Drag == 2);

  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_LeftUp == 1);
  CPPUNIT_ASSERT(!per->IsInteracting());

  // the tool does not use the left button: the default behavior gets down, drag and up
  tool->m_UseLeft = false;
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  CPPUNIT_ASSERT(per->IsInteracting() && per->GetInteractingTool() == NULL);
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(!per->IsInteracting());
  CPPUNIT_ASSERT(tool->m_LeftDown == 2 && tool->m_Drag == 2 && tool->m_LeftUp == 1);

  // a button the tool does not handle goes to the default behavior
  SendToPER(per, mouse, TEST_BUTTON_DOWN, ALBA_MIDDLE_BUTTON);
  CPPUNIT_ASSERT(per->IsInteracting());
  SendToPER(per, mouse, TEST_BUTTON_UP, ALBA_MIDDLE_BUTTON);
  CPPUNIT_ASSERT(!per->IsInteracting());

  // popping a tool while it interacts with a device leaves the device to the camera until the button is released
  tool->m_UseLeft = true;
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  per->PopTool(tool);
  CPPUNIT_ASSERT(per->IsInteracting() && per->GetInteractingTool() == NULL);
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_LeftUp == 1);
  CPPUNIT_ASSERT(!per->GetCameraTool()->IsInteracting());

  per->SetEventTracing(false);
  per->Delete();
  albaDEL(tool);
  delete mouse;
  delete view;
}

void albaPositionalEventRouterTest::TestToolOrder()
{
  albaPERTestMouse *mouse = new albaPERTestMouse();
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestTool *bottom;
  albaNEW(bottom);
  albaPERTestTool *top;
  albaNEW(top);
  per->PushTool(bottom);
  per->PushTool(top);

  // the top tool comes first
  top->m_UseLeft = bottom->m_UseLeft = true;
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(top->m_LeftDown == 1 && top->m_LeftUp == 1);
  CPPUNIT_ASSERT(bottom->m_LeftDown == 0);

  // what the top tool does not use goes to the tool below
  top->m_UseLeft = false;
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(top->m_LeftDown == 2 && top->m_LeftUp == 1);
  CPPUNIT_ASSERT(bottom->m_LeftDown == 1 && bottom->m_LeftUp == 1);

  // each button is offered separately: the right button goes to the tool handling it
  top->m_UseRight = true;
  SendToPER(per, mouse, TEST_BUTTON_DOWN, ALBA_RIGHT_BUTTON);
  SendToPER(per, mouse, TEST_BUTTON_UP, ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(top->m_RightDown == 1 && top->m_RightUp == 1);
  CPPUNIT_ASSERT(bottom->m_RightDown == 0);

  // moves without buttons and wheel follow the same order
  bottom->m_UseMove = true;
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  CPPUNIT_ASSERT(top->m_Move == 1 && bottom->m_Move == 1);
  CPPUNIT_ASSERT(top->m_Drag == 0 && bottom->m_Drag == 0);
  CPPUNIT_ASSERT(bottom->m_LastPos[0] == 10 && bottom->m_LastPos[1] == 20);

  top->m_UseWheel = true;
  SendToPER(per, mouse, TEST_MOUSE_WHEEL, 0, 120);
  CPPUNIT_ASSERT(top->m_Wheel == 1 && bottom->m_Wheel == 0);
  CPPUNIT_ASSERT(top->m_LastRotation == 120);

  per->Delete();
  albaDEL(top);
  albaDEL(bottom);
  delete mouse;
}

void albaPositionalEventRouterTest::TestBehaviorBeforeTools()
{
  albaFakePickView *view = new albaFakePickView();
  albaPERTestMouse *mouse = new albaPERTestMouse();
  SelectView(mouse, view);

  // the picked VME has a behavior, like gizmos
  albaPERTestBehavior *behavior;
  albaNEW(behavior);
  albaVMESurface *vme = albaVMESurface::New();
  vme->SetBehavior(behavior);
  view->m_FakeVme = vme;

  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestTool *tool;
  albaNEW(tool);
  tool->m_UseLeft = true;
  per->PushTool(tool);

  // the behavior of the picked VME comes before the tools, and the picking is done once
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  CPPUNIT_ASSERT(behavior->m_Interacting);
  CPPUNIT_ASSERT(view->m_PickCount == 1);
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  CPPUNIT_ASSERT(behavior->m_Drag == 1);
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(!behavior->m_Interacting);
  CPPUNIT_ASSERT(tool->m_LeftDown == 0 && tool->m_Drag == 0 && tool->m_LeftUp == 0);

  // without a VME with a behavior the tool gets the click
  view->m_FakeVme = NULL;
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_LeftUp == 1);

  per->Delete();
  albaDEL(tool);
  vme->SetBehavior(NULL);
  vme->Delete();
  albaDEL(behavior);
  delete mouse;
  delete view;
}

void albaPositionalEventRouterTest::TestToolGivesDragToCamera()
{
  albaFakePickView *view = new albaFakePickView();
  albaPERTestMouse *mouse = new albaPERTestMouse();
  SelectView(mouse, view);

  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestTool *tool;
  albaNEW(tool);
  tool->m_UseLeft = true;
  per->PushTool(tool);
  albaToolCamera *camera = per->GetCameraTool();

  // the camera gets the button down of the tool too, ready to move with the drags the tool does not use
  tool->m_UseDrag = false;
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  CPPUNIT_ASSERT(camera->IsInteracting());
  CPPUNIT_ASSERT(per->GetInteractingTool() == tool);
  per->SetEventTracing(true);
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  CPPUNIT_ASSERT(std::string(per->GetLastEventTrace()).find("Tool albaPERTestTool, Camera albaToolCamera") != std::string::npos);
  per->SetEventTracing(false);

  // the button up goes to the tool and ends the camera motion
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_Drag == 1 && tool->m_LeftUp == 1);
  CPPUNIT_ASSERT(!camera->IsInteracting());

  // the drags used by the tool do not reach the camera, which ends anyway with the button up
  tool->m_UseDrag = true;
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_Drag == 2 && tool->m_LeftUp == 2);
  CPPUNIT_ASSERT(!camera->IsInteracting());

  per->Delete();
  albaDEL(tool);
  delete mouse;
  delete view;
}

void albaPositionalEventRouterTest::TestOneInteractionAtATime()
{
  albaPERTestMouse *mouse = new albaPERTestMouse();
  albaPERTestMouse *other = new albaPERTestMouse();
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestTool *tool;
  albaNEW(tool);
  tool->m_UseLeft = tool->m_UseMove = tool->m_UseWheel = true;
  per->PushTool(tool);

  // while a render window interacts, the events of the other render windows are ignored
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  CPPUNIT_ASSERT(per->IsInteracting() && per->GetInteractingTool() == tool);
  SendToPER(per, other, TEST_BUTTON_DOWN);
  SendToPER(per, other, TEST_MOUSE_MOVE);
  SendToPER(per, other, TEST_MOUSE_WHEEL);
  SendToPER(per, other, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(per->IsInteracting());
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_Move == 0 && tool->m_Wheel == 0 && tool->m_LeftUp == 0);

  // a button up without interaction is ignored
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(!per->IsInteracting() && tool->m_LeftUp == 1);
  SendToPER(per, other, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_LeftUp == 1);

  // after the end of the interaction the other render window can interact, and the first one is ignored
  SendToPER(per, other, TEST_BUTTON_DOWN);
  CPPUNIT_ASSERT(per->IsInteracting());
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  CPPUNIT_ASSERT(tool->m_Move == 0 && tool->m_Drag == 0);
  SendToPER(per, other, TEST_BUTTON_UP);

  per->Delete();
  albaDEL(tool);
  delete other;
  delete mouse;
}

void albaPositionalEventRouterTest::TestButtonUpDuringButtonDown()
{
  albaPERTestMouse *mouse = new albaPERTestMouse();
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestModalTool *tool;
  albaNEW(tool);
  tool->m_PER = per;
  tool->m_RWI = mouse->m_RWI;
  per->PushTool(tool);

  // the button up arrives while the tool handles the button down: the interaction ends there,
  // and neither the tool nor the camera keep interacting; the tool is not the handler yet, so it does not get the up
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_LeftUp == 0);
  CPPUNIT_ASSERT(!per->IsInteracting());
  CPPUNIT_ASSERT(per->GetInteractingTool() == NULL);
  CPPUNIT_ASSERT(!per->GetCameraTool()->IsInteracting());

  // the next moves are moves without buttons, not drags
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  CPPUNIT_ASSERT(tool->m_Move == 1 && tool->m_Drag == 0);

  // a new button down starts a new interaction
  per->PopTool(tool);
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  CPPUNIT_ASSERT(per->IsInteracting());
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(!per->IsInteracting());

  per->Delete();
  albaDEL(tool);
  delete mouse;
}

void albaPositionalEventRouterTest::TestBehaviorDeletedDuringInteraction()
{
  albaFakePickView *view = new albaFakePickView();
  albaPERTestMouse *mouse = new albaPERTestMouse();
  SelectView(mouse, view);
  albaPERTestBehavior *behavior;
  albaNEW(behavior);
  albaVMESurface *vme = albaVMESurface::New();
  vme->SetBehavior(behavior);
  view->m_FakeVme = vme;
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();

  // the PER keeps the interacting behavior alive
  SendToPER(per, mouse, TEST_BUTTON_DOWN);
  CPPUNIT_ASSERT(per->GetInteractingTool() == behavior);
  CPPUNIT_ASSERT(behavior->GetReferenceCount() == 2);

  // its owner deletes it during the interaction, e.g. closing the operation: drags and button up still reach it
  view->m_FakeVme = NULL;
  vme->SetBehavior(NULL);
  behavior->Delete(); // the pointer is still used below: the PER keeps the object alive
  SendToPER(per, mouse, TEST_MOUSE_MOVE);
  CPPUNIT_ASSERT(behavior->m_Drag == 1);
  CPPUNIT_ASSERT(behavior->m_Interacting);

  // the button up ends the interaction and deletes it
  SendToPER(per, mouse, TEST_BUTTON_UP);
  CPPUNIT_ASSERT(!per->IsInteracting());

  per->Delete();
  vme->Delete();
  delete mouse;
  delete view;
}

void albaPositionalEventRouterTest::TestTransformStoppedAtButtonDown()
{
  vtkRenderer *ren = vtkRenderer::New();
  albaBehaviorTransform *transform = albaBehaviorTransform::New();
  albaPERTestStoppingListener listener;
  listener.m_Transform = transform;
  transform->SetListener(&listener);
  double pos[2] = {10, 20};

  // the listener releases the button when it gets the button down: the transform stops there
  CPPUNIT_ASSERT(transform->OnLeftButtonDown(NULL, ren, pos));
  CPPUNIT_ASSERT(listener.m_MouseUpCount == 1);
  CPPUNIT_ASSERT(!transform->OnDrag(NULL, ren, pos));
  CPPUNIT_ASSERT(!transform->OnLeftButtonUp(NULL, ren, pos));
  CPPUNIT_ASSERT(listener.m_MouseUpCount == 1);

  transform->SetListener(NULL);
  transform->Delete();
  ren->Delete();
}
