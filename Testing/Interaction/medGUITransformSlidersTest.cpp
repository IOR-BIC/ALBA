/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUITransformSlidersTest.cpp,v $
Language:  C++
Date:      $Date: 2010-07-21 13:04:47 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "medGUITransformSlidersTest.h"
#include "medGUITransformSliders.h"

#include "mafVMEGroup.h"
#include "mafMatrix.h"
#include "mafTransform.h"

#define EPSILON 0.001

//----------------------------------------------------------------------------
class DummyObserver : public mafObserver
//----------------------------------------------------------------------------
{
public:

  DummyObserver() 
  {
  };
  ~DummyObserver()
  {
  };

  virtual void OnEvent(mafEventBase *maf_event)
  {
  }

protected:
};
//-----------------------------------------------------------
void medGUITransformSlidersTest::setUp()
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medGUITransformSlidersTest::tearDown()
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medGUITransformSlidersTest::TestFixture()
//-----------------------------------------------------------
{

}
//-----------------------------------------------------------
void medGUITransformSlidersTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  mafSmartPointer<mafVMEGroup> group;
  double range[6] = {0,0,0,0,0,0};
  medGUITransformSliders *transform = new medGUITransformSliders(group,range,NULL,true,true);
  delete transform;
  delete observer;
}
//-----------------------------------------------------------
void medGUITransformSlidersTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medGUITransformSlidersTest::TestSetAbsPose1() 
//-----------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  mafSmartPointer<mafVMEGroup> group;
  double range[6] = {0,0,0,0,0,0};
  medGUITransformSliders *transform = new medGUITransformSliders(group,range,NULL,true,true);

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
  delete observer;
}
//-----------------------------------------------------------
void medGUITransformSlidersTest::TestSetAbsPose2() 
//-----------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  mafSmartPointer<mafVMEGroup> group;
  group->SetAbsPose(15,-10,20,0.0,0.0,0.0);
  group->Update();
  double range[6] = {0,0,0,0,0,0};
  medGUITransformSliders *transform = new medGUITransformSliders(group,range,NULL,true,true);

  mafMatrix *matrix = new mafMatrix();
  mafTransform::SetPosition(*matrix,10,20,30);

  transform->SetAbsPose(matrix);

  double pos[3],orientation[3],scaling[3];
  transform->GetPosition(pos);

  CPPUNIT_ASSERT(pos[0] == -5 && pos[1] == 30 && pos[2] == 10);

  delete matrix;
  delete transform;
  delete observer;
}
//-----------------------------------------------------------
void medGUITransformSlidersTest::TestSetAbsPose3() 
//-----------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  mafSmartPointer<mafVMEGroup> group;
  group->SetAbsPose(0,0,0,15.0,0.0,0.0);
  group->Update();
  double range[6] = {0,0,0,0,0,0};
  medGUITransformSliders *transform = new medGUITransformSliders(group,range,NULL,true,true);

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
  delete observer;
}
//-----------------------------------------------------------
void medGUITransformSlidersTest::TestReset()
//-----------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  mafSmartPointer<mafVMEGroup> group;
  double range[6] = {0,0,0,0,0,0};
  medGUITransformSliders *transform = new medGUITransformSliders(group,range,NULL,true,true);

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
  delete observer;
}
