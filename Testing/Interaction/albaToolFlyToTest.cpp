/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolFlyToTest
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
#include "albaToolFlyToTest.h"
#include "albaToolFlyTo.h"
#include "albaView.h"
#include "albaRWIBase.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"

//----------------------------------------------------------------------------
class albaFlyToTestView : public albaView // view picking a fixed point and counting the camera updates
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaFlyToTestView, albaView);

  albaFlyToTestView() : albaView("flyToTestView") {m_CanPick = true; m_CameraUpdates = 0;};

  virtual void SetBackgroundColor(wxColor color) {};
  virtual void CameraUpdate() {m_CameraUpdates++;};

  virtual bool Pick(int x, int y)
  {
    m_PickedPosition[0] = 1.0; m_PickedPosition[1] = 2.0; m_PickedPosition[2] = 3.0;
    return m_CanPick;
  };

  bool m_CanPick;
  int  m_CameraUpdates;
};

albaCxxTypeMacro(albaFlyToTestView);

//----------------------------------------------------------------------------
static vtkRenderer *NewTestRenderer()
//----------------------------------------------------------------------------
{
  // focal point in the origin, camera on the Z axis at distance 10
  vtkRenderer *ren = vtkRenderer::New();
  ren->GetActiveCamera()->SetFocalPoint(0, 0, 0);
  ren->GetActiveCamera()->SetPosition(0, 0, 10);
  return ren;
}

//----------------------------------------------------------------------------
static bool Equal(double a[3], double x, double y, double z)
//----------------------------------------------------------------------------
{
  return fabs(a[0] - x) < 1e-6 && fabs(a[1] - y) < 1e-6 && fabs(a[2] - z) < 1e-6;
}

//----------------------------------------------------------------------------
void albaToolFlyToTest::TestNoPick()
//----------------------------------------------------------------------------
{
  vtkRenderer *ren = NewTestRenderer();
  albaFlyToTestView *view = new albaFlyToTestView();
  double pos[2] = {10, 20};

  // no view, no renderer or nothing picked: the camera does not move
  CPPUNIT_ASSERT(!albaToolFlyTo::FlyTo(NULL, ren, pos, 1));
  CPPUNIT_ASSERT(!albaToolFlyTo::FlyTo(view, NULL, pos, 1));
  view->m_CanPick = false;
  CPPUNIT_ASSERT(!albaToolFlyTo::FlyTo(view, ren, pos, 1));

  double focal[3], position[3];
  ren->GetActiveCamera()->GetFocalPoint(focal);
  ren->GetActiveCamera()->GetPosition(position);
  CPPUNIT_ASSERT(Equal(focal, 0, 0, 0) && Equal(position, 0, 0, 10));
  CPPUNIT_ASSERT(view->m_CameraUpdates == 0);

  delete view;
  ren->Delete();
}
//----------------------------------------------------------------------------
void albaToolFlyToTest::TestFlyTo()
//----------------------------------------------------------------------------
{
  vtkRenderer *ren = NewTestRenderer();
  albaFlyToTestView *view = new albaFlyToTestView();
  double pos[2] = {10, 20};

  // the focal point goes to the picked point, the camera follows it and halves its distance
  CPPUNIT_ASSERT(albaToolFlyTo::FlyTo(view, ren, pos, 1, 20));

  double focal[3], position[3];
  ren->GetActiveCamera()->GetFocalPoint(focal);
  ren->GetActiveCamera()->GetPosition(position);
  CPPUNIT_ASSERT(Equal(focal, 1, 2, 3));
  CPPUNIT_ASSERT(Equal(position, 1, 2, 8));
  CPPUNIT_ASSERT(view->m_CameraUpdates == 20);

  delete view;
  ren->Delete();
}
//----------------------------------------------------------------------------
void albaToolFlyToTest::TestFlyAway()
//----------------------------------------------------------------------------
{
  vtkRenderer *ren = NewTestRenderer();
  albaFlyToTestView *view = new albaFlyToTestView();
  double pos[2] = {10, 20};

  // with a negative zoom the camera moves away from the picked point
  CPPUNIT_ASSERT(albaToolFlyTo::FlyTo(view, ren, pos, -0.6, 10));

  double focal[3], position[3];
  ren->GetActiveCamera()->GetFocalPoint(focal);
  ren->GetActiveCamera()->GetPosition(position);
  CPPUNIT_ASSERT(Equal(focal, 1, 2, 3));
  CPPUNIT_ASSERT(Equal(position, 1, 2, 16));
  CPPUNIT_ASSERT(view->m_CameraUpdates == 10);

  delete view;
  ren->Delete();
}
//----------------------------------------------------------------------------
void albaToolFlyToTest::TestButtons()
//----------------------------------------------------------------------------
{
  vtkRenderer *ren = NewTestRenderer();
  albaFlyToTestView *view = new albaFlyToTestView();
  albaRWIBase *rwi = albaRWIBase::New();
  rwi->SetView(view);
  albaToolFlyTo *tool;
  albaNEW(tool);
  double pos[2] = {10, 20};

  // the left and right clicks are always used, also when nothing is picked; the middle button is not used
  CPPUNIT_ASSERT(tool->OnLeftButtonDown(rwi, ren, pos));
  double focal[3];
  ren->GetActiveCamera()->GetFocalPoint(focal);
  CPPUNIT_ASSERT(Equal(focal, 1, 2, 3));
  view->m_CanPick = false;
  CPPUNIT_ASSERT(tool->OnRightButtonDown(rwi, ren, pos));
  CPPUNIT_ASSERT(tool->OnLeftButtonDown(rwi, NULL, pos));
  CPPUNIT_ASSERT(!tool->OnMiddleButtonDown(rwi, ren, pos));

  albaDEL(tool);
  rwi->Delete();
  delete view;
  ren->Delete();
}
