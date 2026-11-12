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

#include "albaDeviceButtonsPad.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEvent.h"
#include "albaEventInteraction.h"
#include "albaInteractor2DMeasure.h"
#include "albaInteractor2DMeasureEllipse.h"
#include "albaInteractorPER.h"
#include "albaInteractorPERPicker.h"
#include "albaCameraTool.h"
#include "albaCameraMouseTool.h"
#include "albaCameraTrackerTool.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaRWIBase.h"
#include "albaView.h"
#include "albaVMESurface.h"
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
class albaRecording2DMeasure : public albaInteractor2DMeasure // records the calls of the 2D measure base class
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaRecording2DMeasure, albaInteractor2DMeasure);

  albaRecording2DMeasure() {m_DrawCount = m_HighlightCount = 0;};

  int m_DrawCount;
  int m_HighlightCount;

protected:
  virtual void DrawNewMeasure(double *wp) {m_DrawCount++;};
  virtual void FindAndHighlight(double *pointCoord) {m_HighlightCount++;};
};

albaCxxTypeMacro(albaRecording2DMeasure);

//----------------------------------------------------------------------------
class albaRoutingTestEllipse : public albaInteractor2DMeasureEllipse // exposes the current action
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaRoutingTestEllipse, albaInteractor2DMeasureEllipse);

  int GetCurrentAction() {return GetAction();};
};

