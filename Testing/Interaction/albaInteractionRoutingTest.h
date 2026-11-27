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
class albaRoutingTestView;
class albaPositionalEventRouter;
class albaInteractionTool;
class albaVME;
class albaVMESurface;

/** Records the ids of the events received and the VME of the last one */
class albaRoutingTestListener : public albaObserver
{
public:
  albaRoutingTestListener() {m_LastVme = NULL;}
  virtual void OnEvent(albaEventBase *event);
  int Count(albaID id);
  void Clear() {m_Ids.clear(); m_LastVme = NULL;}

  std::vector<albaID> m_Ids;
  albaVME *m_LastVme;
};

/** Characterization tests: they record how the PER and the interactors replacing it
  (camera, PERPicker, 2D measures) react to mouse and key events, so that their
  conversion to tools can be verified with the same tests. */
class albaInteractionRoutingTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaInteractionRoutingTest );
  CPPUNIT_TEST( TestCameraDragAndWheel );
  CPPUNIT_TEST( TestCameraButtons );
  CPPUNIT_TEST( TestPERPickerClick );
  CPPUNIT_TEST( TestPERPickerTargets );
  CPPUNIT_TEST( TestPERPickerContinuous );
  CPPUNIT_TEST( Test2DMeasureAddWithoutModifiers );
  CPPUNIT_TEST( Test2DMeasureModifiersMoveCamera );
  CPPUNIT_TEST( Test2DMeasureRightButton );
  CPPUNIT_TEST( Test2DMeasureMoveHighlights );
  CPPUNIT_TEST( Test2DMeasurePerspectiveView );
  CPPUNIT_TEST( TestEllipseCtrlRotates );
  CPPUNIT_TEST( TestModifierKeysReachAllTools );
  CPPUNIT_TEST( TestCaptureLostReleasesButtons );
  CPPUNIT_TEST_SUITE_END();

protected:

  virtual void BeforeTest();
  virtual void AfterTest();

  void TestCameraDragAndWheel();
  void TestCameraButtons();
  void TestPERPickerClick();
  void TestPERPickerTargets();
  void TestPERPickerContinuous();
  void Test2DMeasureAddWithoutModifiers();
  void Test2DMeasureModifiersMoveCamera();
  void Test2DMeasureRightButton();
  void Test2DMeasureMoveHighlights();
  void Test2DMeasurePerspectiveView();
  void TestEllipseCtrlRotates();
  void TestModifierKeysReachAllTools();
  void TestCaptureLostReleasesButtons();

  /** Call the method of the mouse event on the registered PER, as the render window interactor does */
  void SendMouse(albaID id, double x, double y, int button = 0, int modifier = -1, int modifier2 = -1);

  /** Press a button at (400, 300), drag to (x, y) and release it */
  void Drag(int button, double x, double y, int modifier = -1, int modifier2 = -1);

  /** Send a wheel rotation to the registered PER */
  void SendWheel(double x, double y, double rotation);

  /** Return a new PER registered in albaServiceLocator, with the tool pushed and m_Listener as listener */
  albaPositionalEventRouter *NewPERWithTool(albaInteractionTool *tool);

  /** Register again the previous PER and delete this one */
  void DeletePER(albaPositionalEventRouter *per);

  /** Add to the renderer a sphere of radius 10 at the origin, picked as the returned VME */
  albaVMESurface *AddSphereVME();

  /** Use a parallel camera and disable the spin, as the 2D views do */
  void Set2DCamera();

  wxFrame                   *m_Frame;
  albaRWIBase               *m_Rwi;
  albaRoutingTestView       *m_View;
  albaRoutingTestListener    m_Listener;
  albaPositionalEventRouter *m_OldPER; ///< the PER registered before the test, restored at its end
};

#endif
