/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEditTest
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
#include "albaOpEditTest.h"

#include "albaOpSelect.h"
#include "albaVMEGroup.h"

//----------------------------------------------------------------------------
void albaOpEditTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpEditTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_OpEdit=new albaOpEdit("EDIT");
}
//----------------------------------------------------------------------------
void albaOpEditTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_OpEdit);
}
//----------------------------------------------------------------------------
void albaOpEditTest::TestClipboardIsEmpty()
//----------------------------------------------------------------------------
{
  CPPUNIT_ASSERT(m_OpEdit->ClipboardIsEmpty());
  
  albaVMEGroup *group;
  albaNEW(group);
  
  m_OpEdit->SetClipboard(group);

  CPPUNIT_ASSERT(!m_OpEdit->ClipboardIsEmpty());

  albaDEL(group);
}
//----------------------------------------------------------------------------
void albaOpEditTest::TestClipboardClear()
//----------------------------------------------------------------------------
{
  albaVMEGroup *group;
  albaNEW(group);

  m_OpEdit->SetClipboard(group);

  m_OpEdit->ClipboardClear();

  CPPUNIT_ASSERT(m_OpEdit->ClipboardIsEmpty());

  albaDEL(group);
}
//----------------------------------------------------------------------------
void albaOpEditTest::TestClipboardBackupRestore()
//----------------------------------------------------------------------------
{
  albaVMEGroup *group;
  albaNEW(group);

  m_OpEdit->SetClipboard(group);

  m_OpEdit->ClipboardBackup();

  m_OpEdit->ClipboardClear();

  m_OpEdit->ClipboardRestore();

  CPPUNIT_ASSERT(!m_OpEdit->ClipboardIsEmpty());

  CPPUNIT_ASSERT(m_OpEdit->GetClipboard()==group);

  albaDEL(group);
}
