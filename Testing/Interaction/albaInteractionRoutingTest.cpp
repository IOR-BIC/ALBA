/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionRoutingTest
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
#include "albaInteractionRoutingTest.h"

#include "albaEvent.h"
#include "albaTool2DMeasure.h"
#include "albaTool2DMeasureEllipse.h"
#include "albaPositionalEventRouter.h"
#include "albaToolPicker.h"
#include "albaToolCamera.h"
#include "albaRWIBase.h"
#include "albaView.h"
#include "albaVMESurface.h"
#include "albaServiceLocator.h"
#include "albaFakeLogicForTest.h"
#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkSphereSource.h"
#include "vtkMath.h"

#include <string>

// the mouse events the tests send to the PER
enum TEST_MOUSE_EVENTS
{
  TEST_BUTTON_DOWN,
  TEST_BUTTON_UP,
  TEST_MOUSE_MOVE,
};

//----------------------------------------------------------------------------
class albaRoutingTestView : public albaView // view returning the test render window interactor
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaRoutingTestView, albaView);

  albaRoutingTestView() : albaView("routingTestView") {m_TestRwi = NULL;};

  virtual void SetBackgroundColor(wxColor color) {};
  virtual albaRWIBase *GetRWI() {return m_TestRwi;};

  albaRWIBase *m_TestRwi;
};

albaCxxTypeMacro(albaRoutingTestView);

//----------------------------------------------------------------------------
class albaRecording2DMeasure : public albaTool2DMeasure // records the calls of the 2D measure base class
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaRecording2DMeasure, albaTool2DMeasure);

  albaRecording2DMeasure() {m_DrawCount = m_HighlightCount = 0;};

  int m_DrawCount;
  int m_HighlightCount;

protected:
  virtual void DrawNewMeasure(double *wp) {m_DrawCount++;};
  virtual void FindAndHighlight(double *pointCoord) {m_HighlightCount++;};
};

albaCxxTypeMacro(albaRecording2DMeasure);

//----------------------------------------------------------------------------
class albaRoutingTestEllipse : public albaTool2DMeasureEllipse // exposes the current action
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaRoutingTestEllipse, albaTool2DMeasureEllipse);

  int GetCurrentAction() {return GetAction();};
};

albaCxxTypeMacro(albaRoutingTestEllipse);

//----------------------------------------------------------------------------
class albaRoutingTestKeyTool : public albaInteractionTool // uses all the keys
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaRoutingTestKeyTool, albaInteractionTool);

  bool OnKey(int keyCode, bool pressed) override {return true;}
};

albaCxxTypeMacro(albaRoutingTestKeyTool);

//----------------------------------------------------------------------------
void albaRoutingTestListener::OnEvent(albaEventBase *event)
{
  m_Ids.push_back(event->GetId());
}

//----------------------------------------------------------------------------
int albaRoutingTestListener::Count(albaID id)
{
  int count = 0;
  for (int i = 0; i < m_Ids.size(); i++)
    if (m_Ids[i] == id)
      count++;
  return count;
}

