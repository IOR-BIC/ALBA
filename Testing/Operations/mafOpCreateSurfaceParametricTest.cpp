/*=========================================================================

 Program: MAF2
 Module: mafOpCreateSurfaceParametricTest
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

#include "mafOpCreateSurfaceParametricTest.h"

#include "mafVMESurfaceParametric.h"
#include "mafOpCreateSurfaceParametric.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafOpCreateSurfaceParametricTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCreateSurfaceParametricTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_CreateSurfaceParametric = new mafOpCreateSurfaceParametric();
}
//----------------------------------------------------------------------------
void mafOpCreateSurfaceParametricTest::AfterTest()
//----------------------------------------------------------------------------
{
  mafDEL(m_CreateSurfaceParametric);
}
//----------------------------------------------------------------------------
void mafOpCreateSurfaceParametricTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummy = new DummyVme();

  result = m_CreateSurfaceParametric->Accept(dummy);
  TEST_RESULT;

  delete dummy;
}
//----------------------------------------------------------------------------
void mafOpCreateSurfaceParametricTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_CreateSurfaceParametric->OpRun();
  result = mafVMESurfaceParametric::SafeDownCast(m_CreateSurfaceParametric->GetOutput())!=NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOpCreateSurfaceParametricTest::TestOpDo()
//----------------------------------------------------------------------------
{
  DummyObserver *observer = new DummyObserver();
  DummyVme *vme = new DummyVme();
  m_CreateSurfaceParametric->SetListener(observer);
  m_CreateSurfaceParametric->SetInput(vme);
  m_CreateSurfaceParametric->OpRun();
  m_CreateSurfaceParametric->OpDo();
  result = mafVMESurfaceParametric::SafeDownCast(vme->GetChild(0))!=NULL;
  TEST_RESULT;
  m_CreateSurfaceParametric->OpUndo();
  result = vme->GetNumberOfChildren()==0;
  TEST_RESULT;
  delete vme;
  delete observer;
}
