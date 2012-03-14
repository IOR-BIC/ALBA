/*==============================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpManagerTest.cpp,v $
Language:  C++
Date:      $Date: 2009-12-01 10:22:33 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#include "mafDefines.h" 
//------------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//------------------------------------------------------------------------------

#include "mafOpManagerTest.h"
#include "mafOpManager.h"
#include "mafUser.h"
#include "mafOpDummy.h"
#include "mafVMEDummy.h"
#include "mafGUISettings.h"


bool mafOpDummy::m_Do = false;
bool mafOpDummy::m_Undo = false;
bool mafOpDummy::m_Stop = false;
bool mafOpDummy::m_Executed = false;
//------------------------------------------------------------------------------
void mafOpManagerTest::setUp()
//------------------------------------------------------------------------------
{
  m_OpManager = new mafOpManager(); 

  mafOpDummy::m_Do = false;
  mafOpDummy::m_Undo = false;
  mafOpDummy::m_Stop = false;
  mafOpDummy::m_Executed = false;
}
//------------------------------------------------------------------------------
void mafOpManagerTest::tearDown()
//------------------------------------------------------------------------------
{
  cppDEL(m_OpManager);

  delete wxLog::SetActiveTarget(NULL);
}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestDynamicAllocation()
//------------------------------------------------------------------------------
{
  mafOpManager *OpManager = new mafOpManager();
  cppDEL(OpManager);
}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestStaticAllocation()
//------------------------------------------------------------------------------
{
  mafOpManager OpManager;
}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestOpAdd()
//------------------------------------------------------------------------------
{
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);

  //Check if the number of the operations added is equals to 1
  CPPUNIT_ASSERT( m_OpManager->m_OpList.size() == 1 );
  CPPUNIT_ASSERT( m_OpManager->m_NumOp == 1 );

  mafOpDummy *op2 = new mafOpDummy();
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
void mafOpManagerTest::TestVmeSelected()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme);

  //Check the selected vme
  CPPUNIT_ASSERT( m_OpManager->GetSelectedVme() == vme.GetPointer() );
}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestOpRun1()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme);

  int id = op1->m_Id;
  m_OpManager->OpRun(id);

  mafOpDummy *opCopied1 = (mafOpDummy*)(m_OpManager->m_RunningOp);
  //Check if the operation has been executed
  CPPUNIT_ASSERT( mafOpDummy::m_Executed == true );

  opCopied1->End();

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( mafOpDummy::m_Do == true );

  m_OpManager->OpRun(OP_UNDO);
  
  //Check if the OpDo has been called
  CPPUNIT_ASSERT( mafOpDummy::m_Undo == true );

}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestOpRun2()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme);

  int id = op1->m_Id;
  m_OpManager->OpRun(op1);

  mafOpDummy *opCopied1 = (mafOpDummy*)(m_OpManager->m_RunningOp);
  //Check if the operation has been executed
  CPPUNIT_ASSERT( mafOpDummy::m_Executed == true );

  opCopied1->End();

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( mafOpDummy::m_Do == true );

  m_OpManager->OpRun(OP_UNDO);

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( mafOpDummy::m_Undo == true );

}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestOpRun3()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme);

  int id = op1->m_Id;
  mafString name = "mafOpDummy";
  m_OpManager->OpRun(name);

  mafOpDummy *opCopied1 = (mafOpDummy*)(m_OpManager->m_RunningOp);
  //Check if the operation has been executed
  CPPUNIT_ASSERT( mafOpDummy::m_Executed == true );

  opCopied1->End();

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( mafOpDummy::m_Do == true );

  m_OpManager->OpRun(OP_UNDO);

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( mafOpDummy::m_Undo == true );

}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestOpExec()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme);

  m_OpManager->OpExec(op1);

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( mafOpDummy::m_Do == true );
}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestOpSelect()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme1;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);

  mafSmartPointer<mafVMEDummy> vme2;
  m_OpManager->OpSelect(vme2);

  //Check if the OpDo has been called
  CPPUNIT_ASSERT( mafOpDummy::m_Do == false );
}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestRunning()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme1;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);
  m_OpManager->OpRun(op1->m_Id);
  
  mafOpDummy *opCopied1 = (mafOpDummy*)(m_OpManager->m_RunningOp);
  //Check if an operation is running
  CPPUNIT_ASSERT( m_OpManager->Running() == true );

  opCopied1->End();

  //Check if an operation is running
  CPPUNIT_ASSERT( m_OpManager->Running() == false );
}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestClearUndoStack()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme1;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);
  m_OpManager->OpRun(op1->m_Id);

  mafOpDummy *opCopied1 = (mafOpDummy*)(m_OpManager->m_RunningOp);
  opCopied1->End();

  mafSmartPointer<mafVMEDummy> vme2;
  mafOpDummy *op2 = new mafOpDummy();
  m_OpManager->OpAdd(op2);
  m_OpManager->VmeSelected(vme2);
  m_OpManager->OpRun(op2->m_Id);

  mafOpDummy *opCopied2 = (mafOpDummy*)(m_OpManager->m_RunningOp);
  opCopied2->End();

  //Check if the undo stack isn't empty
  CPPUNIT_ASSERT( m_OpManager->m_Context.Undo_IsEmpty() == false );

  m_OpManager->ClearUndoStack();
  //Check if the undo stack is empty
  CPPUNIT_ASSERT( m_OpManager->m_Context.Undo_IsEmpty() == true );
}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestForceStopWithOk()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme1;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);
  m_OpManager->OpRun(op1->m_Id);

  m_OpManager->ForceStopWithOk();

  //The command ForceStopWithOk doen't work under test because it's necessary the gui
  CPPUNIT_ASSERT( mafOpDummy::m_Do == false && m_OpManager->Running() == true );

  //To clear the memory
  mafOpDummy *opCopied1 = (mafOpDummy*)(m_OpManager->m_RunningOp);
  opCopied1->End();

}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestForceStopWithCancel()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme1;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);
  m_OpManager->OpRun(op1->m_Id);

  m_OpManager->ForceStopWithCancel();

  //The command ForceStopWithCancel work under test
  CPPUNIT_ASSERT( m_OpManager->Running() == false );

}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestStopCurrentOperation()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme1;
  mafOpDummy *op1 = new mafOpDummy();
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
void mafOpManagerTest::TestGetRunningOperation()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme1;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);
  m_OpManager->VmeSelected(vme1);
  m_OpManager->OpRun(op1->m_Id);

  CPPUNIT_ASSERT( m_OpManager->Running() == true );

  mafOp *currentOp = m_OpManager->GetRunningOperation();

  // Check if the running operation is right
  CPPUNIT_ASSERT( currentOp->GetTypeName() == op1->GetTypeName() );
  CPPUNIT_ASSERT( currentOp->GetTypeId() == op1->GetTypeId() );

  mafOpDummy *opCopied1 = (mafOpDummy*)(m_OpManager->m_RunningOp);
  CPPUNIT_ASSERT( opCopied1 == currentOp );

  //To clear the memory
  opCopied1->End();
}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestGetOperationById()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme1;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);

  int id1 = op1->m_Id;

  mafSmartPointer<mafVMEDummy> vme2;
  mafOpDummy *op2 = new mafOpDummy();
  m_OpManager->OpAdd(op2);

  int id2 = op2->m_Id;

  // Check if the the operation returned is right
  CPPUNIT_ASSERT( m_OpManager->GetOperationById(id1) == op1 );
  CPPUNIT_ASSERT( m_OpManager->GetOperationById(id2) == op2 );
}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestCollaborate()
//------------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEDummy> vme1;
  mafOpDummy *op1 = new mafOpDummy();
  m_OpManager->OpAdd(op1);

  mafSmartPointer<mafVMEDummy> vme2;
  mafOpDummy *op2 = new mafOpDummy();
  m_OpManager->OpAdd(op2);

  m_OpManager->Collaborate(false);

  // Check the collaborate status
  CPPUNIT_ASSERT( op1->GetCollaborateStatus() == false );
  CPPUNIT_ASSERT( op2->GetCollaborateStatus() == false );

  m_OpManager->Collaborate(true);

  CPPUNIT_ASSERT( op1->GetCollaborateStatus() == true );
  CPPUNIT_ASSERT( op2->GetCollaborateStatus() == true );
}
//------------------------------------------------------------------------------
void mafOpManagerTest::TestSetMafUser()
//------------------------------------------------------------------------------
{
  // The user default is set to NULL
  CPPUNIT_ASSERT( m_OpManager->GetMafUser() == NULL );

  mafUser *user = new mafUser();

  m_OpManager->SetMafUser(user);

  // Check if the user is right
  CPPUNIT_ASSERT( m_OpManager->GetMafUser() == user );
}
