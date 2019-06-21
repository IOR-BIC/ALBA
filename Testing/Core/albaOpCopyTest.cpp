/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCopyTest
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
#include "albaOpCopyTest.h"

#include "albaOpSelect.h"
#include "albaVMEGroup.h"
#include "albaVMESurface.h"

//----------------------------------------------------------------------------
void albaOpCopyTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpCopyTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_OpCopy=new albaOpCopy("COPY");
  m_OpCopy->ClipboardClear(); //m_Clipboard is a static variable, so is necessary reinitialize before any test
}
//----------------------------------------------------------------------------
void albaOpCopyTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_OpCopy);
}
//----------------------------------------------------------------------------
void albaOpCopyTest::TestOpDo()
//----------------------------------------------------------------------------
{
  albaVMEGroup *groupParent;
  albaNEW(groupParent);
  groupParent->SetName("GROUP_PARENT");
  albaVMEGroup *groupChild;
  albaNEW(groupChild);
  groupChild->SetName("GROUP_CHILD");
  albaVMESurface *surfaceChild;
  albaNEW(surfaceChild);
  surfaceChild->SetName("SURFACE_CHILD");

  groupChild->ReparentTo(groupParent);
  surfaceChild->ReparentTo(groupChild);
  m_OpCopy->SetInput(groupChild);
  m_OpCopy->OpDo();

  CPPUNIT_ASSERT(m_OpCopy->GetClipboard()!=groupChild);
  CPPUNIT_ASSERT(strcmp(m_OpCopy->GetClipboard()->GetName(),"copy of GROUP_CHILD")==0);
  CPPUNIT_ASSERT(m_OpCopy->GetClipboard()->IsA("albaVMEGroup"));
  CPPUNIT_ASSERT(m_OpCopy->GetClipboard()->GetNumberOfChildren()==1);
  CPPUNIT_ASSERT(m_OpCopy->GetClipboard()->GetChild(0)->IsA("albaVMESurface"));

  albaDEL(surfaceChild);
  albaDEL(groupChild);
  albaDEL(groupParent);
}
//----------------------------------------------------------------------------
void albaOpCopyTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  albaVMEGroup *groupParent;
  albaNEW(groupParent);
  groupParent->SetName("GROUP_PARENT");
  albaVMEGroup *groupChild;
  albaNEW(groupChild);
  groupChild->SetName("GROUP_CHILD");
  albaVMESurface *surfaceChild;
  albaNEW(surfaceChild);
  surfaceChild->SetName("SURFACE_CHILD");

  groupChild->ReparentTo(groupParent);
  surfaceChild->ReparentTo(groupChild);
  m_OpCopy->SetInput(groupChild);
  m_OpCopy->OpDo();

  m_OpCopy->OpUndo();

  CPPUNIT_ASSERT(m_OpCopy->GetClipboard()==NULL);

  albaDEL(surfaceChild);
  albaDEL(groupChild);
  albaDEL(groupParent);
}
//----------------------------------------------------------------------------
void albaOpCopyTest::TestOpAccept()
//----------------------------------------------------------------------------
{
  albaVMEGroup *groupParent;
  albaNEW(groupParent);

  CPPUNIT_ASSERT(m_OpCopy->Accept(groupParent));

  albaDEL(groupParent);
}
