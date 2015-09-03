/*=========================================================================

 Program: MAF2
 Module: mafOpCreateRefSysTest
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
#include "mafOpCreateRefSysTest.h"

#include "mafVMEOutput.h"
#include "mafVMEOutputNULL.h"
#include "mafVMERefSys.h"
#include "mafOpCreateRefSys.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafOpCreateRefSysTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCreateRefSysTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_CreateRefSys = new mafOpCreateRefSys();
}
//----------------------------------------------------------------------------
void mafOpCreateRefSysTest::AfterTest()
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
