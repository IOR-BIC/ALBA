/*=========================================================================

 Program: MAF2
 Module: mafOpCreateSlicerTest
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

#include "mafOpCreateSlicerTest.h"

#include "mafVMEOutputNULL.h"
#include "mafVMESlicer.h"
#include "mafOpCreateSlicer.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

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
