/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSplineTest
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
#include "albaOperationsTests.h"

#include <cppunit/config/SourcePrefix.h>
#include "albaOpCreateSplineTest.h"

#include "albaVMEPolylineSpline.h"
#include "albaOpCreateSpline.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaOpCreateSplineTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpCreateSplineTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_CreateSpline = new albaOpCreateSpline();
}
//----------------------------------------------------------------------------
void albaOpCreateSplineTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_CreateSpline);
}
//----------------------------------------------------------------------------
void albaOpCreateSplineTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateSpline->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void albaOpCreateSplineTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_CreateSpline->OpRun();
  result = albaVMEPolylineSpline::SafeDownCast(m_CreateSpline->GetOutput())!=NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOpCreateSplineTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateSpline->SetListener(observer);
  m_CreateSpline->SetInput(vme);
  m_CreateSpline->OpRun();
  m_CreateSpline->OpDo();
  result = albaVMEPolylineSpline::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateSpline->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
