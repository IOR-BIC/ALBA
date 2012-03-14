/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpCreateProberTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:32 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2008
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafOpCreateProberTest.h"

#include "mafVMEProber.h"
#include "mafOpCreateProber.h"

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

  DummyObserver() {};
  ~DummyObserver() {};

  /*virtual*/ void OnEvent(mafEventBase *maf_event);

protected:
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
void mafOpCreateProberTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCreateProberTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;

  m_CreateProber = new mafOpCreateProber();
}
//----------------------------------------------------------------------------
void mafOpCreateProberTest::tearDown()
//----------------------------------------------------------------------------
{
  cppDEL(m_CreateProber);
}
//----------------------------------------------------------------------------
void mafOpCreateProberTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateProber->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void mafOpCreateProberTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_CreateProber->OpRun();
  result = mafVMEProber::SafeDownCast(m_CreateProber->GetOutput())!=NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOpCreateProberTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateProber->SetListener(observer);
  m_CreateProber->SetInput(vme);
  m_CreateProber->OpRun();
  m_CreateProber->OpDo();
  result = mafVMEProber::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateProber->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
