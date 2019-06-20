/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmaApplicationLayoutTest
 Authors: Paolo Quadrani
 
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
#include "mmaApplicationLayoutTest.h"
#include "albaCoreTests.h"
#include "albaSmartPointer.h"
#include "mmaApplicationLayout.h"
#include "albaView.h"
#include "albaGUIMDIChild.h"

#include "wx/module.h"
#include <iostream>
#include "wx/app.h"
#include "albaViewVTK.h"


#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mmaApplicationLayoutTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmaApplicationLayoutTest::BeforeTest()
//----------------------------------------------------------------------------
{
	m_Toplevel = new wxFrame(m_App->GetTopWindow(),-1,"Title");
}
//----------------------------------------------------------------------------
void mmaApplicationLayoutTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mmaApplicationLayout *appLayout = mmaApplicationLayout::New();
  result = appLayout->GetReferenceCount() == 0;
  TEST_RESULT;
  appLayout->Delete();

  albaNEW(appLayout);
  result = appLayout->GetReferenceCount() == 1;
  TEST_RESULT;
  appLayout->Delete();
}


//----------------------------------------------------------------------------
void mmaApplicationLayoutTest::TestEquals()
//----------------------------------------------------------------------------
{
  albaSmartPointer<mmaApplicationLayout> appLayout1;
  albaSmartPointer<mmaApplicationLayout> appLayout2;

	albaView *v1 = new albaViewVTK("v1");
	albaView *v2 = new albaViewVTK("v2");
  v1->SetFrame(m_Toplevel);
  v2->SetFrame(m_Toplevel);

  result = appLayout1->Equals(appLayout2);
  TEST_RESULT;

  cppDEL(v1);
  cppDEL(v2);
}
//----------------------------------------------------------------------------
void mmaApplicationLayoutTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  albaSmartPointer<mmaApplicationLayout> appLayout1;
  albaSmartPointer<mmaApplicationLayout> appLayout2;

  albaView *v1 = new albaViewVTK("v1");
  albaView *v2 = new albaViewVTK("v2");
  v1->SetFrame(m_Toplevel);
  v2->SetFrame(m_Toplevel);


  albaView *v3 = new albaViewVTK("v3");
  v3->SetFrame(m_Toplevel);

  appLayout2->DeepCopy(appLayout1);

  result = appLayout1->Equals(appLayout2);
  TEST_RESULT;

  cppDEL(v1);
  cppDEL(v2);
  cppDEL(v3);
}
 