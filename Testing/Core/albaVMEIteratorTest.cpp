/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEIteratorTest
 Authors: Roberto Mucci
 
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
#include "albaVMEIteratorTest.h"
#include "albaCoreTests.h"
#include "albaSmartPointer.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaVMERoot.h"
#include "albaVMEFactory.h"
#include "albaVMEStorage.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);



//----------------------------------------------------------------------------
void albaVMEIteratorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEIteratorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;
  albaVMEIterator *iter = root->NewIterator();
  iter->Delete();
}

//----------------------------------------------------------------------------
void albaVMEIteratorTest::GetFirstNodeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);


  albaVMEIterator *iter = storage->GetRoot()->NewIterator();
  albaVME* first = iter->GetFirstNode();

  //First node is Root
  CPPUNIT_ASSERT(first->GetId() == -1);
  CPPUNIT_ASSERT(first->IsA("albaVMERoot"));

  iter->Delete();
  albaDEL(storage);
}
//----------------------------------------------------------------------------
void albaVMEIteratorTest::GetLastNodeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);


  albaVMEIterator *iter = storage->GetRoot()->NewIterator();
  albaVME* last = iter->GetLastNode();

  //Last node is a landmark
  CPPUNIT_ASSERT(last->GetId() == 3);
  CPPUNIT_ASSERT(last->IsA("albaVMELandmark"));

  iter->Delete();
  albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaVMEIteratorTest::GetNextNodeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);


  albaVMEIterator *iter = storage->GetRoot()->NewIterator();
  albaVME* first = iter->GetFirstNode();
  albaVME* next = iter->GetNextNode();

  //next node is a volumeGray
  CPPUNIT_ASSERT(next->GetId() == 1);
  CPPUNIT_ASSERT(next->IsA("albaVMEVolumeGray"));

  iter->Delete();
  albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaVMEIteratorTest::GetPreviousNodeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);


  albaVMEIterator *iter = storage->GetRoot()->NewIterator();
  albaVME* last = iter->GetLastNode();
  albaVME* next = iter->GetPreviousNode();

  //previous node is a albaVMELandmarkCloud
  CPPUNIT_ASSERT(next->GetId() == 12);
  CPPUNIT_ASSERT(next->IsA("albaVMELandmarkCloud"));

  iter->Delete();
  albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaVMEIteratorTest::IsVisibleTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);


  albaVMEIterator *iter = storage->GetRoot()->NewIterator();
  albaVME* last = iter->GetLastNode();
  
 
  //last node is visible
  CPPUNIT_ASSERT(iter->IsVisible(last));

  iter->Delete();
  albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaVMEIteratorTest::SetRootNodeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);


  albaVMEIterator *iter = storage->GetRoot()->NewIterator();
  albaVME* first = iter->GetFirstNode();
  albaVME* next = iter->GetNextNode();
  iter->SetRootNode(next);

  iter->InitTraversal();
  albaVME* newRoot = iter->GetFirstNode();


  //new root node is a albaVMEVolumeGray
  CPPUNIT_ASSERT(newRoot->GetId() == 1);
  CPPUNIT_ASSERT(newRoot->IsA("albaVMEVolumeGray"));

  iter->Delete();
  albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaVMEIteratorTest::GetTraversalModeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVMEIterator *iter = storage->GetRoot()->NewIterator();
  albaVME* first = iter->GetFirstNode();

  bool traversal = (iter->GetTraversalMode() == 0);

  //traversal off
  CPPUNIT_ASSERT(traversal);

  iter->SetTraversalMode(1);
  first = iter->GetFirstNode();
  
  traversal = (iter->GetTraversalMode() == 1);
  //traversal on
  CPPUNIT_ASSERT(traversal);

  iter->Delete();
  albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaVMEIteratorTest::SetTraversalModeToPreOrderTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;
  albaVMEIterator *iter = root->NewIterator();
  iter->SetTraversalModeToPreOrder();
  bool traversal = (iter->GetTraversalMode() == 0);

  //traversal off
  CPPUNIT_ASSERT(traversal);

  iter->Delete();
}
//----------------------------------------------------------------------------
void albaVMEIteratorTest::SetTraversalModeToPostOrderTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;
  albaVMEIterator *iter = root->NewIterator();
  iter->SetTraversalModeToPostOrder();
  bool traversal = (iter->GetTraversalMode() == 1);

  //traversal on
  CPPUNIT_ASSERT(traversal);

  iter->Delete();

}
