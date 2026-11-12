/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPERTest
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
#include "albaInteractorPERTest.h"
#include "albaInteractorPER.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "albaVMESurface.h"
#include "albaView.h"
#include "albaEvent.h"
#include "albaDeviceButtonsPad.h"
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"
#include "albaTransform.h"
#include "albaInteractionTool.h"
#include "albaCameraTool.h"
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

  virtual bool FindPokedVme(albaDevice *device, albaMatrix &point_pose, vtkProp3D *&picked_prop, albaVME *&picked_vme, albaInteractor *&picked_behavior)
  {
    if (!Pick((int)point_pose.GetElement(0,3), (int)point_pose.GetElement(1,3)))
      return false;
    picked_vme = m_PickedVME;
    picked_prop = m_PickedProp;
    picked_behavior = m_PickedVME->GetBehavior();
    return true;
  };

  virtual bool Pick(int x, int y)
  {
    m_PickCount++;
    m_LastPickX = x;
    m_LastPickY = y;
    m_PickedVME = m_FakeVme;
    m_PickedProp = NULL;
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
    if (e && e->GetId() == ID_TRANSFORM && e->GetArg() == albaInteractorGenericMouse::MOUSE_DOWN)
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
  };

  virtual bool OnLeftButtonDown(albaEventInteraction *e) {m_LeftDown++; return m_UseLeft;};
  virtual bool OnLeftButtonUp(albaEventInteraction *e) {m_LeftUp++; return true;};
  virtual bool OnRightButtonDown(albaEventInteraction *e) {m_RightDown++; return m_UseRight;};
  virtual bool OnRightButtonUp(albaEventInteraction *e) {m_RightUp++; return true;};
  virtual bool OnDrag(albaEventInteraction *e) {m_Drag++; return m_UseDrag;};
  virtual bool OnMove(albaEventInteraction *e) {m_Move++; return m_UseMove;};
  virtual bool OnWheel(albaEventInteraction *e) {m_Wheel++; return m_UseWheel;};
  virtual void OnPush() {m_Push++;};
  virtual void OnPop() {m_Pop++;};

  bool m_UseLeft, m_UseRight, m_UseMove, m_UseWheel, m_UseDrag;
  int m_LeftDown, m_LeftUp, m_RightDown, m_RightUp, m_Drag, m_Move, m_Wheel, m_Push, m_Pop;
};

albaCxxTypeMacro(albaPERTestTool);

//----------------------------------------------------------------------------
static void SendToPER(albaInteractorPER *per, albaDeviceButtonsPadMouse *mouse, albaID id, int button = ALBA_LEFT_BUTTON)
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


void albaInteractorPERTest::TestFixture()
{

}

void albaInteractorPERTest::TestConstructorDestructor()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);
  mip->Delete();
}

void albaInteractorPERTest::TestEnableSelect()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  CPPUNIT_ASSERT(mip->m_CanSelect == true);
  mip->EnableSelect(false);
  CPPUNIT_ASSERT(mip->m_CanSelect == false);

  mip->Delete();
}

void albaInteractorPERTest::TestFlyToMode()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  CPPUNIT_ASSERT(mip->m_FlyToFlag == false);
  mip->FlyToMode();
  CPPUNIT_ASSERT(mip->m_FlyToFlag == true);

  mip->Delete();
}

void albaInteractorPERTest::TestIsInteracting()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();

  bool isInteracting = mip->IsInteracting(dummyDevice);
  CPPUNIT_ASSERT(isInteracting == false);
  dummyDevice->Delete();
  mip->Delete();
}

void albaInteractorPERTest::TestStartInteraction()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);
  
  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();

  int started = mip->StartInteraction(dummyDevice, -1);
  CPPUNIT_ASSERT(started == 1);

  dummyDevice->Delete();
  mip->Delete();
}

void albaInteractorPERTest::TestStopInteraction()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();

  int started = mip->StartInteraction(dummyDevice, -1);
  CPPUNIT_ASSERT(started == 1);
  
  int stopped = mip->StopInteraction(dummyDevice, -1);
  CPPUNIT_ASSERT(stopped == 1);

  dummyDevice->Delete();
  mip->Delete();
}

void albaInteractorPERTest::TestOnEvent()
{
  // mostly to test for leaks
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaEventInteraction dummyMouse2DMoveEvent;
  dummyMouse2DMoveEvent.SetChannel(MCH_INPUT);
  dummyMouse2DMoveEvent.SetSender(this);
  dummyMouse2DMoveEvent.SetId(albaDeviceButtonsPadMouse::GetMouse2DMoveId());

  mip->OnEvent(&dummyMouse2DMoveEvent);
  mip->Delete();
}

