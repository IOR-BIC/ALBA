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

#ifndef __CPP_UNIT_albaInteractionRoutingTest_H__
#define __CPP_UNIT_albaInteractionRoutingTest_H__

#include "albaTest.h"
#include "albaObserver.h"
#include <vector>

class wxFrame;
class albaRWIBase;
class albaDeviceButtonsPadMouse;
class albaRoutingTestView;

/** Records the ids of the events received */
class albaRoutingTestListener : public albaObserver
{
public:
  virtual void OnEvent(albaEventBase *event);
  int Count(albaID id);
  void Clear() {m_Ids.clear();}

  std::vector<albaID> m_Ids;
};

/** Characterization tests: they record how the PER and the interactors replacing it
  (camera, PERPicker, 2D measures) react to mouse and key events, so that their
  conversion to tools can be verified with the same tests. */
class albaInteractionRoutingTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaInteractionRoutingTest );
  CPPUNIT_TEST( TestCameraDragAndWheel );
  CPPUNIT_TEST( TestPERPickerClick );
  CPPUNIT_TEST( Test2DMeasureAddWithoutModifiers );
  CPPUNIT_TEST( Test2DMeasureModifiersMoveCamera );
  CPPUNIT_TEST( Test2DMeasureRightClick );
  CPPUNIT_TEST( Test2DMeasureMoveHighlights );
  CPPUNIT_TEST( TestEllipseCtrlRotates );
  CPPUNIT_TEST_SUITE_END();

protected:

  virtual void BeforeTest();
  virtual void AfterTest();

  void TestCameraDragAndWheel();
  void TestPERPickerClick();
  void Test2DMeasureAddWithoutModifiers();
  void Test2DMeasureModifiersMoveCamera();
  void Test2DMeasureRightClick();
  void Test2DMeasureMoveHighlights();
  void TestEllipseCtrlRotates();

  /** Send a mouse event to the mouse device as the render window interactor does */
  void SendMouse(albaID id, double x, double y, int button = 0, int modifier = -1);

  /** Send a wheel rotation to the mouse device */
  void SendWheel(double x, double y, double rotation);

  /** Use a parallel camera and disable the spin, as the 2D views do */
  void Set2DCamera();

  wxFrame                   *m_Frame;
  albaRWIBase               *m_Rwi;
  albaRoutingTestView       *m_View;
  albaDeviceButtonsPadMouse *m_Mouse;
  albaRoutingTestListener    m_Listener;
};

#endif
