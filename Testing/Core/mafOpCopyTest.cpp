/*=========================================================================

 Program: MAF2
 Module: mafOpCopyTest
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
#include "mafOpCopyTest.h"

#include "mafOpSelect.h"
#include "mafVMEGroup.h"
#include "mafVMESurface.h"

//----------------------------------------------------------------------------
void mafOpCopyTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCopyTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_OpCopy=new mafOpCopy("COPY");
  m_OpCopy->ClipboardClear(); //m_Clipboard is a static variable, so is necessary reinitialize before any test
}
//----------------------------------------------------------------------------
void mafOpCopyTest::AfterTest()
//----------------------------------------------------------------------------
{
  mafDEL(m_OpCopy);
}
//----------------------------------------------------------------------------
void mafOpCopyTest::TestOpDo()
//----------------------------------------------------------------------------
{
  mafVMEGroup *groupParent;
  mafNEW(groupParent);
  groupParent->SetName("GROUP_PARENT");
  mafVMEGroup *groupChild;
  mafNEW(groupChild);
  groupChild->SetName("GROUP_CHILD");
  mafVMESurface *surfaceChild;
  mafNEW(surfaceChild);
  surfaceChild->SetName("SURFACE_CHILD");

  groupChild->ReparentTo(groupParent);
  surfaceChild->ReparentTo(groupChild);
  m_OpCopy->SetInput(groupChild);
  m_OpCopy->OpDo();

  CPPUNIT_ASSERT(m_OpCopy->GetClipboard()!=groupChild);
  CPPUNIT_ASSERT(strcmp(m_OpCopy->GetClipboard()->GetName(),"copy of GROUP_CHILD")==0);
  CPPUNIT_ASSERT(m_OpCopy->GetClipboard()->IsA("mafVMEGroup"));
  CPPUNIT_ASSERT(m_OpCopy->GetClipboard()->GetNumberOfChildren()==1);
  CPPUNIT_ASSERT(m_OpCopy->GetClipboard()->GetChild(0)->IsA("mafVMESurface"));

  mafDEL(surfaceChild);
  mafDEL(groupChild);
  mafDEL(groupParent);
}
//----------------------------------------------------------------------------
void mafOpCopyTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  mafVMEGroup *groupParent;
  mafNEW(groupParent);
  groupParent->SetName("GROUP_PARENT");
  mafVMEGroup *groupChild;
  mafNEW(groupChild);
  groupChild->SetName("GROUP_CHILD");
  mafVMESurface *surfaceChild;
  mafNEW(surfaceChild);
  surfaceChild->SetName("SURFACE_CHILD");

  groupChild->ReparentTo(groupParent);
  surfaceChild->ReparentTo(groupChild);
  m_OpCopy->SetInput(groupChild);
  m_OpCopy->OpDo();

  m_OpCopy->OpUndo();

  CPPUNIT_ASSERT(m_OpCopy->GetClipboard()==NULL);

  mafDEL(surfaceChild);
  mafDEL(groupChild);
  mafDEL(groupParent);
}
//----------------------------------------------------------------------------
void mafOpCopyTest::TestOpAccept()
//----------------------------------------------------------------------------
{
  mafVMEGroup *groupParent;
  mafNEW(groupParent);

  CPPUNIT_ASSERT(m_OpCopy->Accept(groupParent));

  mafDEL(groupParent);
}