void albaInteractorPERTest::TestOnVmeSelected()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaVMESurface *dummy = albaVMESurface::New();

  CPPUNIT_ASSERT(mip->m_VME == NULL);
  mip->OnVmeSelected(dummy);
  CPPUNIT_ASSERT(mip->m_VME == dummy);

  dummy->Delete();
  mip->Delete();
}

void albaInteractorPERTest::TestSetGetMode()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  int mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::SINGLE_BUTTON);

  mip->SetMode(albaInteractorPER::MULTI_BUTTON);
  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::MULTI_BUTTON);

  mip->Delete();
}

void albaInteractorPERTest::TestSetModeToSingleButton()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  int mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::SINGLE_BUTTON);

  mip->SetMode(albaInteractorPER::MULTI_BUTTON);
  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::MULTI_BUTTON);
  
  mip->SetModeToSingleButton();

  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::SINGLE_BUTTON);

  mip->Delete();
}

void albaInteractorPERTest::TestSetModeToMultiButton()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  int mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::SINGLE_BUTTON);

  mip->SetModeToMultiButton();
  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::MULTI_BUTTON);

  mip->Delete();
}


void albaInteractorPERTest::TestGetCameraTool()
{
  albaInteractorPER *mip = albaInteractorPER::New();

  albaCameraTool *camera = mip->GetCameraTool();
  CPPUNIT_ASSERT(camera != NULL);
  CPPUNIT_ASSERT(camera->GetMouseTool() != NULL);
  CPPUNIT_ASSERT(camera->GetTrackerTool() != NULL);

  // the camera tool is not part of the tool stack
  CPPUNIT_ASSERT(mip->GetNumberOfTools() == 0);

  mip->Delete();
}

void albaInteractorPERTest::TestEventPickCache()
{
  albaFakePickView *view = new albaFakePickView();
  albaFakePickView *otherView = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  albaVMESurface *vme = albaVMESurface::New();
  view->m_FakeVme = vme;

  albaEventInteraction e(mouse, albaDeviceButtonsPad::GetButtonDownId());
  e.Set2DPosition(10, 20);

  vtkProp3D *pickedProp = NULL;
  albaVME *pickedVme = NULL;
  albaInteractor *pickedBehavior = NULL;
  double pickedPos[3];

  // first request: the view picks at the event position
  CPPUNIT_ASSERT(e.FindPokedVme(view, mouse, pickedProp, pickedVme, pickedBehavior));
  CPPUNIT_ASSERT(pickedVme == vme);
  CPPUNIT_ASSERT(view->m_PickCount == 1);
  CPPUNIT_ASSERT(view->m_LastPickX == 10 && view->m_LastPickY == 20);
  e.GetPickedPosition(pickedPos);
  CPPUNIT_ASSERT(pickedPos[0] == 1.0 && pickedPos[1] == 2.0 && pickedPos[2] == 3.0);

  // next requests reuse the cached picking
  pickedVme = NULL;
  CPPUNIT_ASSERT(e.FindPokedVme(view, mouse, pickedProp, pickedVme, pickedBehavior));
  CPPUNIT_ASSERT(pickedVme == vme);
  CPPUNIT_ASSERT(view->m_PickCount == 1);

  // changing the position, resetting or using another view picks again
  e.Set2DPosition(30, 40);
  e.FindPokedVme(view, mouse, pickedProp, pickedVme, pickedBehavior);
  CPPUNIT_ASSERT(view->m_PickCount == 2);
  CPPUNIT_ASSERT(view->m_LastPickX == 30 && view->m_LastPickY == 40);

  e.ResetPick();
  e.FindPokedVme(view, mouse, pickedProp, pickedVme, pickedBehavior);
  CPPUNIT_ASSERT(view->m_PickCount == 3);

  CPPUNIT_ASSERT(!e.FindPokedVme(otherView, mouse, pickedProp, pickedVme, pickedBehavior));
  CPPUNIT_ASSERT(pickedVme == NULL);
  CPPUNIT_ASSERT(otherView->m_PickCount == 1);

  // the cached picking is not copied
  albaEventInteraction copy;
  copy.DeepCopy(&e);
  copy.FindPokedVme(view, mouse, pickedProp, pickedVme, pickedBehavior);
  CPPUNIT_ASSERT(view->m_PickCount == 4);

  vme->Delete();
  mouse->Delete();
  delete otherView;
  delete view;
}

