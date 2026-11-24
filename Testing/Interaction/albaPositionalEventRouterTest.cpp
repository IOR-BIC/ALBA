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
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "albaVMESurface.h"
#include "albaView.h"
#include "albaEvent.h"
#include "albaDeviceButtonsPad.h"
#include "albaBehaviorTransform.h"
#include "albaTransform.h"
#include "albaInteractionTool.h"
#include "albaToolCamera.h"
#include "albaToolSelect.h"
#include "vtkCamera.h"

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

  virtual bool FindPokedVme(albaDevice *device, albaMatrix &point_pose, albaVME *&picked_vme, albaInteractionBehavior *&picked_behavior)
  {
    if (!Pick((int)point_pose.GetElement(0,3), (int)point_pose.GetElement(1,3)))
      return false;
    picked_vme = m_PickedVME;
    picked_behavior = m_PickedVME->GetBehavior();
    return true;
  };

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

  albaPERTestModalTool() {m_PER = NULL; m_Mouse = NULL; m_UseLeft = true;};

  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
  {
    m_LeftDown++;
    albaEventInteraction up(m_Mouse, DEVICE_BUTTON_UP);
    up.SetButton(ALBA_LEFT_BUTTON);
    up.SetChannel(MCH_INPUT);
    m_PER->OnEvent(&up);
    return m_UseLeft;
  };

  albaPositionalEventRouter *m_PER;
  albaDeviceButtonsPadMouse *m_Mouse;
};

albaCxxTypeMacro(albaPERTestModalTool);

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
static void SendToPER(albaPositionalEventRouter *per, albaDeviceButtonsPadMouse *mouse, albaID id, int button = ALBA_LEFT_BUTTON)
//----------------------------------------------------------------------------
{
  albaEventInteraction e(mouse, id);
  e.SetButton(button);
  e.Set2DPosition(10, 20);
  e.SetChannel(MCH_INPUT);
  per->OnEvent(&e);
}

//----------------------------------------------------------------------------
static void SelectView(albaDeviceButtonsPadMouse *mouse, albaView *view)
//----------------------------------------------------------------------------
{
  mouse->SetView(view);
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

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();

  bool isInteracting = mip->IsInteracting(dummyDevice);
  CPPUNIT_ASSERT(isInteracting == false);
  dummyDevice->Delete();
  mip->Delete();
}

void albaPositionalEventRouterTest::TestOnEvent()
{
  // mostly to test for leaks: a move without view and without tools
  albaPositionalEventRouter *mip = albaPositionalEventRouter::New();
  CPPUNIT_ASSERT(mip);
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();

  SendToPER(mip, mouse, DEVICE_MOUSE_2D_MOVE);
  CPPUNIT_ASSERT(!mip->IsInteracting(mouse));

  mouse->Delete();
  mip->Delete();
}

void albaPositionalEventRouterTest::TestGetCameraTool()
{
  albaPositionalEventRouter *mip = albaPositionalEventRouter::New();

  albaToolCamera *camera = mip->GetCameraTool();
  CPPUNIT_ASSERT(camera != NULL);
  CPPUNIT_ASSERT(camera->GetMouseTool() != NULL);
  CPPUNIT_ASSERT(camera->GetTrackerTool() != NULL);

  // the camera tool is not part of the tool stack
  CPPUNIT_ASSERT(mip->GetNumberOfTools() == 0);

  mip->Delete();
}

