/*=========================================================================

 Program: MAF2
 Module: mafOpSelectTest
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
#include "mafOpSelectTest.h"

#include "mafOpSelect.h"
#include "mafVMEGroup.h"


//----------------------------------------------------------------------------
void mafOpSelectTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpSelectTest::setUp()
//----------------------------------------------------------------------------
{
  m_OpSelect=new mafOpSelect("EDIT");
}
//----------------------------------------------------------------------------
void mafOpSelectTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_OpSelect);
}
//----------------------------------------------------------------------------
void mafOpSelectTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpSelect *OpSelect=new mafOpSelect("EDIT");
  mafDEL(OpSelect);
}
//----------------------------------------------------------------------------
void mafOpSelectTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafVMEGroup *group;
  mafNEW(group);

  bool result=m_OpSelect->Accept(group);
  
  CPPUNIT_ASSERT(result);
  
  mafDEL(group);
}
//----------------------------------------------------------------------------
void mafOpSelectTest::TestSetInput()
//----------------------------------------------------------------------------
{
  mafVMEGroup *group;
  mafNEW(group);

  m_OpSelect->SetInput(group);

  CPPUNIT_ASSERT(m_OpSelect->GetInput()==group);

  mafDEL(group);
}
//----------------------------------------------------------------------------
void mafOpSelectTest::TestSetNewSel()
//----------------------------------------------------------------------------
{
  mafVMEGroup *group;
  mafNEW(group);

  m_OpSelect->SetNewSel(group);

  CPPUNIT_ASSERT(m_OpSelect->GetNewSel()==group);

  mafDEL(group);
}
//----------------------------------------------------------------------------
void mafOpSelectTest::TestCopy()
//----------------------------------------------------------------------------
{
  mafOpSelect *opCopy;

  mafVMEGroup *groupInput;
  mafNEW(groupInput);

  m_OpSelect->SetInput(groupInput);

  mafVMEGroup *groupNewSel;
  mafNEW(groupNewSel);

  m_OpSelect->SetNewSel(groupNewSel);

  opCopy=(mafOpSelect*)m_OpSelect->Copy();

  CPPUNIT_ASSERT(opCopy->GetInput()->Equals(m_OpSelect->GetInput()));
  CPPUNIT_ASSERT(opCopy->GetNewSel()->Equals(m_OpSelect->GetNewSel()));

  mafDEL(groupNewSel);
  mafDEL(groupInput);
  mafDEL(opCopy);

}
//----------------------------------------------------------------------------
void mafOpSelectTest::TestOpDo()
//----------------------------------------------------------------------------
{
  mafVMEGroup *groupInput;
  mafNEW(groupInput);

  m_OpSelect->SetInput(groupInput);

  mafVMEGroup *groupNewSel;
  mafNEW(groupNewSel);

  m_OpSelect->SetNewSel(groupNewSel);

  //OpDo method contain only a mafEventMacro, so it's impossible to test this method with cppUnit
  //this test checks only that there isn't crash.
  m_OpSelect->OpDo();

  mafDEL(groupNewSel);
  mafDEL(groupInput);
}
//----------------------------------------------------------------------------
void mafOpSelectTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  mafVMEGroup *groupInput;
  mafNEW(groupInput);

  m_OpSelect->SetInput(groupInput);

  mafVMEGroup *groupNewSel;
  mafNEW(groupNewSel);

  m_OpSelect->SetNewSel(groupNewSel);

  //OpUndo method contain only a mafEventMacro, so it's impossible to test this method with cppUnit
  //this test checks only that there isn't crash.
  m_OpSelect->OpUndo();

  mafDEL(groupNewSel);
  mafDEL(groupInput);
}