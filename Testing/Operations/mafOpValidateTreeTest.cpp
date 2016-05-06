/*=========================================================================

 Program: MAF2
 Module: mafOpValidateTreeTest
 Authors: Daniele Giunchi
 
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
#include "mafOpValidateTreeTest.h"
#include "mafOpValidateTree.h"

#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMEFactory.h"
#include "mafVMESurface.h"
#include "mafVMEVolume.h"
#include "mafDataVector.h"
#include "mafVMEItem.h"

//---------------------------------------------------------
void mafOpValidateTreeTest::TestAccept()
//---------------------------------------------------------
{
  mafOpValidateTree *op = new mafOpValidateTree();
  op->TestModeOn();
  int result = op->Accept(NULL);
  CPPUNIT_ASSERT(result);
  mafDEL(op);
}

//---------------------------------------------------------
void mafOpValidateTreeTest::TestOpRun()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

  mafOpValidateTree *op = new mafOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  op->OpRun();

  mafDEL(op);
  mafDEL(storageImport);
}

//---------------------------------------------------------
void mafOpValidateTreeTest::TestValidateTree_ValidTree()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

  mafOpValidateTree *op = new mafOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();
  
  CPPUNIT_ASSERT(result);
  
  mafDEL(op);
  mafDEL(storageImport);
}

//---------------------------------------------------------
void mafOpValidateTreeTest::TestValidateTree_InvalidNode()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST_INVALID_NODE.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

  mafOpValidateTree *op = new mafOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();

  //id will be updated by validate tree
  int newNodeId = storageImport->GetRoot()->GetChild(0)->GetId();
  CPPUNIT_ASSERT(result == mafOpValidateTree::VALIDATE_WARNING &&  newNodeId != -1);

  mafDEL(op);
  mafDEL(storageImport);
}

//---------------------------------------------------------
void mafOpValidateTreeTest::TestValidateTree_LinkNotPresent()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );


  mafVMESurface *external;
  mafNEW(external);
  //set a link not parented inside tree
  storageImport->GetRoot()->GetChild(0)->GetChild(1)->GetChild(0)->SetLink("test",external);

  mafOpValidateTree *op = new mafOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();

  CPPUNIT_ASSERT(result == mafOpValidateTree::VALIDATE_ERROR);

  mafDEL(external);
  mafDEL(op);
  mafDEL(storageImport);
}
//---------------------------------------------------------
void mafOpValidateTreeTest::TestValidateTree_LinkNull()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

 
  mafVME::mafLinksMap *linksmap = storageImport->GetRoot()->GetChild(0)->GetChild(1)->GetChild(0)->GetLinks();
  (*linksmap)["SlicedVME"] = mmuNodeLink(1000,NULL,-1);
  
  
  mafOpValidateTree *op = new mafOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();

  CPPUNIT_ASSERT(result == mafOpValidateTree::VALIDATE_ERROR);

  
  mafDEL(op);
  mafDEL(storageImport);
}
//---------------------------------------------------------
void mafOpValidateTreeTest::TestValidateTree_BinaryFileNotPresent()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST_BINARY_FILE_NOT_PRESENT.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

  mafOpValidateTree *op = new mafOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();

  
  CPPUNIT_ASSERT(result == mafOpValidateTree::VALIDATE_ERROR);

  mafDEL(op);
  mafDEL(storageImport);
}
//---------------------------------------------------------
void mafOpValidateTreeTest::TestValidateTree_UrlEmpty()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST_URL_EMPTY.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

  mafOpValidateTree *op = new mafOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();


  CPPUNIT_ASSERT(result == mafOpValidateTree::VALIDATE_ERROR);

  mafDEL(op);
  mafDEL(storageImport);
}
//---------------------------------------------------------
void mafOpValidateTreeTest::TestValidateTree_ItemNotPresent()
//---------------------------------------------------------
{
  printf("\n This section is not testable due to asserts (and protection of datavector class) inserted after\
             several incoherence inside tree.\n");
}
//---------------------------------------------------------
void mafOpValidateTreeTest::TestValidateTree_MaxItemIdPatched()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

  //set a link not parented inside tree
  mafVMEVolume *volume = mafVMEVolume::SafeDownCast(storageImport->GetRoot()->GetChild(0)->GetChild(1));
  

  storageImport->GetRoot()->SetMaxItemId(VTK_INT_MIN);

  mafOpValidateTree *op = new mafOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();

  int newMaxItemId = storageImport->GetRoot()->GetMaxItemId() > VTK_INT_MIN;
  CPPUNIT_ASSERT(result == mafOpValidateTree::VALIDATE_WARNING && newMaxItemId);

  mafDEL(op);
  mafDEL(storageImport);
}
//---------------------------------------------------------
void mafOpValidateTreeTest::TestValidateTree_ArchiveFileNotPresent()
//---------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ValidateTree\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

  //set a link not parented inside tree
  mafVMEVolume *volume = mafVMEVolume::SafeDownCast(storageImport->GetRoot()->GetChild(0)->GetChild(1));
  mafDataVector *dv = volume->GetDataVector();
  dv->SetSingleFileMode(true);
  dv->GetItem(0)->SetArchiveFileName(mafString(""));

  mafOpValidateTree *op = new mafOpValidateTree();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->ValidateTree();


  CPPUNIT_ASSERT(result == mafOpValidateTree::VALIDATE_ERROR);

  mafDEL(op);
  mafDEL(storageImport);
}