/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateRefSysTest
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
#include "albaOpCreateRefSysTest.h"

#include "albaVMEOutput.h"
#include "albaVMEOutputNULL.h"
#include "albaVMERefSys.h"
#include "albaOpCreateRefSys.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaOpCreateRefSysTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpCreateRefSysTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_CreateRefSys = new albaOpCreateRefSys();
}
//----------------------------------------------------------------------------
void albaOpCreateRefSysTest::AfterTest()
//----------------------------------------------------------------------------
{
  cppDEL(m_CreateRefSys);
}
//----------------------------------------------------------------------------
void albaOpCreateRefSysTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateRefSys->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void albaOpCreateRefSysTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_CreateRefSys->OpRun();
  result = albaVMERefSys::SafeDownCast(m_CreateRefSys->GetOutput())!=NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOpCreateRefSysTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateRefSys->SetListener(observer);
  m_CreateRefSys->SetInput(vme);
  m_CreateRefSys->OpRun();
  m_CreateRefSys->OpDo();
  result = albaVMERefSys::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateRefSys->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