void albaPositionalEventRouterTest::TestEventPickCache()
{
  albaFakePickView *view = new albaFakePickView();
  albaFakePickView *otherView = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  albaVMESurface *vme = albaVMESurface::New();
  view->m_FakeVme = vme;

  albaEventInteraction e(mouse, DEVICE_BUTTON_DOWN);
  e.Set2DPosition(10, 20);

  albaVME *pickedVme = NULL;
  albaInteractionBehavior *pickedBehavior = NULL;
  double pickedPos[3];

  // first request: the view picks at the event position
  CPPUNIT_ASSERT(e.FindPokedVme(view, mouse, pickedVme, pickedBehavior));
  CPPUNIT_ASSERT(pickedVme == vme);
  CPPUNIT_ASSERT(view->m_PickCount == 1);
  CPPUNIT_ASSERT(view->m_LastPickX == 10 && view->m_LastPickY == 20);
  e.GetPickedPosition(pickedPos);
  CPPUNIT_ASSERT(pickedPos[0] == 1.0 && pickedPos[1] == 2.0 && pickedPos[2] == 3.0);

  // next requests reuse the cached picking
  pickedVme = NULL;
  CPPUNIT_ASSERT(e.FindPokedVme(view, mouse, pickedVme, pickedBehavior));
  CPPUNIT_ASSERT(pickedVme == vme);
  CPPUNIT_ASSERT(view->m_PickCount == 1);

  // changing the position, resetting or using another view picks again
  e.Set2DPosition(30, 40);
  e.FindPokedVme(view, mouse, pickedVme, pickedBehavior);
  CPPUNIT_ASSERT(view->m_PickCount == 2);
  CPPUNIT_ASSERT(view->m_LastPickX == 30 && view->m_LastPickY == 40);

  e.ResetPick();
  e.FindPokedVme(view, mouse, pickedVme, pickedBehavior);
  CPPUNIT_ASSERT(view->m_PickCount == 3);

  CPPUNIT_ASSERT(!e.FindPokedVme(otherView, mouse, pickedVme, pickedBehavior));
  CPPUNIT_ASSERT(pickedVme == NULL);
  CPPUNIT_ASSERT(otherView->m_PickCount == 1);

  // the cached picking is not copied
  albaEventInteraction copy;
  copy.DeepCopy(&e);
  copy.FindPokedVme(view, mouse, pickedVme, pickedBehavior);
  CPPUNIT_ASSERT(view->m_PickCount == 4);

  vme->Delete();
  mouse->Delete();
  delete otherView;
  delete view;
}

void albaPositionalEventRouterTest::TestButtonDownPicksOnce()
{
  albaFakePickView *view = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  SelectView(mouse, view);

  // the picked VME has a behavior, like gizmos
  albaPERTestBehavior *behavior = albaPERTestBehavior::New();
  albaVMESurface *vme = albaVMESurface::New();
  vme->SetBehavior(behavior);
  view->m_FakeVme = vme;

  albaPositionalEventRouter *per = albaPositionalEventRouter::New();

  albaEventInteraction down(mouse, DEVICE_BUTTON_DOWN);
  down.SetButton(ALBA_LEFT_BUTTON);
  down.Set2DPosition(10, 20);
  down.SetChannel(MCH_INPUT);
  per->OnEvent(&down);

  // the select tool and the behavior share the same picking
  CPPUNIT_ASSERT(per->GetSelectTool()->GetPickedVME() == vme);
  CPPUNIT_ASSERT(behavior->m_Interacting);
  CPPUNIT_ASSERT(per->GetInteractingTool(mouse) == behavior);
  CPPUNIT_ASSERT(per->GetCameraTool()->GetInteractingTool(mouse) == NULL);
  CPPUNIT_ASSERT(view->m_PickCount == 1);

  albaEventInteraction up(mouse, DEVICE_BUTTON_UP);
  up.SetButton(ALBA_LEFT_BUTTON);
  up.Set2DPosition(10, 20);
  up.SetChannel(MCH_INPUT);
  per->OnEvent(&up);

  CPPUNIT_ASSERT(!behavior->m_Interacting);
  CPPUNIT_ASSERT(view->m_PickCount == 1);

  // the PER gives the picked position to the behavior
  double *pickedPos = behavior->GetPickedPosition();
  CPPUNIT_ASSERT(pickedPos[0] == 1.0 && pickedPos[1] == 2.0 && pickedPos[2] == 3.0);

  // a button the behavior does not use goes to the camera, and the drags do not reach the behavior
  behavior->m_UseLeft = false;
  down.ResetPick();
  per->OnEvent(&down);
  CPPUNIT_ASSERT(per->IsInteracting(mouse) && per->GetInteractingTool(mouse) == NULL);
  CPPUNIT_ASSERT(per->GetCameraTool()->GetInteractingTool(mouse) != NULL);
  albaEventInteraction move(mouse, DEVICE_MOUSE_2D_MOVE);
  move.Set2DPosition(15, 25);
  move.SetChannel(MCH_INPUT);
  per->OnEvent(&move);
  CPPUNIT_ASSERT(behavior->m_Drag == 0);
  per->OnEvent(&up);
  CPPUNIT_ASSERT(per->GetCameraTool()->GetInteractingTool(mouse) == NULL);

  per->Delete();
  vme->SetBehavior(NULL);
  vme->Delete();
  behavior->Delete();
  mouse->Delete();
  delete view;
}

