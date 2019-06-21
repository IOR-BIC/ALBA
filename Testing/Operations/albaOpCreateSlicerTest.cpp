/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSlicerTest
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

#include "albaOpCreateSlicerTest.h"

#include "albaVMEOutputNULL.h"
#include "albaVMESlicer.h"
#include "albaOpCreateSlicer.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaOpCreateSlicerTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpCreateSlicerTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_CreateSlicer = new albaOpCreateSlicer();
}
//----------------------------------------------------------------------------
void albaOpCreateSlicerTest::AfterTest()
//----------------------------------------------------------------------------
{
  cppDEL(m_CreateSlicer);
}
//----------------------------------------------------------------------------
void albaOpCreateSlicerTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateSlicer->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void albaOpCreateSlicerTest::TestOpRun()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  m_CreateSlicer->SetListener(observer);
  m_CreateSlicer->OpRun();
  result = albaVMESlicer::SafeDownCast(m_CreateSlicer->GetOutput())!=NULL;
  TEST_RESULT;
  delete observer;
}
//----------------------------------------------------------------------------
void albaOpCreateSlicerTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateSlicer->SetListener(observer);
  m_CreateSlicer->SetInput(vme);
  m_CreateSlicer->OpRun();
  m_CreateSlicer->OpDo();
  result = albaVMESlicer::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateSlicer->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
