/*=========================================================================

 Program: MAF2
 Module: mafVMEIteratorTest
 Authors: Roberto Mucci
 
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
#include "mafVMEIteratorTest.h"
#include "mafCoreTests.h"
#include "mafSmartPointer.h"
#include "mafVME.h"
#include "mafVMEIterator.h"
#include "mafVMERoot.h"
#include "mafVMEFactory.h"
#include "mafVMEStorage.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);



//----------------------------------------------------------------------------
void mafVMEIteratorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEIteratorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERoot> root;
  mafVMEIterator *iter = root->NewIterator();
  iter->Delete();
}

//----------------------------------------------------------------------------
void mafVMEIteratorTest::GetFirstNodeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);


  mafVMEIterator *iter = storage->GetRoot()->NewIterator();
  mafVME* first = iter->GetFirstNode();

  //First node is Root
  CPPUNIT_ASSERT(first->GetId() == -1);
  CPPUNIT_ASSERT(first->IsA("mafVMERoot"));

  iter->Delete();
  mafDEL(storage);
}
//----------------------------------------------------------------------------
void mafVMEIteratorTest::GetLastNodeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);


  mafVMEIterator *iter = storage->GetRoot()->NewIterator();
  mafVME* last = iter->GetLastNode();

  //Last node is a landmark
  CPPUNIT_ASSERT(last->GetId() == 3);
  CPPUNIT_ASSERT(last->IsA("mafVMELandmark"));

  iter->Delete();
  mafDEL(storage);
}

//----------------------------------------------------------------------------
void mafVMEIteratorTest::GetNextNodeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);


  mafVMEIterator *iter = storage->GetRoot()->NewIterator();
  mafVME* first = iter->GetFirstNode();
  mafVME* next = iter->GetNextNode();

  //next node is a volumeGray
  CPPUNIT_ASSERT(next->GetId() == 1);
  CPPUNIT_ASSERT(next->IsA("mafVMEVolumeGray"));

  iter->Delete();
  mafDEL(storage);
}

//----------------------------------------------------------------------------
void mafVMEIteratorTest::GetPreviousNodeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);


  mafVMEIterator *iter = storage->GetRoot()->NewIterator();
  mafVME* last = iter->GetLastNode();
  mafVME* next = iter->GetPreviousNode();

  //previous node is a mafVMELandmarkCloud
  CPPUNIT_ASSERT(next->GetId() == 12);
  CPPUNIT_ASSERT(next->IsA("mafVMELandmarkCloud"));

  iter->Delete();
  mafDEL(storage);
}

//----------------------------------------------------------------------------
void mafVMEIteratorTest::IsVisibleTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);


  mafVMEIterator *iter = storage->GetRoot()->NewIterator();
  mafVME* last = iter->GetLastNode();
  
 
  //last node is visible
  CPPUNIT_ASSERT(iter->IsVisible(last));

  iter->Delete();
  mafDEL(storage);
}

//----------------------------------------------------------------------------
void mafVMEIteratorTest::SetRootNodeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);


  mafVMEIterator *iter = storage->GetRoot()->NewIterator();
  mafVME* first = iter->GetFirstNode();
  mafVME* next = iter->GetNextNode();
  iter->SetRootNode(next);

  iter->InitTraversal();
  mafVME* newRoot = iter->GetFirstNode();


  //new root node is a mafVMEVolumeGray
  CPPUNIT_ASSERT(newRoot->GetId() == 1);
  CPPUNIT_ASSERT(newRoot->IsA("mafVMEVolumeGray"));

  iter->Delete();
  mafDEL(storage);
}

//----------------------------------------------------------------------------
void mafVMEIteratorTest::GetTraversalModeTest()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_NodeIteretor/Test_NodeIterator.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafVMEIterator *iter = storage->GetRoot()->NewIterator();
  mafVME* first = iter->GetFirstNode();

  bool traversal = (iter->GetTraversalMode() == 0);

  //traversal off
  CPPUNIT_ASSERT(traversal);

  iter->SetTraversalMode(1);
  first = iter->GetFirstNode();
  
  traversal = (iter->GetTraversalMode() == 1);
  //traversal on
  CPPUNIT_ASSERT(traversal);

  iter->Delete();
  mafDEL(storage);
}

//----------------------------------------------------------------------------
void mafVMEIteratorTest::SetTraversalModeToPreOrderTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERoot> root;
  mafVMEIterator *iter = root->NewIterator();
  iter->SetTraversalModeToPreOrder();
  bool traversal = (iter->GetTraversalMode() == 0);

  //traversal off
  CPPUNIT_ASSERT(traversal);

  iter->Delete();
}
//----------------------------------------------------------------------------
void mafVMEIteratorTest::SetTraversalModeToPostOrderTest()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMERoot> root;
  mafVMEIterator *iter = root->NewIterator();
  iter->SetTraversalModeToPostOrder();
  bool traversal = (iter->GetTraversalMode() == 1);

  //traversal on
  CPPUNIT_ASSERT(traversal);

  iter->Delete();

}
