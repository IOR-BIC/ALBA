/*=========================================================================

 Program: MAF2
 Module: mafOpCreateSplineTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafOpCreateSplineTest.h"

#include "mafVMEPolylineSpline.h"
#include "mafOpCreateSpline.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
class DummyVme : public mafVME
//----------------------------------------------------------------------------
{
public:
  DummyVme(){};
  ~DummyVme(){};

  mafTypeMacro(DummyVme,mafVME);

  /*virtual*/ void SetMatrix(const mafMatrix &mat){};
  /*virtual*/ void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){};

protected:
private:
};

mafCxxTypeMacro(DummyVme);

//----------------------------------------------------------------------------
class DummyObserver : public mafObserver
  //----------------------------------------------------------------------------
{
public:

  DummyObserver() {m_DummyVme = new DummyVme();};
  ~DummyObserver() {delete m_DummyVme;};

  /*virtual*/ void OnEvent(mafEventBase *maf_event);

protected:
  DummyVme *m_DummyVme;
};
//----------------------------------------------------------------------------
void DummyObserver::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    if(e->GetId() == VME_REMOVE)
    {
      e->GetVme()->ReparentTo(NULL);
    }
  }
}

//----------------------------------------------------------------------------
void mafOpCreateSplineTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCreateSplineTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
  m_CreateSpline = new mafOpCreateSpline();
}
//----------------------------------------------------------------------------
void mafOpCreateSplineTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_CreateSpline);
}
//----------------------------------------------------------------------------
void mafOpCreateSplineTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateSpline->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void mafOpCreateSplineTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_CreateSpline->OpRun();
  result = mafVMEPolylineSpline::SafeDownCast(m_CreateSpline->GetOutput())!=NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOpCreateSplineTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateSpline->SetListener(observer);
  m_CreateSpline->SetInput(vme);
  m_CreateSpline->OpRun();
  m_CreateSpline->OpDo();
  result = mafVMEPolylineSpline::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateSpline->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
