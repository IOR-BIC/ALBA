/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateProberTest
 Authors: Matteo Giacomoni
 
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
#include "albaOperationsTests.h"


#include "albaOpCreateProberTest.h"

#include "albaVMEProber.h"
#include "albaOpCreateProber.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaOpCreateProberTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpCreateProberTest::BeforeTest()
//----------------------------------------------------------------------------
{
	m_CreateProber = new albaOpCreateProber();
}
//----------------------------------------------------------------------------
void albaOpCreateProberTest::AfterTest()
//----------------------------------------------------------------------------
{
  cppDEL(m_CreateProber);
}
//----------------------------------------------------------------------------
void albaOpCreateProberTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateProber->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void albaOpCreateProberTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_CreateProber->OpRun();
  result = albaVMEProber::SafeDownCast(m_CreateProber->GetOutput())!=NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOpCreateProberTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateProber->SetListener(observer);
  m_CreateProber->SetInput(vme);
  m_CreateProber->OpRun();
  m_CreateProber->OpDo();
  result = albaVMEProber::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateProber->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
