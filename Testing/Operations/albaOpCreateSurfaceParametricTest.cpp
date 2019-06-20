/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSurfaceParametricTest
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

#include "albaOpCreateSurfaceParametricTest.h"

#include "albaVMESurfaceParametric.h"
#include "albaOpCreateSurfaceParametric.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaOpCreateSurfaceParametricTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpCreateSurfaceParametricTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_CreateSurfaceParametric = new albaOpCreateSurfaceParametric();
}
//----------------------------------------------------------------------------
void albaOpCreateSurfaceParametricTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_CreateSurfaceParametric);
}
//----------------------------------------------------------------------------
void albaOpCreateSurfaceParametricTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateSurfaceParametric->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void albaOpCreateSurfaceParametricTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_CreateSurfaceParametric->OpRun();
  result = albaVMESurfaceParametric::SafeDownCast(m_CreateSurfaceParametric->GetOutput())!=NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOpCreateSurfaceParametricTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateSurfaceParametric->SetListener(observer);
  m_CreateSurfaceParametric->SetInput(vme);
  m_CreateSurfaceParametric->OpRun();
  m_CreateSurfaceParametric->OpDo();
  result = albaVMESurfaceParametric::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateSurfaceParametric->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