void albaPositionalEventRouterTest::TestGenericMouseUsesButtonDownPick()
{
  albaFakePickView *view = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  SelectView(mouse, view);
  albaVMESurface *vme = albaVMESurface::New();
  view->m_FakeVme = vme;

  albaBehaviorTransform *generic = albaBehaviorTransform::New();
  albaPERTestTransformListener listener;
  generic->SetListener(&listener);
  generic->SetRendererAndView(NULL, view);

  // picking set by the caller: no picking on the view
  double callerPos[3] = {7.0, 8.0, 9.0};
  generic->SetButtonDownPick(true, callerPos);
  generic->OnButtonDownAction(10, 20);

  CPPUNIT_ASSERT(view->m_PickCount == 0);
  CPPUNIT_ASSERT(listener.m_MouseDownCount == 1);
  CPPUNIT_ASSERT(listener.m_PickedPosition[0] == 7.0 && listener.m_PickedPosition[1] == 8.0 && listener.m_PickedPosition[2] == 9.0);

  // the caller picking is used only once, then the behavior picks by itself
  generic->OnButtonDownAction(10, 20);

  CPPUNIT_ASSERT(view->m_PickCount == 1);
  CPPUNIT_ASSERT(listener.m_MouseDownCount == 2);
  CPPUNIT_ASSERT(listener.m_PickedPosition[0] == 1.0 && listener.m_PickedPosition[1] == 2.0 && listener.m_PickedPosition[2] == 3.0);

  generic->Delete();
  vme->Delete();
  mouse->Delete();
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
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  mouse->SetName("TestMouse");
  SelectView(mouse, view);

  albaPERTestBehavior *behavior = albaPERTestBehavior::New();
  albaVMESurface *vme = albaVMESurface::New();
  vme->SetBehavior(behavior);
  view->m_FakeVme = vme;

  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  CPPUNIT_ASSERT(!per->GetEventTracing());

  albaEventInteraction down(mouse, DEVICE_BUTTON_DOWN);
  down.SetButton(ALBA_LEFT_BUTTON);
  down.Set2DPosition(10, 20);
  down.SetChannel(MCH_INPUT);

  albaEventInteraction up(mouse, DEVICE_BUTTON_UP);
  up.SetButton(ALBA_LEFT_BUTTON);
  up.Set2DPosition(10, 20);
  up.SetChannel(MCH_INPUT);

  // tracing disabled: nothing is traced
  per->OnEvent(&down);
  per->OnEvent(&up);
  CPPUNIT_ASSERT(std::string(per->GetLastEventTrace()).empty());

  // the button down selects the picked VME and starts its behavior
  per->SetEventTracing(true);
  down.ResetPick();
  per->OnEvent(&down);
  std::string trace = per->GetLastEventTrace();
  CPPUNIT_ASSERT(trace.find("device=TestMouse") != std::string::npos);
  CPPUNIT_ASSERT(trace.find("button=0") != std::string::npos); // ALBA_LEFT_BUTTON
  CPPUNIT_ASSERT(trace.find("Select, Behavior albaPERTestBehavior") != std::string::npos);

  // the button up goes to the behavior that started the interaction
  per->OnEvent(&up);
  trace = per->GetLastEventTrace();
  CPPUNIT_ASSERT(trace.find("-> Behavior albaPERTestBehavior") != std::string::npos);

  // a move without interaction is not traced
  albaEventInteraction move(mouse, DEVICE_MOUSE_2D_MOVE);
  move.Set2DPosition(15, 25);
  move.SetChannel(MCH_INPUT);
  per->OnEvent(&move);
  CPPUNIT_ASSERT(trace == per->GetLastEventTrace());

  per->SetEventTracing(false);

  per->Delete();
  vme->SetBehavior(NULL);
  vme->Delete();
  behavior->Delete();
  mouse->Delete();
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
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  SelectView(mouse, view);

  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestTool *tool;
  albaNEW(tool);
  per->PushTool(tool);
  per->SetEventTracing(true);

  // the tool uses the left button down: it interacts with the mouse until the left button is released
  tool->m_UseLeft = true;
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  CPPUNIT_ASSERT(std::string(per->GetLastEventTrace()).find("-> Tool albaPERTestTool") != std::string::npos);
  CPPUNIT_ASSERT(per->IsInteracting(mouse) && per->GetInteractingTool(mouse) == tool);
  SendToPER(per, mouse, DEVICE_MOUSE_2D_MOVE);

  // the other buttons pressed in the meantime go to the same tool, and do not end the interaction
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN, ALBA_RIGHT_BUTTON);
  SendToPER(per, mouse, DEVICE_BUTTON_UP, ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(tool->m_RightDown == 1 && tool->m_RightUp == 1);
  SendToPER(per, mouse, DEVICE_MOUSE_2D_MOVE);
  CPPUNIT_ASSERT(tool->m_Drag == 2);

  SendToPER(per, mouse, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_LeftUp == 1);
  CPPUNIT_ASSERT(!per->IsInteracting(mouse));

  // the tool does not use the left button: the default behavior gets down, drag and up
  tool->m_UseLeft = false;
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  CPPUNIT_ASSERT(per->IsInteracting(mouse) && per->GetInteractingTool(mouse) == NULL);
  SendToPER(per, mouse, DEVICE_MOUSE_2D_MOVE);
  SendToPER(per, mouse, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(!per->IsInteracting(mouse));
  CPPUNIT_ASSERT(tool->m_LeftDown == 2 && tool->m_Drag == 2 && tool->m_LeftUp == 1);

  // a button the tool does not handle goes to the default behavior
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN, ALBA_MIDDLE_BUTTON);
  CPPUNIT_ASSERT(per->IsInteracting(mouse));
  SendToPER(per, mouse, DEVICE_BUTTON_UP, ALBA_MIDDLE_BUTTON);
  CPPUNIT_ASSERT(!per->IsInteracting(mouse));

  // popping a tool while it interacts with a device leaves the device to the camera until the button is released
  tool->m_UseLeft = true;
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  per->PopTool(tool);
  CPPUNIT_ASSERT(per->IsInteracting(mouse) && per->GetInteractingTool(mouse) == NULL);
  SendToPER(per, mouse, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_LeftUp == 1);
  CPPUNIT_ASSERT(!per->GetCameraTool()->GetInteractingTool(mouse));

  per->SetEventTracing(false);
  per->Delete();
  albaDEL(tool);
  mouse->Delete();
  delete view;
}

