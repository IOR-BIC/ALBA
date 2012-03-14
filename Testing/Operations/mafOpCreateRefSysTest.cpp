/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpCreateRefSysTest.cpp,v $
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
#include "mafOpCreateRefSysTest.h"

#include "mafVMEOutput.h"
#include "mafVMEOutputNULL.h"
#include "mafVMERefSys.h"
#include "mafOpCreateRefSys.h"

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
  /*virtual*/ mafVMEOutput *GetOutput();

protected:
private:
};

//-------------------------------------------------------------------------
mafVMEOutput *DummyVme::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputNULL::New()); // create the output
  }
  return m_Output;
}

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
void mafOpCreateRefSysTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCreateRefSysTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
  m_CreateRefSys = new mafOpCreateRefSys();
}
//----------------------------------------------------------------------------
void mafOpCreateRefSysTest::tearDown()
//----------------------------------------------------------------------------
{
  cppDEL(m_CreateRefSys);
}
//----------------------------------------------------------------------------
void mafOpCreateRefSysTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateRefSys->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void mafOpCreateRefSysTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_CreateRefSys->OpRun();
  result = mafVMERefSys::SafeDownCast(m_CreateRefSys->GetOutput())!=NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOpCreateRefSysTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateRefSys->SetListener(observer);
  m_CreateRefSys->SetInput(vme);
  m_CreateRefSys->OpRun();
  m_CreateRefSys->OpDo();
  result = mafVMERefSys::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateRefSys->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
