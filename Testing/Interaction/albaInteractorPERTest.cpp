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
static void SelectView(albaDeviceButtonsPadMouse *mouse, albaView *view)
//----------------------------------------------------------------------------
{
  albaEvent e(NULL, VIEW_SELECT, view);
  mouse->OnEvent(&e);
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


void albaInteractorPERTest::TestGetCameraMouseInteractor()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaInteractor *interactor = mip->GetCameraMouseInteractor();
  CPPUNIT_ASSERT(interactor->GetDevice() == NULL);

  mip->Delete();
}

void albaInteractorPERTest::TestGetCameraInteractor()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaInteractor *interactor = mip->GetCameraInteractor();
  CPPUNIT_ASSERT(interactor->GetDevice() == NULL);

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