/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpCopyTest.cpp,v $
Language:  C++
Date:      $Date: 2008-02-22 14:34:43 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2008
CINECA - Interuniversity Consortium (www.cineca.it)
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
void mafOpCopyTest::setUp()
//----------------------------------------------------------------------------
{
  m_OpCopy=new mafOpCopy("COPY");
  m_OpCopy->ClipboardClear(); //m_Clipboard is a static variable, so is necessary reinitialize before any test
}
//----------------------------------------------------------------------------
void mafOpCopyTest::tearDown()
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

  CPPUNIT_ASSERT(m_OpCopy->m_Clipboard.GetPointer()!=groupChild);
  CPPUNIT_ASSERT(strcmp(m_OpCopy->m_Clipboard.GetPointer()->GetName(),"copy of GROUP_CHILD")==0);
  CPPUNIT_ASSERT(m_OpCopy->m_Clipboard.GetPointer()->IsA("mafVMEGroup"));
  CPPUNIT_ASSERT(m_OpCopy->m_Clipboard.GetPointer()->GetNumberOfChildren()==1);
  CPPUNIT_ASSERT(m_OpCopy->m_Clipboard.GetPointer()->GetChild(0)->IsA("mafVMESurface"));

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

  CPPUNIT_ASSERT(m_OpCopy->m_Clipboard==NULL);

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
