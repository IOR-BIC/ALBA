/*=========================================================================

 Program: MAF2
 Module: mafGUITransformSlidersTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "mafInteractionTests.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafGUITransformSlidersTest.h"
#include "mafGUITransformSliders.h"

#include "mafVMEGroup.h"
#include "mafMatrix.h"
#include "mafTransform.h"

#define EPSILON 0.001


//-----------------------------------------------------------
void mafGUITransformSlidersTest::setUp()
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void mafGUITransformSlidersTest::tearDown()
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void mafGUITransformSlidersTest::TestFixture()
//-----------------------------------------------------------
{

}
//-----------------------------------------------------------
void mafGUITransformSlidersTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  double range[6] = {0,0,0,0,0,0};
  mafGUITransformSliders *transform = new mafGUITransformSliders(group,range,NULL,true,true);
  delete transform;
}
//-----------------------------------------------------------
void mafGUITransformSlidersTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void mafGUITransformSlidersTest::TestSetAbsPose1() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  double range[6] = {0,0,0,0,0,0};
  mafGUITransformSliders *transform = new mafGUITransformSliders(group,range,NULL,true,true);

  mafMatrix *matrix = new mafMatrix();
  mafTransform::SetPosition(*matrix,10,20,30);
  mafTransform::SetOrientation(*matrix,10.5,-15,32.8);
  mafTransform::Scale(*matrix,2.4,0.5,3.0,PRE_MULTIPLY);

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
void mafGUITransformSlidersTest::TestSetAbsPose2() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  group->SetAbsPose(15,-10,20,0.0,0.0,0.0);
  group->Update();
  double range[6] = {0,0,0,0,0,0};
  mafGUITransformSliders *transform = new mafGUITransformSliders(group,range,NULL,true,true);

  mafMatrix *matrix = new mafMatrix();
  mafTransform::SetPosition(*matrix,10,20,30);

  transform->SetAbsPose(matrix);

  double pos[3];
  transform->GetPosition(pos);

  CPPUNIT_ASSERT(pos[0] == -5 && pos[1] == 30 && pos[2] == 10);

  delete matrix;
  delete transform;
}
//-----------------------------------------------------------
void mafGUITransformSlidersTest::TestSetAbsPose3() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  group->SetAbsPose(0,0,0,15.0,0.0,0.0);
  group->Update();
  double range[6] = {0,0,0,0,0,0};
  mafGUITransformSliders *transform = new mafGUITransformSliders(group,range,NULL,true,true);

  mafMatrix *matrix = new mafMatrix();
  mafTransform::SetOrientation(*matrix,10,0,0);

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
void mafGUITransformSlidersTest::TestReset()
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  double range[6] = {0,0,0,0,0,0};
  mafGUITransformSliders *transform = new mafGUITransformSliders(group,range,NULL,true,true);

  mafMatrix *matrix = new mafMatrix();
  mafTransform::SetPosition(*matrix,10,20,30);
  mafTransform::SetOrientation(*matrix,10.5,-15,32.8);
  mafTransform::Scale(*matrix,2.4,0.5,3.0,PRE_MULTIPLY);

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
void mafGUITransformSlidersTest::TestOnEvent() 
//-----------------------------------------------------------
{
  DummyObserver *dummyObserver = new DummyObserver();

  mafSmartPointer<mafVMEGroup> group;
  double range[6] = {0,0,0,0,0,0};
  mafGUITransformSliders *transform = new mafGUITransformSliders(group,range,dummyObserver,true,true);

  //  in response to an ID_TRANSLATE_X event from GUI...(*)
  mafEvent eventSent(this, mafGUITransformSliders::ID_TRANSLATE_X);
  transform->OnEvent(&eventSent);

  int dummyReceivedEventID = dummyObserver->GetLastReceivedEventID();

  // (*)... the observer is notified with a ID_TRANSFORM event 
  // containing the already set VME ABS matrix
  CPPUNIT_ASSERT(dummyReceivedEventID == ID_TRANSFORM);

  cppDEL(transform);

  cppDEL(dummyObserver);
  delete transform;
}
