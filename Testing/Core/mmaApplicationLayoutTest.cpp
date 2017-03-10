/*=========================================================================

 Program: MAF2
 Module: mmaApplicationLayoutTest
 Authors: Paolo Quadrani
 
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
#include "mmaApplicationLayoutTest.h"
#include "mafCoreTests.h"
#include "mafSmartPointer.h"
#include "mmaApplicationLayout.h"
#include "mafView.h"
#include "mafGUIMDIChild.h"

#include "wx/module.h"
#include <iostream>
#include "wx/app.h"
#include "mafViewVTK.h"


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

  mafNEW(appLayout);
  result = appLayout->GetReferenceCount() == 1;
  TEST_RESULT;
  appLayout->Delete();
}


//----------------------------------------------------------------------------
void mmaApplicationLayoutTest::TestEquals()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mmaApplicationLayout> appLayout1;
  mafSmartPointer<mmaApplicationLayout> appLayout2;

	mafView *v1 = new mafViewVTK("v1");
	mafView *v2 = new mafViewVTK("v2");
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
  mafSmartPointer<mmaApplicationLayout> appLayout1;
  mafSmartPointer<mmaApplicationLayout> appLayout2;

  mafView *v1 = new mafViewVTK("v1");
  mafView *v2 = new mafViewVTK("v2");
  v1->SetFrame(m_Toplevel);
  v2->SetFrame(m_Toplevel);


  mafView *v3 = new mafViewVTK("v3");
  v3->SetFrame(m_Toplevel);

  appLayout2->DeepCopy(appLayout1);

  result = appLayout1->Equals(appLayout2);
  TEST_RESULT;

  cppDEL(v1);
  cppDEL(v2);
  cppDEL(v3);
}
 