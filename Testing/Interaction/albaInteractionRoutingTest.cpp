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
#include "albaInteractor2DMeasure_Ellipse.h"
#include "albaInteractorPER.h"
#include "albaInteractorPERPicker.h"
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

  void SetCurrentMeasure(int index) {m_CurrMeasure = index;};

  int m_DrawCount;
  int m_HighlightCount;

protected:
  virtual void DrawNewMeasure(double *wp) {m_DrawCount++;};
  virtual void FindAndHighlight(double *pointCoord) {m_HighlightCount++;};
};

albaCxxTypeMacro(albaRecording2DMeasure);

//----------------------------------------------------------------------------
class albaRoutingTestEllipse : public albaInteractor2DMeasure_Ellipse // exposes the current action
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaRoutingTestEllipse, albaInteractor2DMeasure_Ellipse);

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
  albaEvent e(NULL, VIEW_SELECT, m_View);
  m_Mouse->OnEvent(&e);

  m_Listener.Clear();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::AfterTest()
{
  albaInteractorPER::SetEventTracing(false);

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
void albaInteractionRoutingTest::SendMouse(albaID id, double x, double y, int button, int modifier)
{
  albaEventInteraction e(m_Rwi, id, x, y, button);
  if (modifier >= 0)
    e.SetModifier(modifier);
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
  albaInteractorPER::SetEventTracing(true);

  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startPosition[3], position[3];
  camera->GetPosition(startPosition);

  // a left drag without picked VME rotates the camera
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Camera albaInteractorCameraMove"));
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 450, 320);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 450, 320, ALBA_LEFT_BUTTON);

  camera->GetPosition(position);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(startPosition, position) > 1e-6);

  // the wheel zooms the camera
  double startDistance = camera->GetDistance();
  SendWheel(400, 300, 120);
  CPPUNIT_ASSERT(Contains(per->GetLastEventTrace(), "-> Camera albaInteractorCameraMove"));
  CPPUNIT_ASSERT(camera->GetDistance() < startDistance);

  m_Mouse->RemoveObserver(per);
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

  albaInteractorPERPicker *picker = albaInteractorPERPicker::New();
  picker->SetListener(&m_Listener);
  m_Mouse->AddObserver(picker, MCH_INPUT);

  // a click without dragging picks the VME
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 400, 300, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(VME_PICKED) == 1);

  // a drag does not pick
  m_Listener.Clear();
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 420, 300);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 420, 300, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(VME_PICKED) == 0);

  // a click on the background does not pick
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 5, 5, ALBA_LEFT_BUTTON);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 5, 5, ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(VME_PICKED) == 0);

  m_Mouse->RemoveObserver(picker);
  picker->Delete();
  m_Renderer->RemoveAllViewProps();
  albaDEL(vme);
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Test2DMeasureAddWithoutModifiers()
{
  Set2DCamera();
  albaRecording2DMeasure *measure = albaRecording2DMeasure::New();
  measure->SetListener(&m_Listener);
  m_Mouse->AddObserver(measure, MCH_INPUT);
  measure->SetAction(albaInteractor2DMeasure::ACTION_ADD_MEASURE);

  vtkCamera *camera = m_Renderer->GetActiveCamera();
  double startPosition[3], position[3];
  camera->GetPosition(startPosition);
  double startScale = camera->GetParallelScale();

  // press, drag and release draw the new measure and do not move the camera
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 420, 310);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 420, 310, ALBA_LEFT_BUTTON);

  // the move is drawn twice: OnEvent calls OnMove and then the PER, which is interacting, calls it again
  CPPUNIT_ASSERT(measure->m_DrawCount == 4);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_STARTED) == 1);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_FINISHED) == 1);

  camera->GetPosition(position);
  CPPUNIT_ASSERT(vtkMath::Distance2BetweenPoints(startPosition, position) < 1e-6);
  CPPUNIT_ASSERT(camera->GetParallelScale() == startScale);

  m_Mouse->RemoveObserver(measure);
  measure->Delete();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Test2DMeasureModifiersMoveCamera()
{
  Set2DCamera();
  albaRecording2DMeasure *measure = albaRecording2DMeasure::New();
  measure->SetListener(&m_Listener);
  m_Mouse->AddObserver(measure, MCH_INPUT);
  albaInteractorPER::SetEventTracing(true);

  // SHIFT leaves press, drag and release to the camera
  measure->SetAction(albaInteractor2DMeasure::ACTION_ADD_MEASURE);
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
  CPPUNIT_ASSERT(Contains(measure->GetLastEventTrace(), "-> Camera albaInteractorCameraMove"));
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 420, 310, 0, ALBA_SHIFT_KEY);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 420, 310, ALBA_LEFT_BUTTON, ALBA_SHIFT_KEY);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_STARTED) == 0);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_FINISHED) == 0);

  // CTRL does the same while adding a measure
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(Contains(measure->GetLastEventTrace(), "-> Camera albaInteractorCameraMove"));
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 420, 310, 0, ALBA_CTRL_KEY);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 420, 310, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_STARTED) == 0);

  // with no action CTRL is ignored: the measure handles the press
  measure->SetAction(albaInteractor2DMeasure::ACTION_NONE);
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 400, 300, ALBA_LEFT_BUTTON, ALBA_CTRL_KEY);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_STARTED) == 1);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_FINISHED) == 1);
  CPPUNIT_ASSERT(measure->m_DrawCount == 0);

  m_Mouse->RemoveObserver(measure);
  measure->Delete();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Test2DMeasureRightClick()
{
  Set2DCamera();
  albaRecording2DMeasure *measure = albaRecording2DMeasure::New();
  measure->SetListener(&m_Listener);
  m_Mouse->AddObserver(measure, MCH_INPUT);

  // no current measure: the right click does nothing, not even the context menu
  measure->SetCurrentMeasure(-1);
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_RIGHT_BUTTON);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 400, 300, ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_RCLICK) == 0);
  CPPUNIT_ASSERT(m_Listener.Count(SHOW_VIEW_CONTEXTUAL_MENU) == 0);

  // with a current measure the right click is sent to the operation
  measure->SetCurrentMeasure(0);
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_RIGHT_BUTTON);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 400, 300, ALBA_RIGHT_BUTTON);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_RCLICK) == 1);
  CPPUNIT_ASSERT(m_Listener.Count(SHOW_VIEW_CONTEXTUAL_MENU) == 0);

  // after a move with SHIFT the right click shows the context menu of the view
  m_Listener.Clear();
  measure->SetCurrentMeasure(-1);
  SendMouse(albaDeviceButtonsPadMouse::GetMouse2DMoveId(), 400, 300, 0, ALBA_SHIFT_KEY);
  SendMouse(albaDeviceButtonsPad::GetButtonDownId(), 400, 300, ALBA_RIGHT_BUTTON, ALBA_SHIFT_KEY);
  SendMouse(albaDeviceButtonsPad::GetButtonUpId(), 400, 300, ALBA_RIGHT_BUTTON, ALBA_SHIFT_KEY);
  CPPUNIT_ASSERT(m_Listener.Count(SHOW_VIEW_CONTEXTUAL_MENU) == 1);
  CPPUNIT_ASSERT(m_Listener.Count(albaInteractor2DMeasure::ID_MEASURE_RCLICK) == 0);

  m_Mouse->RemoveObserver(measure);
  measure->Delete();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::Test2DMeasureMoveHighlights()
{
  Set2DCamera();
  albaRecording2DMeasure *measure = albaRecording2DMeasure::New();
  measure->SetListener(&m_Listener);
  m_Mouse->AddObserver(measure, MCH_INPUT);

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

  m_Mouse->RemoveObserver(measure);
  measure->Delete();
}

//----------------------------------------------------------------------------
void albaInteractionRoutingTest::TestEllipseCtrlRotates()
{
  Set2DCamera();
  albaRoutingTestEllipse *ellipse = albaRoutingTestEllipse::New();
  ellipse->SetListener(&m_Listener);
  m_Mouse->AddObserver(ellipse, MCH_INPUT);

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
  albaEvent ctrlDown(this, KEY_PRESSED, (long)WXK_CONTROL);
  ellipse->OnEvent(&ctrlDown);
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaInteractor2DMeasure::ACTION_ROTATE_MEASURE);

  // other keys are ignored
  albaEvent otherKey(this, KEY_PRESSED, (long)'A');
  ellipse->OnEvent(&otherKey);
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaInteractor2DMeasure::ACTION_ROTATE_MEASURE);

  // releasing CTRL goes back to editing
  albaEvent ctrlUp(this, KEY_RELEASED, (long)WXK_CONTROL);
  ellipse->OnEvent(&ctrlUp);
  CPPUNIT_ASSERT(ellipse->GetCurrentAction() == albaInteractor2DMeasure::ACTION_EDIT_MEASURE);

  m_Mouse->RemoveObserver(ellipse);
  ellipse->Delete();
}