//----------------------------------------------------------------------------
static bool Contains(const char *text, const char *word)
{
  return std::string(text).find(word) != std::string::npos;
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::BeforeTest()
{
  InitializeRenderWindow();
  m_Renderer->ResetCamera();

  m_Frame = new wxFrame(NULL, -1, "Routing test");
  m_Rwi = new albaRWIBase(m_Frame, -1);
  m_Rwi->SetRenderWindow(m_RenderWindow);

  m_View = new albaRoutingTestView();
  m_View->m_TestRwi = m_Rwi;

  m_Rwi->SetView(m_View);
  m_OldPER = albaServiceLocator::GetPositionalEventRouter();

  m_Listener.Clear();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::AfterTest()
{
  albaServiceLocator::SetPositionalEventRouter(m_OldPER);
  delete m_View;

  m_Renderer->RemoveAllViewProps();

  // the render window keeps a reference to its interactor: give it back the original one
  m_RenderWindow->SetInteractor(m_RenderWindowInteractor);
  m_Rwi->SetRenderWindow(NULL);
  vtkDEL(m_Rwi);
  delete m_Frame;
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::SendMouse(albaID id, double x, double y, int button, int modifier, int modifier2)
{
  // as the render window does: the modifier keys first, then the method of the event of the registered PER, with the render window and the position
  albaPositionalEventRouter *per = albaServiceLocator::GetPositionalEventRouter();
  bool ctrl = modifier == ALBA_CTRL_KEY || modifier2 == ALBA_CTRL_KEY;
  bool alt = modifier == ALBA_ALT_KEY || modifier2 == ALBA_ALT_KEY;
  bool shift = modifier == ALBA_SHIFT_KEY || modifier2 == ALBA_SHIFT_KEY;
  double pos[2] = {x, y};
  per->SetModifierKeys(ctrl, alt, shift);
  switch (id)
  {
    case TEST_BUTTON_DOWN:
      switch (button)
      {
        case ALBA_LEFT_BUTTON:   per->OnLeftButtonDown(m_Rwi, pos);   break;
        case ALBA_MIDDLE_BUTTON: per->OnMiddleButtonDown(m_Rwi, pos); break;
        case ALBA_RIGHT_BUTTON:  per->OnRightButtonDown(m_Rwi, pos);  break;
      }
      break;
    case TEST_BUTTON_UP:
      switch (button)
      {
        case ALBA_LEFT_BUTTON:   per->OnLeftButtonUp(m_Rwi, pos);   break;
        case ALBA_MIDDLE_BUTTON: per->OnMiddleButtonUp(m_Rwi, pos); break;
        case ALBA_RIGHT_BUTTON:  per->OnRightButtonUp(m_Rwi, pos);  break;
      }
      break;
    case TEST_MOUSE_MOVE: per->OnMouseMove(m_Rwi, pos); break;
  }
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::SendWheel(double x, double y, double rotation)
{
  albaPositionalEventRouter *per = albaServiceLocator::GetPositionalEventRouter();
  double pos[2] = {x, y};
  per->SetModifierKeys(false, false, false);
  per->OnMouseWheel(m_Rwi, pos, rotation);
}
//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Set2DCamera()
{
  m_Renderer->GetActiveCamera()->ParallelProjectionOn();
  m_Renderer->GetActiveCamera()->SetParallelScale(100);
  m_View->SetCanSpin(false);
  m_RenderWindow->Render();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::TestCameraDragAndWheel()
{
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaServiceLocator::SetPositionalEventRouter(per);
  per->SetEventTracing(true);

  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startPosition[3], position[3];
  camera->GetPosition(startPosition);

  // a left drag without picked VME rotates the camera
  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Camera albaToolCamera"));
  SendMouse(TEST_MOUSE_MOVE, 450, 320);
  SendMouse(TEST_BUTTON_UP, 450, 320, ALBA_LEFT_BUTTON);

  camera->GetPosition(position);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(startPosition, position) > 1e-6);

  // the wheel zooms the camera
  double startDistance = camera->GetDistance();
  SendWheel(400, 300, 120);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Camera albaToolCamera"));
  CPPUNIT_ASSERT(camera->GetDistance() < startDistance);

  albaServiceLocator::SetPositionalEventRouter(m_OldPER);
  per->Delete();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Drag(int button, double x, double y, int modifier, int modifier2)
{
  SendMouse(TEST_BUTTON_DOWN, 400, 300, button, modifier, modifier2);
  SendMouse(TEST_MOUSE_MOVE, x, y, button, modifier, modifier2);
  SendMouse(TEST_BUTTON_UP, x, y, button, modifier, modifier2);
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::TestCameraButtons()
{
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaServiceLocator::SetPositionalEventRouter(per);
  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double focalPoint[3], newFocalPoint[3], viewUp[3], newViewUp[3], distance;

  // middle button: pan
  camera->GetFocalPoint(focalPoint);
  distance = camera->GetDistance();
  Drag(ALBA_MIDDLE_BUTTON, 420, 340);
  camera->GetFocalPoint(newFocalPoint);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(focalPoint, newFocalPoint) > 1e-6);
  CPPUNIT_ASSERT(fabs(camera->GetDistance() - distance) < 1e-6);

  // left button with SHIFT: pan
  camera->GetFocalPoint(focalPoint);
  Drag(ALBA_LEFT_BUTTON, 420, 340, ALBA_SHIFT_KEY);
  camera->GetFocalPoint(newFocalPoint);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(focalPoint, newFocalPoint) > 1e-6);
  CPPUNIT_ASSERT(fabs(camera->GetDistance() - distance) < 1e-6);

  // right button: dolly
  Drag(ALBA_RIGHT_BUTTON, 400, 340);
  CPPUNIT_ASSERT(camera->GetDistance() < distance);

  // left button with SHIFT and CTRL: dolly
  distance = camera->GetDistance();
  Drag(ALBA_LEFT_BUTTON, 400, 340, ALBA_SHIFT_KEY, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(camera->GetDistance() < distance);

  // left button with CTRL: spin around the view direction
  camera->GetViewUp(viewUp);
  camera->GetFocalPoint(focalPoint);
  distance = camera->GetDistance();
  SendMouse(TEST_BUTTON_DOWN, 500, 300, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  SendMouse(TEST_MOUSE_MOVE, 400, 400, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  SendMouse(TEST_BUTTON_UP, 400, 400, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  camera->GetViewUp(newViewUp);
  camera->GetFocalPoint(newFocalPoint);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(viewUp, newViewUp) > 1e-6);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(focalPoint, newFocalPoint) < 1e-6);
  CPPUNIT_ASSERT(fabs(camera->GetDistance() - distance) < 1e-6);

  // the release of a button ends the camera motion
  albaToolCamera *mouseTool = per->GetCameraTool();
  CPPUNIT_ASSERT(!mouseTool->IsInteracting());
  CPPUNIT_ASSERT(mouseTool->GetMotion() == albaToolCamera::MOTION_NONE);

  albaServiceLocator::SetPositionalEventRouter(m_OldPER);
  per->Delete();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::TestPERPickerClick()
{
  albaVMESurface *vme;
  albaNEW(vme);
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(10);
  vtkALBASmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphere->GetOutputPort());
  vtkALBASmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);
  vtkALBASmartPointer<vtkALBAAssembly> assembly;
  assembly->AddPart(actor);
  assembly->SetVme(vme);
  m_Renderer->AddActor(assembly);
  m_Renderer->ResetCamera();
  m_RenderWindow->Render();

  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaServiceLocator::SetPositionalEventRouter(per);
  albaToolPicker *picker;
  albaNEW(picker);
  picker->SetListener(&m_Listener);
  per->PushTool(picker);

  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startPosition[3], position[3];
  camera->GetPosition(startPosition);

  // a click without dragging picks the VME and does not move the camera
  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(TEST_BUTTON_UP, 400, 300, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(VME_PICKED) == 1);
  camera->GetPosition(position);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(startPosition, position) < 1e-6);

  // a drag does not pick: the picker does not use it, so it rotates the camera
  m_Listener.Clear();
  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(TEST_MOUSE_MOVE, 420, 300);
  SendMouse(TEST_BUTTON_UP, 420, 300, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(VME_PICKED) == 0);
  camera->GetPosition(position);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(startPosition, position) > 1e-6);
  CPPUNIT_ASSERT(!per->IsInteracting());

  // a click on the background does not pick
  SendMouse(TEST_BUTTON_DOWN, 5, 5, ALBA_LEFT_BUTTON);
  SendMouse(TEST_BUTTON_UP, 5, 5, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(VME_PICKED) == 0);

  // with SHIFT the click is left to the PER
  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
  SendMouse(TEST_BUTTON_UP, 400, 300, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
  CPPUNIT_ASSERT(m_Listener.Count(VME_PICKED) == 0);

  per->PopTool(picker);
  albaServiceLocator::SetPositionalEventRouter(m_OldPER);
  per->Delete();
  albaDEL(picker);
  m_Renderer->RemoveAllViewProps();
  albaDEL(vme);
}

//----------------------------------------------------------------------------
albaPositionalEventRouter *albaInteractionRoutingTest::NewPERWithTool(albaInteractionTool *tool)
{
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  per->SetListener(&m_Listener);
  albaServiceLocator::SetPositionalEventRouter(per);
  per->PushTool(tool);
  return per;
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::DeletePER(albaPositionalEventRouter *per)
{
  albaServiceLocator::SetPositionalEventRouter(m_OldPER);
  per->Delete();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Test2DMeasureAddWithoutModifiers()
{
  Set2DCamera();
  albaRecording2DMeasure *measure;
  albaNEW(measure);
  measure->SetListener(&m_Listener);
  albaPositionalEventRouter *per = NewPERWithTool(measure);
  measure->SetAction(albaTool2DMeasure::ACTION_ADD_MEASURE);

  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startPosition[3], position[3];
  camera->GetPosition(startPosition);
  double startScale = camera->GetParallelScale();

  // button down, drag and button up draw the new measure once each and do not move the camera
  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(TEST_MOUSE_MOVE, 420, 310);
  SendMouse(TEST_BUTTON_UP, 420, 310, ALBA_LEFT_BUTTON);

  CPPUNIT_ASSERT(measure->m_DrawCount == 3);
  CPPUNIT_ASSERT(m_Listener.Count(albaTool2DMeasure::ID_MEASURE_STARTED) == 1);
  CPPUNIT_ASSERT(m_Listener.Count(albaTool2DMeasure::ID_MEASURE_FINISHED) == 1);

  camera->GetPosition(position);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(startPosition, position) < 1e-6);
  CPPUNIT_ASSERT(camera->GetParallelScale() == startScale);

  DeletePER(per);
  albaDEL(measure);
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Test2DMeasureModifiersMoveCamera()
{
  Set2DCamera();
  albaRecording2DMeasure *measure;
  albaNEW(measure);
  measure->SetListener(&m_Listener);
  albaPositionalEventRouter *per = NewPERWithTool(measure);
  per->SetEventTracing(true);

  // SHIFT leaves button down, drag and button up to the camera
  measure->SetAction(albaTool2DMeasure::ACTION_ADD_MEASURE);
  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Camera albaToolCamera"));
  SendMouse(TEST_MOUSE_MOVE, 420, 310, 0, ALBA_SHIFT_KEY);
  SendMouse(TEST_BUTTON_UP, 420, 310, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);
  CPPUNIT_ASSERT(m_Listener.Count(albaTool2DMeasure::ID_MEASURE_STARTED) == 0);
  CPPUNIT_ASSERT(m_Listener.Count(albaTool2DMeasure::ID_MEASURE_FINISHED) == 0);

  // CTRL does the same while adding a measure
  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Camera albaToolCamera"));
  SendMouse(TEST_MOUSE_MOVE, 420, 310, 0, ALBA_CTRL_KEY);
  SendMouse(TEST_BUTTON_UP, 420, 310, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);
  CPPUNIT_ASSERT(m_Listener.Count(albaTool2DMeasure::ID_MEASURE_STARTED) == 0);

  // with no action CTRL is ignored: the measure handles the button down
  measure->SetAction(albaTool2DMeasure::ACTION_NONE);
  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Tool albaRecording2DMeasure"));
  SendMouse(TEST_BUTTON_UP, 400, 300, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(m_Listener.Count(albaTool2DMeasure::ID_MEASURE_STARTED) == 1);
  CPPUNIT_ASSERT(m_Listener.Count(albaTool2DMeasure::ID_MEASURE_FINISHED) == 1);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);

  // the choice is made at the button down: SHIFT pressed during the drag does not move the camera
  measure->SetAction(albaTool2DMeasure::ACTION_ADD_MEASURE);
  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startScale = camera->GetParallelScale();
  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(TEST_MOUSE_MOVE, 420, 310, 0, ALBA_SHIFT_KEY);
  SendMouse(TEST_BUTTON_UP, 420, 310, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
  CPPUNIT_ASSERT(measure->m_DrawCount == 3);
  CPPUNIT_ASSERT(camera->GetParallelScale() == startScale);

  DeletePER(per);
  albaDEL(measure);
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Test2DMeasureRightButton()
{
  Set2DCamera();
  albaRecording2DMeasure *measure;
  albaNEW(measure);
  measure->SetListener(&m_Listener);
  albaPositionalEventRouter *per = NewPERWithTool(measure);

  // the measures do not use the right button: the right click shows the context menu of the view
  albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
  logic->ClearCalls();
  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_RIGHT_BUTTON);
  SendMouse(TEST_BUTTON_UP, 400, 300, ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(logic->GetNumberOfCalls(albaFakeLogicForTest::FKL_SHOW_VIEW_CONTEXTUAL_MENU) == 1);

  // and the drags of the right button zoom the camera, without the context menu
  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startScale = camera->GetParallelScale();
  m_Listener.Clear();
  logic->ClearCalls();
  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_RIGHT_BUTTON);
  SendMouse(TEST_MOUSE_MOVE, 400, 340, ALBA_RIGHT_BUTTON);
  SendMouse(TEST_BUTTON_UP, 400, 340, ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(camera->GetParallelScale() != startScale);
  CPPUNIT_ASSERT(logic->GetNumberOfCalls(albaFakeLogicForTest::FKL_SHOW_VIEW_CONTEXTUAL_MENU) == 0);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);

  DeletePER(per);
  albaDEL(measure);
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Test2DMeasureMoveHighlights()
{
  Set2DCamera();
  albaRecording2DMeasure *measure;
  albaNEW(measure);
  measure->SetListener(&m_Listener);
  albaPositionalEventRouter *per = NewPERWithTool(measure);

  // a move without buttons highlights the measure under the cursor
  measure->SetAction(albaTool2DMeasure::ACTION_NONE);
  SendMouse(TEST_MOUSE_MOVE, 410, 300);
  SendMouse(TEST_MOUSE_MOVE, 420, 300);
  CPPUNIT_ASSERT(measure->m_HighlightCount == 2);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);

  // a disabled measure ignores the events
  measure->Enable(false);
  SendMouse(TEST_MOUSE_MOVE, 430, 300);
  CPPUNIT_ASSERT(measure->m_HighlightCount == 2);

  DeletePER(per);
  albaDEL(measure);
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Test2DMeasurePerspectiveView()
{
  // in a perspective view the measure does not work: the left button rotates the camera
  albaRecording2DMeasure *measure;
  albaNEW(measure);
  measure->SetListener(&m_Listener);
  albaPositionalEventRouter *per = NewPERWithTool(measure);
  measure->SetAction(albaTool2DMeasure::ACTION_ADD_MEASURE);

  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startPosition[3], position[3];
  camera->GetPosition(startPosition);

  SendMouse(TEST_BUTTON_DOWN, 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(TEST_MOUSE_MOVE, 450, 320);
  SendMouse(TEST_BUTTON_UP, 450, 320, ALBA_LEFT_BUTTON);

  camera->GetPosition(position);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(startPosition, position) > 1e-6);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);
  CPPUNIT_ASSERT(m_Listener.Count(albaTool2DMeasure::ID_MEASURE_STARTED) == 0);

  DeletePER(per);
  albaDEL(measure);
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::TestEllipseCtrlRotates()
{
  Set2DCamera();
  albaRoutingTestEllipse *ellipse;
  albaNEW(ellipse);
  ellipse->SetListener(&m_Listener);
  albaPositionalEventRouter *per = NewPERWithTool(ellipse);

  // draw an ellipse
  SendMouse(TEST_MOUSE_MOVE, 300, 300);
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaTool2DMeasure::ACTION_ADD_MEASURE);
  SendMouse(TEST_BUTTON_DOWN, 300, 300, ALBA_LEFT_BUTTON);
  SendMouse(TEST_MOUSE_MOVE, 500, 300);
  SendMouse(TEST_BUTTON_UP, 500, 300, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(ellipse->GetMeasureCount() == 1);

  // on the first point the ellipse can be edited
  SendMouse(TEST_MOUSE_MOVE, 300, 300);
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaTool2DMeasure::ACTION_EDIT_MEASURE);

  // pressing CTRL, without moving the mouse, switches to rotation; the key is not used, it only changes the modifier state
  CPPUNIT_ASSERT(!per->OnKey(WXK_CONTROL, true));
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaTool2DMeasure::ACTION_ROTATE_MEASURE);

  // other keys are not used
  CPPUNIT_ASSERT(!per->OnKey('A', true));
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaTool2DMeasure::ACTION_ROTATE_MEASURE);

  // releasing CTRL goes back to editing
  CPPUNIT_ASSERT(!per->OnKey(WXK_CONTROL, false));
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaTool2DMeasure::ACTION_EDIT_MEASURE);

  DeletePER(per);
  albaDEL(ellipse);
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::TestModifierKeysReachAllTools()
{
  albaRoutingTestKeyTool *bottom, *top, *pushedLater;
  albaNEW(bottom);
  albaNEW(top);
  albaNEW(pushedLater);
  albaPositionalEventRouter *per = NewPERWithTool(bottom);
  per->PushTool(top);

  // the tool on top uses CTRL, but all the tools and the camera see it
  CPPUNIT_ASSERT(per->OnKey(WXK_CONTROL, true));
  CPPUNIT_ASSERT(per->GetCtrlDown() && top->GetCtrlDown() && bottom->GetCtrlDown());
  CPPUNIT_ASSERT(per->GetCameraTool()->GetCtrlDown());
  CPPUNIT_ASSERT(!bottom->GetAltDown() && !bottom->GetShiftDown());

  // a tool pushed later starts with the current state
  per->PushTool(pushedLater);
  CPPUNIT_ASSERT(pushedLater->GetCtrlDown());

  // other keys do not change the state
  per->OnKey('A', true);
  CPPUNIT_ASSERT(bottom->GetCtrlDown());

  // the mouse events carry the state of the modifier keys
  SendMouse(TEST_MOUSE_MOVE, 300, 300, 0, ALBA_SHIFT_KEY, ALBA_ALT_KEY);
  CPPUNIT_ASSERT(!bottom->GetCtrlDown() && bottom->GetAltDown() && bottom->GetShiftDown());
  CPPUNIT_ASSERT(per->GetCameraTool()->GetShiftDown());

  per->OnKey(WXK_SHIFT, false);
  CPPUNIT_ASSERT(!top->GetShiftDown() && top->GetAltDown());

  DeletePER(per);
  albaDEL(pushedLater);
  albaDEL(top);
  albaDEL(bottom);
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::TestCaptureLostReleasesButtons()
{
  albaPositionalEventRouter *per = albaPositionalEventRouter::New();
  albaServiceLocator::SetPositionalEventRouter(per);
  m_Rwi->vtkRenderWindowInteractor::Enable();
  albaToolCamera *mouseTool = per->GetCameraTool();

  // the right button starts the interaction: losing the capture releases it, not the left one
  wxMouseEvent rightDown(wxEVT_RIGHT_DOWN);
  m_Rwi->OnRightMouseButtonDown(rightDown);
  CPPUNIT_ASSERT(per->IsInteracting() && mouseTool->IsInteracting());
  wxMouseCaptureLostEvent lost;
  m_Rwi->OnMouseCaptureLost(lost);
  CPPUNIT_ASSERT(!per->IsInteracting() && !mouseTool->IsInteracting());

  // with two buttons pressed, both are released and the capture is given back
  wxMouseEvent leftDown(wxEVT_LEFT_DOWN);
  m_Rwi->OnLeftMouseButtonDown(leftDown);
  m_Rwi->OnRightMouseButtonDown(rightDown);
  CPPUNIT_ASSERT(per->IsInteracting());
  m_Rwi->OnMouseCaptureLost(lost);
  CPPUNIT_ASSERT(!per->IsInteracting() && !mouseTool->IsInteracting());
  CPPUNIT_ASSERT(wxWindow::GetCapture() != m_Rwi);

  // a capture lost without buttons pressed does nothing
  m_Rwi->OnMouseCaptureLost(lost);
  CPPUNIT_ASSERT(!per->IsInteracting());

  albaServiceLocator::SetPositionalEventRouter(m_OldPER);
  per->Delete();
}