void albaInteractorPERTest::TestButtonDownPicksOnce()
{
  albaFakePickView *view = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  SelectView(mouse, view);

  // the picked VME has a mouse compositor as behavior, like gizmos
  albaInteractorCompositorMouse *behavior = albaInteractorCompositorMouse::New();
  albaVMESurface *vme = albaVMESurface::New();
  vme->SetBehavior(behavior);
  view->m_FakeVme = vme;

  albaInteractorPER *per = albaInteractorPER::New();

  albaEventInteraction down(mouse, albaDeviceButtonsPad::GetButtonDownId());
  down.SetButton(ALBA_LEFT_BUTTON);
  down.Set2DPosition(10, 20);
  down.SetChannel(MCH_INPUT);
  per->OnEvent(&down);

  // the PER and the behavior share the same picking
  CPPUNIT_ASSERT(per->GetPickedVME(mouse) == vme);
  CPPUNIT_ASSERT(behavior->IsInteracting());
  CPPUNIT_ASSERT(view->m_PickCount == 1);

  albaEventInteraction up(mouse, albaDeviceButtonsPad::GetButtonUpId());
  up.SetButton(ALBA_LEFT_BUTTON);
  up.Set2DPosition(10, 20);
  up.SetChannel(MCH_INPUT);
  per->OnEvent(&up);

  CPPUNIT_ASSERT(!behavior->IsInteracting());
  CPPUNIT_ASSERT(view->m_PickCount == 1);

  per->Delete();
  vme->SetBehavior(NULL);
  vme->Delete();
  behavior->Delete();
  mouse->Delete();
  delete view;
}

void albaInteractorPERTest::TestGenericMouseUsesButtonDownPick()
{
  albaFakePickView *view = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  SelectView(mouse, view);
  albaVMESurface *vme = albaVMESurface::New();
  view->m_FakeVme = vme;

  albaInteractorGenericMouse *generic = albaInteractorGenericMouse::New();
  albaPERTestTransformListener listener;
  generic->SetListener(&listener);
  generic->StartInteraction(mouse);

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

  generic->StopInteraction(mouse);
  generic->Delete();
  vme->Delete();
  mouse->Delete();
  delete view;
}

void albaInteractorPERTest::TestEventTracing()
{
  albaFakePickView *view = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  mouse->SetName("TestMouse");
  SelectView(mouse, view);

  albaInteractorCompositorMouse *behavior = albaInteractorCompositorMouse::New();
  albaVMESurface *vme = albaVMESurface::New();
  vme->SetBehavior(behavior);
  view->m_FakeVme = vme;

  albaInteractorPER *per = albaInteractorPER::New();
  CPPUNIT_ASSERT(!albaInteractorPER::GetEventTracing());

  albaEventInteraction down(mouse, albaDeviceButtonsPad::GetButtonDownId());
  down.SetButton(ALBA_LEFT_BUTTON);
  down.Set2DPosition(10, 20);
  down.SetChannel(MCH_INPUT);

  albaEventInteraction up(mouse, albaDeviceButtonsPad::GetButtonUpId());
  up.SetButton(ALBA_LEFT_BUTTON);
  up.Set2DPosition(10, 20);
  up.SetChannel(MCH_INPUT);

  // tracing disabled: nothing is traced
  per->OnEvent(&down);
  per->OnEvent(&up);
  CPPUNIT_ASSERT(std::string(per->GetLastEventTrace()).empty());

  // the button down selects the picked VME and starts its behavior
  albaInteractorPER::SetEventTracing(true);
  down.ResetPick();
  per->OnEvent(&down);
  std::string trace = per->GetLastEventTrace();
  CPPUNIT_ASSERT(trace.find("device=TestMouse") != std::string::npos);
  CPPUNIT_ASSERT(trace.find("button=0") != std::string::npos); // ALBA_LEFT_BUTTON
  CPPUNIT_ASSERT(trace.find("Select, Behavior albaInteractorCompositorMouse") != std::string::npos);

  // the button up goes to the behavior that started the interaction
  per->OnEvent(&up);
  trace = per->GetLastEventTrace();
  CPPUNIT_ASSERT(trace.find("-> Behavior albaInteractorCompositorMouse") != std::string::npos);

  // a move without interaction is not traced
  albaEventInteraction move(mouse, albaDeviceButtonsPadMouse::GetMouse2DMoveId());
  move.Set2DPosition(15, 25);
  move.SetChannel(MCH_INPUT);
  per->OnEvent(&move);
  CPPUNIT_ASSERT(trace == per->GetLastEventTrace());

  albaInteractorPER::SetEventTracing(false);

  per->Delete();
  vme->SetBehavior(NULL);
  vme->Delete();
  behavior->Delete();
  mouse->Delete();
  delete view;
}