albaCxxTypeMacro(albaRoutingTestEllipse);

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

  m_Mouse = albaDeviceButtonsPadMouse::New();
  m_Mouse->SetView(m_View);

  m_Listener.Clear();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::AfterTest()
{
  m_Mouse->Delete();
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
  albaEventInteraction e(m_Rwi, id, x, y, button);
  if (modifier >= 0)
    e.SetModifier(modifier);
  if (modifier2 >= 0)
    e.SetModifier(modifier2);
  m_Mouse->OnEvent(&e);
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::SendWheel(double x, double y, double rotation)
{
  albaEventInteraction e(m_Rwi, albaDeviceButtonsPadMouse::GetWheelId(), x, y);
  e.SetData(&rotation);
  m_Mouse->OnEvent(&e);
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
  albaInteractorPER *per = albaInteractorPER::New();
  m_Mouse->AddObserver(per, MCH_INPUT);
  per->SetEventTracing(true);

  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startPosition[3], position[3];
  camera->GetPosition(startPosition);

  // a left drag without picked VME rotates the camera
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Camera albaCameraMouseTool"));
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 450, 320);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 450, 320, ALBA_LEFT_BUTTON);

  camera->GetPosition(position);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(startPosition, position) > 1e-6);

  // the wheel zooms the camera
  double startDistance = camera->GetDistance();
  SendWheel(400, 300, 120);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Camera albaCameraMouseTool"));
  CPPUNIT_ASSERT(camera->GetDistance() < startDistance);

  m_Mouse->RemoveObserver(per);
  per->Delete();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Drag(int button, double x, double y, int modifier, int modifier2)
{
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, button, modifier, modifier2);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), x, y, button, modifier, modifier2);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), x, y, button, modifier, modifier2);
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::TestCameraButtons()
{
  albaInteractorPER *per = albaInteractorPER::New();
  m_Mouse->AddObserver(per, MCH_INPUT);
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
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 500, 300, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 400, 400, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 400, 400, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  camera->GetViewUp(newViewUp);
  camera->GetFocalPoint(newFocalPoint);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(viewUp, newViewUp) > 1e-6);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(focalPoint, newFocalPoint) < 1e-6);
  CPPUNIT_ASSERT(fabs(camera->GetDistance() - distance) < 1e-6);

  // the release of a button ends the camera motion
  albaCameraMouseTool *mouseTool = per->GetCameraTool()->GetMouseTool();
  CPPUNIT_ASSERT(!mouseTool->IsInteracting(m_Mouse));
  CPPUNIT_ASSERT(mouseTool->GetMotion() == albaCameraMouseTool::MOTION_NONE);

  m_Mouse->RemoveObserver(per);
  per->Delete();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::TestCameraTrackerTool()
{
  albaCameraTrackerTool *trackerTool;
  albaNEW(trackerTool);
  albaDeviceButtonsPadTracker *tracker = albaDeviceButtonsPadTracker::New();

  // the mouse is not handled by the tracker tool
  albaEventInteraction mouseDown(m_Mouse, albaDeviceButtonsPad::GetButtonDownId());
  mouseDown.SetButton(ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(!trackerTool->OnLeftButtonDown(&mouseDown));
  CPPUNIT_ASSERT(!trackerTool->IsInteracting(m_Mouse));

  // a tracker without an albaAvatar3D cannot move the camera
  albaEventInteraction trackerDown(tracker, albaDeviceButtonsPad::GetButtonDownId());
  trackerDown.SetButton(ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(!trackerTool->OnLeftButtonDown(&trackerDown));
  CPPUNIT_ASSERT(!trackerTool->IsInteracting(tracker));

  // events of devices not interacting are not used
  albaEventInteraction trackerMove(tracker, albaDeviceButtonsPadTracker::GetTracker3DMoveId());
  CPPUNIT_ASSERT(!trackerTool->OnDrag(&trackerMove));
  CPPUNIT_ASSERT(!trackerTool->OnLeftButtonUp(&trackerDown));

  // only the left button is handled
  CPPUNIT_ASSERT(!trackerTool->OnRightButtonDown(&trackerDown));
  CPPUNIT_ASSERT(!trackerTool->OnMiddleButtonDown(&trackerDown));

  tracker->Delete();
  albaDEL(trackerTool);
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

  albaInteractorPER *per = albaInteractorPER::New();
  m_Mouse->AddObserver(per, MCH_INPUT);
  albaInteractorPERPicker *picker;
  albaNEW(picker);
  picker->SetListener(&m_Listener);
  per->PushTool(picker);

  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startPosition[3], position[3];
  camera->GetPosition(startPosition);

  // a click without dragging picks the VME and does not move the camera
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 400, 300, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(VME_PICKED) == 1);
  camera->GetPosition(position);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(startPosition, position) < 1e-6);

  // a drag does not pick: the picker does not use it, so it rotates the camera
  m_Listener.Clear();
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 420, 300);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 420, 300, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(VME_PICKED) == 0);
  camera->GetPosition(position);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(startPosition, position) > 1e-6);
  CPPUNIT_ASSERT(!per->IsInteracting(m_Mouse));

  // a click on the background does not pick
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 5, 5, ALBA_LEFT_BUTTON);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 5, 5, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(VME_PICKED) == 0);

  // with SHIFT the click is left to the PER
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 400, 300, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
  CPPUNIT_ASSERT(m_Listener.Count(VME_PICKED) == 0);

  per->PopTool(picker);
  m_Mouse->RemoveObserver(per);
  per->Delete();
  albaDEL(picker);
  m_Renderer->RemoveAllViewProps();
  albaDEL(vme);
}

