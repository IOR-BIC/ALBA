/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateMeterTest
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

#include "albaOpCreateMeterTest.h"

#include "albaOpCreateMeter.h"
#include "albaVMEMeter.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaOpCreateMeterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpCreateMeterTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_CreateMeter = new albaOpCreateMeter();
}
//----------------------------------------------------------------------------
void albaOpCreateMeterTest::AfterTest()
//----------------------------------------------------------------------------
{
  cppDEL(m_CreateMeter);
}
//----------------------------------------------------------------------------
void albaOpCreateMeterTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateMeter->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void albaOpCreateMeterTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_CreateMeter->OpRun();
  result = albaVMEMeter::SafeDownCast(m_CreateMeter->GetOutput())!=NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOpCreateMeterTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateMeter->SetListener(observer);
  m_CreateMeter->SetInput(vme);
  m_CreateMeter->OpRun();
  m_CreateMeter->OpDo();
  result = albaVMEMeter::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateMeter->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
