/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpCreateSlicerTest.cpp,v $
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
#include "mafOpCreateSlicerTest.h"

#include "mafVMEOutputNULL.h"
#include "mafVMESlicer.h"
#include "mafOpCreateSlicer.h"

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
    else if (e->GetId() == VME_CHOOSE)
    {
      e->SetVme(m_DummyVme);
    }
  }
}

//----------------------------------------------------------------------------
void mafOpCreateSlicerTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCreateSlicerTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
  m_CreateSlicer = new mafOpCreateSlicer();
}
//----------------------------------------------------------------------------
void mafOpCreateSlicerTest::tearDown()
//----------------------------------------------------------------------------
{
  cppDEL(m_CreateSlicer);
}
//----------------------------------------------------------------------------
void mafOpCreateSlicerTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateSlicer->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void mafOpCreateSlicerTest::TestOpRun()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  m_CreateSlicer->SetListener(observer);
  m_CreateSlicer->OpRun();
  result = mafVMESlicer::SafeDownCast(m_CreateSlicer->GetOutput())!=NULL;
  TEST_RESULT;
  delete observer;
}
//----------------------------------------------------------------------------
void mafOpCreateSlicerTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateSlicer->SetListener(observer);
  m_CreateSlicer->SetInput(vme);
  m_CreateSlicer->OpRun();
  m_CreateSlicer->OpDo();
  result = mafVMESlicer::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateSlicer->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
