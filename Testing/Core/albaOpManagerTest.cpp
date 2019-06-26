/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpManagerTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//------------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//------------------------------------------------------------------------------

#include "albaOpManagerTest.h"
#include "albaOpManager.h"
#include "albaUser.h"
#include "albaOpDummy.h"
#include "albaVMEDummy.h"
#include "albaGUISettings.h"


bool albaOpDummy::m_Do = false;
bool albaOpDummy::m_Undo = false;
bool albaOpDummy::m_Stop = false;
bool albaOpDummy::m_Executed = false;
//------------------------------------------------------------------------------
void albaOpManagerTest::BeforeTest()
//------------------------------------------------------------------------------
{
  m_OpManager = new albaOpManager(); 

  albaOpDummy::m_Do = false;
  albaOpDummy::m_Undo = false;
  albaOpDummy::m_Stop = false;
  albaOpDummy::m_Executed = false;
}
//------------------------------------------------------------------------------
void albaOpManagerTest::AfterTest()
//------------------------------------------------------------------------------
{
  cppDEL(m_OpManager);
}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestDynamicAllocation()
//------------------------------------------------------------------------------
{
  albaOpManager *OpManager = new albaOpManager();
  cppDEL(OpManager);
}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestStaticAllocation()
//------------------------------------------------------------------------------
{
  albaOpManager OpManager;
}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestOpAdd()
//------------------------------------------------------------------------------
{
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);

  //Check if the number of the operations added is equals to 1
  CPPUNIT_ASSERT( m_OpManager->m_OpList.size() == 1 );
  CPPUNIT_ASSERT( m_OpManager->m_NumOp == 1 );

  albaOpDummy *op2 = new albaOpDummy();
  m_OpManager->OpAdd(op2);

  //Check if the number of the operations added is equals to 2
  CPPUNIT_ASSERT( m_OpManager->m_OpList.size() == 2 );
  CPPUNIT_ASSERT( m_OpManager->m_NumOp == 2 );

  //Check if the first operation is the same to op1
  CPPUNIT_ASSERT( m_OpManager->m_OpList[0] == op1 );

  //Check if the second operation is the same to op2
  CPPUNIT_ASSERT( m_OpManager->m_OpList[1] == op2 );

}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestVmeSelected()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme);

  //Check the selected vme
  CPPUNIT_ASSERT( m_OpManager->GetSelectedVme() == vme.GetPointer() );
}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestOpRun1()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme);

  int id = op1->m_Id;
  m_OpManager->OpRun(id);

  albaOpDummy *opCopied1 = (albaOpDummy*)(m_OpManager->m_RunningOp);
  //Check if the operation has been executed
  CPPUNIT_ASSERT( albaOpDummy::m_Executed == true );

  opCopied1->End();

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( albaOpDummy::m_Do == true );

  m_OpManager->OpRun(OP_UNDO);
  
  //Check if the OpDo has been called
  CPPUNIT_ASSERT( albaOpDummy::m_Undo == true );

}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestOpRun2()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme);

  int id = op1->m_Id;
  m_OpManager->OpRun(op1);

  albaOpDummy *opCopied1 = (albaOpDummy*)(m_OpManager->m_RunningOp);
  //Check if the operation has been executed
  CPPUNIT_ASSERT( albaOpDummy::m_Executed == true );

  opCopied1->End();

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( albaOpDummy::m_Do == true );

  m_OpManager->OpRun(OP_UNDO);

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( albaOpDummy::m_Undo == true );

}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestOpRun3()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme);

  int id = op1->m_Id;
  albaString name = "albaOpDummy";
  m_OpManager->OpRun(name);

  albaOpDummy *opCopied1 = (albaOpDummy*)(m_OpManager->m_RunningOp);
  //Check if the operation has been executed
  CPPUNIT_ASSERT( albaOpDummy::m_Executed == true );

  opCopied1->End();

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( albaOpDummy::m_Do == true );

  m_OpManager->OpRun(OP_UNDO);

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( albaOpDummy::m_Undo == true );

}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestOpExec()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme);

  m_OpManager->OpExec(op1);

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( albaOpDummy::m_Do == true );
}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestOpSelect()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme1;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);

  albaSmartPointer<albaVMEDummy> vme2;
  m_OpManager->OpSelect(vme2);

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( albaOpDummy::m_Do == false );
}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestRunning()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme1;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);
  m_OpManager->OpRun(op1->m_Id);
  
  albaOpDummy *opCopied1 = (albaOpDummy*)(m_OpManager->m_RunningOp);
  //Check if an operation is running
  CPPUNIT_ASSERT( m_OpManager->Running() == true );

  opCopied1->End();

  //Check if an operation is running
  CPPUNIT_ASSERT( m_OpManager->Running() == false );
}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestClearUndoStack()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme1;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);
  m_OpManager->OpRun(op1->m_Id);

  albaOpDummy *opCopied1 = (albaOpDummy*)(m_OpManager->m_RunningOp);
  opCopied1->End();

  albaSmartPointer<albaVMEDummy> vme2;
  albaOpDummy *op2 = new albaOpDummy();
  m_OpManager->OpAdd(op2);
  m_OpManager->VmeSelected(vme2);
  m_OpManager->OpRun(op2->m_Id);

  albaOpDummy *opCopied2 = (albaOpDummy*)(m_OpManager->m_RunningOp);
  opCopied2->End();

  //Check if the undo stack isn't empty
  CPPUNIT_ASSERT( m_OpManager->m_Context.Undo_IsEmpty() == false );

  m_OpManager->ClearUndoStack();
  //Check if the undo stack is empty
  CPPUNIT_ASSERT( m_OpManager->m_Context.Undo_IsEmpty() == true );
}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestForceStopWithOk()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme1;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);
  m_OpManager->OpRun(op1->m_Id);

  m_OpManager->ForceStopWithOk();

  //The command ForceStopWithOk doen't work under test because it's necessary the gui
  CPPUNIT_ASSERT( albaOpDummy::m_Do == false && m_OpManager->Running() == true );

  //To clear the memory
  albaOpDummy *opCopied1 = (albaOpDummy*)(m_OpManager->m_RunningOp);
  opCopied1->End();

}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestForceStopWithCancel()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme1;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);
  m_OpManager->OpRun(op1->m_Id);

  m_OpManager->ForceStopWithCancel();

  //The command ForceStopWithCancel work under test
  CPPUNIT_ASSERT( m_OpManager->Running() == false );

}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestStopCurrentOperation()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme1;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);
  m_OpManager->OpRun(op1->m_Id);

  // Check if there is a running operation
  CPPUNIT_ASSERT( m_OpManager->Running() == true );

  m_OpManager->StopCurrentOperation();

  // Check if the current operation has been stopped
  CPPUNIT_ASSERT( m_OpManager->Running() == false );

}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestGetRunningOperation()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme1;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);
  m_OpManager->OpRun(op1->m_Id);

  CPPUNIT_ASSERT( m_OpManager->Running() == true );

  albaOp *currentOp = m_OpManager->GetRunningOperation();

  // Check if the running operation is right
  CPPUNIT_ASSERT( currentOp->GetTypeName() == op1->GetTypeName() );
  CPPUNIT_ASSERT( currentOp->GetTypeId() == op1->GetTypeId() );

  albaOpDummy *opCopied1 = (albaOpDummy*)(m_OpManager->m_RunningOp);
  CPPUNIT_ASSERT( opCopied1 == currentOp );

  //To clear the memory
  opCopied1->End();
}
//------------------------------------------------------------------------------
void albaOpManagerTest::TestGetOperationById()
//------------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEDummy> vme1;
  albaOpDummy *op1 = new albaOpDummy();
  m_OpManager->OpAdd(op1);

  int id1 = op1->m_Id;

  albaSmartPointer<albaVMEDummy> vme2;
  albaOpDummy *op2 = new albaOpDummy();
  m_OpManager->OpAdd(op2);

  int id2 = op2->m_Id;

  // Check if the the operation returned is right
  CPPUNIT_ASSERT( m_OpManager->GetOperationById(id1) == op1 );
  CPPUNIT_ASSERT( m_OpManager->GetOperationById(id2) == op2 );
}

//------------------------------------------------------------------------------
void albaOpManagerTest::TestSetMafUser()
//------------------------------------------------------------------------------
{
  // The user default is set to NULL
  CPPUNIT_ASSERT( m_OpManager->GetMafUser() == NULL );

  albaUser *user = new albaUser();

  m_OpManager->SetMafUser(user);

  // Check if the user is right
  CPPUNIT_ASSERT( m_OpManager->GetMafUser() == user );
}