//----------------------------------------------------------------------------
albaInteractorPER *albaInteractionRoutingTest::NewPERWithTool(albaInteractionTool *tool)
{
  albaInteractorPER *per = albaInteractorPER::New();
  per->SetListener(&m_Listener);
  m_Mouse->AddObserver(per, MCH_INPUT);
  per->PushTool(tool);
  return per;
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::DeletePER(albaInteractorPER *per)
{
  m_Mouse->RemoveObserver(per);
  per->Delete();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Test2DMeasureAddWithoutModifiers()
{
  Set2DCamera();
  albaRecording2DMeasure *measure;
  albaNEW(measure);
  measure->SetListener(&m_Listener);
  albaInteractorPER *per = NewPERWithTool(measure);
  measure->SetAction(albaInteractor2DMeasure::ACTION_ADD_MEASURE);

  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startPosition[3], position[3];
  camera->GetPosition(startPosition);
  double startScale = camera->GetParallelScale();

  // button down, drag and button up draw the new measure once each and do not move the camera
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 420, 310);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 420, 310, ALBA_LEFT_BUTTON);

  CPPUNIT_ASSERT(measure->m_DrawCount == 3);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_STARTED) == 1);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_FINISHED) == 1);

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
  albaInteractorPER *per = NewPERWithTool(measure);
  per->SetEventTracing(true);

  // SHIFT leaves button down, drag and button up to the camera
  measure->SetAction(albaInteractor2DMeasure::ACTION_ADD_MEASURE);
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Camera albaCameraMouseTool"));
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 420, 310, 0, ALBA_SHIFT_KEY);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 420, 310, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_STARTED) == 0);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_FINISHED) == 0);

  // CTRL does the same while adding a measure
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Camera albaCameraMouseTool"));
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 420, 310, 0, ALBA_CTRL_KEY);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 420, 310, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_STARTED) == 0);

  // with no action CTRL is ignored: the measure handles the button down
  measure->SetAction(albaInteractor2DMeasure::ACTION_NONE);
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Tool albaRecording2DMeasure"));
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 400, 300, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_STARTED) == 1);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_FINISHED) == 1);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);

  // the choice is made at the button down: SHIFT pressed during the drag does not move the camera
  measure->SetAction(albaInteractor2DMeasure::ACTION_ADD_MEASURE);
  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startScale = camera->GetParallelScale();
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 420, 310, 0, ALBA_SHIFT_KEY);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 420, 310, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
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
  albaInteractorPER *per = NewPERWithTool(measure);

  // the measures do not use the right button: the right click shows the context menu of the view
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_RIGHT_BUTTON);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 400, 300, ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(SHOW_VIEW_CONTEXTUAL_MENU) == 1);

  // and the drags of the right button zoom the camera, without the context menu
  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startScale = camera->GetParallelScale();
  m_Listener.Clear();
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_RIGHT_BUTTON);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 400, 340, ALBA_RIGHT_BUTTON);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 400, 340, ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(camera->GetParallelScale() != startScale);
  CPPUNIT_ASSERT(m_Listener.Count(SHOW_VIEW_CONTEXTUAL_MENU) == 0);
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
  albaInteractorPER *per = NewPERWithTool(measure);

  // a move without buttons highlights the measure under the cursor
  measure->SetAction(albaInteractor2DMeasure::ACTION_NONE);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 410, 300);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 420, 300);
  CPPUNIT_ASSERT(measure->m_HighlightCount == 2);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);

  // a disabled measure ignores the events
  measure->Enable(false);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 430, 300);
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
  albaInteractorPER *per = NewPERWithTool(measure);
  measure->SetAction(albaInteractor2DMeasure::ACTION_ADD_MEASURE);

  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startPosition[3], position[3];
  camera->GetPosition(startPosition);

  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 450, 320);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 450, 320, ALBA_LEFT_BUTTON);

  camera->GetPosition(position);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(startPosition, position) > 1e-6);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_STARTED) == 0);

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
  albaInteractorPER *per = NewPERWithTool(ellipse);

  // draw an ellipse
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 300, 300);
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaInteractor2DMeasure::ACTION_ADD_MEASURE);
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 300, 300, ALBA_LEFT_BUTTON);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 500, 300);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 500, 300, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(ellipse->GetMeasureCount() == 1);

  // on the first point the ellipse can be edited
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 300, 300);
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaInteractor2DMeasure::ACTION_EDIT_MEASURE);

  // pressing CTRL, without moving the mouse, switches to rotation
  CPPUNIT_ASSERT(per->DispatchKey(WXK_CONTROL, true));
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaInteractor2DMeasure::ACTION_ROTATE_MEASURE);

  // other keys are not used
  CPPUNIT_ASSERT(!per->DispatchKey('A', true));
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaInteractor2DMeasure::ACTION_ROTATE_MEASURE);

  // releasing CTRL goes back to editing
  CPPUNIT_ASSERT(per->DispatchKey(WXK_CONTROL, false));
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaInteractor2DMeasure::ACTION_EDIT_MEASURE);

  DeletePER(per);
  albaDEL(ellipse);
}
