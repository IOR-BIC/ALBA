/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformSlidersTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include "albaInteractionTests.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "albaGUITransformSlidersTest.h"
#include "albaGUITransformSliders.h"

#include "albaVMEGroup.h"
#include "albaMatrix.h"
#include "albaTransform.h"

#define EPSILON 0.001

//-----------------------------------------------------------
void albaGUITransformSlidersTest::TestFixture()
//-----------------------------------------------------------
{

}
//-----------------------------------------------------------
void albaGUITransformSlidersTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  double range[6] = {0,0,0,0,0,0};
  albaGUITransformSliders *transform = new albaGUITransformSliders(group,range,NULL,true,true);
  delete transform;
}
//-----------------------------------------------------------
void albaGUITransformSlidersTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void albaGUITransformSlidersTest::TestSetAbsPose1() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  double range[6] = {0,0,0,0,0,0};
  albaGUITransformSliders *transform = new albaGUITransformSliders(group,range,NULL,true,true);

  albaMatrix *matrix = new albaMatrix();
  albaTransform::SetPosition(*matrix,10,20,30);
  albaTransform::SetOrientation(*matrix,10.5,-15,32.8);
  albaTransform::Scale(*matrix,2.4,0.5,3.0,PRE_MULTIPLY);

  transform->SetAbsPose(matrix);

  double pos[3],orientation[3],scaling[3];
  transform->GetPosition(pos);
  transform->GetOrientation(orientation);
  transform->GetScaling(scaling);

  CPPUNIT_ASSERT(pos[0] == 10 && pos[1] == 20 && pos[2] == 30);
  CPPUNIT_ASSERT(orientation[0]+EPSILON > 10.5 && orientation[0]-EPSILON < 10.5);
  CPPUNIT_ASSERT(orientation[1]+EPSILON > -15 && orientation[1]-EPSILON < -15);
  CPPUNIT_ASSERT(orientation[2]+EPSILON > 32.8 && orientation[2]-EPSILON < 32.8);
  CPPUNIT_ASSERT(scaling[0]+EPSILON > 2.4 && scaling[0]-EPSILON < 2.4);
  CPPUNIT_ASSERT(scaling[1]+EPSILON > 0.5 && scaling[1]-EPSILON < 0.5);
  CPPUNIT_ASSERT(scaling[2]+EPSILON > 3.0 && scaling[2]-EPSILON < 3.0);

  delete matrix;
  delete transform;
}
//-----------------------------------------------------------
void albaGUITransformSlidersTest::TestSetAbsPose2() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  group->SetAbsPose(15,-10,20,0.0,0.0,0.0);
  group->Update();
  double range[6] = {0,0,0,0,0,0};
  albaGUITransformSliders *transform = new albaGUITransformSliders(group,range,NULL,true,true);

  albaMatrix *matrix = new albaMatrix();
  albaTransform::SetPosition(*matrix,10,20,30);

  transform->SetAbsPose(matrix);

  double pos[3];
  transform->GetPosition(pos);

  CPPUNIT_ASSERT(pos[0] == -5 && pos[1] == 30 && pos[2] == 10);

  delete matrix;
  delete transform;
}
//-----------------------------------------------------------
void albaGUITransformSlidersTest::TestSetAbsPose3() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  group->SetAbsPose(0,0,0,15.0,0.0,0.0);
  group->Update();
  double range[6] = {0,0,0,0,0,0};
  albaGUITransformSliders *transform = new albaGUITransformSliders(group,range,NULL,true,true);

  albaMatrix *matrix = new albaMatrix();
  albaTransform::SetOrientation(*matrix,10,0,0);

  transform->SetAbsPose(matrix);

  double orientation[3];
  transform->GetOrientation(orientation);

  CPPUNIT_ASSERT(orientation[0]+EPSILON > -5.0 && orientation[0]-EPSILON < -5.0);
  CPPUNIT_ASSERT(orientation[1]+EPSILON > 0.0 && orientation[1]-EPSILON < 0.0);
  CPPUNIT_ASSERT(orientation[2]+EPSILON > 0.0 && orientation[2]-EPSILON < 0.0);

  delete matrix;
  delete transform;
}
//-----------------------------------------------------------
void albaGUITransformSlidersTest::TestReset()
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  double range[6] = {0,0,0,0,0,0};
  albaGUITransformSliders *transform = new albaGUITransformSliders(group,range,NULL,true,true);

  albaMatrix *matrix = new albaMatrix();
  albaTransform::SetPosition(*matrix,10,20,30);
  albaTransform::SetOrientation(*matrix,10.5,-15,32.8);
  albaTransform::Scale(*matrix,2.4,0.5,3.0,PRE_MULTIPLY);

  transform->SetAbsPose(matrix);

  double pos[3],orientation[3],scaling[3];
  transform->GetPosition(pos);
  transform->GetOrientation(orientation);
  transform->GetScaling(scaling);

  CPPUNIT_ASSERT(pos[0] == 10 && pos[1] == 20 && pos[2] == 30);
  CPPUNIT_ASSERT(orientation[0]+EPSILON > 10.5 && orientation[0]-EPSILON < 10.5);
  CPPUNIT_ASSERT(orientation[1]+EPSILON > -15 && orientation[1]-EPSILON < -15);
  CPPUNIT_ASSERT(orientation[2]+EPSILON > 32.8 && orientation[2]-EPSILON < 32.8);
  CPPUNIT_ASSERT(scaling[0]+EPSILON > 2.4 && scaling[0]-EPSILON < 2.4);
  CPPUNIT_ASSERT(scaling[1]+EPSILON > 0.5 && scaling[1]-EPSILON < 0.5);
  CPPUNIT_ASSERT(scaling[2]+EPSILON > 3.0 && scaling[2]-EPSILON < 3.0);

  transform->Reset();

  transform->GetPosition(pos);
  transform->GetOrientation(orientation);
  transform->GetScaling(scaling);

  CPPUNIT_ASSERT(pos[0] == .0 && pos[1] == .0 && pos[2] == .0);
  CPPUNIT_ASSERT(orientation[0]+EPSILON > .0 && orientation[0]-EPSILON < .0);
  CPPUNIT_ASSERT(orientation[1]+EPSILON > .0 && orientation[1]-EPSILON < .0);
  CPPUNIT_ASSERT(orientation[2]+EPSILON > .0 && orientation[2]-EPSILON < .0);
  CPPUNIT_ASSERT(scaling[0]+EPSILON > 1.0 && scaling[0]-EPSILON < 1.0);
  CPPUNIT_ASSERT(scaling[1]+EPSILON > 1.0 && scaling[1]-EPSILON < 1.0);
  CPPUNIT_ASSERT(scaling[2]+EPSILON > 1.0 && scaling[2]-EPSILON < 1.0);

  delete matrix;
  delete transform;
}
//-----------------------------------------------------------
void albaGUITransformSlidersTest::TestOnEvent() 
//-----------------------------------------------------------
{
  DummyObserver *dummyObserver = new DummyObserver();

  albaSmartPointer<albaVMEGroup> group;
  double range[6] = {0,0,0,0,0,0};
  albaGUITransformSliders *transform = new albaGUITransformSliders(group,range,dummyObserver,true,true);

  //  in response to an ID_TRANSLATE_X event from GUI...(*)
  albaEvent eventSent(this, albaGUITransformSliders::ID_TRANSLATE_X);
  transform->OnEvent(&eventSent);

  int dummyReceivedEventID = dummyObserver->GetLastReceivedEventID();

  // (*)... the observer is notified with a ID_TRANSFORM event 
  // containing the already set VME ABS matrix
  CPPUNIT_ASSERT(dummyReceivedEventID == ID_TRANSFORM);

  cppDEL(transform);

  cppDEL(dummyObserver);
  delete transform;
}