void albaInteractorPERTest::TestPushPopTool()
{
  albaInteractorPER *per = albaInteractorPER::New();
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

void albaInteractorPERTest::TestToolInteraction()
{
  albaFakePickView *view = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  SelectView(mouse, view);

  albaInteractorPER *per = albaInteractorPER::New();
  albaPERTestTool *tool;
  albaNEW(tool);
  per->PushTool(tool);
  albaInteractorPER::SetEventTracing(true);

  // the tool uses the left button down: it interacts with the mouse until the left button is released
  tool->m_UseLeft = true;
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId());
  CPPUNIT_ASSERT(std::string(per->GetLastEventTrace()).find("-> Tool albaPERTestTool") != std::string::npos);
  CPPUNIT_ASSERT(!per->IsInteracting(mouse));
  SendToPER(per, mouse, albaDeviceButtonsPadMouse::GetMouse2DMoveId());

  // the other buttons pressed in the meantime go to the same tool, and do not end the interaction
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId(), ALBA_RIGHT_BUTTON);
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId(), ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(tool->m_RightDown == 1 && tool->m_RightUp == 1);
  SendToPER(per, mouse, albaDeviceButtonsPadMouse::GetMouse2DMoveId());
  CPPUNIT_ASSERT(tool->m_Drag == 2);

  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId());
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_LeftUp == 1);
  CPPUNIT_ASSERT(!per->IsInteracting(mouse));

  // the tool does not use the left button: the default behavior gets down, drag and up
  tool->m_UseLeft = false;
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId());
  CPPUNIT_ASSERT(per->IsInteracting(mouse));
  SendToPER(per, mouse, albaDeviceButtonsPadMouse::GetMouse2DMoveId());
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId());
  CPPUNIT_ASSERT(!per->IsInteracting(mouse));
  CPPUNIT_ASSERT(tool->m_LeftDown == 2 && tool->m_Drag == 2 && tool->m_LeftUp == 1);

  // a button the tool does not handle goes to the default behavior
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId(), ALBA_MIDDLE_BUTTON);
  CPPUNIT_ASSERT(per->IsInteracting(mouse));
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId(), ALBA_MIDDLE_BUTTON);
  CPPUNIT_ASSERT(!per->IsInteracting(mouse));

  // popping a tool while it interacts with a device gives the device back to the default behavior
  tool->m_UseLeft = true;
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId());
  per->PopTool(tool);
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId());
  CPPUNIT_ASSERT(tool->m_LeftUp == 1);
  CPPUNIT_ASSERT(!per->GetCameraTool()->GetInteractingTool(mouse));

  albaInteractorPER::SetEventTracing(false);
  per->Delete();
  albaDEL(tool);
  mouse->Delete();
  delete view;
}

void albaInteractorPERTest::TestToolOrder()
{
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  albaInteractorPER *per = albaInteractorPER::New();
  albaPERTestTool *bottom;
  albaNEW(bottom);
  albaPERTestTool *top;
  albaNEW(top);
  per->PushTool(bottom);
  per->PushTool(top);

  // the top tool comes first
  top->m_UseLeft = bottom->m_UseLeft = true;
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId());
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId());
  CPPUNIT_ASSERT(top->m_LeftDown == 1 && top->m_LeftUp == 1);
  CPPUNIT_ASSERT(bottom->m_LeftDown == 0);

  // what the top tool does not use goes to the tool below
  top->m_UseLeft = false;
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId());
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId());
  CPPUNIT_ASSERT(top->m_LeftDown == 2 && top->m_LeftUp == 1);
  CPPUNIT_ASSERT(bottom->m_LeftDown == 1 && bottom->m_LeftUp == 1);

  // each button is offered separately: the right button goes to the tool handling it
  top->m_UseRight = true;
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId(), ALBA_RIGHT_BUTTON);
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId(), ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(top->m_RightDown == 1 && top->m_RightUp == 1);
  CPPUNIT_ASSERT(bottom->m_RightDown == 0);

  // moves without buttons and wheel follow the same order
  bottom->m_UseMove = true;
  SendToPER(per, mouse, albaDeviceButtonsPadMouse::GetMouse2DMoveId());
  CPPUNIT_ASSERT(top->m_Move == 1 && bottom->m_Move == 1);
  CPPUNIT_ASSERT(top->m_Drag == 0 && bottom->m_Drag == 0);

  top->m_UseWheel = true;
  double rotation = 120;
  albaEventInteraction wheel(mouse, albaDeviceButtonsPadMouse::GetWheelId());
  wheel.SetData(&rotation);
  wheel.SetChannel(MCH_INPUT);
  per->OnEvent(&wheel);
  CPPUNIT_ASSERT(top->m_Wheel == 1 && bottom->m_Wheel == 0);

  per->Delete();
  albaDEL(top);
  albaDEL(bottom);
  mouse->Delete();
}

