/*=========================================================================

 Program: MAF2
 Module: mafOpCreateProberTest
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
#include "mafOperationsTests.h"


#include "mafOpCreateProberTest.h"

#include "mafVMEProber.h"
#include "mafOpCreateProber.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafOpCreateProberTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCreateProberTest::BeforeTest()
//----------------------------------------------------------------------------
{
	m_CreateProber = new mafOpCreateProber();
}
//----------------------------------------------------------------------------
void mafOpCreateProberTest::AfterTest()
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
