/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSelectTest
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
#include "albaOpSelectTest.h"

#include "albaOpSelect.h"
#include "albaVMEGroup.h"


//----------------------------------------------------------------------------
void albaOpSelectTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpSelectTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_OpSelect=new albaOpSelect("EDIT");
}
//----------------------------------------------------------------------------
void albaOpSelectTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_OpSelect);
}
//----------------------------------------------------------------------------
void albaOpSelectTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpSelect *OpSelect=new albaOpSelect("EDIT");
  albaDEL(OpSelect);
}
//----------------------------------------------------------------------------
void albaOpSelectTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaVMEGroup *group;
  albaNEW(group);

  bool result=m_OpSelect->Accept(group);
  
  CPPUNIT_ASSERT(result);
  
  albaDEL(group);
}
//----------------------------------------------------------------------------
void albaOpSelectTest::TestSetInput()
//----------------------------------------------------------------------------
{
  albaVMEGroup *group;
  albaNEW(group);

  m_OpSelect->SetInput(group);

  CPPUNIT_ASSERT(m_OpSelect->GetInput()==group);

  albaDEL(group);
}
//----------------------------------------------------------------------------
void albaOpSelectTest::TestSetNewSel()
//----------------------------------------------------------------------------
{
  albaVMEGroup *group;
  albaNEW(group);

  m_OpSelect->SetNewSel(group);

  CPPUNIT_ASSERT(m_OpSelect->GetNewSel()==group);

  albaDEL(group);
}
//----------------------------------------------------------------------------
void albaOpSelectTest::TestCopy()
//----------------------------------------------------------------------------
{
  albaOpSelect *opCopy;

  albaVMEGroup *groupInput;
  albaNEW(groupInput);

  m_OpSelect->SetInput(groupInput);

  albaVMEGroup *groupNewSel;
  albaNEW(groupNewSel);

  m_OpSelect->SetNewSel(groupNewSel);

  opCopy=(albaOpSelect*)m_OpSelect->Copy();

  CPPUNIT_ASSERT(opCopy->GetInput()->Equals(m_OpSelect->GetInput()));
  CPPUNIT_ASSERT(opCopy->GetNewSel()->Equals(m_OpSelect->GetNewSel()));

  albaDEL(groupNewSel);
  albaDEL(groupInput);
  albaDEL(opCopy);

}
//----------------------------------------------------------------------------
void albaOpSelectTest::TestOpDo()
//----------------------------------------------------------------------------
{
  albaVMEGroup *groupInput;
  albaNEW(groupInput);

  m_OpSelect->SetInput(groupInput);

  albaVMEGroup *groupNewSel;
  albaNEW(groupNewSel);

  m_OpSelect->SetNewSel(groupNewSel);

  //OpDo method contain only a albaEventMacro, so it's impossible to test this method with cppUnit
  //this test checks only that there isn't crash.
  m_OpSelect->OpDo();

  albaDEL(groupNewSel);
  albaDEL(groupInput);
}
//----------------------------------------------------------------------------
void albaOpSelectTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  albaVMEGroup *groupInput;
  albaNEW(groupInput);

  m_OpSelect->SetInput(groupInput);

  albaVMEGroup *groupNewSel;
  albaNEW(groupNewSel);

  m_OpSelect->SetNewSel(groupNewSel);

  //OpUndo method contain only a albaEventMacro, so it's impossible to test this method with cppUnit
  //this test checks only that there isn't crash.
  m_OpSelect->OpUndo();

  albaDEL(groupNewSel);
  albaDEL(groupInput);
}