void albaInteractorPERTest::TestBehaviorBeforeTools()
{
  albaFakePickView *view = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  SelectView(mouse, view);

  // the picked VME has a mouse compositor as behavior, like gizmos
  albaInteractorCompositorMouse *behavior = albaInteractorCompositorMouse::New();
  albaVMESurface *vme = albaVMESurface::New();
  vme->SetBehavior(behavior);
  view->m_FakeVme = vme;

  albaInteractorPER *per = albaInteractorPER::New();
  albaPERTestTool *tool;
  albaNEW(tool);
  tool->m_UseLeft = true;
  per->PushTool(tool);

  // the behavior of the picked VME comes before the tools, and the picking is done once
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId());
  CPPUNIT_ASSERT(behavior->IsInteracting());
  CPPUNIT_ASSERT(view->m_PickCount == 1);
  SendToPER(per, mouse, albaDeviceButtonsPadMouse::GetMouse2DMoveId());
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId());
  CPPUNIT_ASSERT(!behavior->IsInteracting());
  CPPUNIT_ASSERT(tool->m_LeftDown == 0 && tool->m_Drag == 0 && tool->m_LeftUp == 0);

  // without a VME with a behavior the tool gets the click
  view->m_FakeVme = NULL;
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId());
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId());
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_LeftUp == 1);

  per->Delete();
  albaDEL(tool);
  vme->SetBehavior(NULL);
  vme->Delete();
  behavior->Delete();
  mouse->Delete();
  delete view;
}

void albaInteractorPERTest::TestToolGivesDragToCamera()
{
  albaFakePickView *view = new albaFakePickView();
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  SelectView(mouse, view);

  albaInteractorPER *per = albaInteractorPER::New();
  albaPERTestTool *tool;
  albaNEW(tool);
  tool->m_UseLeft = true;
  per->PushTool(tool);
  albaCameraTool *camera = per->GetCameraTool();

  // the camera gets the button down of the tool too, ready to move with the drags the tool does not use
  tool->m_UseDrag = false;
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId());
  CPPUNIT_ASSERT(camera->GetInteractingTool(mouse) != NULL);
  CPPUNIT_ASSERT(!per->IsInteracting(mouse));
  albaInteractorPER::SetEventTracing(true);
  SendToPER(per, mouse, albaDeviceButtonsPadMouse::GetMouse2DMoveId());
  CPPUNIT_ASSERT(std::string(per->GetLastEventTrace()).find("Tool albaPERTestTool, Camera albaCameraMouseTool") != std::string::npos);
  albaInteractorPER::SetEventTracing(false);

  // the button up goes to the tool and ends the camera motion
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId());
  CPPUNIT_ASSERT(tool->m_LeftDown == 1 && tool->m_Drag == 1 && tool->m_LeftUp == 1);
  CPPUNIT_ASSERT(camera->GetInteractingTool(mouse) == NULL);

  // the drags used by the tool do not reach the camera, which ends anyway with the button up
  tool->m_UseDrag = true;
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonDownId());
  SendToPER(per, mouse, albaDeviceButtonsPadMouse::GetMouse2DMoveId());
  SendToPER(per, mouse, albaDeviceButtonsPad::GetButtonUpId());
  CPPUNIT_ASSERT(tool->m_Drag == 2 && tool->m_LeftUp == 2);
  CPPUNIT_ASSERT(camera->GetInteractingTool(mouse) == NULL);

  per->Delete();
  albaDEL(tool);
  mouse->Delete();
  delete view;
}
