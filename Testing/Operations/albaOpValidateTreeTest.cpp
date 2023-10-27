/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpValidateTreeTest
 Authors: Daniele Giunchi
 
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
#include "albaOpValidateTreeTest.h"
#include "albaOpValidateTree.h"

#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMEFactory.h"
#include "albaVMESurface.h"
#include "albaVMEVolume.h"
#include "albaDataVector.h"
#include "albaVMEItem.h"

//---------------------------------------------------------
void albaOpValidateTreeTest::TestAccept()
//---------------------------------------------------------
{
  albaOpValidateTree *op = new albaOpValidateTree();
  op->TestModeOn();
  int result = op->Accept(NULL);
  CPPUNIT_ASSERT(result);
  albaDEL(op);
}

//---------------------------------------------------------
void albaOpValidateTreeTest::TestOpRun()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = ALBA_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.char_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

  albaOpValidateTree *op = new albaOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  op->OpRun();

  albaDEL(op);
  albaDEL(storageImport);
}

//---------------------------------------------------------
void albaOpValidateTreeTest::TestValidateTree_ValidTree()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = ALBA_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.char_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

  albaOpValidateTree *op = new albaOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();
  
  CPPUNIT_ASSERT(result);
  
  albaDEL(op);
  albaDEL(storageImport);
}

//---------------------------------------------------------
void albaOpValidateTreeTest::TestValidateTree_InvalidNode()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = ALBA_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST_INVALID_NODE.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.char_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

  albaOpValidateTree *op = new albaOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();

  //id will be updated by validate tree
  int newNodeId = storageImport->GetRoot()->GetChild(0)->GetId();
  CPPUNIT_ASSERT(result == albaOpValidateTree::VALIDATE_WARNING &&  newNodeId != -1);

  albaDEL(op);
  albaDEL(storageImport);
}

//---------------------------------------------------------
void albaOpValidateTreeTest::TestValidateTree_LinkNotPresent()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = ALBA_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.char_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );


  albaVMESurface *external;
  albaNEW(external);
  //set a link not parented inside tree
  storageImport->GetRoot()->GetChild(0)->GetChild(1)->GetChild(0)->SetLink("test",external);

  albaOpValidateTree *op = new albaOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();

  CPPUNIT_ASSERT(result == albaOpValidateTree::VALIDATE_ERROR);

  albaDEL(external);
  albaDEL(op);
  albaDEL(storageImport);
}
//---------------------------------------------------------
void albaOpValidateTreeTest::TestValidateTree_LinkNull()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = ALBA_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.char_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

 
  albaVME::albaLinksMap *linksmap = storageImport->GetRoot()->GetChild(0)->GetChild(1)->GetChild(0)->GetLinks();
  (*linksmap)["SlicedVME"] = albaVMELink(1000,NULL);
  
  
  albaOpValidateTree *op = new albaOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();

  CPPUNIT_ASSERT(result == albaOpValidateTree::VALIDATE_ERROR);

  
  albaDEL(op);
  albaDEL(storageImport);
}
//---------------------------------------------------------
void albaOpValidateTreeTest::TestValidateTree_BinaryFileNotPresent()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = ALBA_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST_BINARY_FILE_NOT_PRESENT.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.char_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

  albaOpValidateTree *op = new albaOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();

  
  CPPUNIT_ASSERT(result == albaOpValidateTree::VALIDATE_ERROR);

  albaDEL(op);
  albaDEL(storageImport);
}
//---------------------------------------------------------
void albaOpValidateTreeTest::TestValidateTree_UrlEmpty()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = ALBA_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST_URL_EMPTY.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.char_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

  albaOpValidateTree *op = new albaOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();


  CPPUNIT_ASSERT(result == albaOpValidateTree::VALIDATE_ERROR);

  albaDEL(op);
  albaDEL(storageImport);
}
//---------------------------------------------------------
void albaOpValidateTreeTest::TestValidateTree_ItemNotPresent()
//---------------------------------------------------------
{
  printf("\n This section is not testable due to asserts (and protection of datavector class) inserted after\
             several incoherence inside tree.\n");
}
//---------------------------------------------------------
void albaOpValidateTreeTest::TestValidateTree_MaxItemIdPatched()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = ALBA_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.char_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

  //set a link not parented inside tree
  albaVMEVolume *volume = albaVMEVolume::SafeDownCast(storageImport->GetRoot()->GetChild(0)->GetChild(1));
  

  storageImport->GetRoot()->SetMaxItemId(VTK_INT_MIN);

  albaOpValidateTree *op = new albaOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();

  int newMaxItemId = storageImport->GetRoot()->GetMaxItemId() > VTK_INT_MIN;
  CPPUNIT_ASSERT(result == albaOpValidateTree::VALIDATE_WARNING && newMaxItemId);

  albaDEL(op);
  albaDEL(storageImport);
}
//---------------------------------------------------------
void albaOpValidateTreeTest::TestValidateTree_ArchiveFileNotPresent()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = ALBA_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.char_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

  //set a link not parented inside tree
  albaVMEVolume *volume = albaVMEVolume::SafeDownCast(storageImport->GetRoot()->GetChild(0)->GetChild(1));
  albaDataVector *dv = volume->GetDataVector();
  dv->SetSingleFileMode(true);
  dv->GetItem(0)->SetArchiveFileName(albaString(""));

  albaOpValidateTree *op = new albaOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();


  CPPUNIT_ASSERT(result == albaOpValidateTree::VALIDATE_ERROR);

  albaDEL(op);
  albaDEL(storageImport);
}