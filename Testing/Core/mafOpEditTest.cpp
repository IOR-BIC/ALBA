/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpEditTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:32 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
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
#include "mafOpEditTest.h"

#include "mafOpSelect.h"
#include "mafVMEGroup.h"

//----------------------------------------------------------------------------
void mafOpEditTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpEditTest::setUp()
//----------------------------------------------------------------------------
{
  m_OpEdit=new mafOpEdit("EDIT");
}
//----------------------------------------------------------------------------
void mafOpEditTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_OpEdit);
}
//----------------------------------------------------------------------------
void mafOpEditTest::TestClipboardIsEmpty()
//----------------------------------------------------------------------------
{
  CPPUNIT_ASSERT(m_OpEdit->ClipboardIsEmpty());
  
  mafVMEGroup *group;
  mafNEW(group);
  
  m_OpEdit->SetClipboard(group);

  CPPUNIT_ASSERT(!m_OpEdit->ClipboardIsEmpty());

  mafDEL(group);
}
//----------------------------------------------------------------------------
void mafOpEditTest::TestClipboardClear()
//----------------------------------------------------------------------------
{
  mafVMEGroup *group;
  mafNEW(group);

  m_OpEdit->SetClipboard(group);

  m_OpEdit->ClipboardClear();

  CPPUNIT_ASSERT(m_OpEdit->ClipboardIsEmpty());

  mafDEL(group);
}
//----------------------------------------------------------------------------
void mafOpEditTest::TestClipboardBackupRestore()
//----------------------------------------------------------------------------
{
  mafVMEGroup *group;
  mafNEW(group);

  m_OpEdit->SetClipboard(group);

  m_OpEdit->ClipboardBackup();

  m_OpEdit->ClipboardClear();

  m_OpEdit->ClipboardRestore();

  CPPUNIT_ASSERT(!m_OpEdit->ClipboardIsEmpty());

  CPPUNIT_ASSERT(m_OpEdit->GetClipboard()==group);

  mafDEL(group);
}