void albaPositionalEventRouterTest::TestToolOrder()
{
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestTool *bottom;
  albaNEW(bottom);
  albaPERTestTool *top;
  albaNEW(top);
  per->PushTool(bottom);
  per->PushTool(top);

  // the top tool comes first
  top->m_UseLeft = bottom->m_UseLeft = true;
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  SendToPER(per, mouse, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(top->m_LeftDown == 1 && top->m_LeftUp == 1);
  CPPUNIT_ASSERT(bottom->m_LeftDown == 0);

  // what the top tool does not use goes to the tool below
  top->m_UseLeft = false;
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  SendToPER(per, mouse, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(top->m_LeftDown == 2 && top->m_LeftUp == 1);
  CPPUNIT_ASSERT(bottom->m_LeftDown == 1 && bottom->m_LeftUp == 1);

  // each button is offered separately: the right button goes to the tool handling it
  top->m_UseRight = true;
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN, ALBA_RIGHT_BUTTON);
  SendToPER(per, mouse, DEVICE_BUTTON_UP, ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(top->m_RightDown == 1 && top->m_RightUp == 1);
  CPPUNIT_ASSERT(bottom->m_RightDown == 0);

  // moves without buttons and wheel follow the same order
  bottom->m_UseMove = true;
  SendToPER(per, mouse, DEVICE_MOUSE_2D_MOVE);
  CPPUNIT_ASSERT(top->m_Move == 1 && bottom->m_Move == 1);
  CPPUNIT_ASSERT(top->m_Drag == 0 && bottom->m_Drag == 0);
  CPPUNIT_ASSERT(bottom->m_LastPos[0] == 10 && bottom->m_LastPos[1] == 20);

  top->m_UseWheel = true;
  double rotation = 120;
  albaEventInteraction wheel(mouse, DEVICE_MOUSE_WHEEL);
  wheel.SetData(&rotation);
  wheel.SetChannel(MCH_INPUT);
  per->OnEvent(&wheel);
  CPPUNIT_ASSERT(top->m_Wheel == 1 && bottom->m_Wheel == 0);
  CPPUNIT_ASSERT(top->m_LastRotation == 120);

  per->Delete();
  albaDEL(top);
  albaDEL(bottom);
  mouse->Delete();
}

void albaPositionalEventRouterTest::TestBehaviorBeforeTools()
{
  albaFakePickView *view = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  SelectView(mouse, view);

  // the picked VME has a behavior, like gizmos
  albaPERTestBehavior *behavior = albaPERTestBehavior::New();
  albaVMESurface *vme = albaVMESurface::New();
  vme->SetBehavior(behavior);
  view->m_FakeVme = vme;

  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestTool *tool;
  albaNEW(tool);
  tool->m_UseLeft = true;
  per->PushTool(tool);

  // the behavior of the picked VME comes before the tools, and the picking is done once
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  CPPUNIT_ASSERT(behavior->m_Interacting);
  CPPUNIT_ASSERT(view->m_PickCount == 1);
  SendToPER(per, mouse, DEVICE_MOUSE_2D_MOVE);
  CPPUNIT_ASSERT(behavior->m_Drag == 1);
  SendToPER(per, mouse, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(!behavior->m_Interacting);
  CPPUNIT_ASSERT(tool->m_LeftDown == 0 && tool->m_Drag == 0 && tool->m_LeftUp == 0);

  // without a VME with a behavior the tool gets the click
  view->m_FakeVme = NULL;
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  SendToPER(per, mouse, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_LeftUp == 1);

  per->Delete();
  albaDEL(tool);
  vme->SetBehavior(NULL);
  vme->Delete();
  behavior->Delete();
  mouse->Delete();
  delete view;
}

void albaPositionalEventRouterTest::TestToolGivesDragToCamera()
{
  albaFakePickView *view = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  SelectView(mouse, view);

  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestTool *tool;
  albaNEW(tool);
  tool->m_UseLeft = true;
  per->PushTool(tool);
  albaToolCamera *camera = per->GetCameraTool();

  // the camera gets the button down of the tool too, ready to move with the drags the tool does not use
  tool->m_UseDrag = false;
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  CPPUNIT_ASSERT(camera->GetInteractingTool(mouse) != NULL);
  CPPUNIT_ASSERT(per->GetInteractingTool(mouse) == tool);
  per->SetEventTracing(true);
  SendToPER(per, mouse, DEVICE_MOUSE_2D_MOVE);
  CPPUNIT_ASSERT(std::string(per->GetLastEventTrace()).find("Tool albaPERTestTool, Camera albaToolCameraMouse") != std::string::npos);
  per->SetEventTracing(false);

  // the button up goes to the tool and ends the camera motion
  SendToPER(per, mouse, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_Drag == 1 && tool->m_LeftUp == 1);
  CPPUNIT_ASSERT(camera->GetInteractingTool(mouse) == NULL);

  // the drags used by the tool do not reach the camera, which ends anyway with the button up
  tool->m_UseDrag = true;
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  SendToPER(per, mouse, DEVICE_MOUSE_2D_MOVE);
  SendToPER(per, mouse, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_Drag == 2 && tool->m_LeftUp == 2);
  CPPUNIT_ASSERT(camera->GetInteractingTool(mouse) == NULL);

  per->Delete();
  albaDEL(tool);
  mouse->Delete();
  delete view;
}

void albaPositionalEventRouterTest::TestOneInteractionAtATime()
{
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  albaDeviceButtonsPadMouse *other = albaDeviceButtonsPadMouse::New();
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestTool *tool;
  albaNEW(tool);
  tool->m_UseLeft = tool->m_UseMove = tool->m_UseWheel = true;
  per->PushTool(tool);

  // while a device interacts, the events of the other devices are ignored
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  CPPUNIT_ASSERT(per->IsInteracting(mouse) && per->GetInteractingTool(mouse) == tool);
  SendToPER(per, other, DEVICE_BUTTON_DOWN);
  SendToPER(per, other, DEVICE_MOUSE_2D_MOVE);
  SendToPER(per, other, DEVICE_MOUSE_WHEEL);
  SendToPER(per, other, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(!per->IsInteracting(other));
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_Move == 0 && tool->m_Wheel == 0 && tool->m_LeftUp == 0);

  // a button up without interaction is ignored
  SendToPER(per, mouse, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(!per->IsInteracting(mouse) && tool->m_LeftUp == 1);
  SendToPER(per, other, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(tool->m_LeftUp == 1);

  // after the end of the interaction the other device can interact
  SendToPER(per, other, DEVICE_BUTTON_DOWN);
  CPPUNIT_ASSERT(per->IsInteracting(other) && !per->IsInteracting(mouse));
  SendToPER(per, other, DEVICE_BUTTON_UP);

  per->Delete();
  albaDEL(tool);
  other->Delete();
  mouse->Delete();
}

void albaPositionalEventRouterTest::TestButtonUpDuringButtonDown()
{
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaPERTestModalTool *tool;
  albaNEW(tool);
  tool->m_PER = per;
  tool->m_Mouse = mouse;
  per->PushTool(tool);

  // the button up arrives while the tool handles the button down: the interaction ends there,
  // and neither the tool nor the camera keep interacting; the tool is not the handler yet, so it does not get the up
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_LeftUp == 0);
  CPPUNIT_ASSERT(!per->IsInteracting(mouse));
  CPPUNIT_ASSERT(per->GetInteractingTool(mouse) == NULL);
  CPPUNIT_ASSERT(per->GetCameraTool()->GetInteractingTool(mouse) == NULL);

  // the next moves are moves without buttons, not drags
  SendToPER(per, mouse, DEVICE_MOUSE_2D_MOVE);
  CPPUNIT_ASSERT(tool->m_Move == 1 && tool->m_Drag == 0);

  // a new button down starts a new interaction
  per->PopTool(tool);
  SendToPER(per, mouse, DEVICE_BUTTON_DOWN);
  CPPUNIT_ASSERT(per->IsInteracting(mouse));
  SendToPER(per, mouse, DEVICE_BUTTON_UP);
  CPPUNIT_ASSERT(!per->IsInteracting(mouse));

  per->Delete();
  albaDEL(tool);
  mouse->Delete();
}
